#include "session.hpp"
#include "conf.hpp"
#include "exception.hpp"
#include "file_data.hpp"
#include "server.hpp"
#include "string.hpp"
#include "util.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <array>
#include <cerrno>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>

using namespace ftr;

void Session::start() {
    while (true) {
        std::array<char, ftr::DEFAULT_CMD_SIZE> client_cmd = {0};

        int res = read(m_control_conn_fd, client_cmd.data(), client_cmd.size());

        if (res < 0) {
            // an error ocurred
            m_log->log_err("read error: ", std::strerror(errno));
            m_server.send_response(m_control_conn_fd,
                                   ftr::STATUS_CODE_UNKNOWN_ERROR, "");
            close(m_control_conn_fd);
            break;
        } else if (res == 0) {
            // connection closed
            m_log->log_err("connection finished by client");
            close(m_control_conn_fd);
            break;
        }

        try {
            handle_command(client_cmd);
        } catch (std::exception &e) {
            m_log->log_err("error handling the command", e.what());
            m_server.send_response(m_control_conn_fd,
                                   ftr::STATUS_CODE_UNKNOWN_ERROR, "");
            continue;
        }
    }
}

// end the current session, this will assume
// that the data connection is already closed
// and that there are no transfers in progress
void Session::end() {
    if (m_control_conn_fd > 0) {
        int res = shutdown(m_control_conn_fd, SHUT_WR);

        if (res < 0) {
            m_log->log_err("shutdown error on control connection: ",
                           std::strerror(errno));
        }
    }
}

void Session::open_data_conn(const struct sockaddr *conn_addr,
                             socklen_t addr_size) {
    int fd = socket(conn_addr->sa_family, SOCK_STREAM, 0);

    if (fd < 0) {
        m_log->log_err(std::strerror(errno));
        throw SessionError(std::strerror(errno));
    }

    int opt = 1;
    int res = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

    if (res < 0) {
        close(fd);
        m_log->log_err(std::strerror(errno));
        throw SessionError(std::strerror(errno));
    }

    res = bind(fd, conn_addr, addr_size);

    if (res < 0) {
        close(fd);
        m_log->log_err("bind error: ", std::strerror(errno));
        throw SessionError(std::strerror(errno));
    }

    res = listen(fd, ftr::BACKLOG);

    if (res < 0) {
        close(fd);
        m_log->log_err("listen failure: ", std::strerror(errno));
        throw SessionError(std::strerror(errno));
    }

    // start accepting connections
    std::thread handle(&Session::accept_on_data_conn, this, fd);
    handle.detach();
}

void Session::accept_on_data_conn(int listener_fd) {
    int conn_fd = accept(listener_fd, nullptr, nullptr);

    if (conn_fd < 0) {
        close(listener_fd);
        m_log->log_err("accept error on date connection: ",
                       std::strerror(errno));
        return;
    }

    m_data_conn_fd = conn_fd;

    // the connection is now ready to be used
    m_transfer_ready = true;
    m_session_cv.notify_one();

    // wait until the thread that is doing the transfer
    // is done, and close the connection
    std::unique_lock conn_lock(m_session_mu);
    m_session_cv.wait(conn_lock, [&] { return m_transfer_done; });

    close(m_data_conn_fd);
    close(listener_fd);

    listener_fd = -1;
    m_data_conn_fd = -1;
    m_transfer_ready = false;
    m_transfer_done = false;
}

void Session::connect_to_data_conn(const unsigned int port, bool use_ipv6) {
    int res = 0;
    std::string resolved_host = m_server.get_resolved_host();
    sa_family_t fam = AF_INET;
    socklen_t addr_size = 0;
    struct sockaddr *conn_addr = nullptr;
    struct sockaddr_in6 addr6 {};
    struct sockaddr_in addr4 {};

    if (use_ipv6) {
        // IPv6 address
        fam = AF_INET6;
        conn_addr = reinterpret_cast<struct sockaddr *>(&addr6);

        addr6.sin6_family = AF_INET6;
        addr6.sin6_port = htons(port);
        addr_size = sizeof(addr6);

        res = inet_pton(fam, resolved_host.c_str(), &addr6.sin6_addr);
    } else {
        // IPv4 address
        fam = AF_INET;
        conn_addr = reinterpret_cast<struct sockaddr *>(&addr4);

        addr4.sin_family = AF_INET;
        addr4.sin_port = htons(port);
        addr_size = sizeof(addr4);

        res = inet_pton(fam, resolved_host.c_str(), &addr4.sin_addr);
    }

    if (res == 0) {
        throw SessionError("The network address is invalid");
    } else if (res < 0) {
        m_log->log_err(std::strerror(errno));
        throw SessionError(std::strerror(errno));
    }

    int conn_fd = socket(fam, SOCK_STREAM, 0);

    if (conn_fd < 0) {
        m_log->log_err(std::strerror(errno));
        throw SessionError(std::strerror(errno));
    }

    res = connect(conn_fd, conn_addr, addr_size);

    if (res < 0) {
        m_log->log_err(std::strerror(errno));
        throw SessionError(std::strerror(errno));
    }

    m_data_conn_fd = conn_fd;
    m_data_conn_port = port;

    // create new thread that will handle the transfer
    std::thread handle(&Session::transfer_on_data_conn, this);
    handle.detach();
}

