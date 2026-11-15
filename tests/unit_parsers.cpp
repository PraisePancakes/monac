
#include "../src/utils/unit_parsers.hpp"

#include <sstream>
#include <string>

#include "doctest.h"

using namespace utils::unit;

TEST_CASE("white_space") {
    SUBCASE("Matches a space") {
        std::stringstream ss{" "};
        CHECK(white_space(ss).has_value());
    }

    SUBCASE("Matches a tab") {
        std::stringstream ss{"\t"};
        CHECK(white_space(ss).has_value());
    }

    SUBCASE("Rejects newline") {
        std::stringstream ss{"\n"};
        CHECK_FALSE(white_space(ss).has_value());
    }

    SUBCASE("Rejects arbitrary characters") {
        std::stringstream ss{"a"};
        CHECK_FALSE(white_space(ss).has_value());
    }
}

TEST_CASE("at_least_space") {
    SUBCASE("Matches one space") {
        std::stringstream ss{" "};
        auto result = at_least_space(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == " ");
    }

    SUBCASE("Matches multiple spaces") {
        std::stringstream ss{"     "};
        auto result = at_least_space(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == " ");
    }

    SUBCASE("Matches tabs") {
        std::stringstream ss{"\t\t"};
        auto result = at_least_space(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == " ");
    }

    SUBCASE("Matches mixed whitespace") {
        std::stringstream ss{" \t \t"};
        auto result = at_least_space(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == " ");
    }

    SUBCASE("Rejects empty input") {
        std::stringstream ss{""};
        CHECK_FALSE(at_least_space(ss).has_value());
    }

    SUBCASE("Rejects non-whitespace") {
        std::stringstream ss{"abc"};
        CHECK_FALSE(at_least_space(ss).has_value());
    }
}

TEST_CASE("opt_white_space") {
    SUBCASE("Matches empty input") {
        std::stringstream ss{""};
        auto result = opt_white_space(ss);

        REQUIRE(result.has_value());
        CHECK(result.value().empty());
    }

    SUBCASE("Matches one space") {
        std::stringstream ss{" "};
        auto result = opt_white_space(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == " ");
    }

    SUBCASE("Matches multiple spaces") {
        std::stringstream ss{"     "};
        auto result = opt_white_space(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == " ");
    }

    SUBCASE("Matches mixed whitespace") {
        std::stringstream ss{" \t \t"};
        auto result = opt_white_space(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == " ");
    }

    SUBCASE("Does not consume non-whitespace") {
        std::stringstream ss{"abc"};
        auto result = opt_white_space(ss);

        REQUIRE(result.has_value());
        CHECK(result.value().empty());
        CHECK(ss.peek() == 'a');
    }
}

TEST_CASE("identifier_start") {
    SUBCASE("Matches lowercase letters") {
        for (char c = 'a'; c <= 'z'; ++c) {
            std::stringstream ss{std::string(1, c)};
            CHECK(identifier_start(ss).has_value());
        }
    }

    SUBCASE("Matches uppercase letters") {
        for (char c = 'A'; c <= 'Z'; ++c) {
            std::stringstream ss{std::string(1, c)};
            CHECK(identifier_start(ss).has_value());
        }
    }

    SUBCASE("Matches underscore") {
        std::stringstream ss{"_"};
        CHECK(identifier_start(ss).has_value());
    }

    SUBCASE("Rejects digits") {
        std::stringstream ss{"0"};
        CHECK_FALSE(identifier_start(ss).has_value());
    }

    SUBCASE("Rejects punctuation") {
        std::stringstream ss{"!"};
        CHECK_FALSE(identifier_start(ss).has_value());
    }

    SUBCASE("Rejects whitespace") {
        std::stringstream ss{" "};
        CHECK_FALSE(identifier_start(ss).has_value());
    }
}

TEST_CASE("identifier_continue") {
    SUBCASE("Matches letters") {
        std::stringstream ss{"a"};
        CHECK(identifier_continue(ss).has_value());
    }

    SUBCASE("Matches underscore") {
        std::stringstream ss{"_"};
        CHECK(identifier_continue(ss).has_value());
    }

    SUBCASE("Matches digits") {
        for (char c = '0'; c <= '9'; ++c) {
            std::stringstream ss{std::string(1, c)};
            CHECK(identifier_continue(ss).has_value());
        }
    }

    SUBCASE("Rejects punctuation") {
        std::stringstream ss{"!"};
        CHECK_FALSE(identifier_continue(ss).has_value());
    }

    SUBCASE("Rejects whitespace") {
        std::stringstream ss{" "};
        CHECK_FALSE(identifier_continue(ss).has_value());
    }
}

