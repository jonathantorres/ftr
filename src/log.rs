extern crate chrono;

use self::chrono::Local;
use ftr::Conf;
use std::fs::{File, OpenOptions};
use std::io::Write;

pub struct Log {
    log_stderr: bool,
    acc_log_handle: File,
    err_log_handle: File,
}

impl Log {
    pub fn new(prefix: &str, conf: &Conf, log_stderr: bool) -> std::io::Result<Self> {
        let prefix = String::from(prefix);
        let err_log_path = &conf.error_log;
        let acc_log_path = &conf.access_log;

        let err_file_handle = OpenOptions::new()
            .append(true)
            .create(true)
            .open(prefix.clone() + err_log_path)?;

        let acc_file_handle = OpenOptions::new()
            .append(true)
            .create(true)
            .open(prefix + acc_log_path)?;

        Ok(Log {
            log_stderr: log_stderr,
            acc_log_handle: acc_file_handle,
            err_log_handle: err_file_handle,
        })
    }

    pub fn log_err(&mut self, msg: &str) {
        let cur_msg = self.log_msg(msg);
        let _ = self.err_log_handle.write(cur_msg.as_bytes());

        if self.log_stderr {
            eprint!("{}", cur_msg);
        }
    }

    pub fn log_acc(&mut self, msg: &str) {
        let cur_msg = self.log_msg(msg);
        let _ = self.acc_log_handle.write(cur_msg.as_bytes());

        if self.log_stderr {
            eprint!("{}", cur_msg);
        }
    }

    fn log_msg(&self, msg: &str) -> String {
        let now = Local::now();
        let mut s = format!("[{}] ", now.format("%Y/%m/%d %H:%M:%S"));
        s.push_str(msg);
        s.push('\n');
        s
    }
}
