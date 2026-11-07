#include "../src/utils/parsers.hpp"
#include "doctest.h"

TEST_CASE("Flags single") {
    std::stringstream ss{"-t"};
    auto flag = utils::parsers::single_flag(ss);
    REQUIRE(flag.has_value());
    CHECK(flag.value() == 't');
};

TEST_CASE("Flags double") {
    std::stringstream ss{"--test"};
    auto flag = utils::parsers::double_flag(ss);
    REQUIRE(flag.has_value());
    CHECK(flag.value() == "test");
};

TEST_CASE("Invalid flag") {
    std::stringstream ss{"-test"};
    auto flag = utils::parsers::double_flag(ss);
    REQUIRE(!flag.has_value());
    CHECK(ss.peek() == '-');

    std::stringstream ss1{"notflag"};
    auto flag1 = utils::parsers::double_flag(ss1);
    REQUIRE(!flag1.has_value());
    CHECK(ss1.peek() == 'n');
};
