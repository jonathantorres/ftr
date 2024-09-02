use ftr::{Conf, Log, Session, Sessions, StatusCode};
use std::collections::HashMap;
use std::fmt;
use std::io;
use std::io::Write;
use std::net::{SocketAddr, TcpListener, TcpStream, ToSocketAddrs};
use std::os::unix::io::{AsRawFd, RawFd};
use std::sync::atomic::{AtomicUsize, Ordering};
use std::sync::{Arc, Mutex};
use std::thread::JoinHandle;

pub struct Server {
    host: String,
    resolved_host: SocketAddr,
    conf: Conf,
    log: Arc<Log>,
    sessions: Arc<Mutex<Sessions>>,
    session_threads: Mutex<HashMap<usize, JoinHandle<()>>>,
    port: i32,
    ctrl_listener_fd: Option<RawFd>,
    next_session_id: AtomicUsize,
    is_reloading: bool,
    is_shutting_down: bool,
}

impl Server {
    pub fn new(conf: Conf, log: Log) -> Result<Self, ServerError> {
        let port = conf.port;
        let host = String::from(&conf.server_name);

        log.log_acc("Initializing server...");

        let addr = format!("{}:{}", host, port);
        let res_addr = match addr.to_socket_addrs() {
            Ok(mut addrs) => {
                if let Some(addr) = addrs.next() {
                    addr
                } else {
                    return Err(ServerError::HostnameNotResolved(
                        io::ErrorKind::AddrNotAvailable,
                    ));
                }
            }
            Err(err) => return Err(ServerError::HostnameNotResolved(err.kind())),
        };

        Ok(Server {
            host: host,
            resolved_host: res_addr,
            conf: conf,
            log: Arc::new(log),
            sessions: Arc::new(Mutex::new(Sessions::new())),
            session_threads: Mutex::new(HashMap::new()),
            next_session_id: AtomicUsize::new(0),
            port: port,
            ctrl_listener_fd: None,
            is_reloading: false,
            is_shutting_down: false,
        })
    }

    pub fn start(&self) -> Result<(), ServerError> {
        self.log.log_acc("Server starting...");

        let listener = match TcpListener::bind(self.resolved_host) {
            Ok(l) => l,
            Err(err) => return Err(ServerError::BindFailed(err.kind())),
        };

        self.log.log_acc("Server started OK.");

        // TODO: still don't know how to use this
        // self.ctrl_listener_fd = Some(listener.as_raw_fd());

        loop {
            match listener.accept() {
                Ok((conn, _remote_addr)) => {
                    let id = self.get_next_session_id();
                    let st_key = id;
                    let sessions = self.sessions.clone();
                    let log = self.log.clone();

                    let client = std::thread::spawn(move || {
                        handle_conn(conn, id, log);
                    });

                    if let Ok(mut session_threads) = self.session_threads.lock() {
                        session_threads.insert(st_key, client);
                    }
                }
                Err(err) => {
                    // the server is shutting down
                    if self.is_shutting_down {
                        break;
                    }
                    self.log.log_err(&format!("accept error: {}", err));

                    // interrupted system call, let's try again
                    if err.kind() == io::ErrorKind::Interrupted {
                        continue;
                    }

                    break;
                }
            }
        }

        Ok(())
    }

    pub fn shutdown(&self) {
        println!("shutting down the server...");
        std::process::exit(0);
    }

    pub fn reload(&self) {
        println!("reloading configuration file and restarting the server....");
        std::process::exit(0);
    }

    fn get_next_session_id(&self) -> usize {
        self.next_session_id.fetch_add(1, Ordering::SeqCst);
        self.next_session_id.load(Ordering::SeqCst)
    }
}

fn handle_conn(mut conn: TcpStream, session_id: usize, log: Arc<Log>) {
    // send welcome message
    if let Err(err) = send_response(&mut conn, log.clone(), StatusCode::ServiceReady, "") {
        log.log_err(&format!("error writing response: {}", err));
        return;
    }

    // start the session
    let mut session = Session::new(session_id);
    session.start();

    // TODO: figure out a way to have the server in the main thread
    //       send a message to this session, that it should end for whatever reason
    //       either for shutting down, or for restarting

    // TODO: client finished normally (not from a shutdown)
    // remove this session from the map of sessions
}

fn send_response(
    conn: &mut TcpStream,
    log: Arc<Log>,
    status_code: StatusCode,
    extra: &str,
) -> Result<(), ServerError> {
    let code_msg = format!("{}", status_code);
    let mut resp_msg = format!("{} {}", status_code.code(), code_msg);

    if extra.len() > 0 {
        resp_msg.push(' ');
        resp_msg.push_str(extra);
    }

    log.log_acc(&resp_msg);

    resp_msg.push('\n');

    if let Err(err) = conn.write(resp_msg.as_bytes()) {
        return Err(ServerError::ResponseFailed);
    }

    Ok(())
}

pub enum ServerError {
    HostnameNotResolved(std::io::ErrorKind),
    BindFailed(std::io::ErrorKind),
    ResponseFailed,
}

impl fmt::Display for ServerError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match *self {
            ServerError::HostnameNotResolved(err) => {
                write!(f, "the hostname could not be resolved: {}", err)
            }
            ServerError::BindFailed(err) => {
                write!(f, "could not bind the resolved address: {}", err)
            }
            ServerError::ResponseFailed => {
                write!(f, "could not send the response")
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn generate_next_session_id() {
        // let mut conf = Conf::new();
        // let mut log = Log::new("foo/bar", &conf, false).unwrap();
        // let mut server = Server::new(conf, log);

        // assert_eq!(server.next_session_id, 0);
    }
}
