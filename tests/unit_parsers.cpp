#include "../src/utils/parsers.hpp"
#include "doctest.h"

TEST_SUITE("Unit Parsers") {
    TEST_CASE("wspace") {
        SUBCASE("Skipping wspace") {
            std::stringstream ss{"    t"};
            auto v = utils::parsers::skip_wspace(ss);
            REQUIRE(v.has_value());
            CHECK(v.value() == "");
            CHECK(ss.peek() == 't');
        };

        SUBCASE("Skipping tabspace") {
            std::stringstream ss{"  t"};
            auto v = utils::parsers::skip_wspace(ss);
            REQUIRE(v.has_value());
            CHECK(v.value() == "");
            CHECK(ss.peek() == 't');
        }

        SUBCASE("Skipping tabspace encoded") {
            std::stringstream ss{"\tt"};
            auto v = utils::parsers::skip_wspace(ss);
            REQUIRE(v.has_value());
            CHECK(v.value() == "");
            CHECK(ss.peek() == 't');
        }

        SUBCASE("Skipping no whitespace still holds the parser true") {
            std::stringstream ss{"t"};
            auto v = utils::parsers::skip_wspace(ss);
            REQUIRE(v.has_value());
            CHECK(v.value() == "");
            CHECK(ss.peek() == 't');
        }
    }

    TEST_CASE("Directory") {
        SUBCASE("Directory passes") {
            std::stringstream ss{".././test/testing/test123.mx"};
            auto v = utils::parsers::directory(ss);
            REQUIRE(v.has_value());
            CHECK(v.value() == ".././test/testing/test123.mx");
        };

        SUBCASE("Directory passes") {
            std::stringstream ss{".././test/   testing/test123.mx"};
            auto v = utils::parsers::directory(ss);
            REQUIRE(v.has_value());
            CHECK(v.value() == ".././test/");
        };
    }

    TEST_CASE("Flags") {
        SUBCASE("Flags single") {
            std::stringstream ss{"-t"};
            auto flag = utils::parsers::single_flag(ss);
            REQUIRE(flag.has_value());
            CHECK(flag.value() == 't');
        };

        SUBCASE("Flags double") {
            std::stringstream ss{"--test"};
            auto flag = utils::parsers::double_flag(ss);
            REQUIRE(flag.has_value());
            CHECK(flag.value() == "test");
        };

        SUBCASE("Invalid flag") {
            std::stringstream ss{"-test"};
            auto flag = utils::parsers::double_flag(ss);
            REQUIRE(!flag.has_value());
            CHECK(ss.peek() == '-');

            std::stringstream ss1{"notflag"};
            auto flag1 = utils::parsers::double_flag(ss1);
            REQUIRE(!flag1.has_value());
            CHECK(ss1.peek() == 'n');
        };
    }

    TEST_CASE("identifiers") {
        SUBCASE("Basic identifier passes") {
            std::stringstream ss{"t13"};
            auto v = utils::parsers::identifier(ss);
            REQUIRE(v.has_value());
            CHECK(v.value() == "t13");
            CHECK(mona::_eof()(ss).value());
        };

        SUBCASE("Identifier doesn't pass with leading numeric") {
            std::stringstream ss{"1t3"};
            auto v = utils::parsers::identifier(ss);
            REQUIRE(!v.has_value());
            CHECK(ss.peek() == '1');
        }
        SUBCASE("Identifier doesn't pass with non identifiable symbols") {
            std::stringstream ss{"s%rt"};
            auto v = utils::parsers::identifier(ss);
            REQUIRE(!v.has_value());
            CHECK(ss.peek() == 's');
        }
    }

    TEST_CASE("defines") {
        SUBCASE("Basic define passes") {
            std::stringstream ss{"#define     TES#$!"};
            auto v = utils::parsers::define(ss);
            REQUIRE(v.has_value());
            REQUIRE(v.value().first == "TES");
            REQUIRE(v.value().second == "#$!");
        }

        SUBCASE("Basic define passes") {
            std::stringstream ss{"#define     TES          #$!"};
            auto v = utils::parsers::define(ss);
            REQUIRE(v.has_value());
            REQUIRE(v.value().first == "TES");
            REQUIRE(v.value().second == "#$!");
        }
        SUBCASE("Basic define passes with space(s) in value") {
            std::stringstream ss{"#define     TES          #   $    !"};
            auto v = utils::parsers::define(ss);
            REQUIRE(v.has_value());
            REQUIRE(v.value().first == "TES");
            REQUIRE(v.value().second == "#   $    !");
        }
    }
}
