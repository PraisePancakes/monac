#include <sstream>

#include "../src/cli/parsers.hpp"
#include "doctest.h"

TEST_SUITE("Cli Parsers") {
    TEST_CASE("Directory") {
        SUBCASE("Directory passes") {
            std::stringstream ss{".././test/testing/test123.mx"};
            auto v = cli::parsers::directory(ss);
            REQUIRE(v.has_value());
            CHECK(v.value() == ".././test/testing/test123.mx");
        };

        SUBCASE("Directory passes") {
            std::stringstream ss{".././test/   testing/test123.mx"};
            auto v = cli::parsers::directory(ss);
            REQUIRE(v.has_value());
            CHECK(v.value() == ".././test/");
        };
    }

    TEST_CASE("Flags") {
        SUBCASE("Flags single") {
            std::stringstream ss{"-t"};
            auto flag = cli::parsers::single_flag(ss);
            REQUIRE(flag.has_value());
            CHECK(flag.value() == 't');
        };

        SUBCASE("Flags double") {
            std::stringstream ss{"--test"};
            auto flag = cli::parsers::double_flag(ss);
            REQUIRE(flag.has_value());
            CHECK(flag.value() == "test");
        };

        SUBCASE("Invalid flag") {
            std::stringstream ss{"-test"};
            auto flag = cli::parsers::double_flag(ss);
            REQUIRE(!flag.has_value());
            CHECK(ss.peek() == '-');

            std::stringstream ss1{"notflag"};
            auto flag1 = cli::parsers::double_flag(ss1);
            REQUIRE(!flag1.has_value());
            CHECK(ss1.peek() == 'n');
        };
    }
}
