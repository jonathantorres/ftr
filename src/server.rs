use ftr::Conf;
use ftr::Log;
use std::collections::HashMap;
use std::fmt;
use std::io;
use std::io::Write;
use std::net::{TcpListener, TcpStream, ToSocketAddrs};
use std::os::unix::io::{AsRawFd, RawFd};

pub struct Server {
    host: String,
    resolved_host: String,
    conf: Conf,
    log: Log,
    sessions: HashMap<i32, String>, // change to use the session object
    session_threads: HashMap<i32, String>, // chage to use a thread object
    port: i32,
    ctrl_listener_fd: Option<RawFd>,
    next_session_id: usize,
    is_reloading: bool,
    is_shutting_down: bool,
}

enum HostType {
    DomainName,
    IPv4,
    IPv6,
    Invalid,
}

impl Server {
    pub fn new(conf: Conf, log: Log) -> Self {
        let conf_port = conf.port;
        Server {
            host: String::from(&conf.server_name),
            resolved_host: String::new(),
            conf: conf,
            log: log,
            sessions: HashMap::new(),
            session_threads: HashMap::new(),
            port: conf_port,
            ctrl_listener_fd: None,
            next_session_id: 0,
            is_reloading: false,
            is_shutting_down: false,
        }
    }

    pub fn start(&mut self) -> Result<(), ServerError> {
        self.log.log_acc("Server starting...");

        let addr = format!("{}:{}", self.host, self.port);
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

        self.resolved_host.push_str(&res_addr.ip().to_string());

        let listener = match TcpListener::bind(res_addr) {
            Ok(l) => l,
            Err(err) => return Err(ServerError::BindFailed(err.kind())),
        };

        self.log.log_acc("Server started OK.");
        self.ctrl_listener_fd = Some(listener.as_raw_fd());

        loop {
            match listener.accept() {
                Ok((conn, _remote_addr)) => {
                    self.next_session_id += 1;
                    let session_id = self.next_session_id;

                    let client = std::thread::spawn(move || {
                        handle_conn(conn, session_id);
                    });
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
        todo!();
    }

    pub fn reload(&self) {
        todo!();
    }

    pub fn send_response(&self) {
        todo!();
    }
}

fn handle_conn(mut conn: TcpStream, session_id: usize) {
    let m = format!("hi!, from thread that handles the conn => {}\n", session_id);
    let _ = conn.write(m.as_bytes());
}

pub enum ServerError {
    HostnameNotResolved(std::io::ErrorKind),
    BindFailed(std::io::ErrorKind),
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
        }
    }
}
