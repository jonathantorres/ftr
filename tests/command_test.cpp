#include "command.hpp"
#include <catch2/catch_test_macros.hpp>
#include <string>

TEST_CASE("parse command line flags") {
    int argc_ = 3;
    const char *argv_[] = {
        "prog",
        "-a",
        "-foo",
    };

    bool a;
    bool foo;
    bool bar;

    ftr::Command c{argc_, argv_};
    c.add_flag('a', a);
    c.add_flag("foo", foo);
    c.add_flag(std::string("bar"), bar);
    c.parse();

    REQUIRE(a);
    REQUIRE(foo == true);
    REQUIRE_FALSE(bar);
}

TEST_CASE("parse command line options") {
    int argc_ = 7;
    const char *argv_[] = {
        "prog", "-one", "thisisone", "-two", "thisistwo", "-a", "another",
    };

    std::string one;
    std::string two;
    std::string another;

    ftr::Command c{argc_, argv_};
    c.add_option('a', another);
    c.add_option("one", one);
    c.add_option("two", two);
    c.parse();

    REQUIRE(another == "another");
    REQUIRE(one == "thisisone");
    REQUIRE(two == "thisistwo");
}

TEST_CASE("unknown flag is found") {
    int argc_ = 3;
    const char *argv_[] = {
        "prog",
        "-foo",
        "-bar",
    };

    bool foo;
    ftr::Command c{argc_, argv_};
    c.add_flag("foo", foo);
    c.parse();

    REQUIRE(foo);
    REQUIRE(c.unknown_value_found());
    REQUIRE(c.unknown_flag() == "-bar");
}
