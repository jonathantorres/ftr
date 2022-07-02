#include "session.hpp"
#include "constants.hpp"
#include "server.hpp"
#include "util.hpp"
#include <array>
#include <cerrno>
#include <cstring>
#include <exception>
#include <iostream>
#include <unistd.h>

using namespace ftr;

void Session::start() {
    while (true) {
        std::array<char, ftr::DEFAULT_CMD_SIZE> client_cmd;

        int res =
            read(control_conn_fd, reinterpret_cast<char *>(client_cmd.data()),
                 client_cmd.size());

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
    }

    exec_command(cmd, cmd_params);
}

void Session::exec_command(std::string cmd, std::string cmd_params) {
    // TODO: log the command executed
    std::cerr << cmd << ' ' << cmd_params;

    if (cmd == CMD_USER) {
        run_user();
    } else if (cmd == CMD_PASSWORD) {
        run_password();
    } else if (cmd == CMD_PRINT_DIR) {
        run_print_dir();
    } else if (cmd == CMD_CHANGE_DIR) {
        run_change_dir();
    } else if (cmd == CMD_TYPE) {
        run_type();
    } else if (cmd == CMD_PASSIVE) {
        run_passive();
    } else if (cmd == CMD_LIST) {
        run_list();
    } else if (cmd == CMD_FILE_NAMES) {
        run_file_names();
    } else if (cmd == CMD_RETRIEVE) {
        run_retrieve();
    } else if (cmd == CMD_ACCEPT_AND_STORE || cmd == CMD_STORE_FILE) {
        run_accept_and_store();
    } else if (cmd == CMD_APPEND) {
        run_accept_and_store(); // uses a different flag
    } else if (cmd == CMD_SYSTEM_TYPE) {
        run_system_type();
    } else if (cmd == CMD_CHANGE_PARENT || cmd == CMD_CHANGE_TO_PARENT_DIR) {
        run_change_parent();
    } else if (cmd == CMD_MAKE_DIR || cmd == CMD_MAKE_A_DIR) {
        run_make_dir();
    } else if (cmd == CMD_REMOVE_DIR) {
        run_remove_dir();
    } else if (cmd == CMD_DELETE) {
        run_delete();
    } else if (cmd == CMD_EXT_PASSV_MODE) {
        run_ext_passv_mode();
    } else if (cmd == CMD_PORT) {
        run_port();
    } else if (cmd == CMD_EXT_ADDR_PORT) {
        run_ext_addr_port();
    } else if (cmd == CMD_HELP) {
        run_help();
    } else if (cmd == CMD_NOOP) {
        run_noop();
    } else if (cmd == CMD_ALLO) {
        run_allo();
    } else if (cmd == CMD_ACCOUNT) {
        run_account();
    } else if (cmd == CMD_SITE) {
        run_site();
    } else if (cmd == CMD_MODE) {
        run_mode();
    } else if (cmd == CMD_ABORT) {
        run_abort();
    } else if (cmd == CMD_FILE_STRUCT) {
        run_file_struct();
    } else if (cmd == CMD_SERVER_STATUS) {
        run_server_status();
    } else if (cmd == CMD_RENAME_FROM) {
        run_rename_from();
    } else if (cmd == CMD_RENAME_TO) {
        run_rename_to();
    } else if (cmd == CMD_REINIT) {
        run_reinit();
    } else if (cmd == CMD_QUIT) {
        run_quit();
    }

    run_not_implemented();
}

void Session::run_user() {
    // TODO
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
    // TODO
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
