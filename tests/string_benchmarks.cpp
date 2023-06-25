#include "string.hpp"
#include <catch2/benchmark/catch_benchmark_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>
#include <vector>

TEST_CASE("benchmarks using a std::string") {
    std::string s1("   this is a string                ");
    std::string s2("this ONE contains numbers 3 4 7575");
    std::string s3("This is a string");

    REQUIRE(ftr::trim_whitespace(s1) == "this is a string");
    REQUIRE(ftr::trim(s1) == "this is a string");
    REQUIRE(ftr::trim_right(s1) == "   this is a string");
    REQUIRE(ftr::trim_left(s1) == "this is a string                ");
    REQUIRE(ftr::to_lower(s2) == "this one contains numbers 3 4 7575");
    REQUIRE(ftr::to_upper(s2) == "THIS ONE CONTAINS NUMBERS 3 4 7575");
    REQUIRE(ftr::starts_with(s3, "T") == true);
    REQUIRE(ftr::ends_with(s3, "g") == true);
    REQUIRE(ftr::contains(s3, "s") == true);

    std::string sp1("one/two/three");
    std::string sp2("/");
    std::vector<std::string> it1 = ftr::split(sp1, sp2);

    REQUIRE(it1.size() == 3);
    REQUIRE(it1[0] == "one");
    REQUIRE(it1[1] == "two");
    REQUIRE(it1[2] == "three");

    std::vector<std::string> v1 = {"a", "b", "c"};

    REQUIRE(ftr::join<std::vector<std::string>>(v1, ",") == "a,b,c");

    BENCHMARK("trim_whitespace") { return ftr::trim_whitespace(s1); };
    BENCHMARK("trim") { return ftr::trim(s1); };
    BENCHMARK("trim_right") { return ftr::trim_right(s1); };
    BENCHMARK("trim_left") { return ftr::trim_left(s1); };
    BENCHMARK("to_lower") { return ftr::to_lower(s2); };
    BENCHMARK("to_upper") { return ftr::to_upper(s2); };
    BENCHMARK("split") { return ftr::split(sp1, sp2); };
    BENCHMARK("starts_with") { return ftr::starts_with(s3, "T"); };
    BENCHMARK("ends_with") { return ftr::ends_with(s3, "g"); };
    BENCHMARK("contains") { return ftr::contains(s3, "s"); };
    BENCHMARK("join") { return ftr::join<std::vector<std::string>>(v1, ","); };
}
