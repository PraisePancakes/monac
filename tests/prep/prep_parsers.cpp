#include "../../src/monac/prep/prep_parsers.hpp"

#include "../doctest.h"

TEST_SUITE("preprocessor parsers") {
    TEST_CASE("identifier_list") {
        SUBCASE("single identifier") {
            std::stringstream ss{"foo"};

            const auto result = monac::prep::parsers::identifier_list(ss);

            REQUIRE(result.has_value());
            CHECK(result.value() == std::vector<std::string>{"foo"});
        }

        SUBCASE("two identifiers") {
            std::stringstream ss{"foo,bar"};

            const auto result = monac::prep::parsers::identifier_list(ss);

            REQUIRE(result.has_value());
            CHECK(result.value() == std::vector<std::string>{"foo", "bar"});
        }

        SUBCASE("multiple identifiers") {
            std::stringstream ss{"foo,bar,baz"};

            const auto result = monac::prep::parsers::identifier_list(ss);

            REQUIRE(result.has_value());
            CHECK(result.value() == std::vector<std::string>{"foo", "bar", "baz"});
        }

        SUBCASE("identifiers with digits") {
            std::stringstream ss{"foo1,bar2,baz3"};

            const auto result = monac::prep::parsers::identifier_list(ss);

            REQUIRE(result.has_value());
            CHECK(result.value() == std::vector<std::string>{"foo1", "bar2", "baz3"});
        }

        SUBCASE("empty input") {
            std::stringstream ss{""};

            CHECK_FALSE(monac::prep::parsers::identifier_list(ss).has_value());
        }

        SUBCASE("trailing comma") {
            std::stringstream ss{"foo,bar,"};

            const auto result = monac::prep::parsers::identifier_list(ss);

            REQUIRE(result.has_value());
            CHECK(result.value() == std::vector<std::string>{"foo", "bar"});
            CHECK(ss.peek() == ',');
        }

        SUBCASE("invalid first identifier") {
            std::stringstream ss{"123foo"};

            CHECK_FALSE(monac::prep::parsers::identifier_list(ss).has_value());
        }
    }

    TEST_CASE("replacement_list") {
        SUBCASE("single identifier") {
            std::stringstream ss{"foo"};

            const auto result = monac::prep::parsers::replacement_list(ss);

            REQUIRE(result.has_value());
            CHECK(result.value() == std::vector<std::string>{"foo"});
        }

        SUBCASE("multiple preprocessing tokens") {
            std::stringstream ss{"foo + bar"};

            const auto result = monac::prep::parsers::replacement_list(ss);

            REQUIRE(result.has_value());
            CHECK(result.value() == std::vector<std::string>{"foo", "+", "bar"});
        }

        SUBCASE("integer tokens") {
            std::stringstream ss{"123 456"};

            const auto result = monac::prep::parsers::replacement_list(ss);

            REQUIRE(result.has_value());
            CHECK(result.value() == std::vector<std::string>{"123", "456"});
        }

        SUBCASE("punctuation tokens") {
            std::stringstream ss{"foo(a,b)"};

            const auto result = monac::prep::parsers::replacement_list(ss);

            REQUIRE(result.has_value());
            CHECK(result.value() == std::vector<std::string>{
                                        "foo", "(", "a", ",", "b", ")"});
        }

        SUBCASE("string literal") {
            std::stringstream ss{"\"hello\""};

            const auto result = monac::prep::parsers::replacement_list(ss);

            REQUIRE(result.has_value());
            CHECK(result.value() == std::vector<std::string>{"\"hello\""});
        }

        SUBCASE("character literal") {
            std::stringstream ss{"'a'"};

            const auto result = monac::prep::parsers::replacement_list(ss);

            REQUIRE(result.has_value());
            CHECK(result.value() == std::vector<std::string>{"'a'"});
        }

        SUBCASE("empty input") {
            std::stringstream ss{""};

            const auto result = monac::prep::parsers::replacement_list(ss);

            REQUIRE(result.has_value());
            CHECK(result.value().empty());
        }
    }
}