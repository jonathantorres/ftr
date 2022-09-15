#ifndef ftr_conf_hpp
#define ftr_conf_hpp

#include <memory>
#include <string>
#include <vector>

namespace ftr {

class user {
  public:
    user() = default;
    ~user() = default;
    user(const user &u) = delete;
    user(user &&u) = delete;
    user &operator=(const user &rhs) = delete;
    user &operator=(user &&rhs) = delete;

    void add_option(const std::string &op_name, const std::string &op_value);
    const std::string get_username() { return m_username; }
    const std::string get_password() { return m_password; }
    const std::string get_root() { return m_root; }

  private:
    std::string m_username;
    std::string m_password;
    std::string m_root;
};

class conf {
  public:
    conf() : m_port{0} {};
    ~conf() = default;
    conf(const conf &conf) = delete;
    conf(conf &&conf) = delete;
    conf &operator=(conf &&rhs) = delete;
    conf &operator=(const conf &rhs) = delete;

    void load(const std::string &path);
    const std::vector<std::shared_ptr<ftr::user>> &get_users() const {
        return m_users;
    }
    const std::string get_root() { return m_root; }
    const std::string get_server_name() { return m_server_name; }
    const std::string get_error_log() { return m_error_log; }
    const std::string get_access_log() { return m_access_log; }
    int get_port() { return m_port; }

    static constexpr char EQUAL_SIGN = '=';
    static constexpr char OPEN_BRACKET = '{';
    static constexpr char CLOSE_BRACKET = '}';
    static constexpr char COMMENT_SIGN = '#';

    static constexpr char const *SERVER_OPT = "server";
    static constexpr char const *ROOT_OPT = "root";
    static constexpr char const *PORT_OPT = "port";
    static constexpr char const *USER_OPT = "user";
    static constexpr char const *USERNAME_OPT = "username";
    static constexpr char const *PASSWORD_OPT = "password";
    static constexpr char const *ERROR_LOG_OPT = "error_log";
    static constexpr char const *ACCESS_LOG_OPT = "access_log";
    static constexpr char const *INCLUDE_OPT = "include";

  private:
    std::string m_prefix;
    std::string m_server_name;
    std::string m_root;
    std::string m_error_log;
    std::string m_access_log;
    std::vector<std::shared_ptr<ftr::user>> m_users;
    int m_port;

    void add_option(const std::string &op_name, const std::string &op_value);
    std::vector<std::string> open_and_strip_comments(const std::string &path);
    std::string strip_comment_from_line(std::string line);
    std::vector<std::string>
    parse_includes(const std::vector<std::string> &conf_vec);
    void check_for_syntax_errors(const std::vector<std::string> &conf_vec);
    void build(const std::vector<std::string> &conf_vec);
};

} // namespace ftr
#endif