TEST_CASE("identifier") {
    SUBCASE("Matches a single lowercase letter") {
        std::stringstream ss{"a"};
        auto result = identifier(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "a");
    }

    SUBCASE("Matches a single uppercase letter") {
        std::stringstream ss{"A"};
        auto result = identifier(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "A");
    }

    SUBCASE("Matches underscore") {
        std::stringstream ss{"_"};
        auto result = identifier(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "_");
    }

    SUBCASE("Matches an identifier containing digits") {
        std::stringstream ss{"variable123"};
        auto result = identifier(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "variable123");
    }

    SUBCASE("Matches an identifier beginning with underscore") {
        std::stringstream ss{"_variable"};
        auto result = identifier(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "_variable");
    }

    SUBCASE("Matches consecutive underscores") {
        std::stringstream ss{"__MY_VARIABLE__"};
        auto result = identifier(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "__MY_VARIABLE__");
    }

    SUBCASE("Does not consume trailing punctuation") {
        std::stringstream ss{"variable!"};
        auto result = identifier(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "variable");
        CHECK(ss.peek() == '!');
    }

    SUBCASE("Rejects identifiers beginning with digits") {
        std::stringstream ss{"123abc"};
        CHECK_FALSE(identifier(ss).has_value());
    }

    SUBCASE("Rejects empty input") {
        std::stringstream ss{""};
        CHECK_FALSE(identifier(ss).has_value());
    }

    SUBCASE("Stops at whitespace") {
        std::stringstream ss{"hello world"};
        auto result = identifier(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "hello");
        CHECK(ss.peek() == ' ');
    }
}

TEST_CASE("string_character") {
    SUBCASE("Matches ordinary characters") {
        std::stringstream ss{"a"};
        CHECK(string_character(ss).has_value());
    }

    SUBCASE("Matches digits") {
        std::stringstream ss{"5"};
        CHECK(string_character(ss).has_value());
    }

    SUBCASE("Matches spaces") {
        std::stringstream ss{" "};
        CHECK(string_character(ss).has_value());
    }

    SUBCASE("Rejects double quotes") {
        std::stringstream ss{"\""};
        CHECK_FALSE(string_character(ss).has_value());
    }

    SUBCASE("Rejects backslashes") {
        std::stringstream ss{"\\"};
        CHECK_FALSE(string_character(ss).has_value());
    }

    SUBCASE("Rejects newlines") {
        std::stringstream ss{"\n"};
        CHECK_FALSE(string_character(ss).has_value());
    }
}

TEST_CASE("escape_character") {
    SUBCASE("Accepts supported escape characters") {
        for (const std::string& c : {"n", "t", "r", "0", "\\", "\"", "'"}) {
            std::stringstream ss{c};
            CHECK(escape_character(ss).has_value());
        }
    }

    SUBCASE("Rejects unsupported escape characters") {
        for (const std::string& c : {"a", "b", "x", "1", " "}) {
            std::stringstream ss{c};
            CHECK_FALSE(escape_character(ss).has_value());
        }
    }
}

TEST_CASE("escape_sequence") {
    SUBCASE("Matches newline escape") {
        std::stringstream ss{"\\n"};
        auto result = escape_sequence(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "\\n");
    }

    SUBCASE("Matches tab escape") {
        std::stringstream ss{"\\t"};
        auto result = escape_sequence(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "\\t");
    }

    SUBCASE("Matches null escape") {
        std::stringstream ss{"\\0"};
        auto result = escape_sequence(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "\\0");
    }

    SUBCASE("Matches escaped backslash") {
        std::stringstream ss{"\\\\"};
        auto result = escape_sequence(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "\\\\");
    }

    SUBCASE("Matches escaped quotation mark") {
        std::stringstream ss{"\\\""};
        auto result = escape_sequence(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "\\\"");
    }

    SUBCASE("Rejects unsupported escapes") {
        std::stringstream ss{"\\x"};
        CHECK_FALSE(escape_sequence(ss).has_value());
    }

    SUBCASE("Rejects missing escape character") {
        std::stringstream ss{"\\"};
        CHECK_FALSE(escape_sequence(ss).has_value());
    }
}