void Session::transfer_on_data_conn() {
    // the connection is now ready to be used
    m_transfer_ready = true;
    m_session_cv.notify_one();

    // wait until the thread that is doing the transfer
    // is done, and close the connection
    std::unique_lock conn_lock(m_session_mu);
    m_session_cv.wait(conn_lock, [&] { return m_transfer_done; });

    close(m_data_conn_fd);

    m_data_conn_fd = -1;
    m_data_conn_port = 0;
    m_transfer_ready = false;
    m_transfer_done = false;
}

void Session::handle_command(
    const std::array<char, ftr::DEFAULT_CMD_SIZE> &client_cmd) {
    std::string cmd_string =
        std::string(reinterpret_cast<const char *>(client_cmd.data()));
    cmd_string = ftr::trim_whitespace(cmd_string);
    std::string cmd;
    std::string cmd_params;
    bool found_first_space = false;

    for (auto &c : cmd_string) {
        if (found_first_space == false && c == ' ') {
            found_first_space = true;
            continue;
        }

        if (found_first_space) {
            cmd_params += c;
        } else {
            cmd += c;
        }
    }

    if (cmd_params == "") {
        exec_command(cmd, "");
        return;
    }

    exec_command(cmd, cmd_params);
}

void Session::exec_command(const std::string &cmd,
                           const std::string &cmd_params) {
    m_log->log_acc(cmd + " ", cmd_params);

    if (cmd == CMD_USER) {
        run_user(cmd_params);
        return;
    } else if (cmd == CMD_PASSWORD) {
        run_password(cmd_params);
        return;
    } else if (cmd == CMD_PRINT_DIR) {
        run_print_dir();
        return;
    } else if (cmd == CMD_CHANGE_DIR) {
        run_change_dir(cmd_params);
        return;
    } else if (cmd == CMD_TYPE) {
        run_type(cmd_params);
        return;
    } else if (cmd == CMD_PASSIVE) {
        run_passive();
        return;
    } else if (cmd == CMD_LIST) {
        run_list(cmd_params);
        return;
    } else if (cmd == CMD_FILE_NAMES) {
        run_file_names(cmd_params);
        return;
    } else if (cmd == CMD_RETRIEVE) {
        run_retrieve(cmd_params);
        return;
    } else if (cmd == CMD_ACCEPT_AND_STORE || cmd == CMD_STORE_FILE) {
        run_accept_and_store(cmd_params, false);
        return;
    } else if (cmd == CMD_APPEND) {
        run_accept_and_store(cmd_params, true);
        return;
    } else if (cmd == CMD_SYSTEM_TYPE) {
        run_system_type();
        return;
    } else if (cmd == CMD_CHANGE_PARENT || cmd == CMD_CHANGE_TO_PARENT_DIR) {
        run_change_parent();
        return;
    } else if (cmd == CMD_MAKE_DIR || cmd == CMD_MAKE_A_DIR) {
        run_make_dir(cmd_params);
        return;
    } else if (cmd == CMD_REMOVE_DIR) {
        run_remove_dir(cmd_params);
        return;
    } else if (cmd == CMD_DELETE) {
        run_delete(cmd_params);
        return;
    } else if (cmd == CMD_EXT_PASSV_MODE) {
        run_ext_passv_mode(cmd_params);
        return;
    } else if (cmd == CMD_PORT) {
        run_port(cmd_params);
        return;
    } else if (cmd == CMD_EXT_ADDR_PORT) {
        run_ext_addr_port(cmd_params);
        return;
    } else if (cmd == CMD_HELP) {
        run_help(cmd_params);
        return;
    } else if (cmd == CMD_NOOP) {
        run_noop();
        return;
    } else if (cmd == CMD_ALLO) {
        run_allo();
        return;
    } else if (cmd == CMD_ACCOUNT) {
        run_account(cmd_params);
        return;
    } else if (cmd == CMD_SITE) {
        run_site();
        return;
    } else if (cmd == CMD_MODE) {
        run_mode(cmd_params);
        return;
    } else if (cmd == CMD_ABORT) {
        run_abort();
        return;
    } else if (cmd == CMD_FILE_STRUCT) {
        run_file_struct(cmd_params);
        return;
    } else if (cmd == CMD_SERVER_STATUS) {
        run_server_status(cmd_params);
        return;
    } else if (cmd == CMD_RENAME_FROM) {
        run_rename_from(cmd_params);
        return;
    } else if (cmd == CMD_RENAME_TO) {
        run_rename_to(cmd_params);
        return;
    } else if (cmd == CMD_REINIT) {
        run_reinit();
        return;
    } else if (cmd == CMD_QUIT) {
        run_quit();
        return;
    }

    run_not_implemented();
}

