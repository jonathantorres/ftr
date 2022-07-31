#include "session.hpp"
#include "conf.hpp"
#include "constants.hpp"
#include "exception.hpp"
#include "file_data_line.hpp"
#include "server.hpp"
#include "util.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <array>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string.hpp>
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

        int res = read(control_conn_fd, client_cmd.data(), client_cmd.size());

        if (res < 0) {
            // an error ocurred
            // TODO: Log this error
            std::cerr << "read error: " << strerror(errno) << '\n';
            server.send_response(control_conn_fd,
                                 ftr::STATUS_CODE_UNKNOWN_ERROR, "");
            close(control_conn_fd);
            break;
        } else if (res == 0) {
            // connection closed
            // TODO: Log this
            std::cerr << "connection finished by client\n";
            close(control_conn_fd);
            break;
        }

        try {
            handle_command(client_cmd);
        } catch (std::exception &e) {
            // TODO: log this error
            std::cerr << "error handling the command: " << e.what() << '\n';
            server.send_response(control_conn_fd,
                                 ftr::STATUS_CODE_UNKNOWN_ERROR, "");
            continue;
        }
    }
}

void Session::end() {}

void Session::open_data_conn(struct sockaddr *conn_addr) {
    int fd = socket(conn_addr->sa_family, SOCK_STREAM, 0);

    if (fd < 0) {
        // TODO: log this error
        throw SessionError(strerror(errno));
    }

    int opt = 1;
    int res = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

    if (res < 0) {
        // TODO: log this error
        close(fd);
        throw SessionError(strerror(errno));
    }

    res = bind(fd, conn_addr, sizeof(*conn_addr));

    if (res < 0) {
        // TODO: log this error
        close(fd);
        std::cout << "bind failure\n";
        throw SessionError(strerror(errno));
    }

    res = listen(fd, ftr::BACKLOG);

    if (res < 0) {
        // TODO: log this error
        close(fd);
        std::cout << "listen failure\n";
        throw SessionError(strerror(errno));
    }

    // start accepting connections
    std::thread handle(&Session::accept_on_data_conn, this, fd);
    handle.detach();
}

void Session::accept_on_data_conn(int listener_fd) {
    int conn_fd = accept(listener_fd, nullptr, nullptr);

    if (conn_fd < 0) {
        // TODO: log this error
        close(listener_fd);
        std::cout << "accept failure when accepting on the data connection\n";
        return;
    }

    data_conn_fd = conn_fd;

    // the connection is now ready to be used
    transfer_ready = true;
    session_cv.notify_one();

    // wait until the thread that is doing the transfer
    // is done, and close the connection
    std::unique_lock conn_lock(session_mu);
    session_cv.wait(conn_lock, [&] { return transfer_done; });

    close(data_conn_fd);
    close(listener_fd);

    data_conn_fd = -1;
    listener_fd = -1;
    transfer_ready = false;
    transfer_done = false;
}

