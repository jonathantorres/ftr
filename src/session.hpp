#ifndef ftr_session_hpp
#define ftr_session_hpp

#include "ftrd.hpp"
#include "log.hpp"
#include <arpa/inet.h>
#include <array>
#include <condition_variable>
#include <filesystem>
#include <memory>
#include <mutex>
#include <netdb.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>

namespace ftr {
class Server;

// the current user logged in for this session
struct SessionUser {
    std::string username;
    std::string password;
    std::string root;
};

class Session {
  public:
    Session() = delete;
    ~Session() = default;
    Session(const int conn_fd, ftr::Server &server,
            std::shared_ptr<ftr::Log> log)
        : m_control_conn_fd{conn_fd},
          m_data_conn_fd{0},
          m_data_conn_port{0},
          m_pass_mode{false},
          m_transfer_in_progress{false},
          m_server{server},
          m_log{log},
          m_transfer_ready{false},
          m_transfer_done{false} {}

    Session(const Session &session) = delete;
    Session(Session &&session) = delete;
    Session &operator=(const Session &session) = delete;
    Session &operator=(Session &&session) = delete;

    void start();
    void end();
    void quit() { run_quit(); }

  private:
    int m_control_conn_fd;
    int m_data_conn_fd;
    unsigned int m_data_conn_port;
    bool m_pass_mode;
    bool m_transfer_in_progress;
    SessionUser m_session_user;
    ftr::Server &m_server;
    std::shared_ptr<ftr::Log> m_log;
    std::string m_transfer_type;
    std::string m_cwd;
    std::string m_rename_from;
    std::mutex m_session_mu;
    std::condition_variable m_session_cv;
    bool m_transfer_ready;
    bool m_transfer_done;

    bool is_logged_in();
    void open_data_conn(const struct sockaddr *conn_addr, socklen_t addr_size);
    void connect_to_data_conn(const unsigned int port, bool use_ipv6);
    void accept_on_data_conn(int listener_fd);
    void transfer_on_data_conn();
    void
    handle_command(const std::array<char, ftr::DEFAULT_CMD_SIZE> &client_cmd);
    void exec_command(const std::string &cmd, const std::string &cmd_params);
    void run_not_implemented();
    void run_user(const std::string &username);
    void run_password(const std::string &password);
    void run_print_dir();
    void run_change_dir(const std::string dir);
    void run_type(const std::string selected_transfer_type);
    void run_passive();
    void run_list(const std::string file);
    void run_file_names(const std::string file);
    void run_retrieve(const std::string filename);
    void run_accept_and_store(const std::string filename, bool append_mode);
    void run_system_type();
    void run_change_parent();
    void run_make_dir(const std::string dir_name);
    void run_remove_dir(const std::string path);
    void run_delete(const std::string filename);
    void run_ext_passv_mode(const std::string &cmd_params);
    void run_port(const std::string &cmd_params);
    void run_ext_addr_port(const std::string &cmd_params);
    void run_help(const std::string &cmd_args);
    void run_noop();
    void run_allo();
    void run_account(const std::string &args);
    void run_site();
    void run_mode(const std::string &args);
    void run_abort();
    void run_file_struct(const std::string &args);
    void run_server_status(const std::string args);
    void run_rename_from(const std::string &cmd_args);
    void run_rename_to(const std::string &cmd_args);
    void run_reinit();
    void run_quit();
};

} // namespace ftr
#endif
