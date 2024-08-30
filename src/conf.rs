use std::fmt;
use std::fs::File;
use std::io::{BufRead, BufReader};

const EQUAL_SIGN: char = '=';
const OPEN_BRACKET: char = '{';
const CLOSE_BRACKET: char = '}';
const COMMENT_SIGN: char = '#';

const SERVER_OPT: &str = "server";
const ROOT_OPT: &str = "root";
const PORT_OPT: &str = "port";
const USER_OPT: &str = "user";
const USERNAME_OPT: &str = "username";
const PASSWORD_OPT: &str = "password";
const ERROR_LOG_OPT: &str = "error_log";
const ACCESS_LOG_OPT: &str = "access_log";
const INCLUDE_OPT: &str = "include";

struct User {
    username: String,
    password: String,
    root: String,
}

impl User {
    fn new() -> Self {
        User {
            username: String::new(),
            password: String::new(),
            root: String::new(),
        }
    }

    fn add_option(&mut self, op_name: &str, op_value: &str) -> Result<(), ConfError> {
        if op_name == USERNAME_OPT {
            self.username.clear();
            self.username.push_str(op_value);
        } else if op_name == PASSWORD_OPT {
            self.password.clear();
            self.password.push_str(op_value);
        } else if op_name == ROOT_OPT {
            self.root.clear();
            self.root.push_str(op_value);
        } else {
            return Err(ConfError::InvalidOption);
        }

        Ok(())
    }
}

pub struct Conf {
    pub error_log: String,
    pub access_log: String,
    pub server_name: String,
    pub port: i32,
    root: String,
    users: Vec<User>,
}

impl Conf {
    pub fn new() -> Self {
        Conf {
            server_name: String::new(),
            root: String::new(),
            error_log: String::new(),
            access_log: String::new(),
            port: 0,
            users: Vec::new(),
        }
    }

    pub fn load(&mut self, path: &str) -> Result<(), ConfError> {
        let conf_file_lines = Self::open_and_strip_comments(path)?;
        self.build(conf_file_lines)?;
        Ok(())
    }

    fn build(&mut self, conf_vec: Vec<String>) -> Result<(), ConfError> {
        if conf_vec.len() == 0 {
            return Err(ConfError::Empty);
        }

        let mut inside_usr_cmd = false;
        let mut cur_usr: Option<User> = None;

        for line in conf_vec {
            if line.contains(EQUAL_SIGN) {
                // this is a line with an option
                let mut pieces = line.split(EQUAL_SIGN);
                let op_name = match pieces.nth(0) {
                    Some(n) => n,
                    None => {
                        return Err(ConfError::InvalidOption);
                    }
                };
                let op_value = match pieces.nth(0) {
                    Some(v) => v,
                    None => {
                        return Err(ConfError::InvalidOption);
                    }
                };
                if inside_usr_cmd {
                    // option for the current user
                    if let Some(mut usr) = cur_usr {
                        if let Err(err) = usr.add_option(op_name.trim(), op_value.trim()) {
                            return Err(err);
                        }
                        cur_usr = Some(usr);
                    }
                } else {
                    if let Err(err) = self.add_option(op_name.trim(), op_value.trim()) {
                        return Err(err);
                    }
                }
            } else if line.contains(USER_OPT) {
                // this is a line with a user command
                inside_usr_cmd = true;
                cur_usr = Some(User::new());
            } else if line.contains(CLOSE_BRACKET) {
                // closing bracket for a user command
                if inside_usr_cmd {
                    if let Some(usr) = cur_usr {
                        self.users.push(usr);
                    }
                    inside_usr_cmd = false;
                    cur_usr = None;
                }
            }
        }

        Ok(())
    }

    fn add_option(&mut self, op_name: &str, op_value: &str) -> Result<(), ConfError> {
        if op_name == SERVER_OPT {
            self.server_name.clear();
            self.server_name.push_str(op_value);
        } else if op_name == ROOT_OPT {
            self.root.clear();
            self.root.push_str(op_value);
        } else if op_name == ERROR_LOG_OPT {
            self.error_log.clear();
            self.error_log.push_str(op_value);
        } else if op_name == ACCESS_LOG_OPT {
            self.access_log.clear();
            self.access_log.push_str(op_value);
        } else if op_name == PORT_OPT {
            if let Ok(p) = op_value.parse::<i32>() {
                self.port = p;
            } else {
                return Err(ConfError::ZeroPort);
            }
        } else {
            return Err(ConfError::InvalidOption);
        }

        Ok(())
    }

    fn open_and_strip_comments(path: &str) -> Result<Vec<String>, ConfError> {
        let file = match File::open(path) {
            Ok(f) => f,
            Err(err) => return Err(ConfError::NotOpen(err.kind())),
        };
        let mut file = BufReader::new(file);
        let mut conf_file_lines = Vec::new();
        let mut line = String::new();

        loop {
            line.clear();
            match file.read_line(&mut line) {
                Ok(bytes_read) => {
                    if bytes_read == 0 {
                        break;
                    } else {
                        let mut tline = line.trim();

                        // ignore line if it starts with a comment
                        if tline.len() == 0 || line_is_comment(tline) {
                            continue;
                        }

                        tline = strip_comment_from_line(tline);
                        conf_file_lines.push(String::from(tline));
                    }
                }
                Err(err) => return Err(ConfError::NotRead(err.kind())),
            }
        }
        Ok(conf_file_lines)
    }

