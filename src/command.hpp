#ifndef ftr_command_hpp
#define ftr_command_hpp

#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace ftr {

class Command {
  public:
    Command(const int argc, const char *argv[]) : m_argc{argc}, m_argv{argv} {};
    ~Command() = default;
    Command(const ftr::Command &other) = delete;
    Command(ftr::Command &&other) = delete;
    ftr::Command &operator=(const ftr::Command &rhs) = delete;
    ftr::Command &operator=(ftr::Command &&rhs) = delete;

    bool unknown_value_found() noexcept;
    std::string unknown_flag() noexcept;
    void add_flag(const char flag, bool &val) noexcept;
    void add_flag(const std::string flag, bool &val) noexcept;
    void add_flag(const char *flag, bool &val) noexcept;
    void add_option(const char opt, std::string &val) noexcept;
    void add_option(const std::string opt, std::string &val) noexcept;
    void parse();

  private:
    std::vector<std::pair<std::string, bool &>> m_flags;
    std::vector<std::pair<std::string, std::string &>> m_options;
    std::vector<std::string> m_unknowns;
    int m_argc;
    const char **m_argv;
};
} // namespace ftr

#endif
