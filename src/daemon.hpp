#ifndef daemon_hpp
#define daemon_hpp

#include <array>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <signal.h>
#include <string>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

namespace ftr {

inline void daemonize(const std::string cmd);
inline bool daemon_is_running(const std::string cmd,
                              const std::string lockfile_path);

inline void daemonize(const std::string cmd) {
    int i = 0;
    int fd0 = 0;
    int fd1 = 0;
    int fd2 = 0;
    pid_t pid = 0;
    struct rlimit rl = {};
    struct sigaction sa = {};

    // clear file creation mask
    umask(0);

    // get maximum number of file descriptors
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        std::cerr << cmd << ": can't get file limit: " << std::strerror(errno)
                  << '\n';
        std::exit(EXIT_FAILURE);
    }

    // become a session leader to lose controlling tty
    if ((pid = fork()) < 0) {
        std::cerr << cmd << ": can't fork: " << std::strerror(errno) << '\n';
        std::exit(EXIT_FAILURE);
    } else if (pid != 0) {
        // parent process
        std::exit(EXIT_SUCCESS);
    }

    setsid();

    // ensure future opens won't allocate controlling ttys
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGHUP, &sa, NULL) < 0) {
        std::cerr << cmd
                  << ": could not ignore SIGHUP: " << std::strerror(errno)
                  << '\n';
        std::exit(EXIT_FAILURE);
    }

    if ((pid = fork()) < 0) {
        std::cerr << cmd << ": can't fork: " << std::strerror(errno) << '\n';
        std::exit(EXIT_FAILURE);
    } else if (pid != 0) {
        // parent process
        std::exit(EXIT_SUCCESS);
    }

    // change the current working directory to the root so we won't prevent
    // file systems from being mounted
    if (chdir("/") < 0) {
        std::cerr << cmd << ": could not change directory to /: "
                  << std::strerror(errno) << '\n';
        std::exit(EXIT_FAILURE);
    }

    // close all open file descriptors
    if (rl.rlim_max == RLIM_INFINITY) {
        rl.rlim_max = 1024;
    }

    for (i = 0; i < static_cast<int>(rl.rlim_max); i++) {
        close(i);
    }

    // attach file descriptors 0, 1 and 2 to /dev/null
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    // initialize the log file
    openlog(cmd.c_str(), LOG_CONS, LOG_DAEMON);

    if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
        syslog(LOG_ERR, "unexpected file descriptors %d %d %d", fd0, fd1, fd2);
        std::exit(EXIT_FAILURE);
    }
}

inline bool daemon_is_running(const std::string cmd,
                              const std::string lockfile_path) {
    int fd = 0;
    std::array<char, 16> buf = {0};

    auto lockfile = [](const int fd) {
        struct flock fl = {};
        fl.l_type = F_WRLCK;
        fl.l_start = 0;
        fl.l_whence = SEEK_SET;
        fl.l_len = 0;

        return fcntl(fd, F_SETLK, &fl);
    };

    // initialize the log file
    openlog(cmd.c_str(), LOG_CONS, LOG_DAEMON);

    fd = open(lockfile_path.c_str(), O_RDWR | O_CREAT,
              S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if (fd < 0) {
        syslog(LOG_ERR, "can't open %s: %s", lockfile_path.c_str(),
               std::strerror(errno));
        std::exit(EXIT_FAILURE);
    }

    if (lockfile(fd) < 0) {
        if (errno == EACCES || errno == EAGAIN) {
            close(fd);
            return true;
        }

        syslog(LOG_ERR, "can't lock %s: %s", lockfile_path.c_str(),
               std::strerror(errno));
        std::exit(EXIT_FAILURE);
    }

    ftruncate(fd, 0);
    snprintf(buf.data(), buf.max_size(), "%ld\n", static_cast<long>(getpid()));
    write(fd, buf.data(), std::strlen(buf.data()));

    return false;
}
} // namespace ftr
#endif
