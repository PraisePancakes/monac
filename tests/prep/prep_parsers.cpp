#include "../../src/monac/prep/prep_parsers.hpp"

#include <sstream>
#include <string>
#include <vector>

#include "../doctest.h"

using namespace monac::prep::parsers;

// ------------------------------------------------------------
// pp_tokens
// ------------------------------------------------------------

TEST_CASE("pp_tokens") {
    SUBCASE("parses a single identifier") {
        std::istringstream stream{"hello"};

        auto result = pp_tokens(stream);

        REQUIRE(result.has_value());
        CHECK(result.value() == "hello");
    }

    SUBCASE("parses a single number") {
        std::istringstream stream{"123"};

        auto result = pp_tokens(stream);

        REQUIRE(result.has_value());
        CHECK(result.value() == "123");
    }

    SUBCASE("parses a single operator") {
        std::istringstream stream{"+"};

        auto result = pp_tokens(stream);

        REQUIRE(result.has_value());
        CHECK(result.value() == "+");
    }

    SUBCASE("parses multiple preprocessing tokens") {
        std::istringstream stream{"hello + 123"};

        auto result = pp_tokens(stream);

        REQUIRE(result.has_value());
        CHECK(result.value() == "hello");
    }

    SUBCASE("recursively parses multiple preprocessing tokens") {
        std::istringstream stream{"hello"};

        auto result = pp_tokens(stream);

        REQUIRE(result.has_value());
        CHECK(result.value() == "hello");
    }
}

// ------------------------------------------------------------
// replacement_list
// ------------------------------------------------------------

TEST_CASE("replacement_list") {
    SUBCASE("parses an empty replacement list") {
        std::istringstream stream{""};

        auto result = replacement_list(stream);

        REQUIRE(result.has_value());
        CHECK(result->empty());
    }

    SUBCASE("parses a single preprocessing token") {
        std::istringstream stream{"hello"};

        auto result = replacement_list(stream);

        REQUIRE(result.has_value());
        REQUIRE(result->size() == 1);
        CHECK((*result)[0] == "hello");
    }

    SUBCASE("parses multiple identifiers") {
        std::istringstream stream{"hello world"};

        auto result = replacement_list(stream);

        REQUIRE(result.has_value());
        CHECK(result->size() == 2);
        CHECK((*result)[0] == "hello");
        CHECK((*result)[1] == "world");
    }

    SUBCASE("parses an expression") {
        std::istringstream stream{"x + y * 5"};

        auto result = replacement_list(stream);

        REQUIRE(result.has_value());
        CHECK(result->size() == 5);
        CHECK((*result)[0] == "x");
        CHECK((*result)[1] == "+");
        CHECK((*result)[2] == "y");
        CHECK((*result)[3] == "*");
        CHECK((*result)[4] == "5");
    }

    // SUBCASE("parses function-like macro replacement") {
    //     std::istringstream stream{"ADD(x, y)"};

    //     auto result = replacement_list(stream);

    //     REQUIRE(result.has_value());
    //     CHECK(result->size() == 1);
    //     CHECK((*result)[0] == "ADD(x, y)");
    // }
}

// ------------------------------------------------------------
// identifier_list
// ------------------------------------------------------------
TEST_CASE("identifier parser backtracking") {
    std::istringstream stream{"x"};

    auto first = mona::_transform([](const auto seq) -> std::string {
        const auto [f, _] = seq;
        return f;
    },
                                  mona::_sequence(utils::unit::identifier, mona::_match(",")));
    auto result = mona::_either(first, utils::unit::identifier)(stream);

    REQUIRE(result.has_value());
    CHECK(result.value() == "x");
}
TEST_CASE("identifier_list") {
    SUBCASE("parses a single identifier") {
        std::istringstream stream{"x"};

        auto result = identifier_list(stream);

        REQUIRE(result.has_value());
        REQUIRE(result->size() == 1);
        CHECK((*result)[0] == "x");
    }

    SUBCASE("parses two identifiers") {
        std::istringstream stream{"x,y"};

        auto result = identifier_list(stream);

        REQUIRE(result.has_value());
        REQUIRE(result->size() == 2);
        CHECK((*result)[0] == "x");
        CHECK((*result)[1] == "y");
    }

    SUBCASE("parses multiple identifiers") {
        std::istringstream stream{"x,y,z"};

        auto result = identifier_list(stream);

        REQUIRE(result.has_value());
        REQUIRE(result->size() == 3);
        CHECK((*result)[0] == "x");
        CHECK((*result)[1] == "y");
        CHECK((*result)[2] == "z");
    }

    SUBCASE("does not consume identifiers separated by invalid syntax") {
        std::istringstream stream{"x y"};

        auto result = identifier_list(stream);

        REQUIRE(result.has_value());
        REQUIRE(result->size() == 1);
        CHECK((*result)[0] == "x");
    }

    SUBCASE("rejects an empty identifier list") {
        std::istringstream stream{","};

        auto result = identifier_list(stream);

        CHECK_FALSE(result.has_value());
    }
}

// ------------------------------------------------------------
// text_line
// ------------------------------------------------------------