void Session::run_user(const std::string &username) {
    bool user_found = false;
    const std::vector<std::shared_ptr<ftr::User>> users =
        m_server.get_conf()->get_users();

    for (auto &u : users) {
        if (u->get_username() == username) {
            user_found = true;
            SessionUser new_session_user = {};
            new_session_user.username = std::string(username);
            new_session_user.password = "";
            new_session_user.root = "";
            m_session_user = new_session_user;
            break;
        }
    }

    if (user_found) {
        m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_USERNAME_OK,
                               "");
        return;
    }

    m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_INVALID_USERNAME,
                           "");
}

void Session::run_password(const std::string &password) {
    bool pass_found = false;
    const std::shared_ptr<ftr::Conf> conf = m_server.get_conf();
    const std::vector<std::shared_ptr<ftr::User>> users = conf->get_users();

    for (auto &u : users) {
        if (u->get_username() == m_session_user.username &&
            u->get_password() == password) {
            pass_found = true;

            m_session_user.password = std::string(password);
            m_session_user.root = std::string(u->get_root());
            break;
        }
    }

    if (pass_found) {
        // change to home directory
        std::string path(conf->get_root() + m_session_user.root);
        int res = chdir(path.c_str());

        if (res < 0) {
            m_log->log_err("chdir error: ", std::strerror(errno));
            m_server.send_response(m_control_conn_fd,
                                   ftr::STATUS_CODE_FILE_NOT_FOUND, "");
            return;
        }

        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_USER_LOGGED_IN, "");
        return;
    }

    m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_INVALID_USERNAME,
                           "");
}

void Session::run_print_dir() {
    m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_PATH_CREATED,
                           "\"/" + m_cwd + "\" is current directory");
}

void Session::run_change_dir(const std::string dir) {
    if (!is_logged_in()) {
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_NOT_LOGGED_IN, "");
        return;
    }

    std::string cur_wd = m_cwd;

    if (dir[0] == '/') {
        // moving to relative path
        cur_wd = std::string(dir, 1, dir.size() - 1);
    } else {
        if (cur_wd != "") {
            cur_wd += "/" + dir;
        } else {
            cur_wd = dir;
        }
    }

    const std::shared_ptr<ftr::Conf> conf = m_server.get_conf();
    std::string path(conf->get_root() + m_session_user.root + "/" + cur_wd);
    int res = chdir(path.c_str());

    if (res < 0) {
        m_log->log_err("chdir error: ", std::strerror(errno));
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_FILE_NOT_FOUND, "");
        return;
    }

    m_cwd = cur_wd;
    m_server.send_response(m_control_conn_fd,
                           ftr::STATUS_CODE_REQUESTED_FILE_OK,
                           "\"" + dir + "\" is current directory");
}

void Session::run_type(const std::string selected_transfer_type) {
    if (selected_transfer_type == ftr::TRANSFER_TYPE_ASCII ||
        selected_transfer_type == TRANSFER_TYPE_IMG) {
        m_transfer_type = selected_transfer_type;
        m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_OK,
                               "Transfer Type OK");
        return;
    }

    run_not_implemented();
}