    fn parse_includes(conf_vec: &Vec<String>) -> Vec<String> {
        todo!();
    }

    fn check_for_syntax_errors(conf_vec: &Vec<String>) {
        todo!();
    }
}

pub enum ConfError {
    NotOpen(std::io::ErrorKind),
    NotRead(std::io::ErrorKind),
    Empty,
    ZeroPort,
    InvalidOption,
}

impl fmt::Display for ConfError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match *self {
            ConfError::NotOpen(err_kind) => {
                write!(f, "could not open the configuration file: {}", err_kind)
            }
            ConfError::NotRead(err_kind) => {
                write!(
                    f,
                    "could not read data from the configuration file: {}",
                    err_kind
                )
            }
            ConfError::Empty => write!(f, "the configuration file cannot be empty"),
            ConfError::ZeroPort => write!(f, "the port number cannot be zero"),
            ConfError::InvalidOption => write!(f, "an invalid option was found"),
        }
    }
}

fn line_is_comment(line: &str) -> bool {
    if let Some(c) = line.trim().chars().nth(0) {
        if c == COMMENT_SIGN {
            return true;
        }
    }
    false
}

fn strip_comment_from_line<'a>(line: &'a str) -> &'a str {
    if let Some(i) = line.find(COMMENT_SIGN) {
        if let Some(sub) = line.get(0..i) {
            return sub.trim();
        }
    }
    line
}

#[cfg(test)]
mod tests {
    use conf::*;

    #[test]
    fn test_build_conf_file() {
        let mut c = Conf::new();
        let r = c.load("./ftrd.conf");
        let user1 = &c.users[0];
        let user2 = &c.users[1];

        assert!(r.is_ok());
        assert_eq!(c.server_name, "localhost");
        assert_eq!(c.root, "/home/jonathan/ftrd_test");
        assert_eq!(c.error_log, "errors.log");
        assert_eq!(c.access_log, "access.log");
        assert_eq!(c.port, 9090);
        assert_eq!(c.users.len(), 2);
        assert_eq!(user1.username, "jt");
        assert_eq!(user1.password, "test");
        assert_eq!(user1.root, "/jt");
        assert_eq!(user2.username, "test");
        assert_eq!(user2.password, "test");
        assert_eq!(user2.root, "/test");
    }

    #[test]
    fn test_add_option() {
        let mut c = Conf::new();
        let server_opt = c.add_option("server", "foo");
        let root_opt = c.add_option("root", "bar");
        let err_log_opt = c.add_option("error_log", "baz");
        let acc_log_opt = c.add_option("access_log", "access");
        let port_opt = c.add_option("port", "9999");
        let bad_opt = c.add_option("bad", "more bad");

        assert!(server_opt.is_ok());
        assert!(root_opt.is_ok());
        assert!(err_log_opt.is_ok());
        assert!(acc_log_opt.is_ok());
        assert!(port_opt.is_ok());
        assert!(bad_opt.is_err());

        assert_eq!(c.server_name, "foo");
        assert_eq!(c.root, "bar");
        assert_eq!(c.error_log, "baz");
        assert_eq!(c.access_log, "access");
        assert_eq!(c.port, 9999);
    }

    #[test]
    fn test_user_add_option() {
        let mut u = User::new();
        let user_opt = u.add_option("username", "jt");
        let pass_opt = u.add_option("password", "1234");
        let root_opt = u.add_option("root", "/here");
        let bad_opt = u.add_option("dunno", "something");

        assert!(user_opt.is_ok());
        assert!(pass_opt.is_ok());
        assert!(root_opt.is_ok());
        assert!(bad_opt.is_err());

        assert_eq!(u.username, "jt");
        assert_eq!(u.password, "1234");
        assert_eq!(u.root, "/here");
    }

    #[test]
    fn test_strip_comment_from_line() {
        let line = strip_comment_from_line("this is a conf file");
        assert_eq!(line, "this is a conf file");

        let line = strip_comment_from_line("#this is a comment");
        assert_eq!(line, "");

        let line = strip_comment_from_line("foo = bar #this is a comment");
        assert_eq!(line, "foo = bar");

        let line = strip_comment_from_line("foo = bar # this is a comment");
        assert_eq!(line, "foo = bar");
    }

    #[test]
    fn test_line_is_comment() {
        let is_comment = line_is_comment("foo = bar # comment here");
        assert!(!is_comment);

        let is_comment = line_is_comment("# this is a comment");
        assert!(is_comment);

        let is_comment = line_is_comment("  # this is a comment");
        assert!(is_comment);
    }
}
