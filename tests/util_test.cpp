#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "util.hpp"
#include <string>
#include <vector>

TEST_CASE("whitespace is trimmed from a string") {
    std::string s1("foo");
    std::string s2(" bar");
    std::string s3("baz ");
    std::string s4(" this ");
    std::string s5("this is a string");
    std::string s6("");

    REQUIRE(ftr::trim_whitespace(s1) == "foo");
    REQUIRE(ftr::trim_whitespace(s2) == "bar");
    REQUIRE(ftr::trim_whitespace(s3) == "baz");
    REQUIRE(ftr::trim_whitespace(s4) == "this");
    REQUIRE(ftr::trim_whitespace(s5) == "this is a string");
    REQUIRE(ftr::trim_whitespace(s6) == "");
}

TEST_CASE("right whitespace is trimmed from a string") {
    std::string s1("foo");
    std::string s2(" bar");
    std::string s3("baz ");
    std::string s4(" this ");
    std::string s5("this is a string");
    std::string s6("");

    REQUIRE(ftr::trim_right(s1) == "foo");
    REQUIRE(ftr::trim_right(s2) == " bar");
    REQUIRE(ftr::trim_right(s3) == "baz");
    REQUIRE(ftr::trim_right(s4) == " this");
    REQUIRE(ftr::trim_right(s5) == "this is a string");
    REQUIRE(ftr::trim_right(s6) == "");
}

TEST_CASE("left whitespace is trimmed from a string") {
    std::string s1("foo");
    std::string s2(" bar");
    std::string s3("baz ");
    std::string s4(" this ");
    std::string s5("this is a string");
    std::string s6("");

    REQUIRE(ftr::trim_left(s1) == "foo");
    REQUIRE(ftr::trim_left(s2) == "bar");
    REQUIRE(ftr::trim_left(s3) == "baz ");
    REQUIRE(ftr::trim_left(s4) == "this ");
    REQUIRE(ftr::trim_left(s5) == "this is a string");
    REQUIRE(ftr::trim_left(s6) == "");
}

TEST_CASE("string is converted to lower case") {
    std::string s1("foo");
    std::string s2("FOO");
    std::string s3("Jonathan");
    std::string s4("THIS iS aNoTHer STRINg");
    std::string s5("this ONE contains numbers 3 4 7575");

    REQUIRE(ftr::to_lower(s1) == "foo");
    REQUIRE(ftr::to_lower(s2) == "foo");
    REQUIRE(ftr::to_lower(s3) == "jonathan");
    REQUIRE(ftr::to_lower(s4) == "this is another string");
    REQUIRE(ftr::to_lower(s5) == "this one contains numbers 3 4 7575");
}

TEST_CASE("string is converted to upper case") {
    std::string s1("foo");
    std::string s2("FOO");
    std::string s3("Jonathan");
    std::string s4("THIS iS aNoTHer STRINg");
    std::string s5("this ONE contains numbers 3 4 7575");

    REQUIRE(ftr::to_upper(s1) == "FOO");
    REQUIRE(ftr::to_upper(s2) == "FOO");
    REQUIRE(ftr::to_upper(s3) == "JONATHAN");
    REQUIRE(ftr::to_upper(s4) == "THIS IS ANOTHER STRING");
    REQUIRE(ftr::to_upper(s5) == "THIS ONE CONTAINS NUMBERS 3 4 7575");
}

TEST_CASE("string is split into a vector of strings") {
    std::vector<std::string> it1 =
        ftr::split(std::string("one/two/three"), std::string("/"));

    REQUIRE(it1.size() == 3);
    REQUIRE(it1[0] == "one");
    REQUIRE(it1[1] == "two");
    REQUIRE(it1[2] == "three");

    std::vector<std::string> it2 =
        ftr::split(std::string("a.b.c"), std::string("."));

    REQUIRE(it2.size() == 3);
    REQUIRE(it2[0] == "a");
    REQUIRE(it2[1] == "b");
    REQUIRE(it2[2] == "c");

    std::vector<std::string> it3 =
        ftr::split(std::string("a-b-c"), std::string(""));

    REQUIRE(it3.size() == 1);
    REQUIRE(it3[0] == "a-b-c");
}

TEST_CASE("vector of strings is joined into a string") {
    std::vector<std::string> v1 = {"a", "b", "c"};
    std::vector<std::string> v2 = {"one", "two", "three", "four"};

    REQUIRE(ftr::join(v1, ",") == "a,b,c");
    REQUIRE(ftr::join(v2, "--") == "one--two--three--four");
}

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