void Session::run_passive() {
    int fd = -1;

    try {
        fd = m_server.find_open_addr(false);
    } catch (std::exception &e) {
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_CANT_OPEN_DATA_CONN, e.what());
        return;
    }

    struct sockaddr addr {};
    socklen_t addr_size = sizeof(addr);
    int name_res = getsockname(fd, &addr, &addr_size);

    close(fd);

    if (name_res < 0) {
        throw ServerError(strerror(errno));
    }

    std::string addr_str = m_server.get_addr_string(&addr);

    for (auto &c : addr_str) {
        if (c == '.') {
            c = ',';
        }
    }

    struct sockaddr_in *in_addr = reinterpret_cast<sockaddr_in *>(&addr);

    uint16_t p = ntohs(in_addr->sin_port);
    uint8_t p1 = (p >> 8);
    uint8_t p2 = p;

    std::stringstream resp_msg;
    resp_msg << addr_str;
    resp_msg << ',';
    resp_msg << std::to_string(p1);
    resp_msg << ',';
    resp_msg << std::to_string(p2);

    try {
        open_data_conn(&addr, addr_size);
    } catch (std::exception &e) {
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_CANT_OPEN_DATA_CONN, e.what());
        return;
    }

    m_pass_mode = true;

    m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_ENTER_PASS_MODE,
                           resp_msg.str());
}

void Session::run_list(const std::string file) {
    if (!is_logged_in()) {
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_NOT_LOGGED_IN, "");
        return;
    }

    // wait until the data connection is ready for sending/receiving data
    std::unique_lock list_lock(m_session_mu);
    m_session_cv.wait(list_lock, [&] { return m_transfer_ready; });
    list_lock.unlock();

    m_transfer_in_progress = true;

    auto conf = m_server.get_conf();
    std::string path_str(conf->get_root() + m_session_user.root + "/" + m_cwd);

    if (file != "") {
        path_str += "/" + file;
    }

    std::filesystem::path dir_path(path_str);
    std::stringstream dir_data;

    try {
        std::filesystem::directory_iterator dir_iter(dir_path);

        for (auto const &entry : dir_iter) {
            FileData file_data(entry, m_log);
            std::string file_line = file_data.get_file_line();
            dir_data << file_line;
            dir_data << '\n';
        }
    } catch (std::exception &e) {
        m_transfer_in_progress = false;
        m_transfer_done = true;
        m_session_cv.notify_one();

        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_FILE_ACTION_NOT_TAKEN,
                               e.what());

        return;
    }

    std::string dir_data_str = dir_data.str();
    if (write(m_data_conn_fd, dir_data_str.c_str(), dir_data_str.size()) < 0) {
        m_log->log_err("write error: ", std::strerror(errno));
        m_transfer_in_progress = false;
        m_transfer_done = true;
        m_session_cv.notify_one();

        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_FILE_ACTION_NOT_TAKEN,
                               strerror(errno));

        return;
    }

    // send notification that the operation has finished
    m_transfer_in_progress = false;
    m_transfer_done = true;
    m_session_cv.notify_one();

    m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_OK, "");
}

void Session::run_file_names(const std::string file) {
    if (!is_logged_in()) {
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_NOT_LOGGED_IN, "");
        return;
    }

    // wait until the data connection is ready for sending/receiving data
    std::unique_lock list_lock(m_session_mu);
    m_session_cv.wait(list_lock, [&] { return m_transfer_ready; });
    list_lock.unlock();

    m_transfer_in_progress = true;

    auto conf = m_server.get_conf();
    std::string path_str(conf->get_root() + m_session_user.root + "/" + m_cwd);

    if (file != "") {
        path_str += "/" + file;
    }

    std::filesystem::path dir_path(path_str);
    std::stringstream dir_data;

    try {
        std::filesystem::directory_iterator dir_iter(dir_path);

        for (auto const &entry : dir_iter) {
            dir_data << entry.path().filename().string();
            dir_data << '\n';
        }
    } catch (std::exception &e) {
        m_transfer_in_progress = false;
        m_transfer_done = true;
        m_session_cv.notify_one();

        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_FILE_ACTION_NOT_TAKEN,
                               e.what());

        return;
    }

    std::string dir_data_str = dir_data.str();
    if (write(m_data_conn_fd, dir_data_str.c_str(), dir_data_str.size()) < 0) {
        m_log->log_err("write error: ", std::strerror(errno));
        m_transfer_in_progress = false;
        m_transfer_done = true;
        m_session_cv.notify_one();

        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_FILE_ACTION_NOT_TAKEN,
                               std::strerror(errno));

        return;
    }

    // send notification that the operation has finished
    m_transfer_in_progress = false;
    m_transfer_done = true;
    m_session_cv.notify_one();

    m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_OK, "");
}

