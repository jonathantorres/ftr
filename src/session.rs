use std::collections::HashMap;
use std::iter::IntoIterator;

pub struct Session {
    pub id: usize,
    user: SessionUser,
    control_conn_fd: i32,
    data_conn_fd: i32,
    data_conn_port: i32,
    passive_mode: bool,
    transfer_in_progress: bool,
    transfer_ready: bool,
    transfer_done: bool,

    // TODO: implement these, they are not going to be strings
    server: String,
    log: String,

    // TODO: these not so sure about
    cwd: String,
    transfer_type: String,
    rename_from: String,
}

impl Session {
    pub fn new(id: usize) -> Self {
        let user = SessionUser::new();
        Session {
            id: id,
            user: user,
            control_conn_fd: 0,
            data_conn_fd: 0,
            data_conn_port: 0,
            passive_mode: false,
            transfer_in_progress: false,
            transfer_ready: false,
            transfer_done: false,

            // TODO: finish these
            server: String::new(),
            log: String::new(),
            cwd: String::new(),
            transfer_type: String::new(),
            rename_from: String::new(),
        }
    }

    pub fn start(&mut self) {
        self.control_conn_fd = 1;
        loop {
            //
        }
    }

    pub fn end(&self) {
        todo!();
    }

    pub fn quit(&self) {
        todo!();
    }
}

pub struct Sessions {
    sessions: HashMap<usize, Session>,
}

impl Sessions {
    pub fn new() -> Self {
        Sessions {
            sessions: HashMap::new(),
        }
    }

    pub fn add_session(&mut self, session: Session) {
        self.sessions.insert(session.id, session);
    }

    pub fn get_session(&self, session_id: usize) -> Option<&Session> {
        self.sessions.get(&session_id)
    }

    pub fn get_session_mut(&mut self, session_id: usize) -> Option<&mut Session> {
        self.sessions.get_mut(&session_id)
    }
}

impl IntoIterator for Sessions {
    type Item = (usize, Session);
    type IntoIter = std::collections::hash_map::IntoIter<usize, Session>;

    fn into_iter(self) -> Self::IntoIter {
        self.sessions.into_iter()
    }
}

impl<'a> IntoIterator for &'a Sessions {
    type Item = (&'a usize, &'a Session);
    type IntoIter = std::collections::hash_map::Iter<'a, usize, Session>;

    fn into_iter(self) -> Self::IntoIter {
        self.sessions.iter()
    }
}

struct SessionUser {
    username: String,
    password: String,
    root: String,
}

impl SessionUser {
    pub fn new() -> Self {
        SessionUser {
            username: String::new(),
            password: String::new(),
            root: String::new(),
        }
    }
}