TEST_CASE("punctuator") {
    const std::vector<std::string> valid_punctuators{
        "(", ")", "[", "]", "{", "}",
        ".", "...", ",", ";", ":", "?",
        "+", "-", "*", "%", "&", "|",
        "^", "~", "!", "=", "<", ">",
        "++", "--", "->", "&&", "||",
        "==", "!=", "<=", ">=",
        "+=", "-=", "*=", "/=", "%=",
        "&=", "|=", "^=", ">>", "<<",
        "<<=", ">>=", "##", "#"};

    SUBCASE("Recognizes all defined punctuators") {
        for (const auto& p : valid_punctuators) {
            std::stringstream ss{p};
            auto result = punctuator(ss);

            INFO("Testing punctuator: " << p);
            REQUIRE(result.has_value());
        }
    }

    SUBCASE("Rejects invalid punctuators") {
        for (const std::string& p : {"@", "$", "\\", "`"}) {
            std::stringstream ss{p};

            INFO("Testing invalid punctuator: " << p);
            CHECK_FALSE(punctuator(ss).has_value());
        }
    }
}

TEST_CASE("string_content") {
    SUBCASE("Matches ordinary string content") {
        std::stringstream ss{"hello"};
        auto result = string_content(ss);

        REQUIRE(result.has_value());
        REQUIRE(result->size() == 5);
    }

    SUBCASE("Matches empty content") {
        std::stringstream ss{""};
        auto result = string_content(ss);

        REQUIRE(result.has_value());
        CHECK(result->empty());
    }

    SUBCASE("Matches content containing spaces") {
        std::stringstream ss{"hello world"};
        auto result = string_content(ss);

        REQUIRE(result.has_value());
        CHECK(result->size() == 11);
    }

    SUBCASE("Matches escaped characters") {
        std::stringstream ss{"hello\\nworld"};
        auto result = string_content(ss);

        REQUIRE(result.has_value());
        CHECK(result->size() == 11);
    }

    SUBCASE("Stops before closing quotation mark") {
        std::stringstream ss{"hello\""};
        auto result = string_content(ss);

        REQUIRE(result.has_value());
        CHECK(result->size() == 5);
        CHECK(ss.peek() == '"');
    }

    SUBCASE("Rejects no input only if the parser requires content") {
        std::stringstream ss{""};
        CHECK(string_content(ss).has_value());
    }
}

TEST_CASE("string_literal") {
    SUBCASE("Matches empty string") {
        std::stringstream ss{"\"\""};
        auto result = string_literal(ss);

        REQUIRE(result.has_value());
        CHECK(result.value().empty());
    }

    SUBCASE("Matches ordinary string") {
        std::stringstream ss{"\"hello world\""};
        auto result = string_literal(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "hello world");
    }

    SUBCASE("Matches string containing digits") {
        std::stringstream ss{"\"hello123\""};
        auto result = string_literal(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "hello123");
    }

    SUBCASE("Matches escaped newline") {
        std::stringstream ss{"\"hello\\nworld\""};
        auto result = string_literal(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "hello\\nworld");
    }

    SUBCASE("Matches multiple escape sequences") {
        std::stringstream ss{"\"\\n\\t\\r\\\\\\\"\\'\""};
        auto result = string_literal(ss);

        REQUIRE(result.has_value());
        CHECK(result.value() == "\\n\\t\\r\\\\\\\"\\'");
    }

    SUBCASE("Rejects unterminated string") {
        std::stringstream ss{"\"hello"};
        CHECK_FALSE(string_literal(ss).has_value());
    }

    SUBCASE("Rejects invalid escape sequence") {
        std::stringstream ss{"\"hello\\x\""};
        CHECK_FALSE(string_literal(ss).has_value());
    }

    SUBCASE("Rejects missing opening quotation mark") {
        std::stringstream ss{"hello\""};
        CHECK_FALSE(string_literal(ss).has_value());
    }

    SUBCASE("Does not accept raw newline") {
        std::stringstream ss{"\"hello\nworld\""};
        CHECK_FALSE(string_literal(ss).has_value());
    }
}
