#ifndef ftr_conf_hpp
#define ftr_conf_hpp

#include <memory>
#include <string>
#include <vector>

namespace ftr {

class User {
  public:
    User() = default;
    ~User() = default;
    User(const User &u) = delete;
    User(User &&u) = delete;
    User &operator=(const User &rhs) = delete;
    User &operator=(User &&rhs) = delete;

    void add_option(std::string op_name, std::string op_value);
    const std::string get_username() { return username; }
    const std::string get_password() { return password; }
    const std::string get_root() { return root; }

  private:
    std::string username;
    std::string password;
    std::string root;
};

class Conf {
  public:
    Conf() : port{0} {};
    ~Conf() = default;
    Conf(const Conf &conf) = delete;
    Conf(Conf &&conf) = delete;
    Conf &operator=(Conf &&rhs) = delete;
    Conf &operator=(const Conf &rhs) = delete;

    void load(std::string path, std::string prefix);
    const std::vector<std::shared_ptr<ftr::User>> &get_users() { return users; }
    const std::string get_root() { return root; }
    const std::string get_server_name() { return server_name; }
    const std::string get_error_log() { return error_log; }
    const std::string get_access_log() { return access_log; }
    int get_port() { return port; }

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
    std::string prefix;
    std::string server_name;
    std::string root;
    std::string error_log;
    std::string access_log;
    std::vector<std::shared_ptr<ftr::User>> users;
    int port;

    void add_option(std::string op_name, std::string op_value);
    std::vector<std::string> open_and_strip_comments(std::string path);
    std::string strip_comment_from_line(std::string line);
    std::vector<std::string> parse_includes(std::vector<std::string> &conf_vec);
    void check_for_syntax_errors(std::vector<std::string> &conf_vec);
    void build(std::vector<std::string> &conf_vec);
};

} // namespace ftr
#endif
