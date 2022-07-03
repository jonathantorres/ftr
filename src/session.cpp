#include "session.hpp"
#include "constants.hpp"
#include "server.hpp"
#include "util.hpp"
#include <array>
#include <cerrno>
#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
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

void Session::open_data_conn(int port) {
    if (port) {
    }
}

void Session::connect_to_data_conn(int port) {
    if (port) {
    }
}

void Session::handle_data_transfer() {}

void Session::handle_command(
    std::array<char, ftr::DEFAULT_CMD_SIZE> &client_cmd) {
    std::string cmd_string =
        std::string(reinterpret_cast<char *>(client_cmd.data()));
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

void Session::exec_command(std::string cmd, std::string cmd_params) {
    // TODO: log the command executed

    if (cmd == CMD_USER) {
        run_user(cmd_params);
        return;
    } else if (cmd == CMD_PASSWORD) {
        run_password();
        return;
    } else if (cmd == CMD_PRINT_DIR) {
        run_print_dir();
        return;
    } else if (cmd == CMD_CHANGE_DIR) {
        run_change_dir();
        return;
    } else if (cmd == CMD_TYPE) {
        run_type();
        return;
    } else if (cmd == CMD_PASSIVE) {
        run_passive();
        return;
    } else if (cmd == CMD_LIST) {
        run_list();
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
        run_help();
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
        run_file_struct();
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
            SessionUser session_user = {
                username : std::string(username),
                password : "",
                root : "",
            };
            user = session_user;
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

void Session::run_password() {
    // TODO
}

void Session::run_print_dir() {
    // TODO
}

void Session::run_change_dir() {
    // TODO
}

void Session::run_type() {
    // TODO
}

void Session::run_passive() {
    // TODO
}

void Session::run_list() {
    // TODO
}

void Session::run_file_names() {
    // TODO
}

void Session::run_retrieve() {
    // TODO
}

void Session::run_accept_and_store() {
    // TODO
}

void Session::run_system_type() {
    server.send_response(control_conn_fd, ftr::STATUS_CODE_NAME_SYSTEM,
                         " UNIX Type: L8");
}

void Session::run_change_parent() {
    // TODO
}

void Session::run_make_dir() {
    // TODO
}

void Session::run_remove_dir() {
    // TODO
}

void Session::run_delete() {
    // TODO
}

void Session::run_ext_passv_mode() {
    // TODO
}

void Session::run_port() {
    // TODO
}

void Session::run_ext_addr_port() {
    // TODO
}

void Session::run_help() {
    // TODO
}

void Session::run_noop() {
    // TODO
}

void Session::run_allo() {
    // TODO
}

void Session::run_account() {
    // TODO
}

void Session::run_site() {
    // TODO
}

void Session::run_mode() {
    // TODO
}

void Session::run_abort() {
    // TODO
}

void Session::run_file_struct() {
    // TODO
}

void Session::run_server_status() {
    // TODO
}

void Session::run_rename_from() {
    // TODO
}

void Session::run_rename_to() {
    // TODO
}

void Session::run_reinit() {
    // TODO
}

void Session::run_quit() {
    // TODO
}

void Session::run_not_implemented() {
    server.send_response(control_conn_fd, ftr::STATUS_CODE_NOT_IMPLEMENTED, "");
}
