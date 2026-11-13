#include "../src/utils/unit_parsers.hpp"

#include <sstream>

#include "doctest.h"
TEST_SUITE("Unit Parsers") {
    using namespace utils::unit;
    TEST_CASE("wspace") {
        SUBCASE("Skipping wspace") {
            std::stringstream ss{"    t"};
            auto v = opt_white_space(ss);
            REQUIRE(v.has_value());
            CHECK(v.value() == " ");
            CHECK(ss.peek() == 't');
        };

        SUBCASE("Skipping tabspace") {
            std::stringstream ss{"  t"};
            auto v = opt_white_space(ss);
            REQUIRE(v.has_value());
            CHECK(v.value() == " ");
            CHECK(ss.peek() == 't');
        }

        SUBCASE("Skipping tabspace encoded") {
            std::stringstream ss{"\tt"};
            auto v = opt_white_space(ss);
            REQUIRE(v.has_value());
            CHECK(v.value() == " ");
            CHECK(ss.peek() == 't');
        }

        SUBCASE("Skipping no whitespace still holds the parser true") {
            std::stringstream ss{"t"};
            auto v = opt_white_space(ss);
            REQUIRE(v.has_value());
            CHECK(v.value() == "");
            CHECK(ss.peek() == 't');
        }
    }

    TEST_CASE("identifiers") {
        SUBCASE("Basic identifier passes") {
            std::stringstream ss{"t13"};
            auto v = identifier(ss);
            REQUIRE(v.has_value());
            CHECK(v.value() == "t13");
            CHECK(mona::_eof()(ss).value());
        };

        SUBCASE("Digit is not identifier") {
            std::stringstream ss{"1"};
            auto v = identifier(ss);
            REQUIRE(!v.has_value());
        }

        SUBCASE("Identifier doesn't pass with leading numeric") {
            std::stringstream ss{"1t3"};
            auto v = identifier(ss);
            REQUIRE(!v.has_value());
            CHECK(ss.peek() == '1');
        }
        SUBCASE("Identifier doesn't parse non identifiable symbols") {
            std::stringstream ss{"s%rt"};
            auto v = identifier(ss);
            REQUIRE(v.has_value());
            CHECK(v.value() == "s");
        }

        SUBCASE("Identifier parses weird mix") {
            std::stringstream ss{"TES#$!"};
            auto v = identifier(ss);
            REQUIRE(v.has_value());
            CHECK(v.value() == "TES");
        }
    }
}
