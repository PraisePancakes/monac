#include "../src/utils/unit_parsers.hpp"

#include "doctest.h"

TEST_SUITE("unit parsers") {
    TEST_CASE("white_space") {
        SUBCASE("space") {
            std::istringstream ss{" "};
            CHECK(utils::unit::white_space(ss).value() == ' ');
        }

        SUBCASE("tab") {
            std::istringstream ss{"\t"};
            CHECK(utils::unit::white_space(ss).value() == '\t');
        }

        SUBCASE("invalid whitespace") {
            std::istringstream ss{"\n"};
            CHECK_FALSE(utils::unit::white_space(ss));
        }
    }

    TEST_CASE("at_least_space") {
        SUBCASE("single space") {
            std::istringstream ss{" "};
            CHECK(utils::unit::at_least_space(ss).value() == " ");
        }

        SUBCASE("multiple spaces") {
            std::istringstream ss{"   "};
            CHECK(utils::unit::at_least_space(ss).value() == " ");
        }

        SUBCASE("tabs and spaces") {
            std::istringstream ss{" \t \t"};
            CHECK(utils::unit::at_least_space(ss).value() == " ");
        }

        SUBCASE("no whitespace") {
            std::istringstream ss{"abc"};
            CHECK_FALSE(utils::unit::at_least_space(ss));
        }
    }

    TEST_CASE("opt_white_space") {
        SUBCASE("no whitespace") {
            std::istringstream ss{"abc"};
            CHECK(utils::unit::opt_white_space(ss).value() == "");
        }

        SUBCASE("single space") {
            std::istringstream ss{" abc"};
            CHECK(utils::unit::opt_white_space(ss).value() == " ");
        }

        SUBCASE("multiple spaces") {
            std::istringstream ss{"   abc"};
            CHECK(utils::unit::opt_white_space(ss).value() == " ");
        }

        SUBCASE("tabs") {
            std::istringstream ss{"\t\tabc"};
            CHECK(utils::unit::opt_white_space(ss).value() == " ");
        }
    }

    TEST_CASE("triple_unit_punctuator") {
        const std::vector<std::string> valid{
            "...", "<<=", ">>="};

        for (const auto& token : valid) {
            std::istringstream ss{token};
            CHECK(utils::unit::triple_unit_punctuator(ss).value() == token);
        }

        std::istringstream invalid{"+++"};
        CHECK_FALSE(utils::unit::triple_unit_punctuator(invalid));
    }

    TEST_CASE("double_unit_punctuator") {
        const std::vector<std::string> valid{
            "++", "--", "->", "&&", "||", "==", "!=",
            "<=", ">=", "+=", "-=", "*=", "/=", "%=",
            "&=", "|=", "^=", ">>", "<<", "##"};

        for (const auto& token : valid) {
            std::istringstream ss{token};
            CHECK(utils::unit::double_unit_punctuator(ss).value() == token);
        }

        std::istringstream invalid{"+"};
        CHECK_FALSE(utils::unit::double_unit_punctuator(invalid));
    }

    TEST_CASE("single_unit_punctuator") {
        const std::vector<std::string> valid{
            "#", "(", ")", "[", "]", "{", "}", ".",
            ";", ":", "?", "+", "-", "*", "%", "&", "|",
            "^", "~", "!", "=", "<", ">"};

        for (const auto& token : valid) {
            std::istringstream ss{token};
            CHECK(utils::unit::single_unit_punctuator(ss).value() == token);
        }

        std::istringstream invalid{"@"};
        CHECK_FALSE(utils::unit::single_unit_punctuator(invalid));
    }

    TEST_CASE("preprocessing_op_or_punc") {
        const std::vector<std::string> valid{
            "...", "<<=", ">>=", "++", "--", "->", "&&",
            "||", "==", "!=", "<=", ">=", "+=", "-=",
            "*=", "/=", "%=", "&=", "|=", "^=", ">>",
            "<<", "##", "#", "(", ")", "[", "]", "{",
            "}", ".", ",", ";", ":", "?", "+", "-", "*",
            "%", "&", "|", "^", "~", "!", "=", "<", ">"};

        for (const auto& token : valid) {
            std::istringstream ss{token};
            CHECK(utils::unit::preprocessing_op_or_punc(ss).value() == token);
        }

        std::istringstream invalid{"@"};
        CHECK_FALSE(utils::unit::preprocessing_op_or_punc(invalid));
    }

    TEST_CASE("escape_sequence") {
        SUBCASE("backslash") {
            std::istringstream ss{"\\\\"};
            CHECK(utils::unit::escape_sequence(ss).value() == "\\\\");
        }

        SUBCASE("double quote") {
            std::istringstream ss{"\\\""};
            CHECK(utils::unit::escape_sequence(ss).value() == "\\\"");
        }

        SUBCASE("single quote") {
            std::istringstream ss{"\\\'"};
            CHECK(utils::unit::escape_sequence(ss).value() == "\\\'");
        }

        SUBCASE("invalid escape") {
            std::istringstream ss{"n"};
            CHECK_FALSE(utils::unit::escape_sequence(ss));
        }
    }

    TEST_CASE("character") {
        SUBCASE("ordinary character") {
            std::istringstream ss{"a"};
            CHECK(utils::unit::character(ss).value() == "a");
        }

        SUBCASE("digit") {
            std::istringstream ss{"5"};
            CHECK(utils::unit::character(ss).value() == "5");
        }

        SUBCASE("escaped backslash") {
            std::istringstream ss{"\\\\"};
            CHECK(utils::unit::character(ss).value() == "\\\\");
        }

        SUBCASE("escaped quote") {
            std::istringstream ss{"\""};
            CHECK(utils::unit::character(ss).value() == "\"");
        }

        SUBCASE("newline is character") {
            std::istringstream ss{"\\n"};
            CHECK(utils::unit::character(ss));
        }
    }

    TEST_CASE("char_sequence") {
        SUBCASE("single character") {
            std::istringstream ss{"a"};
            CHECK(utils::unit::char_sequence(ss).value() == "a");
        }

        SUBCASE("multiple characters") {
            std::istringstream ss{"hello"};
            CHECK(utils::unit::char_sequence(ss).value() == "hello");
        }

        SUBCASE("escaped characters") {
            std::istringstream ss{"a\\\"b"};
            CHECK(utils::unit::char_sequence(ss).value() == "a\\\"b");
        }

        SUBCASE("empty sequence") {
            std::istringstream ss{""};
            CHECK_FALSE(utils::unit::char_sequence(ss));
        }
    }

    TEST_CASE("character_literal") {
        SUBCASE("ordinary character") {
            std::istringstream ss{"'a'"};
            CHECK(utils::unit::character_literal(ss).value() == "'a'");
        }

        SUBCASE("escaped character") {
            std::istringstream ss{"'\\''"};
            CHECK(utils::unit::character_literal(ss).value() == "'\\''");
        }

        SUBCASE("multiple characters") {
            std::istringstream ss{"'abc'"};
            CHECK(utils::unit::character_literal(ss).value() == "'abc'");
        }

        SUBCASE("missing closing quote") {
            std::istringstream ss{"'a"};
            CHECK_FALSE(utils::unit::character_literal(ss));
        }

        SUBCASE("empty literal") {
            std::istringstream ss{"''"};
            CHECK_FALSE(utils::unit::character_literal(ss));
        }
    }

    TEST_CASE("string_literal") {
        SUBCASE("ordinary string") {
            std::istringstream ss{"\"hello\""};
            CHECK(utils::unit::string_literal(ss).value() == "\"hello\"");
        }

        SUBCASE("empty string") {
            std::istringstream ss{"\"\""};
            CHECK(utils::unit::string_literal(ss));
        }

        SUBCASE("escaped quote") {
            std::istringstream ss{"\"hello \\\"world\\\"\""};
            CHECK(utils::unit::string_literal(ss).value() == "\"hello \\\"world\\\"\"");
        }

        SUBCASE("missing closing quote") {
            std::istringstream ss{"\"hello"};
            CHECK_FALSE(utils::unit::string_literal(ss));
        }
    }

    TEST_CASE("integer") {
        SUBCASE("single digit") {
            std::istringstream ss{"5"};
            CHECK(utils::unit::integer(ss).value() == "5");
        }

        SUBCASE("multiple digits") {
            std::istringstream ss{"12345"};
            CHECK(utils::unit::integer(ss).value() == "12345");
        }

        SUBCASE("zero") {
            std::istringstream ss{"0"};
            CHECK(utils::unit::integer(ss).value() == "0");
        }

        SUBCASE("invalid input") {
            std::istringstream ss{"abc"};
            CHECK_FALSE(utils::unit::integer(ss));
        }
    }

    TEST_CASE("negative_integer") {
        SUBCASE("negative number") {
            std::istringstream ss{"-123"};
            CHECK(utils::unit::negative_integer(ss).value() == "-123");
        }

        SUBCASE("negative zero") {
            std::istringstream ss{"-0"};
            CHECK(utils::unit::negative_integer(ss).value() == "-0");
        }

        SUBCASE("missing digits") {
            std::istringstream ss{"-"};
            CHECK_FALSE(utils::unit::negative_integer(ss));
        }
    }

    TEST_CASE("floating_point") {
        const std::vector<std::string> valid{
            "123.456", ".456", "123."};

        for (const auto& token : valid) {
            std::istringstream ss{token};
            CHECK(utils::unit::floating_point(ss).value() == token);
        }

        std::istringstream invalid{"123"};
        CHECK_FALSE(utils::unit::floating_point(invalid));
    }

    TEST_CASE("negative_floating_point") {
        const std::vector<std::string> valid{
            "-123.456", "-.456", "-123."};

        for (const auto& token : valid) {
            std::istringstream ss{token};
            CHECK(utils::unit::negative_floating_point(ss).value() == token);
        }

        std::istringstream invalid{"-123"};
        CHECK_FALSE(utils::unit::negative_floating_point(invalid));
    }

    TEST_CASE("digit") {
        for (char c = '0'; c <= '9'; ++c) {
            std::istringstream ss{std::string(1, c)};
            CHECK(utils::unit::digit(ss).value() == c);
        }

        std::istringstream invalid{"a"};
        CHECK_FALSE(utils::unit::digit(invalid));
    }

    TEST_CASE("pp_number") {
        const std::vector<std::string> valid{
            "123", "-123", "123.456", ".456", "123.",
            "-123.456", "-.456", "-123."};

        for (const auto& token : valid) {
            std::istringstream ss{token};
            CHECK(utils::unit::pp_number(ss).value() == token);
        }

        std::istringstream invalid{"abc"};
        CHECK_FALSE(utils::unit::pp_number(invalid));
    }

    TEST_CASE("identifier_nondigit") {
        SUBCASE("letter") {
            std::istringstream ss{"a"};
            CHECK(utils::unit::identifier_nondigit(ss).value() == 'a');
        }

        SUBCASE("underscore") {
            std::istringstream ss{"_"};
            CHECK(utils::unit::identifier_nondigit(ss).value() == '_');
        }

        SUBCASE("digit rejected") {
            std::istringstream ss{"5"};
            CHECK_FALSE(utils::unit::identifier_nondigit(ss));
        }
    }

    TEST_CASE("identifier") {
        const std::vector<std::string> valid{
            "hello", "_hello", "hello123", "_123",
            "Hello_World", "a1b2c3", "_"};

        for (const auto& token : valid) {
            std::istringstream ss{token};
            CHECK(utils::unit::identifier(ss).value() == token);
        }

        std::istringstream invalid{"123hello"};
        CHECK_FALSE(utils::unit::identifier(invalid));
    }

    TEST_CASE("preprocessing_token") {
        const std::vector<std::string> valid{
            "identifier", "_identifier123",
            "123", "123.456", "-123",
            "'a'", "\"hello\"",
            "...", "##", "++", "+", "("};

        for (const auto& token : valid) {
            std::istringstream ss{token};
            CHECK(utils::unit::preprocessing_token(ss).value() == token);
        }

        std::istringstream invalid{" "};
        CHECK_FALSE(utils::unit::preprocessing_token(invalid));
    }
}