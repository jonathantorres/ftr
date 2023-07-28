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
