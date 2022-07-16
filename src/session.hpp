#ifndef ftr_session_hpp
#define ftr_session_hpp

#include "constants.hpp"
#include <arpa/inet.h>
#include <array>
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
    Session() = default;
    ~Session() = default;
    Session(int conn_fd, ftr::Server &server, int id)
        : id{id}, control_conn_fd{conn_fd}, data_conn_fd{0}, pass_mode{false},
          transfer_in_progress{false}, server{server} {}

    Session(const Session &session) = delete;
    Session(Session &&session) = delete;
    Session &operator=(const Session &session) = delete;
    Session &operator=(Session &&session) = delete;

    void start();
    void end();

  private:
    int id;
    int control_conn_fd;
    int data_conn_fd;
    bool pass_mode;
    bool transfer_in_progress;
    SessionUser session_user;
    ftr::Server &server;
    std::string transfer_type;
    std::string cwd;
    std::string rename_from;

    void open_data_conn(struct sockaddr *conn_addr);
    void connect_to_data_conn(int port);
    void accept_on_data_conn(int fd);
    void handle_data_transfer(int conn_fd);
    void handle_command(std::array<char, ftr::DEFAULT_CMD_SIZE> &client_cmd);
    void exec_command(std::string cmd, std::string cmd_params);
    void run_not_implemented();
    void run_user(std::string username);
    void run_password(std::string password);
    void run_print_dir();
    void run_change_dir();
    void run_type(std::string selected_transfer_type);
    void run_passive();
    void run_list();
    void run_file_names();
    void run_retrieve();
    void run_accept_and_store();
    void run_system_type();
    void run_change_parent();
    void run_make_dir();
    void run_remove_dir();
    void run_delete();
    void run_ext_passv_mode();
    void run_port();
    void run_ext_addr_port();
    void run_help(std::string cmd_args);
    void run_noop();
    void run_allo();
    void run_account();
    void run_site();
    void run_mode();
    void run_abort();
    void run_file_struct(std::string args);
    void run_server_status();
    void run_rename_from();
    void run_rename_to();
    void run_reinit();
    void run_quit();
};

} // namespace ftr
#endif
