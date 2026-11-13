#include <sstream>

#include "../src/monac/prep/parsers.hpp"
#include "../src/utils/parsers.hpp"
#include "doctest.h"
TEST_SUITE("Unit Parsers") {
    using namespace monac::prep::unit;
    TEST_CASE("wspace") {
        SUBCASE("Skipping wspace") {
            std::stringstream ss{"    t"};
            auto v = opt_white_space(ss);
            REQUIRE(v.has_value());
            CHECK(v.value() == "");
            CHECK(ss.peek() == 't');
        };

        SUBCASE("Skipping tabspace") {
            std::stringstream ss{"  t"};
            auto v = opt_white_space(ss);
            REQUIRE(v.has_value());
            CHECK(v.value() == "");
            CHECK(ss.peek() == 't');
        }

        SUBCASE("Skipping tabspace encoded") {
            std::stringstream ss{"\tt"};
            auto v = opt_white_space(ss);
            REQUIRE(v.has_value());
            CHECK(v.value() == "");
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

        SUBCASE("Digit is not identifier") {
            std::stringstream ss{"1"};
            auto v = utils::parsers::identifier(ss);
            REQUIRE(!v.has_value());
        }

        SUBCASE("Identifier doesn't pass with leading numeric") {
            std::stringstream ss{"1t3"};
            auto v = utils::parsers::identifier(ss);
            REQUIRE(!v.has_value());
            CHECK(ss.peek() == '1');
        }
        SUBCASE("Identifier doesn't parse non identifiable symbols") {
            std::stringstream ss{"s%rt"};
            auto v = utils::parsers::identifier(ss);
            REQUIRE(v.has_value());
            CHECK(v.value() == "s");
        }
    }

    TEST_CASE("define values") {
        SUBCASE("Basic define value passes") {
            std::stringstream ss{"123ab./"};
            auto v = utils::parsers::define_value(ss);
            REQUIRE(v.has_value());
            CHECK(v.value() == "123ab./");
        }

        SUBCASE("Define value with newlines passes") {
            std::stringstream ss{"123\\\n\rab"};
            auto v = utils::parsers::define_value(ss);
            REQUIRE(v.has_value());
            CHECK(v.value() == "123 ab");
        }

        SUBCASE("Define value with multi-lines passes") {
            std::stringstream ss{"123\\\n\rab\\\n\rbc ;"};
            auto v = utils::parsers::define_value(ss);
            REQUIRE(v.has_value());
            CHECK(v.value() == "123 ab bc ;");
        }

        SUBCASE("Define value doesn't confuse backslash with new line") {
            std::stringstream ss{"123\\ab\\\n\rbc ;"};
            auto v = utils::parsers::define_value(ss);
            REQUIRE(v.has_value());
            CHECK(v.value() == "123\\ab bc ;");
        };

        SUBCASE("Define value stops after non-define multiline") {
            std::stringstream ss{"123\\\n\rab\\c;\n\rd"};
            auto v = utils::parsers::define_value(ss);
            REQUIRE(v.has_value());
            CHECK(v.value() == "123 ab\\c;");
            CHECK(ss.peek() == '\n');
        }
    }

    TEST_CASE("defines") {
        SUBCASE("Basic define passes") {
            std::stringstream ss{"@define     TES#$!"};
            auto v = utils::parsers::define(ss);
            REQUIRE(v.has_value());
            REQUIRE(v.value().first == "TES");
            REQUIRE(v.value().second == "#$!");
        }

        SUBCASE("Basic define passes") {
            std::stringstream ss{"@define     TES          #$!"};
            auto v = utils::parsers::define(ss);
            REQUIRE(v.has_value());
            REQUIRE(v.value().first == "TES");
            REQUIRE(v.value().second == "#$!");
        }
        SUBCASE("Basic define passes with space(s) in value") {
            std::stringstream ss{"@define     TES          #   $    !"};
            auto v = utils::parsers::define(ss);
            REQUIRE(v.has_value());
            REQUIRE(v.value().first == "TES");
            REQUIRE(v.value().second == "#   $    !");
        }

        SUBCASE("Basic define works with multilines") {
            std::stringstream ss{"@define X 4\\\n\r3;"};
            auto v = utils::parsers::define(ss);
            REQUIRE(v.has_value());
            REQUIRE(v.value().first == "X");
            REQUIRE(v.value().second == "4 3;");
        }

        SUBCASE("Basic define works with more than one multilines") {
            std::stringstream ss{"@define X 4\\\n\r3;\\\n\r45abc"};
            auto v = utils::parsers::define(ss);
            REQUIRE(v.has_value());
            REQUIRE(v.value().first == "X");
            REQUIRE(v.value().second == "4 3; 45abc");
        }

        SUBCASE("Basic define works with multiline before value") {
            std::stringstream ss{"@define X \\\n\r3;\\\n\r45abc"};
            auto v = utils::parsers::define(ss);
            REQUIRE(v.has_value());
            REQUIRE(v.value().first == "X");
            REQUIRE(v.value().second == " 3; 45abc");
        }
    }
}
