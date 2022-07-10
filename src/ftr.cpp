#include "conf.hpp"
#include "log.hpp"
#include "server.hpp"
#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory>
#include <signal.h>
#include <string>
#include <sys/wait.h>

const std::string VERSION = "0.0.1";

// TODO: this will be changed soon
std::string prefix = "/home/jonathan/dev/ftr/";

void parse_opts(int argc, char **argv);
void print_usage();
void print_version();
void handle_signals();

int main(int argc, char **argv) {
    // TODO: set the prefix of the server
    // at compile time

    ftr::Log log;
    std::shared_ptr<ftr::Conf> conf = std::make_shared<ftr::Conf>();
    std::shared_ptr<ftr::Server> server = std::make_shared<ftr::Server>();

    parse_opts(argc, argv);

    try {
        // load and test the configuration file
        log.init();
        conf->load(prefix + ftr::Server::DEFAULT_CONF, "");
    } catch (std::exception &e) {
        std::cerr << "server configuration error: " << e.what() << "\n";
        std::exit(EXIT_FAILURE);
    }

    handle_signals();

    try {
        server->start(conf);
    } catch (std::exception &e) {
        std::cerr << "server error: " << e.what() << "\n";
        std::exit(EXIT_FAILURE);
    }

    return 0;
}

void parse_opts(int argc, char **argv) {
    if (argc <= 1) {
        return;
    }

    for (int i = 1; i < argc; i++) {
        std::string cur_arg(argv[i]);
        if (cur_arg == "-v") {
            // print version and exit
            print_version();
        } else if (cur_arg == "-h") {
            // print help and exit
            print_usage();
        } else if (cur_arg == "-t") {
            // TODO: test the configuration file
            std::cerr << "Testing the configuration file...\n";
            std::exit(EXIT_SUCCESS);
        } else if (cur_arg == "-c") {
            i++;
            if (argv[i] == NULL) {
                std::cerr << "Error! a configuration file is required\n";
                std::exit(EXIT_FAILURE);
            }
            // TODO: a configuration file was specified
            std::cout << "using conf file: " << argv[i] << "\n";
        } else if (cur_arg == "-p") {
            i++;
            if (argv[i] == NULL) {
                std::cerr << "Error! the path of the new prefix is required\n";
                std::exit(EXIT_FAILURE);
            }
            // TODO: a new prefix was specified
            std::cout << "using prefix: " << argv[i] << "\n";
        } else {
            std::cerr << "Unknown option \"" << argv[i]
                      << "\" please try again\n";
            std::exit(EXIT_FAILURE);
        }
    }
}

void wait_for_children() {
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0) {
        // do nothing for now
        // maybe we should log this
    }
    return;
}

void sig_handler(int signum) {
    switch (signum) {
    case SIGTERM:
    case SIGINT:
    case SIGQUIT:
        // TODO: the server should shutdown gracefully
        std::cerr << "Shutting down server...\n";
        std::exit(EXIT_SUCCESS);
        break;
    case SIGHUP:
        // TODO: shutdown the server and restart it with the new configuration
        // file make sure the configuration file is validated first!
        std::cerr << "Reloading the configuration file...\n";
        std::exit(EXIT_SUCCESS);
        break;
    case SIGCHLD:
        wait_for_children();
        break;
    }
}

void handle_signals() {
    struct sigaction act;
    act.sa_handler = sig_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESTART;

    // server shutdown signals
    if (sigaction(SIGTERM, &act, NULL) == -1) {
        // TODO: log the error
    }
    if (sigaction(SIGINT, &act, NULL) == -1) {
        // TODO: log the error
    }
    if (sigaction(SIGQUIT, &act, NULL) == -1) {
        // TODO: log the error
    }

    // server restart signal
    if (sigaction(SIGHUP, &act, NULL) == -1) {
        // TODO: log the error
    }

    // handling of terminated children
    if (sigaction(SIGCHLD, &act, NULL) == -1) {
        // TODO: log the error
    }
}

void print_usage() {
    std::cerr << "Usage: ftr -[htv] [-p prefix] [-c conf]\n";
    std::cerr << "\n";
    std::cerr << "Options:\n";
    std::cerr << "  -h\t\t: This help menu\n";
    std::cerr << "  -v\t\t: Show server version and exit\n";
    std::cerr << "  -t\t\t: Test the configuration file and exit\n";
    std::cerr << "  -p prefix\t: Set the path of the prefix\n";
    std::cerr << "  -c filename\t: Use the specified configuration file\n";
    std::exit(EXIT_SUCCESS);
}

void print_version() {
    std::cerr << "ftr version v" << VERSION << "\n";
    std::exit(EXIT_SUCCESS);
}
