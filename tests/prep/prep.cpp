#include <sstream>

#include "../../src/monac/prep/parsers.hpp"
#include "../doctest.h"

bool valid_walk(const std::string& str, std::vector<std::string> list) {
    std::string s{""};
    for (int i = 0; i < list.size(); i++) {
        s.append(list[i]);
    }
    return s == str;
};

TEST_SUITE("Prep Parsers") {
    using namespace monac::prep::parsers;

    TEST_CASE("defines") {
        SUBCASE("Replacement list") {
            std::stringstream ss{"#$!"};
            auto v = replacement_list(ss);

            REQUIRE(v.has_value());
            REQUIRE(v.value()[0] == "#$!");
        }

        SUBCASE("Object-like macro") {
            std::stringstream ss{"TES#$!"};
            auto v = object_like(ss);

            REQUIRE(v.has_value());
            REQUIRE(v.value().identifier == "TES");
            REQUIRE(v.value().replacement[0] == "#$!");
        }

        SUBCASE("Function-like macro") {
            std::stringstream ss{"TEST(x, b,   c, d)"};
            auto v = function_like(ss);
            REQUIRE(v.has_value());
            CHECK(v.value().function_like);
            CHECK(valid_walk("xbcd", v.value().arguments));
        }

        SUBCASE("Function-like macro with empty args") {
            std::stringstream ss{"TEST()"};
            auto v = function_like(ss);
            REQUIRE(v.has_value());
        }

        SUBCASE("Macro definition") {
            std::stringstream ss{"TES#$!"};
            auto v = macro_def(ss);
            REQUIRE(v.has_value());
            REQUIRE(v.value().identifier == "TES");
        }

        SUBCASE("Define full composition") {
            std::stringstream ss{"@define     TES#$!"};
            auto v = define(ss);
            REQUIRE(v.has_value());
            CHECK(!v.value().function_like);
            CHECK(v.value().identifier == "TES");
            CHECK(v.value().replacement[0] == "#$!");
        }

        SUBCASE("Define full composition 2") {
            std::stringstream ss{"@define     TES  # $ !"};
            auto v = define(ss);
            REQUIRE(v.has_value());
            CHECK(!v.value().function_like);
            CHECK(v.value().identifier == "TES");
            CHECK(v.value().replacement[0] == "#");
        }
    }
}