void Session::run_retrieve(const std::string filename) {
    if (!is_logged_in()) {
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_NOT_LOGGED_IN, "");
        return;
    }

    // wait until the data connection is ready for sending/receiving data
    std::unique_lock list_lock(m_session_mu);
    m_session_cv.wait(list_lock, [&] { return m_transfer_ready; });
    list_lock.unlock();

    m_transfer_in_progress = true;

    auto conf = m_server.get_conf();
    std::string path_str(conf->get_root() + m_session_user.root + "/" + m_cwd +
                         "/" + filename);

    std::ifstream file(path_str);

    if (!file.is_open()) {
        m_transfer_in_progress = false;
        m_transfer_done = true;
        m_session_cv.notify_one();

        m_log->log_err(std::strerror(errno));

        throw SessionError(strerror(errno));
    }

    while (true) {
        std::array<char, ftr::FILE_BUF> buf = {0};
        int bytes_read = buf.size();

        file.read(buf.data(), buf.size());

        if (file.eof()) {
            // we are done reading from the file
            bytes_read = file.gcount();
        } else if (file.fail() || file.bad()) {
            // an error ocurred
            m_log->log_err("an error reading the file occurred ",
                           std::strerror(errno));
            m_transfer_in_progress = false;
            m_transfer_done = true;
            m_session_cv.notify_one();

            m_server.send_response(m_control_conn_fd,
                                   ftr::STATUS_CODE_FILE_ACTION_NOT_TAKEN, "");
            return;
        }

        if (write(m_data_conn_fd, buf.data(), bytes_read) < 0) {
            m_log->log_err("write error: ", std::strerror(errno));
            m_transfer_in_progress = false;
            m_transfer_done = true;
            m_session_cv.notify_one();

            m_server.send_response(m_control_conn_fd,
                                   ftr::STATUS_CODE_FILE_ACTION_NOT_TAKEN,
                                   strerror(errno));
            return;
        }

        if (file.eof()) {
            break;
        }
    }

    // send notification that the operation has finished
    m_transfer_in_progress = false;
    m_transfer_done = true;
    m_session_cv.notify_one();

    m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_OK, "");
}

void Session::run_accept_and_store(const std::string filename,
                                   bool append_mode) {
    if (!is_logged_in()) {
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_NOT_LOGGED_IN, "");
        return;
    }

    // wait until the data connection is ready for sending/receiving data
    std::unique_lock list_lock(m_session_mu);
    m_session_cv.wait(list_lock, [&] { return m_transfer_ready; });
    list_lock.unlock();

    m_transfer_in_progress = true;

    auto conf = m_server.get_conf();
    std::string path_str(conf->get_root() + m_session_user.root + "/" + m_cwd +
                         "/" + filename);

    std::ios_base::openmode mode = std::ios_base::out;

    if (append_mode) {
        mode |= std::ios_base::ate;
    }

    std::ofstream new_file(path_str, mode);

    if (!new_file.is_open()) {
        m_transfer_in_progress = false;
        m_transfer_done = true;
        m_session_cv.notify_one();

        m_log->log_err(std::strerror(errno));

        throw SessionError(std::strerror(errno));
    }

    while (true) {
        std::array<char, ftr::FILE_BUF> buf = {0};

        int res = read(m_data_conn_fd, buf.data(), buf.size());

        if (res < 0) {
            // an error ocurred
            m_log->log_err("read error: ", std::strerror(errno));
            m_transfer_in_progress = false;
            m_transfer_done = true;
            m_session_cv.notify_one();

            m_server.send_response(m_control_conn_fd,
                                   ftr::STATUS_CODE_FILE_ACTION_NOT_TAKEN, "");
            return;
        } else if (res == 0) {
            // connection closed, file was sent completely
            new_file.close();
            break;
        }

        new_file.write(buf.data(), res);

        if (!new_file.good()) {
            m_transfer_in_progress = false;
            m_transfer_done = true;
            m_session_cv.notify_one();

            m_log->log_err(std::strerror(errno));

            throw SessionError("There was a problem writing to the file");
        }
    }

    // send notification that the operation has finished
    m_transfer_in_progress = false;
    m_transfer_done = true;
    m_session_cv.notify_one();

    m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_OK, "");
}

void Session::run_system_type() {
    m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_NAME_SYSTEM,
                           "UNIX Type: L8");
}

