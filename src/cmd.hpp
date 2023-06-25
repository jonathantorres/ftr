#ifndef cmd_hpp
#define cmd_hpp

#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace ftr {

class command {
  public:
    command(const int argc, const char *argv[]) : m_argc{argc}, m_argv{argv} {};
    ~command() = default;
    command(const ftr::command &other) = delete;
    command(ftr::command &&other) = delete;
    ftr::command &operator=(const ftr::command &rhs) = delete;
    ftr::command &operator=(ftr::command &&rhs) = delete;

    bool unknown_value_found() noexcept {
        if (m_unknowns.size() > 0) {
            return true;
        }
        return false;
    }

    std::string unknown_flag() noexcept {
        if (m_unknowns.size() > 0) {
            return m_unknowns.at(0);
        }
        return "";
    }

    void add_flag(const char flag, bool &val) noexcept {
        std::string s_flag(1, flag);
        m_flags.push_back(std::make_pair(s_flag, std::ref(val)));
    }

    void add_flag(const std::string flag, bool &val) noexcept {
        std::string s_flag(flag);
        m_flags.push_back(std::make_pair(s_flag, std::ref(val)));
    }

    void add_flag(const char *flag, bool &val) noexcept {
        std::string s_flag(flag);
        m_flags.push_back(std::make_pair(s_flag, std::ref(val)));
    }

    void add_option(const char opt, std::string &val) noexcept {
        std::string s_opt(1, opt);
        m_options.push_back(std::make_pair(s_opt, std::ref(val)));
    }

    void add_option(const std::string opt, std::string &val) noexcept {
        std::string s_opt(opt);
        m_options.push_back(std::make_pair(s_opt, std::ref(val)));
    }

    void parse() {
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

  private:
    std::vector<std::pair<std::string, bool &>> m_flags;
    std::vector<std::pair<std::string, std::string &>> m_options;
    std::vector<std::string> m_unknowns;
    int m_argc;
    const char **m_argv;
};
} // namespace ftr

#endif
