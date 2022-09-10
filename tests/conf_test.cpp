#include "conf.hpp"
#include <catch2/catch_test_macros.hpp>
#include <config.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace ftr;

std::string prefix(FTR_PREFIX);

TEST_CASE("load configuration file") {
    const std::string conf_file(prefix + "/tests/confs/basic.conf");
    std::cerr << "path: " << prefix + "/tests/confs/basic.conf\n";
    conf c;
    c.load(conf_file);

    REQUIRE(c.get_server_name() == std::string("localhost"));
    REQUIRE(c.get_root() == std::string("/Users/jonathantorres/ftr_test"));
    REQUIRE(c.get_port() == 20);
    REQUIRE(c.get_error_log() == std::string("/etc/log/ftr/errors.log"));
    REQUIRE(c.get_access_log() == std::string("/etc/log/ftr/access.log"));
    REQUIRE(c.get_users().size() != 0);
}

TEST_CASE("higly commented conf file is properly parsed") {
    const std::string conf_file(prefix + "/tests/confs/commented.conf");
    conf c;
    c.load(conf_file);

    REQUIRE(c.get_server_name() == std::string("localhost"));
    REQUIRE(c.get_root() == std::string("/home/jt/ftr_test"));
    REQUIRE(c.get_port() == 20);
    REQUIRE(c.get_error_log() == std::string("/etc/log/ftr/errors.log"));
    REQUIRE(c.get_access_log() == std::string("/etc/log/ftr/access.log"));
    REQUIRE(c.get_users().size() != 0);
    REQUIRE(c.get_users().size() == 3);
}

TEST_CASE("configuration with no users") {
    const std::string conf_file(prefix + "/tests/confs/no_users.conf");
    conf c;
    c.load(conf_file);

    REQUIRE(c.get_server_name() == std::string("127.0.0.1"));
    REQUIRE(c.get_root() == std::string("/home/jt/ftr_test"));
    REQUIRE(c.get_port() == 21);
    REQUIRE(c.get_error_log() == std::string("/etc/log/ftr/errors.log"));
    REQUIRE(c.get_access_log() == std::string("/etc/log/ftr/access.log"));
    REQUIRE(c.get_users().size() == 0);
}

TEST_CASE("users are created") {
    const std::string conf_file(prefix + "/tests/confs/multiple_users.conf");
    conf c;
    c.load(conf_file);

    REQUIRE(c.get_users().size() == 5);
    auto users = c.get_users();
    int i = 1;

    for (auto it = users.begin(); it != users.end(); ++it, ++i) {
        auto user = (*it).get();
        std::string idx = std::to_string(i);

        REQUIRE(user->get_username() == std::string("test" + idx));
        REQUIRE(user->get_password() == std::string("test" + idx));
        REQUIRE(user->get_root() == std::string("/test" + idx));
    }
}