void Session::run_change_parent() {
    if (!is_logged_in()) {
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_NOT_LOGGED_IN, "");
        return;
    }

    std::string cur_wd = m_cwd;
    std::vector<std::string> pieces = ftr::split(m_cwd, "/");

    if (pieces.size() <= 1) {
        cur_wd = "";
    } else {
        pieces.pop_back();

        cur_wd = ftr::join(pieces, "/");
    }

    const std::shared_ptr<ftr::Conf> conf = m_server.get_conf();
    std::string chdir_path =
        conf->get_root() + m_session_user.root + "/" + cur_wd.c_str();
    int res = chdir(chdir_path.c_str());

    if (res < 0) {
        m_log->log_err("chdir error: ", std::strerror(errno));
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_FILE_NOT_FOUND, "");
        return;
    }

    m_cwd = cur_wd;
    auto wd_path = std::filesystem::path(cur_wd);
    std::string base = wd_path.stem().c_str();

    m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_OK,
                           "\"" + base + "\" is current directory");
}

void Session::run_make_dir(const std::string dir_name) {
    if (!is_logged_in()) {
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_NOT_LOGGED_IN, "");
        return;
    }

    std::string cur_wd = m_cwd;
    const std::shared_ptr<ftr::Conf> conf = m_server.get_conf();
    std::filesystem::path location(conf->get_root() + m_session_user.root +
                                   "/" + cur_wd + "/" + dir_name);

    try {
        std::filesystem::create_directory(location);
    } catch (std::exception &e) {
        m_log->log_err("create directory error: ", std::strerror(errno));
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_FILE_NOT_FOUND, e.what());
        return;
    }

    m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_OK,
                           "Directory " + dir_name + " created");
}

void Session::run_remove_dir(const std::string path) {
    if (!is_logged_in()) {
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_NOT_LOGGED_IN, "");
        return;
    }

    std::string cur_wd = m_cwd;
    const std::shared_ptr<ftr::Conf> conf = m_server.get_conf();
    std::filesystem::path location(conf->get_root() + m_session_user.root +
                                   "/" + cur_wd + "/" + path);

    try {
        std::filesystem::remove_all(location);
    } catch (std::exception &e) {
        m_log->log_err("remove all error: ", std::strerror(errno));
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_FILE_NOT_FOUND, e.what());
        return;
    }

    m_server.send_response(m_control_conn_fd,
                           ftr::STATUS_CODE_REQUESTED_FILE_OK,
                           "Directory " + path + " removed");
}

void Session::run_delete(const std::string filename) {
    if (!is_logged_in()) {
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_NOT_LOGGED_IN, "");
        return;
    }

    std::string cur_wd = m_cwd;
    const std::shared_ptr<ftr::Conf> conf = m_server.get_conf();
    std::filesystem::path location(conf->get_root() + m_session_user.root +
                                   "/" + cur_wd + "/" + filename);

    try {
        std::filesystem::remove(location);
    } catch (std::exception &e) {
        m_log->log_err("remove error: ", std::strerror(errno));
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_FILE_NOT_FOUND, e.what());
        return;
    }

    m_server.send_response(m_control_conn_fd,
                           ftr::STATUS_CODE_REQUESTED_FILE_OK,
                           "File " + filename + " deleted");
}

void Session::run_ext_passv_mode(const std::string &cmd_params) {
    int fd = -1;

    if (cmd_params != "") {
        if (cmd_params == "1") {
            // only IPv6 allowed
            m_server.send_response(m_control_conn_fd,
                                   ftr::STATUS_CODE_EXT_PORT_UNKNOWN_PROTOCOL,
                                   "");
            return;
        }

        run_not_implemented();
        return;
    }

    try {
        fd = m_server.find_open_addr(true);
    } catch (std::exception &e) {
        m_log->log_err("find_open_addr error: ", e.what());

        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_CANT_OPEN_DATA_CONN, e.what());
        return;
    }

    struct sockaddr_in6 in6_addr = {};
    struct sockaddr *addr = reinterpret_cast<struct sockaddr *>(&in6_addr);
    socklen_t addr_size = sizeof(in6_addr);
    int name_res = getsockname(fd, addr, &addr_size);

    close(fd);

    if (name_res < 0) {
        m_log->log_err("getsockname error: ", std::strerror(errno));
        throw ServerError(std::strerror(errno));
    }

    try {
        open_data_conn(addr, addr_size);
    } catch (std::exception &e) {
        m_log->log_err("open_data_conn error: ", e.what());
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_CANT_OPEN_DATA_CONN, e.what());
        return;
    }

    m_pass_mode = true;

    uint16_t p = ntohs(in6_addr.sin6_port);

    std::stringstream resp_msg;
    resp_msg << " (|||";
    resp_msg << std::to_string(p);
    resp_msg << "|)";

    m_server.send_response(m_control_conn_fd,
                           ftr::STATUS_CODE_ENTER_EXT_PASS_MODE,
                           resp_msg.str());
}