void Session::handle_command(
    std::array<char, ftr::DEFAULT_CMD_SIZE> &client_cmd) {
    std::string cmd_string =
        std::string(reinterpret_cast<char *>(client_cmd.data()));
    cmd_string = string::trim_whitespace(cmd_string);
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

void Session::exec_command(std::string cmd, std::string cmd_params) {
    // TODO: log the command executed

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
        run_file_names();
        return;
    } else if (cmd == CMD_RETRIEVE) {
        run_retrieve();
        return;
    } else if (cmd == CMD_ACCEPT_AND_STORE || cmd == CMD_STORE_FILE) {
        run_accept_and_store();
        return;
    } else if (cmd == CMD_APPEND) {
        run_accept_and_store(); // uses a different flag
        return;
    } else if (cmd == CMD_SYSTEM_TYPE) {
        run_system_type();
        return;
    } else if (cmd == CMD_CHANGE_PARENT || cmd == CMD_CHANGE_TO_PARENT_DIR) {
        run_change_parent();
        return;
    } else if (cmd == CMD_MAKE_DIR || cmd == CMD_MAKE_A_DIR) {
        run_make_dir();
        return;
    } else if (cmd == CMD_REMOVE_DIR) {
        run_remove_dir();
        return;
    } else if (cmd == CMD_DELETE) {
        run_delete();
        return;
    } else if (cmd == CMD_EXT_PASSV_MODE) {
        run_ext_passv_mode();
        return;
    } else if (cmd == CMD_PORT) {
        run_port();
        return;
    } else if (cmd == CMD_EXT_ADDR_PORT) {
        run_ext_addr_port();
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
        run_account();
        return;
    } else if (cmd == CMD_SITE) {
        run_site();
        return;
    } else if (cmd == CMD_MODE) {
        run_mode();
        return;
    } else if (cmd == CMD_ABORT) {
        run_abort();
        return;
    } else if (cmd == CMD_FILE_STRUCT) {
        run_file_struct(cmd_params);
        return;
    } else if (cmd == CMD_SERVER_STATUS) {
        run_server_status();
        return;
    } else if (cmd == CMD_RENAME_FROM) {
        run_rename_from();
        return;
    } else if (cmd == CMD_RENAME_TO) {
        run_rename_to();
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

void Session::run_user(std::string username) {
    bool user_found = false;
    const std::vector<std::shared_ptr<ftr::User>> users =
        server.get_conf()->get_users();

    for (auto &u : users) {
        if (u->get_username() == username) {
            user_found = true;
            SessionUser new_session_user = {
                .username = std::string(username),
                .password = "",
                .root = "",
            };
            session_user = new_session_user;
            break;
        }
    }

    if (user_found) {
        server.send_response(control_conn_fd, ftr::STATUS_CODE_USERNAME_OK, "");
        return;
    }

    server.send_response(control_conn_fd, ftr::STATUS_CODE_INVALID_USERNAME,
                         "");
}

void Session::run_password(std::string password) {
    bool pass_found = false;
    const std::shared_ptr<ftr::Conf> conf = server.get_conf();
    const std::vector<std::shared_ptr<ftr::User>> users = conf->get_users();

    for (auto &u : users) {
        if (u->get_username() == session_user.username &&
            u->get_password() == password) {
            pass_found = true;

            session_user.password = std::string(password);
            session_user.root = std::string(u->get_root());
            break;
        }
    }

    if (pass_found) {
        // change to home directory
        std::string path(conf->get_root() + session_user.root);
        int res = chdir(path.c_str());

        if (res < 0) {
            // TODO: log this error
            server.send_response(control_conn_fd,
                                 ftr::STATUS_CODE_FILE_NOT_FOUND, "");
            return;
        }

        server.send_response(control_conn_fd, ftr::STATUS_CODE_USER_LOGGED_IN,
                             "");
        return;
    }

    server.send_response(control_conn_fd, ftr::STATUS_CODE_INVALID_USERNAME,
                         "");
}

void Session::run_print_dir() {
    server.send_response(control_conn_fd, ftr::STATUS_CODE_PATH_CREATED,
                         " \"/" + cwd + "\" is current directory");
}

void Session::run_change_dir(std::string dir) {
    if (!is_logged_in()) {
        server.send_response(control_conn_fd, ftr::STATUS_CODE_NOT_LOGGED_IN,
                             "");
        return;
    }

    std::string cur_wd = cwd;

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

    const std::shared_ptr<ftr::Conf> conf = server.get_conf();
    std::string path(conf->get_root() + session_user.root + "/" + cur_wd);
    int res = chdir(path.c_str());

    if (res < 0) {
        // TODO: log this error
        server.send_response(control_conn_fd, ftr::STATUS_CODE_FILE_NOT_FOUND,
                             "");
        return;
    }

    cwd = cur_wd;
    server.send_response(control_conn_fd, ftr::STATUS_CODE_REQUESTED_FILE_OK,
                         " \"" + dir + "\" is current directory");
}

void Session::run_type(std::string selected_transfer_type) {
    if (selected_transfer_type == ftr::TRANSFER_TYPE_ASCII ||
        selected_transfer_type == TRANSFER_TYPE_IMG) {
        transfer_type = selected_transfer_type;
        server.send_response(control_conn_fd, ftr::STATUS_CODE_OK,
                             " Transfer Type OK");
        return;
    }
    run_not_implemented();
}

void Session::run_passive() {
    int fd = -1;

    try {
        fd = server.find_open_addr(false);
    } catch (std::exception &e) {
        server.send_response(control_conn_fd,
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

    std::string addr_str = server.get_addr_string(&addr);

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
        open_data_conn(&addr);
    } catch (std::exception &e) {
        server.send_response(control_conn_fd,
                             ftr::STATUS_CODE_CANT_OPEN_DATA_CONN, e.what());
        return;
    }

    server.send_response(control_conn_fd, ftr::STATUS_CODE_ENTER_PASS_MODE,
                         resp_msg.str());
}

void Session::run_list(std::string file) {
    if (!is_logged_in()) {
        server.send_response(control_conn_fd, ftr::STATUS_CODE_NOT_LOGGED_IN,
                             "");
        return;
    }

    // wait until the data connection is ready for sending/receiving data
    std::unique_lock list_lock(session_mu);
    session_cv.wait(list_lock, [&] { return transfer_ready; });
    list_lock.unlock();

    transfer_in_progress = true;

    auto conf = server.get_conf();
    std::string path_str(conf->get_root() + session_user.root + "/" + cwd);

    if (file != "") {
        path_str += "/" + file;
    }

    std::filesystem::path dir_path(path_str);
    std::stringstream dir_data;

    try {
        std::filesystem::directory_iterator dir_iter(dir_path);

        for (auto const &entry : dir_iter) {
            FileDataLine file_data(entry);
            std::string file_line = file_data.get_file_line();
            dir_data << file_line;
            dir_data << '\n';
        }
    } catch (std::exception &e) {
        transfer_in_progress = false;
        transfer_done = true;
        session_cv.notify_one();

        server.send_response(control_conn_fd,
                             ftr::STATUS_CODE_FILE_ACTION_NOT_TAKEN, e.what());

        return;
    }

    std::string dir_data_str = dir_data.str();
    if (write(data_conn_fd, dir_data_str.c_str(), dir_data_str.size()) < 0) {
        // TODO: log the error
        transfer_in_progress = false;
        transfer_done = true;
        session_cv.notify_one();

        server.send_response(control_conn_fd,
                             ftr::STATUS_CODE_FILE_ACTION_NOT_TAKEN,
                             strerror(errno));

        return;
    }

    // send notification that the operation has finished
    transfer_in_progress = false;
    transfer_done = true;
    session_cv.notify_one();

    server.send_response(control_conn_fd, ftr ::STATUS_CODE_OK, "");
}

void Session::run_file_names() {
    // TODO
    run_not_implemented();
}

void Session::run_retrieve() {
    // TODO
    run_not_implemented();
}

void Session::run_accept_and_store() {
    // TODO
    run_not_implemented();
}

void Session::run_system_type() {
    server.send_response(control_conn_fd, ftr::STATUS_CODE_NAME_SYSTEM,
                         " UNIX Type: L8");
}

void Session::run_change_parent() {
    if (!is_logged_in()) {
        server.send_response(control_conn_fd, ftr::STATUS_CODE_NOT_LOGGED_IN,
                             "");
        return;
    }

    std::string cur_wd = cwd;
    std::vector<std::string> pieces = string::split(cwd, "/");

    if (pieces.size() <= 1) {
        cur_wd = "";
    } else {
        pieces.pop_back();

        cur_wd = string::join(pieces, "/");
    }

    const std::shared_ptr<ftr::Conf> conf = server.get_conf();
    std::string chdir_path =
        conf->get_root() + session_user.root + "/" + cur_wd.c_str();
    int res = chdir(chdir_path.c_str());

    if (res < 0) {
        // TODO: log this error
        server.send_response(control_conn_fd, ftr::STATUS_CODE_FILE_NOT_FOUND,
                             "");
        return;
    }

    cwd = cur_wd;
    auto wd_path = std::filesystem::path(cur_wd);
    std::string base = wd_path.stem().c_str();

    server.send_response(control_conn_fd, ftr::STATUS_CODE_OK,
                         "\"" + base + "\" is current directory");
}

void Session::run_make_dir() {
    // TODO
    run_not_implemented();
}

void Session::run_remove_dir() {
    // TODO
    run_not_implemented();
}

void Session::run_delete() {
    // TODO
    run_not_implemented();
}

void Session::run_ext_passv_mode() {
    // TODO
    run_not_implemented();
}

void Session::run_port() {
    // TODO
    run_not_implemented();
}

void Session::run_ext_addr_port() {
    // TODO
    run_not_implemented();
}

void Session::run_help(std::string cmd_args) {
    std::stringstream ss;

    if (cmd_args == "") {
        ss << "Welcome to FTR, enter a command name to get more information "
              "about it. Current commands: ";
        ss << server.get_all_commands_help_msg() << '\n';
    } else {
        std::string help_msg = server.get_command_help_msg(cmd_args);

        if (help_msg == "") {
            ss << "Sorry, the command " << cmd_args << " is not implemented\n";
        } else {
            ss << string::to_upper(cmd_args) << ": " << help_msg << '\n';
        }
    }

    server.send_response(control_conn_fd, ftr::STATUS_CODE_HELP_MESSAGE,
                         ss.str());
}

void Session::run_noop() {
    server.send_response(control_conn_fd, ftr::STATUS_CODE_OK, "");
}

void Session::run_allo() {
    server.send_response(control_conn_fd, ftr::STATUS_CODE_OK, "");
}

void Session::run_account() {
    // TODO
    run_not_implemented();
}

void Session::run_site() {
    server.send_response(control_conn_fd, ftr::STATUS_CODE_OK,
                         "No SITE options for this server");
}

void Session::run_mode() {
    // TODO
    run_not_implemented();
}

void Session::run_abort() {
    // TODO
    run_not_implemented();
}

void Session::run_file_struct(std::string args) {
    if (string::to_lower(args) != "f") {
        server.send_response(control_conn_fd,
                             ftr::STATUS_CODE_CMD_NOT_IMPLEMENTED_FOR_PARAM,
                             "");
        return;
    }

    server.send_response(control_conn_fd, ftr::STATUS_CODE_OK, "");
}

void Session::run_server_status() {
    // TODO
    run_not_implemented();
}

void Session::run_rename_from() {
    // TODO
    run_not_implemented();
}

void Session::run_rename_to() {
    // TODO
    run_not_implemented();
}

void Session::run_reinit() {
    // TODO
    run_not_implemented();
}

void Session::run_quit() {
    // TODO
    run_not_implemented();
}

void Session::run_not_implemented() {
    server.send_response(control_conn_fd, ftr::STATUS_CODE_NOT_IMPLEMENTED, "");
}

bool Session::is_logged_in() {
    if (session_user.username == "" || session_user.password == "") {
        return false;
    }
    return true;
}
