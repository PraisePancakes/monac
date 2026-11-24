#include <string>
#include <string_view>
#include <tuple>

#include "../src/monac/fsm.hpp"
#include "doctest.h"
TEST_SUITE("Worm machine") {
    TEST_CASE("Testing match with string literals") {
        std::string source = "testing 1 2 3";
        monac::semantic::worm_machine<std::string> wm{source};

        auto v = wm.match("err", "testing", " ", "1", " ", "2", " ", "3");

        REQUIRE(v.has_value());

        auto [s0, _, s1, __, s2, ___, s3] = v.value();

        CHECK(s0 == "testing");
        CHECK(s1 == "1");
        CHECK(s2 == "2");
        CHECK(s3 == "3");
    }

    TEST_CASE("Testing match with const char* decay") {
        std::string source = "testing 1 2 3";
        monac::semantic::worm_machine<std::string> wm{source};

        const char* a = "testing";
        const char* b = " ";
        const char* c = "1";
        const char* d = "2";
        const char* e = "3";

        auto v = wm.match("err", a, b, c, b, d, b, e);

        REQUIRE(v.has_value());
        CHECK(std::get<0>(v.value()) == a);
        CHECK(std::get<2>(v.value()) == c);
    }

    TEST_CASE("Testing match with mutable char arrays") {
        std::string source = "testing 1 2 3";
        monac::semantic::worm_machine<std::string> wm{source};

        char a[] = "testing";
        char b[] = " ";
        char c[] = "1";

        auto v = wm.match("err", a, b, c, b, "2", b, "3");

        REQUIRE(v.has_value());
        CHECK(std::get<0>(v.value()) == std::string("testing"));
        CHECK(std::get<2>(v.value()) == std::string("1"));
    }

    TEST_CASE("Testing match with const char arrays") {
        std::string source = "testing 1 2 3";
        monac::semantic::worm_machine<std::string> wm{source};

        const char a[] = "testing";
        const char b[] = " ";
        const char c[] = "1";

        auto v = wm.match("err", a, b, c, b, "2", b, "3");

        REQUIRE(v.has_value());
        CHECK(std::get<0>(v.value()) == std::string("testing"));
    }

    TEST_CASE("Testing match with std::string") {
        std::string source = "testing 1 2 3";
        monac::semantic::worm_machine<std::string> wm{source};

        std::string a = "testing";
        std::string b = " ";
        std::string c = "1";
        std::string d = "2";
        std::string e = "3";

        auto v = wm.match("err", a, b, c, b, d, b, e);

        REQUIRE(v.has_value());
        CHECK(std::get<0>(v.value()) == a);
        CHECK(std::get<2>(v.value()) == c);
    }

    TEST_CASE("Testing match with const std::string") {
        std::string source = "testing 1 2 3";
        monac::semantic::worm_machine<std::string> wm{source};

        const std::string a = "testing";
        const std::string b = " ";
        const std::string c = "1";

        auto v = wm.match("err", a, b, c, b, "2", b, "3");

        REQUIRE(v.has_value());
        CHECK(std::get<0>(v.value()) == a);
    }

    TEST_CASE("Testing match with std::string_view") {
        std::string source = "testing 1 2 3";
        monac::semantic::worm_machine<std::string> wm{source};

        std::string_view a = "testing";
        std::string_view b = " ";
        std::string_view c = "1";
        std::string_view d = "2";
        std::string_view e = "3";

        auto v = wm.match("err", a, b, c, b, d, b, e);

        REQUIRE(v.has_value());
        CHECK(std::get<0>(v.value()) == a);
        CHECK(std::get<2>(v.value()) == c);
    }

    TEST_CASE("Testing match with mixed string types") {
        std::string source = "testing 1 2 3";
        monac::semantic::worm_machine<std::string> wm{source};

        const char* a = "testing";
        std::string b = " ";
        std::string_view c = "1";
        const char d[] = "2";
        std::string e = "3";

        auto v = wm.match("err", a, b, c, b, d, b, e);

        REQUIRE(v.has_value());
        CHECK(std::get<0>(v.value()) == a);
        CHECK(std::get<2>(v.value()) == c);
    }

    TEST_CASE("Testing match with temporary std::string") {
        std::string source = "testing 1 2 3";
        monac::semantic::worm_machine<std::string> wm{source};

        auto v = wm.match(
            "err",
            std::string("testing"),
            std::string(" "),
            std::string("1"),
            std::string(" "),
            std::string("2"),
            std::string(" "),
            std::string("3"));

        REQUIRE(v.has_value());
    }

    TEST_CASE("Testing match with explicit string_view temporaries") {
        std::string source = "testing 1 2 3";
        monac::semantic::worm_machine<std::string> wm{source};

        auto v = wm.match(
            "err",
            std::string_view("testing"),
            std::string_view(" "),
            std::string_view("1"),
            std::string_view(" "),
            std::string_view("2"),
            std::string_view(" "),
            std::string_view("3"));

        REQUIRE(v.has_value());
    }
}