void Session::run_port(const std::string &cmd_params) {
    // we are ignoring the address here, just use the port part
    std::vector<std::string> pieces = ftr::split(cmd_params, ",");

    unsigned long p1 = std::stoul(ftr::trim_whitespace(pieces[4]));
    unsigned long p2 = std::stoul(ftr::trim_whitespace(pieces[5]));

    unsigned int p = static_cast<unsigned int>(p1);
    p <<= 8;
    p |= static_cast<unsigned int>(p2);

    try {
        connect_to_data_conn(p, false);
    } catch (std::exception &e) {
        m_log->log_err("connect_to_data_conn error: ", e.what());
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_UNKNOWN_ERROR, e.what());
        return;
    }

    m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_OK, "");
}

void Session::run_ext_addr_port(const std::string &cmd_params) {
    // we are ignoring the address here, just use the port part
    std::vector<std::string> pieces = ftr::split(cmd_params, "|");
    bool use_ipv6 = false;

    if (pieces[1] == "2") {
        use_ipv6 = true;
    }

    unsigned long port = std::stoul(ftr::trim_whitespace(pieces[3]));

    try {
        connect_to_data_conn(port, use_ipv6);
    } catch (std::exception &e) {
        m_log->log_err("connect_to_data_conn error: ", e.what());
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_UNKNOWN_ERROR, e.what());
        return;
    }

    m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_OK, "");
}

void Session::run_help(const std::string &cmd_args) {
    std::stringstream ss;

    if (cmd_args == "") {
        ss << "Welcome to FTR, enter a command name to get more information "
              "about it. Current commands: ";
        ss << m_server.get_all_commands_help_msg() << '\n';
    } else {
        std::string help_msg = m_server.get_command_help_msg(cmd_args);

        if (help_msg == "") {
            ss << "Sorry, the command " << cmd_args << " is not implemented\n";
        } else {
            ss << ftr::to_upper(cmd_args) << ": " << help_msg << '\n';
        }
    }

    m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_HELP_MESSAGE,
                           ss.str());
}

void Session::run_noop() {
    m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_OK, "");
}

void Session::run_allo() {
    m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_OK, "");
}

void Session::run_account(const std::string &args) {
    if (!is_logged_in()) {
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_NOT_LOGGED_IN, "");
        return;
    }

    if (args == "") {
        m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_BAD_SEQUENCE,
                               "");
        return;
    }

    if (m_session_user.username == args) {
        std::stringstream ss;
        ss << "Username: " << m_session_user.username;
        ss << ", Root: " << m_session_user.root << '\n';

        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_USER_LOGGED_IN, ss.str());
        return;
    }

    m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_BAD_SEQUENCE,
                           "The account was not found");
}

void Session::run_site() {
    m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_OK,
                           "No SITE options for this server");
}

void Session::run_mode(const std::string &args) {
    if (ftr::to_lower(args) != "s") {
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_CMD_NOT_IMPLEMENTED_FOR_PARAM,
                               "");
        return;
    }

    m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_OK, "");
}

void Session::run_abort() {
    if (!is_logged_in()) {
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_NOT_LOGGED_IN, "");
        return;
    }

    if (m_transfer_in_progress) {
        // send notification to the condition variable
        // that is blocked in accept_on_data_conn()
        // this will act as if the transfer had finished
        // and will close the connection on that thread
        m_transfer_in_progress = false;
        m_transfer_done = true;
        m_session_cv.notify_one();

        m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_CONN_CLOSED,
                               "");
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_CLOSING_DATA_CONN, "");
        return;
    }

    // transfer was not in progress (it must have completed)
    // let's try to close the connection (if it's still open)
    if (m_data_conn_fd > 0) {
        int res = close(m_data_conn_fd);

        if (res < 0) {
            m_log->log_err("error closing the data connection: ",
                           std::strerror(errno));
        }
    }

    m_server.send_response(m_control_conn_fd,
                           ftr::STATUS_CODE_CLOSING_DATA_CONN, "");
}

