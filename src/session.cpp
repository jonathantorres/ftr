#include "session.hpp"
#include "conf.hpp"
#include "constants.hpp"
#include "server.hpp"
#include "util.hpp"
#include <array>
#include <cerrno>
#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
#include <sstream>
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
        run_password(cmd_params);
        return;
    } else if (cmd == CMD_PRINT_DIR) {
        run_print_dir();
        return;
    } else if (cmd == CMD_CHANGE_DIR) {
        run_change_dir();
        return;
    } else if (cmd == CMD_TYPE) {
        run_type(cmd_params);
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

void Session::run_change_dir() {
    // TODO
    run_not_implemented();
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
    // TODO
    run_not_implemented();
}

void Session::run_list() {
    // TODO
    run_not_implemented();
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
    // TODO
    run_not_implemented();
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
            ss << ftr::to_upper(cmd_args) << ": " << help_msg << '\n';
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
    if (ftr::to_lower(args) != "f") {
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
