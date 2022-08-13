#ifndef ftr_session_hpp
#define ftr_session_hpp

#include "constants.hpp"
#include <arpa/inet.h>
#include <array>
#include <condition_variable>
#include <filesystem>
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
    Session() = default;
    ~Session() = default;
    Session(int conn_fd, ftr::Server &server, int id)
        : id{id}, control_conn_fd{conn_fd}, data_conn_fd{0}, pass_mode{false},
          transfer_in_progress{false}, server{server}, transfer_ready{false},
          transfer_done{false} {}

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
    unsigned int data_conn_port;
    bool pass_mode;
    bool transfer_in_progress;
    SessionUser session_user;
    ftr::Server &server;
    std::string transfer_type;
    std::string cwd;
    std::string rename_from;
    std::mutex session_mu;
    std::condition_variable session_cv;
    bool transfer_ready;
    bool transfer_done;

    bool is_logged_in();
    std::string get_file_line(std::filesystem::directory_entry entry);
    void open_data_conn(struct sockaddr *conn_addr);
    void connect_to_data_conn(unsigned int port, bool use_ipv6);
    void accept_on_data_conn(int listener_fd);
    void transfer_on_data_conn();
    void handle_command(std::array<char, ftr::DEFAULT_CMD_SIZE> &client_cmd);
    void exec_command(std::string cmd, std::string cmd_params);
    void run_not_implemented();
    void run_user(std::string username);
    void run_password(std::string password);
    void run_print_dir();
    void run_change_dir(std::string dir);
    void run_type(std::string selected_transfer_type);
    void run_passive();
    void run_list(std::string file);
    void run_file_names(std::string file);
    void run_retrieve(std::string filename);
    void run_accept_and_store(std::string filename, bool append_mode);
    void run_system_type();
    void run_change_parent();
    void run_make_dir(std::string dir_name);
    void run_remove_dir(std::string path);
    void run_delete(std::string filename);
    void run_ext_passv_mode();
    void run_port(std::string cmd_params);
    void run_ext_addr_port();
    void run_help(std::string cmd_args);
    void run_noop();
    void run_allo();
    void run_account(std::string args);
    void run_site();
    void run_mode(std::string args);
    void run_abort();
    void run_file_struct(std::string args);
    void run_server_status(std::string args);
    void run_rename_from(std::string cmd_args);
    void run_rename_to(std::string cmd_args);
    void run_reinit();
    void run_quit();
};

} // namespace ftr
#endif
