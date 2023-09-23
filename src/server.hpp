#ifndef ftr_server_hpp
#define ftr_server_hpp

#include "conf.hpp"
#include "log.hpp"
#include "session.hpp"
#include <arpa/inet.h>
#include <map>
#include <memory>
#include <netdb.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>

namespace ftr {

class Server {
  public:
    Server()
        : m_conf{nullptr},
          m_log{nullptr},
          m_port{0},
          m_ctrl_listener_fd{0},
          m_is_reloading{false},
          m_is_shutting_down{false} {};
    ~Server() = default;

    Server(const Server &server) = delete;
    Server(Server &&server) = delete;
    Server &operator=(const Server &server) = delete;
    Server &operator=(Server &&server) = delete;

    void start(const std::shared_ptr<ftr::Conf> conf,
               const std::shared_ptr<ftr::Log> log);
    void shutdown();
    void reload(const std::string &prefix);
    void send_response(const int conn_fd, const int status_code,
                       const std::string &extra_msg);
    std::string get_host() { return m_host; };
    std::string get_resolved_host() { return m_resolved_host; };
    std::string get_status_code_msg(const int status_code);
    std::string get_command_help_msg(const std::string &cmd);
    std::string get_all_commands_help_msg();
    bool is_reloading() { return m_is_reloading; }
    const std::shared_ptr<ftr::Conf> get_conf() { return m_conf; }
    int find_open_addr(bool use_ipv6);
    std::string get_addr_string(struct sockaddr *addr);

  private:
    std::string m_host;
    std::string m_resolved_host;
    std::shared_ptr<ftr::Conf> m_conf;
    std::shared_ptr<ftr::Log> m_log;
    std::map<int, std::shared_ptr<ftr::Session>> m_sessions;
    std::map<int, std::thread> m_session_threads;
    int m_port;
    int m_ctrl_listener_fd;
    bool m_is_reloading;
    bool m_is_shutting_down;

    enum class host_type { // TODO: fix this
        domain_name,
        ipv4,
        ipv6,
        invalid,
    };

    int get_server_ctrl_listener();
    host_type validate_server_host();
    int bind_address(const struct addrinfo *addr_info);
    void handle_conn(const int conn_fd, const int session_id);
};

} // namespace ftr
#endif
