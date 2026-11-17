#include "../../src/monac/prep/prep_parsers.hpp"

#include "../doctest.h"

TEST_SUITE("preprocessor parsers") {
    using namespace monac::prep::parsers;
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

    TEST_SUITE("define_identifier") {
        TEST_CASE("simple object-like macro") {
            std::stringstream ss("@define VALUE 42\n");
            auto result = _internal_ctrl::define_identifier(ss);

            REQUIRE(result.has_value());
            REQUIRE(std::holds_alternative<variable>(result.value()));

            const auto& node = std::get<variable>(result.value());

            CHECK(node.id == "VALUE");
            CHECK(node.replacement_list == std::vector<std::string>{"42"});
        }

        TEST_CASE("multiple replacement tokens") {
            std::stringstream ss("@define VALUE 5 + 3 - 4\n");
            auto result = _internal_ctrl::define_identifier(ss);

            REQUIRE(result.has_value());

            const auto& node = std::get<variable>(result.value());

            CHECK(node.id == "VALUE");
            CHECK(node.replacement_list ==
                  std::vector<std::string>{"5", "+", "3", "-", "4"});
        }

        TEST_CASE("empty replacement list") {
            std::stringstream ss("@define EMPTY \n");
            auto result = _internal_ctrl::define_identifier(ss);

            REQUIRE(result.has_value());

            const auto& node = std::get<variable>(result.value());

            CHECK(node.id == "EMPTY");
            CHECK(node.replacement_list.empty());
        }
    }
    TEST_SUITE("define_identifier") {
        TEST_CASE("simple object-like macro") {
            std::stringstream ss("@define VALUE 42\n");
            auto result = _internal_ctrl::define_identifier(ss);

            REQUIRE(result.has_value());
            REQUIRE(std::holds_alternative<variable>(result.value()));

            const auto& node = std::get<variable>(result.value());

            CHECK(node.id == "VALUE");
            CHECK(node.replacement_list == std::vector<std::string>{"42"});
        }

        TEST_CASE("multiple replacement tokens") {
            std::stringstream ss("@define VALUE 5 + 3 - 4\n");
            auto result = _internal_ctrl::define_identifier(ss);

            REQUIRE(result.has_value());

            const auto& node = std::get<variable>(result.value());

            CHECK(node.id == "VALUE");
            CHECK(node.replacement_list ==
                  std::vector<std::string>{"5", "+", "3", "-", "4"});
        }

        TEST_CASE("replacement with parentheses") {
            std::stringstream ss("@define VALUE (5 + 3)\n");
            auto result = _internal_ctrl::define_identifier(ss);

            REQUIRE(result.has_value());

            const auto& node = std::get<variable>(result.value());

            CHECK(node.id == "VALUE");
            CHECK(node.replacement_list ==
                  std::vector<std::string>{"(", "5", "+", "3", ")"});
        }

        TEST_CASE("empty replacement list") {
            std::stringstream ss("@define EMPTY \n");
            auto result = _internal_ctrl::define_identifier(ss);

            REQUIRE(result.has_value());

            const auto& node = std::get<variable>(result.value());

            CHECK(node.id == "EMPTY");
            CHECK(node.replacement_list.empty());
        }

        TEST_CASE("replacement with identifiers") {
            std::stringstream ss("@define VALUE SOME_IDENTIFIER + OTHER\n");
            auto result = _internal_ctrl::define_identifier(ss);

            REQUIRE(result.has_value());

            const auto& node = std::get<variable>(result.value());

            CHECK(node.id == "VALUE");
            CHECK(node.replacement_list ==
                  std::vector<std::string>{
                      "SOME_IDENTIFIER", "+", "OTHER"});
        }

        TEST_CASE("replacement with floating point number") {
            std::stringstream ss("@define PI 3.14159\n");
            auto result = _internal_ctrl::define_identifier(ss);

            REQUIRE(result.has_value());

            const auto& node = std::get<variable>(result.value());

            CHECK(node.id == "PI");
            CHECK(node.replacement_list == std::vector<std::string>{"3.14159"});
        }
    }
    TEST_SUITE("define_function") {
        TEST_CASE("single parameter") {
            std::stringstream ss("@define DOUBLE(x) x * 2\n");
            auto result = _internal_ctrl::define_function(ss);

            REQUIRE(result.has_value());
            REQUIRE(std::holds_alternative<function>(result.value()));

            const auto& node = std::get<function>(result.value());

            CHECK(node.id == "DOUBLE");
            CHECK(node.parameters_map.at("x") == 0);
            CHECK(node.replacement_list ==
                  std::vector<std::string>{"x", "*", "2"});
        }

        TEST_CASE("multiple parameters") {
            std::stringstream ss("@define ADD(x, y) x + y\n");
            auto result = _internal_ctrl::define_function(ss);

            REQUIRE(result.has_value());

            const auto& node = std::get<function>(result.value());

            CHECK(node.id == "ADD");
            CHECK(node.parameters_map.at("x") == 0);
            CHECK(node.replacement_list ==
                  std::vector<std::string>{"x", "+", "y"});
        }

        TEST_CASE("no parameters") {
            std::stringstream ss("@define GET_VALUE() 42\n");
            auto result = _internal_ctrl::define_function(ss);

            REQUIRE(result.has_value());

            const auto& node = std::get<function>(result.value());

            CHECK(node.id == "GET_VALUE");
            CHECK(node.parameters_map.empty());
            CHECK(node.replacement_list == std::vector<std::string>{"42"});
        }

        TEST_CASE("whitespace around parameters") {
            std::stringstream ss("@define ADD( x , y ) x + y\n");
            auto result = _internal_ctrl::define_function(ss);

            REQUIRE(result.has_value());

            const auto& node = std::get<function>(result.value());

            CHECK(node.id == "ADD");
            CHECK(node.parameters_map.at("x") == 0);
            CHECK(node.parameters_map.at("y") == 1);
        }

        TEST_CASE("multiple replacement tokens") {
            std::stringstream ss("@define CALCULATE(a, b) (a + b) * 2\n");
            auto result = _internal_ctrl::define_function(ss);

            REQUIRE(result.has_value());

            const auto& node = std::get<function>(result.value());

            CHECK(node.id == "CALCULATE");
            CHECK(node.parameters_map.at("a") == 0);
            CHECK(node.parameters_map.at("b") == 1);
            CHECK(node.replacement_list ==
                  std::vector<std::string>{
                      "(", "a", "+", "b", ")", "*", "2"});
        }

        TEST_CASE("named call in replacement list") {
            std::stringstream ss("@define CALCULATE(a, b) FUNC(a, b) * 2\n");
            auto result = _internal_ctrl::define_function(ss);

            REQUIRE(result.has_value());

            const auto& node = std::get<function>(result.value());

            CHECK(node.id == "CALCULATE");
            CHECK(node.parameters_map.at("a") == 0);
            CHECK(node.parameters_map.at("b") == 1);
            CHECK(node.replacement_list == std::vector<std::string>{"FUNC", "(", "a", ",", "b", ")", "*", "2"});
        }

        TEST_CASE("define_function does not consume the next line") {
            std::istringstream input{
                "@define MULT(x, y) x * y"
                "func main"};

            auto result = monac::prep::parsers::_internal_ctrl::define_function(input);

            REQUIRE(result.has_value());

            // The next input should still be "func".
            auto next = utils::unit::identifier(input);

            REQUIRE(next.has_value());
            CHECK(next.value() == "func");
        }
    }
}