TEST_CASE("text_line") {
    SUBCASE("parses an empty text line") {
        std::istringstream stream{"\n"};

        auto result = text_line(stream);

        REQUIRE(result.has_value());
        CHECK(result.value().empty());
    }

    SUBCASE("parses a single identifier") {
        std::istringstream stream{"hello\n"};

        auto result = text_line(stream);

        REQUIRE(result.has_value());
        CHECK(result.value() == "hello");
    }

    SUBCASE("parses multiple identifiers") {
        std::istringstream stream{"hello world\n"};

        auto result = text_line(stream);

        REQUIRE(result.has_value());
    }

    SUBCASE("parses an expression") {
        std::istringstream stream{"x + y * 5\n"};

        auto result = text_line(stream);

        REQUIRE(result.has_value());
        CHECK(result.value() == "x + y * 5");
    }

    SUBCASE("parses punctuation") {
        std::istringstream stream{"func main(x, y) {}\n"};

        auto result = text_line(stream);

        REQUIRE(result.has_value());
        CHECK(result.value() == "func main(x, y) {}");
    }

    SUBCASE("does not consume the next line") {
        std::istringstream stream{"hello\nworld\n"};

        auto result = text_line(stream);

        REQUIRE(result.has_value());
        CHECK(result.value() == "hello");

        std::string remaining;
        std::getline(stream, remaining);

        CHECK(remaining == "world");
    }
}

TEST_CASE("parses object-like macro definitions") {
    SUBCASE("parses a simple object-like macro") {
        std::stringstream stream{"@define VALUE 42\n"};

        auto result = _internal_ctrl::define_identifier(stream);

        REQUIRE(result.has_value());
        REQUIRE(std::holds_alternative<variable>(*result));

        const auto& macro = std::get<variable>(*result);

        CHECK(macro.id == "VALUE");
        REQUIRE(macro.replacement_list.size() == 1);
        CHECK(macro.replacement_list[0] == "42");
    }

    SUBCASE("parses an identifier replacement") {
        std::stringstream stream{"@define VALUE OTHER\n"};

        auto result = _internal_ctrl::define_identifier(stream);

        REQUIRE(result.has_value());
        REQUIRE(std::holds_alternative<variable>(*result));

        const auto& macro = std::get<variable>(*result);

        CHECK(macro.id == "VALUE");
        REQUIRE(macro.replacement_list.size() == 1);
        CHECK(macro.replacement_list[0] == "OTHER");
    }

    SUBCASE("parses multiple replacement tokens") {
        std::stringstream stream{"@define VALUE x + y\n"};

        auto result = _internal_ctrl::define_identifier(stream);

        REQUIRE(result.has_value());
        REQUIRE(std::holds_alternative<variable>(*result));

        const auto& macro = std::get<variable>(*result);

        CHECK(macro.id == "VALUE");
        REQUIRE(macro.replacement_list.size() == 3);
        CHECK(macro.replacement_list[0] == "x");
        CHECK(macro.replacement_list[1] == "+");
        CHECK(macro.replacement_list[2] == "y");
    }

    SUBCASE("rejects a definition without a replacement list") {
        std::stringstream stream{"@define VALUE\n"};

        auto result = _internal_ctrl::define_identifier(stream);

        CHECK_FALSE(result.has_value());
    }
}

TEST_CASE("parses function-like macro definitions") {
    SUBCASE("parses a function with one parameter") {
        std::stringstream stream{"@define DOUBLE(x) x\n"};

        auto result = _internal_ctrl::define_function(stream);

        REQUIRE(result.has_value());
        REQUIRE(std::holds_alternative<function>(*result));

        const auto& macro = std::get<function>(*result);

        CHECK(macro.id == "DOUBLE");

        REQUIRE(macro.parameters_map.size() == 1);
        CHECK(macro.parameters_map.at("x") == 0);

        REQUIRE(macro.replacement_list.size() == 1);
        CHECK(macro.replacement_list[0] == "x");
    }

    SUBCASE("parses a function with multiple parameters") {
        std::stringstream stream{"@define ADD(x, y) x + y\n"};

        auto result = _internal_ctrl::define_function(stream);

        REQUIRE(result.has_value());
        REQUIRE(std::holds_alternative<function>(*result));

        const auto& macro = std::get<function>(*result);

        CHECK(macro.id == "ADD");

        REQUIRE(macro.parameters_map.size() == 2);
        CHECK(macro.parameters_map.at("x") == 0);
        CHECK(macro.parameters_map.at("y") == 1);

        REQUIRE(macro.replacement_list.size() == 3);
        CHECK(macro.replacement_list[0] == "x");
        CHECK(macro.replacement_list[1] == "+");
        CHECK(macro.replacement_list[2] == "y");
    }

    SUBCASE("parses whitespace around parameters") {
        std::stringstream stream{"@define ADD( x , y ) x + y\n"};

        auto result = _internal_ctrl::define_function(stream);

        REQUIRE(result.has_value());
        REQUIRE(std::holds_alternative<function>(*result));

        const auto& macro = std::get<function>(*result);

        CHECK(macro.id == "ADD");

        REQUIRE(macro.parameters_map.size() == 2);
        CHECK(macro.parameters_map.at("x") == 0);
        CHECK(macro.parameters_map.at("y") == 1);
    }

    SUBCASE("parses a function with no parameters") {
        std::stringstream stream{"@define GET_VALUE() 42\n"};

        auto result = _internal_ctrl::define_function(stream);

        REQUIRE(result.has_value());
        REQUIRE(std::holds_alternative<function>(*result));

        const auto& macro = std::get<function>(*result);

        CHECK(macro.id == "GET_VALUE");
        CHECK(macro.parameters_map.empty());

        REQUIRE(macro.replacement_list.size() == 1);
        CHECK(macro.replacement_list[0] == "42");
    }

    SUBCASE("rejects a function definition without a closing parenthesis") {
        std::stringstream stream{"@define ADD(x, y x + y\n"};

        auto result = _internal_ctrl::define_function(stream);

        CHECK_FALSE(result.has_value());
    }
}