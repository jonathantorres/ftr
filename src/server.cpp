#include "server.hpp"
#include "conf.hpp"
#include "constants.hpp"
#include "exception.hpp"
#include "util.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <iostream>
#include <netdb.h>
#include <regex>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>
#include <vector>

using namespace ftr;

void Server::start(std::unique_ptr<ftr::Conf> &conf) {
    std::cout << "server starting...";

    conf = std::move(conf);
    host = conf->get_server_name();
    port = conf->get_port();

    ctrl_listener_fd = get_server_ctrl_listener();

    if (ctrl_listener_fd < 0) {
        throw ServerError("The hostname could not be resolved");
    }

    listen(ctrl_listener_fd, Server::BACKLOG);

    std::cout << "OK...waiting for connections...\n";

    while (true) {
        int conn_fd = accept(ctrl_listener_fd, nullptr, nullptr);

        if (conn_fd < 0) {
            std::cerr << "accept error: " << std::strerror(errno) << '\n';
            // TODO: check for interrupted system calls
        }

        // TODO: spawn a new thread to handle this client
        handle_conn(conn_fd);
    }
}

void Server::handle_conn(int conn_fd) {
    // handle new client connection here
    send_response(conn_fd, 200, "Hola");
}

void Server::send_response(int conn_fd, int status_code,
                           std::string extra_msg) {
    // TODO
    std::stringstream buf;
    buf << status_code;
    buf << " OK ";
    buf << extra_msg;
    buf << '\n';

    std::string buf_str = buf.str();

    write(conn_fd, buf_str.c_str(), buf_str.size());
}

std::string Server::get_status_code_msg(int status_code) {
    auto sc = ftr::status_codes.find(status_code);

    if (sc != ftr::status_codes.end()) {
        return sc->second;
    }

    return "";
}

std::string Server::get_command_help_msg(std::string cmd) {
    auto msg = ftr::help_messages.find(cmd);

    if (msg != ftr::help_messages.end()) {
        return std::string(msg->second);
    }

    return "";
}

std::string Server::get_all_commands_help_msg() {
    std::stringstream buf;

    for (auto it = ftr::help_messages.begin(); it != ftr::help_messages.end();
         ++it) {
        buf << it->first;
        buf << ": ";
        buf << it->second;
        buf << " ";
    }
    return buf.str();
}

int Server::get_server_ctrl_listener() {
    Server::HostType host_type = validate_server_host();
    int fd = -1;

    if (host_type == Server::HostType::Invalid) {
        throw ServerError("invalid host name");
    }

    struct addrinfo *res, *res_p = nullptr;
    struct addrinfo hints {
        0, AF_UNSPEC, SOCK_STREAM, 0, 0, nullptr, nullptr, nullptr
    };

    int info_res =
        getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &res);

    if (info_res != 0) {
        throw ServerError(gai_strerror(info_res));
    }

    res_p = res;
    while (res_p != NULL) {
        fd = bind_address(res_p);

        if (fd > 0) {
            break;
        }
        res_p = res_p->ai_next;
    }

    freeaddrinfo(res);

    return fd;
}

int Server::bind_address(const struct addrinfo *addr_info) {
    if (!addr_info) {
        // TODO: log this error
        return -1;
    }

    int fd = socket(addr_info->ai_family, addr_info->ai_socktype,
                    addr_info->ai_protocol);

    if (fd < 0) {
        // TODO: log this error
        return -1;
    }

    int opt = 1;
    int res = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

    if (res < 0) {
        // TODO: log this error
        close(fd);
        return -1;
    }

    res = bind(fd, addr_info->ai_addr, addr_info->ai_addrlen);

    if (res < 0) {
        // TODO: log this error
        close(fd);
        return -1;
    }

    return fd;
}

std::string Server::get_addr_string(struct sockaddr *addr) {
    char buf[INET6_ADDRSTRLEN] = {0};

    if (!addr) {
        return std::string("nullptr");
    }

    if (addr->sa_family == AF_INET) {
        sockaddr_in *addr_in = reinterpret_cast<sockaddr_in *>(addr);
        if (inet_ntop(AF_INET, &addr_in->sin_addr, buf, INET6_ADDRSTRLEN) ==
            NULL) {
            return std::string(std::strerror(errno));
        }

        return std::string(buf);
    } else if (addr->sa_family == AF_INET6) {
        sockaddr_in6 *addr_in6 = reinterpret_cast<sockaddr_in6 *>(addr);
        if (inet_ntop(AF_INET6, &addr_in6->sin6_addr, buf, INET6_ADDRSTRLEN) ==
            NULL) {
            return std::string(std::strerror(errno));
        }

        return std::string(buf);
    }

    return std::string("unknown address family");
}

Server::HostType Server::validate_server_host() {
    if (ftr::is_ipv4(host)) {
        // host matches valid ipv4 address
        return Server::HostType::IPv4;
    } else if (ftr::is_ipv6(host)) {
        // host matches a valid ipv6 address
        return Server::HostType::IPv6;
    } else if (ftr::is_domain_name(host)) {
        // host matches valid domain name
        return Server::HostType::DomainName;
    } else if (host == "localhost") {
        // TODO: special case for localhost????
        return Server::HostType::DomainName;
    }

    return Server::HostType::Invalid;
}
