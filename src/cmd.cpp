#include "cmd.hpp"

#include <functional>
#include <string>
#include <utility>
#include <vector>

using namespace ftr;

bool command::unknown_value_found() noexcept {
    if (m_unknowns.size() > 0) {
        return true;
    }
    return false;
}

std::string command::unknown_flag() noexcept {
    if (m_unknowns.size() > 0) {
        return m_unknowns.at(0);
    }
    return "";
}

void command::add_flag(const char flag, bool &val) noexcept {
    std::string s_flag(1, flag);
    m_flags.push_back(std::make_pair(s_flag, std::ref(val)));
}

void command::add_flag(const std::string flag, bool &val) noexcept {
    std::string s_flag(flag);
    m_flags.push_back(std::make_pair(s_flag, std::ref(val)));
}

void command::add_flag(const char *flag, bool &val) noexcept {
    std::string s_flag(flag);
    m_flags.push_back(std::make_pair(s_flag, std::ref(val)));
}

void command::add_option(const char opt, std::string &val) noexcept {
    std::string s_opt(1, opt);
    m_options.push_back(std::make_pair(s_opt, std::ref(val)));
}

void command::add_option(const std::string opt, std::string &val) noexcept {
    std::string s_opt(opt);
    m_options.push_back(std::make_pair(s_opt, std::ref(val)));
}

void command::parse() {
    if (m_argc <= 1) {
        return;
    }

    for (int i = 1; i < m_argc; i++) {
        std::string cur_arg(m_argv[i]);
        bool found = false;

        // check all the flags
        for (auto it = m_flags.begin(); it != m_flags.end(); ++it) {
            std::string exp_flag = "-";
            exp_flag += it->first;

            if (exp_flag == cur_arg) {
                it->second = true;
                found = true;
            }
        }

        // check all the options
        for (auto it = m_options.begin(); it != m_options.end(); ++it) {
            std::string exp_opt = "-";
            exp_opt += it->first;

            if (exp_opt == cur_arg) {
                i++;
                found = true;
                if (m_argv[i]) {
                    it->second = std::string(m_argv[i]);
                }
            }
        }

        // check if a flag or option that we didn't asked for was specified
        if (!found) {
            m_unknowns.push_back(cur_arg);
        }
    }
}
