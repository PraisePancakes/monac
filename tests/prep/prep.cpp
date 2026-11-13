#include <sstream>

#include "../../src/monac/prep/parsers.hpp"
#include "../doctest.h"

bool valid_walk(const std::string& str, std::vector<std::string> list) {
    std::string s{""};
    for (int i = 0; i < list.size(); i++) {
        s.append(list[i]);
    }
    std::cout << s << std::endl;
    return s == str;
};

TEST_SUITE("Prep Parsers") {
    using namespace monac::prep::unit;

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

    TEST_CASE("defines") {
        SUBCASE("Identifier") {
            std::stringstream ss{"TES#$!"};
            auto v = identifier(ss);

            REQUIRE(v.has_value());
            CHECK(v.value() == "TES");
        }

        SUBCASE("Whitespace") {
            std::stringstream ss{"     TES"};
            auto v = white_space(ss);

            REQUIRE(v.has_value());
        }

        SUBCASE("Replacement list") {
            std::stringstream ss{"#$!"};
            auto v = replacement_list(ss);

            REQUIRE(v.has_value());
            REQUIRE(v.value()[0] == "#$!");
        }

        SUBCASE("Replacement list works with continuation") {
            std::stringstream ss{"$\\\n\ra\\\n\r\\\n\r\\\n\rb\\c"};
            auto v = replacement_list(ss);
            REQUIRE(v.has_value());
            CHECK(valid_walk("$ a b\\c", v.value()));
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

        SUBCASE("Define prefix") {
            std::stringstream ss{"@define     TES#$!"};
            auto v = mona::_sequence(
                mona::_match("@define"),
                white_space)(ss);

            REQUIRE(v.has_value());
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

        SUBCASE("Basic define works with more than one multilines") {
            std::stringstream ss{"@define X 4\\\n\r3;\\\n\r45abc"};
            auto v = define(ss);
            REQUIRE(v.has_value());
            auto md = v.value();
            CHECK(!md.function_like);
            CHECK(md.arguments.empty());
            CHECK(md.identifier == "X");
            CHECK(valid_walk("4 3; 45abc", md.replacement));
        }

        SUBCASE("Basic define works with more than one multilines and spaces") {
            std::stringstream ss{"@define X 4\\\n\r3;\\\n\r     45abc"};
            auto v = define(ss);
            REQUIRE(v.has_value());
            auto md = v.value();
            CHECK(!md.function_like);
            CHECK(md.arguments.empty());
            CHECK(md.identifier == "X");
            CHECK(valid_walk("4 3; 45abc", md.replacement));
        }

        SUBCASE("Define source code") {
            std::stringstream ss{R"(@define FACTOR 5 \
         + 3 \
         -4)"};
            auto v = define(ss);
            REQUIRE(v.has_value());
            auto md = v.value();
            CHECK(!md.function_like);
            CHECK(md.arguments.empty());
            CHECK(md.identifier == "FACTOR");
            CHECK(valid_walk("5 + 3 -4", md.replacement));
        }
    }
}
