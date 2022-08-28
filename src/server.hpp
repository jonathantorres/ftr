#ifndef ftr_server_hpp
#define ftr_server_hpp

#include "conf.hpp"
#include "constants.hpp"
#include "log.hpp"
#include "session.hpp"
#include <arpa/inet.h>
#include <map>
#include <memory>
#include <netdb.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>

namespace ftr {

class server {
  public:
    server()
        : m_conf{nullptr}, m_port{0}, m_ctrl_listener_fd{0},
          m_is_reloading{false}, m_is_shutting_down{false} {};
    ~server() = default;

    server(const server &server) = delete;
    server(server &&server) = delete;
    server &operator=(const server &server) = delete;
    server &operator=(server &&server) = delete;

    void start(const std::shared_ptr<ftr::conf> conf,
               const std::shared_ptr<ftr::log> log);
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
    const std::shared_ptr<ftr::conf> get_conf() { return m_conf; }
    int find_open_addr(bool use_ipv6);
    std::string get_addr_string(struct sockaddr *addr);

  private:
    std::string m_host;
    std::string m_resolved_host;
    std::shared_ptr<ftr::conf> m_conf;
    std::shared_ptr<ftr::log> m_log;
    std::map<int, std::shared_ptr<ftr::session>> m_sessions;
    int m_port;
    int m_ctrl_listener_fd;
    bool m_is_reloading;
    bool m_is_shutting_down;

    enum class HostType {
        DomainName,
        IPv4,
        IPv6,
        Invalid,
    };

    int get_server_ctrl_listener();
    HostType validate_server_host();
    int bind_address(const struct addrinfo *addr_info);
    void handle_conn(const int conn_fd);
};

} // namespace ftr
#endif
