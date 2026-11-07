#include "../src/utils/parsers.hpp"
#include "doctest.h"

TEST_CASE("Skipping wspace") {
    std::stringstream ss{"    t"};
    auto v = utils::parsers::skip_wspace(ss);
    REQUIRE(v.has_value());
    CHECK(v.value() == "");
    CHECK(ss.peek() == 't');
};

TEST_CASE("Skipping tabspace") {
    std::stringstream ss{"  t"};
    auto v = utils::parsers::skip_wspace(ss);
    REQUIRE(v.has_value());
    CHECK(v.value() == "");
    CHECK(ss.peek() == 't');
}

TEST_CASE("Skipping tabspace encoded") {
    std::stringstream ss{"\tt"};
    auto v = utils::parsers::skip_wspace(ss);
    REQUIRE(v.has_value());
    CHECK(v.value() == "");
    CHECK(ss.peek() == 't');
}

TEST_CASE("Skipping no whitespace still holds the parser true") {
    std::stringstream ss{"t"};
    auto v = utils::parsers::skip_wspace(ss);
    REQUIRE(v.has_value());
    CHECK(v.value() == "");
    CHECK(ss.peek() == 't');
}