void Session::run_file_struct(const std::string &args) {
    if (ftr::to_lower(args) != "f") {
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_CMD_NOT_IMPLEMENTED_FOR_PARAM,
                               "");
        return;
    }

    m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_OK, "");
}

void Session::run_server_status(const std::string args) {
    if (args == "") {
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_SYSTEM_STATUS, "Server OK");
        return;
    }

    if (!is_logged_in()) {
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_NOT_LOGGED_IN, "");
        return;
    }

    auto conf = m_server.get_conf();
    std::string path_str(conf->get_root() + m_session_user.root + "/" + m_cwd +
                         "/" + args);
    std::filesystem::path dir_path(path_str);
    std::stringstream dir_data;

    try {
        std::filesystem::directory_iterator dir_iter(dir_path);

        for (auto const &entry : dir_iter) {
            FileData file_data(entry, m_log);
            std::string file_line = file_data.get_file_line();
            dir_data << file_line;
            dir_data << '\n';
        }
    } catch (std::exception &e) {
        m_log->log_err(e.what());
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_FILE_ACTION_NOT_TAKEN,
                               e.what());

        return;
    }

    m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_SYSTEM_STATUS,
                           dir_data.str());
}

void Session::run_rename_from(const std::string &from) {
    if (!is_logged_in()) {
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_NOT_LOGGED_IN, "");
        return;
    }

    if (from == "") {
        m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_SYNTAX_ERROR,
                               "A path to rename from is required");
        return;
    }

    m_rename_from = ftr::trim_whitespace(from);
    m_server.send_response(m_control_conn_fd,
                           ftr::STATUS_CODE_REQUESTED_FILE_ACTION, "");
}

void Session::run_rename_to(const std::string &new_file) {
    if (!is_logged_in()) {
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_NOT_LOGGED_IN, "");
        return;
    }

    if (new_file == "") {
        m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_SYNTAX_ERROR,
                               "A path to rename to is required");
        return;
    }

    if (m_rename_from == "") {
        m_server.send_response(
            m_control_conn_fd, ftr::STATUS_CODE_SYNTAX_ERROR,
            "Path to rename from not provided, please run RNFR first");
        return;
    }

    auto conf = m_server.get_conf();
    std::string new_file_name = ftr::trim_whitespace(new_file);
    std::filesystem::path oldpath(conf->get_root() + m_session_user.root + "/" +
                                  m_cwd + "/" + m_rename_from);
    std::filesystem::path newpath(conf->get_root() + m_session_user.root + "/" +
                                  m_cwd + "/" + new_file_name);

    try {
        std::filesystem::rename(oldpath, newpath);
    } catch (std::exception &e) {
        m_log->log_err("rename error: ", e.what());
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_UNKNOWN_ERROR, e.what());
        m_rename_from = "";
        return;
    }

    m_rename_from = "";
    m_server.send_response(m_control_conn_fd,
                           ftr::STATUS_CODE_REQUESTED_FILE_OK, "");
}

void Session::run_reinit() {
    if (!is_logged_in()) {
        m_server.send_response(m_control_conn_fd,
                               ftr::STATUS_CODE_NOT_LOGGED_IN, "");
        return;
    }

    // check if there's a transfer in progress
    // if so, wait until it's done
    while (true) {
        if (!m_transfer_in_progress) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    m_session_user.username = "";
    m_session_user.password = "";
    m_session_user.root = "";

    m_transfer_type = "";
    m_pass_mode = false;
    m_data_conn_fd = -1;
    m_data_conn_port = 0;
    m_cwd = "";
    m_rename_from = "";
    m_transfer_in_progress = false;

    m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_SERVICE_READY,
                           "");
}

void Session::run_quit() {
    if (is_logged_in()) {
        // check if there's a transfer in progress
        // if so, wait until it's done
        while (true) {
            if (!m_transfer_in_progress) {
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    m_server.send_response(m_control_conn_fd,
                           ftr::STATUS_CODE_CLOSING_CONTROL_CONN, "");
    end();
}

void Session::run_not_implemented() {
    m_server.send_response(m_control_conn_fd, ftr::STATUS_CODE_NOT_IMPLEMENTED,
                           "");
}

bool Session::is_logged_in() {
    if (m_session_user.username == "" || m_session_user.password == "") {
        return false;
    }
    return true;
}
