#include "conf.hpp"
#include "exception.hpp"
#include "util.hpp"
#include <cstdlib>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include <string.hpp>
#include <string>
#include <vector>

using namespace ftr;

void Conf::load(const std::string &path, const std::string &prefix) {
    if (prefix == "") {
        // TODO: do something with the prefix
    }

    std::vector<std::string> conf_file_lines = open_and_strip_comments(path);

    if (conf_file_lines.empty()) {
        // TODO: maybe log this?
        throw ftr::ConfError("there are no lines in the configuration");
    }

    conf_file_lines = parse_includes(conf_file_lines); // TODO
    check_for_syntax_errors(conf_file_lines);          // TODO
    build(conf_file_lines);
}

void Conf::build(const std::vector<std::string> &conf_vec) {
    bool inside_usr_cmd = false;
    std::shared_ptr<ftr::User> cur_usr = nullptr;

    if (conf_vec.empty()) {
        throw ftr::ConfError("the configuration file cannot be empty");
    }

    for (std::vector<std::string>::const_iterator i = conf_vec.begin();
         i != conf_vec.end(); i++) {
        std::string line = *i;
        std::string::size_type eq_pos;
        if ((eq_pos = line.find_first_of(ftr::Conf::EQUAL_SIGN)) !=
            std::string::npos) {
            // this is a line with an option
            std::string op_name =
                string::trim_whitespace(line.substr(0, eq_pos));
            std::string op_value =
                string::trim_whitespace(line.substr(eq_pos + 1));

            if (inside_usr_cmd) {
                // option for the current user
                cur_usr->add_option(op_name, op_value);
            } else {
                // top level or global option
                add_option(op_name, op_value);
            }
        } else if (line.find_first_of(ftr::Conf::USER_OPT) !=
                   std::string::npos) {
            // this is a line with a user command
            inside_usr_cmd = true;
            cur_usr = std::make_shared<ftr::User>();
        } else if (line.find_first_of(ftr::Conf::CLOSE_BRACKET) !=
                   std::string::npos) {
            // closing bracket for a user command
            if (inside_usr_cmd) {
                m_users.push_back(cur_usr);
                inside_usr_cmd = false;
                cur_usr = nullptr;
            }
        }
    }
}

void Conf::add_option(const std::string &op_name, const std::string &op_value) {
    if (op_name == ftr::Conf::SERVER_OPT) {
        m_server_name = op_value;
    } else if (op_name == ftr::Conf::ROOT_OPT) {
        m_root = op_value;
    } else if (op_name == ftr::Conf::ERROR_LOG_OPT) {
        m_error_log = op_value;
    } else if (op_name == ftr::Conf::ACCESS_LOG_OPT) {
        m_access_log = op_value;
    } else if (op_name == ftr::Conf::PORT_OPT) {
        m_port = std::atoi(op_value.c_str());

        if (m_port == 0) {
            throw ConfError("the port cannot be zero");
        }
    }
}

std::vector<std::string>
Conf::open_and_strip_comments(const std::string &path) {
    std::fstream fs(path, std::ios::in);
    std::vector<std::string> conf_file_lines;

    if (!fs.is_open() || !fs.good()) {
        throw ConfError("There was a problem opening the file");
    }

    std::string line;
    while (std::getline(fs, line)) {
        // ignore line if it starts with a comment
        if (line.size() > 0 && line.at(0) == ftr::Conf::COMMENT_SIGN) {
            continue;
        }

        line = strip_comment_from_line(line);
        conf_file_lines.push_back(line);
    }

    fs.close();

    return conf_file_lines;
}

std::string Conf::strip_comment_from_line(std::string line) {
    std::string::size_type i = line.find(ftr::Conf::COMMENT_SIGN);

    if (i != std::string::npos) {
        return line.substr(0, i);
    }

    return line;
}

void Conf::check_for_syntax_errors(const std::vector<std::string> &conf_vec) {
    // TODO: this needs to be implemented
    // if there is a syntax error, throw an exception
    if (conf_vec.empty()) {
    }
}

std::vector<std::string>
Conf::parse_includes(const std::vector<std::string> &conf_vec) {
    // TODO: this needs to be implemented
    return conf_vec;
}

void User::add_option(const std::string &op_name, const std::string &op_value) {
    if (op_name == ftr::Conf::USERNAME_OPT) {
        m_username = op_value;
    } else if (op_name == ftr::Conf::PASSWORD_OPT) {
        m_password = op_value;
    } else if (op_name == ftr::Conf::ROOT_OPT) {
        m_root = op_value;
    }
}
