#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "util.hpp"
#include <string>

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