#include <array>

#include "../src/monac/fsm.hpp"
#include "../src/monac/tokens.hpp"

using namespace monac::semantic;

using token_type = token::TOKEN_TYPE;

static token make_token(token_type type, std::string value = "") {
    token t;
    t.type = type;
    t.value = std::move(value);
    return t;
}

TEST_CASE("Token matching: single token") {
    std::array source{
        make_token(token_type::TK_IDENTIFIER, "main"),
        make_token(token_type::TK_LPAREN),
        make_token(token_type::TK_RPAREN)};

    worm_machine<std::array<token, 3>> wm{source};

    auto result = wm.match("Expected identifier",
                           make_token(token_type::TK_IDENTIFIER, "anything"));

    CHECK(result);
    CHECK(result->type == token_type::TK_IDENTIFIER);
    CHECK(wm.peek().type == token_type::TK_LPAREN);
}

TEST_CASE("Token matching: complete sequence") {
    std::array source{
        make_token(token_type::TK_IDENTIFIER, "main"),
        make_token(token_type::TK_LPAREN),
        make_token(token_type::TK_RPAREN),
        make_token(token_type::TK_LCURL),
        make_token(token_type::TK_RCURL)};

    worm_machine<std::array<token, 5>> wm{source};

    auto identifier = make_token(token_type::TK_IDENTIFIER);
    auto lparen = make_token(token_type::TK_LPAREN);
    auto rparen = make_token(token_type::TK_RPAREN);

    auto result = wm.match(
        "Expected function declaration",
        identifier,
        lparen,
        rparen);

    REQUIRE(result);

    CHECK(std::get<0>(*result).type == token_type::TK_IDENTIFIER);
    CHECK(std::get<1>(*result).type == token_type::TK_LPAREN);
    CHECK(std::get<2>(*result).type == token_type::TK_RPAREN);

    CHECK(wm.peek().type == token_type::TK_LCURL);
}

TEST_CASE("Token matching: sequence with different token types") {
    std::array source{
        make_token(token_type::TK_IDENTIFIER, "count"),
        make_token(token_type::TK_EQ),
        make_token(token_type::TK_NUMBER, "42"),
        make_token(token_type::TK_SEMI)};

    worm_machine<std::array<token, 4>> wm{source};

    auto identifier = make_token(token_type::TK_IDENTIFIER);
    auto equal = make_token(token_type::TK_EQ);
    auto number = make_token(token_type::TK_NUMBER);
    auto semicolon = make_token(token_type::TK_SEMI);

    auto result = wm.match(
        "Expected variable assignment",
        identifier,
        equal,
        number,
        semicolon);

    REQUIRE(result);

    CHECK(wm.is_end());
}

TEST_CASE("Token matching: failed sequence rolls back") {
    std::array source{
        make_token(token_type::TK_IDENTIFIER, "main"),
        make_token(token_type::TK_LPAREN),
        make_token(token_type::TK_RPAREN)};

    worm_machine<std::array<token, 3>> wm{source};

    auto identifier = make_token(token_type::TK_IDENTIFIER);
    auto equal = make_token(token_type::TK_EQ);
    auto rparen = make_token(token_type::TK_RPAREN);

    auto result = wm.match(
        "Expected assignment",
        identifier,
        equal,
        rparen);

    CHECK_FALSE(result);

    // The failed sequence must not consume any tokens.
    CHECK(wm.peek().type == token_type::TK_IDENTIFIER);
}

TEST_CASE("Token matching: partial sequence rolls back") {
    std::array source{
        make_token(token_type::TK_IDENTIFIER, "main"),
        make_token(token_type::TK_LPAREN),
        make_token(token_type::TK_RPAREN)};

    worm_machine<std::array<token, 3>> wm{source};

    auto identifier = make_token(token_type::TK_IDENTIFIER);
    auto lparen = make_token(token_type::TK_LPAREN);
    auto semicolon = make_token(token_type::TK_SEMI);

    auto result = wm.match(
        "Expected function declaration",
        identifier,
        lparen,
        semicolon);

    CHECK_FALSE(result);
    CHECK(wm.peek().type == token_type::TK_IDENTIFIER);
}

TEST_CASE("Token matching: token values are ignored") {
    std::array source{
        make_token(token_type::TK_IDENTIFIER, "actual_name")};

    worm_machine<std::array<token, 1>> wm{source};

    auto expected = make_token(token_type::TK_IDENTIFIER, "different_name");

    auto result = wm.match("Expected identifier", expected);

    CHECK(result);
    CHECK(result->value == "actual_name");
}