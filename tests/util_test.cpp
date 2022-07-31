#include "util.hpp"
#include <catch2/catch_test_macros.hpp>
#include <string>

TEST_CASE("parse IPv4 address") {
    std::string s1("192.168.1.1");
    std::string s2("127.0.0.1");
    std::string s3("0.0.0.0");
    std::string s4("255.255.255.255");
    std::string s5("256.256.256.256");
    std::string s6("999.999.999.999");
    std::string s7("1.2.3");
    std::string s8("1.2.3.4");

    REQUIRE(ftr::is_ipv4(s1) == true);
    REQUIRE(ftr::is_ipv4(s2) == true);
    REQUIRE(ftr::is_ipv4(s3) == true);
    REQUIRE(ftr::is_ipv4(s4) == true);
    REQUIRE(ftr::is_ipv4(s5) == false);
    REQUIRE(ftr::is_ipv4(s6) == false);
    REQUIRE(ftr::is_ipv4(s7) == false);
    REQUIRE(ftr::is_ipv4(s8) == true);
}

TEST_CASE("parse IPv6 address") {
    std::string s1("2001:0db8:85a3:0000:0000:8a2e:0370:7334");
    std::string s2("FE80:0000:0000:0000:0202:B3FF:FE1E:8329");
    std::string s3("192.168.1.1");
    std::string s4("test:test:test:test:test:test:test:test");
    std::string s5("::1");
    std::string s6("ff02::1");
    std::string s7("ff02::2");

    REQUIRE(ftr::is_ipv6(s1) == true);
    REQUIRE(ftr::is_ipv6(s2) == true);
    REQUIRE(ftr::is_ipv6(s3) == false);
    REQUIRE(ftr::is_ipv6(s4) == false);
    REQUIRE(ftr::is_ipv6(s5) == true);
    REQUIRE(ftr::is_ipv6(s6) == true);
    REQUIRE(ftr::is_ipv6(s7) == true);
}

TEST_CASE("parse domain name") {
    std::string s1("foodemo.net");
    std::string s2("bar.ba.test.co.uk");
    std::string s3("www.demo.com");
    std::string s4("jonathantorres.com");
    std::string s5("g.com");
    std::string s6("foo,com");
    std::string s7(".com");
    std::string s8("j-.com");

    REQUIRE(ftr::is_domain_name(s1) == true);
    REQUIRE(ftr::is_domain_name(s2) == true);
    REQUIRE(ftr::is_domain_name(s3) == true);
    REQUIRE(ftr::is_domain_name(s4) == true);
    REQUIRE(ftr::is_domain_name(s5) == true);
    REQUIRE(ftr::is_domain_name(s6) == false);
    REQUIRE(ftr::is_domain_name(s7) == false);
    REQUIRE(ftr::is_domain_name(s8) == false);
}
