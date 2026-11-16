
#include <mona/mona.hpp>
#include <sstream>
#include <string>
#include <vector>

#include "../../src/monac/prep/parsers_rewrite.hpp"
#include "../doctest.h"

using namespace monac::prep::parsers_rewrite;

TEST_SUITE("Preprocessor Replacement Tokens") {
    TEST_CASE("Identifier") {
        SUBCASE("Simple identifier") {
            std::istringstream input{"FOO"};
            auto result = replacement_token(input);

            REQUIRE(result.has_value());
            CHECK(*result == "FOO");
        }

        SUBCASE("Identifier containing underscores") {
            std::istringstream input{"_FOO_BAR"};
            auto result = replacement_token(input);

            REQUIRE(result.has_value());
            CHECK(*result == "_FOO_BAR");
        }

        SUBCASE("Identifier containing numbers") {
            std::istringstream input{"FOO123"};
            auto result = replacement_token(input);

            REQUIRE(result.has_value());
            CHECK(*result == "FOO123");
        }
    }

    TEST_CASE("Punctuator") {
        SUBCASE("Single-character punctuator") {
            std::istringstream input{"+"};
            auto result = replacement_token(input);

            REQUIRE(result.has_value());
            CHECK(*result == "+");
        }

        SUBCASE("Arithmetic operator") {
            std::istringstream input{"-"};
            auto result = replacement_token(input);

            REQUIRE(result.has_value());
            CHECK(*result == "-");
        }

        SUBCASE("Multiple-character punctuator") {
            std::istringstream input{"++"};
            auto result = replacement_token(input);

            REQUIRE(result.has_value());
            CHECK(*result == "++");
        }

        SUBCASE("Comparison operator") {
            std::istringstream input{"=="};
            auto result = replacement_token(input);

            REQUIRE(result.has_value());
            CHECK(*result == "==");
        }
    }

    TEST_CASE("String literal") {
        SUBCASE("Empty string") {
            std::istringstream input{"\"\""};
            auto result = replacement_token(input);

            REQUIRE(result.has_value());
            CHECK(*result == "\"\"");
        }

        SUBCASE("Simple string") {
            std::istringstream input{"\"hello world\""};
            auto result = replacement_token(input);

            REQUIRE(result.has_value());
            CHECK(*result == "\"hello world\"");
        }

        SUBCASE("String containing escaped characters") {
            std::istringstream input{"\"hello\\nworld\""};
            auto result = replacement_token(input);

            REQUIRE(result.has_value());
            CHECK(*result == "\"hello\\nworld\"");
        }
    }

    TEST_CASE("Number") {
        SUBCASE("Integer") {
            std::istringstream input{"123"};
            auto result = replacement_token(input);

            REQUIRE(result.has_value());
            CHECK(*result == "123");
        }

        SUBCASE("Zero") {
            std::istringstream input{"0"};
            auto result = replacement_token(input);

            REQUIRE(result.has_value());
            CHECK(*result == "0");
        }
        SUBCASE("single dig") {
            std::istringstream input{"5"};
            auto result = replacement_token(input);

            REQUIRE(result.has_value());
            CHECK(*result == "5");
        }
        SUBCASE("Floating-point number") {
            std::istringstream input{"3.14"};
            auto result = replacement_token(input);

            REQUIRE(result.has_value());
            CHECK(*result == "3.14");
        }
    }

    TEST_CASE("Invalid replacement token") {
        SUBCASE("Empty input") {
            std::istringstream input{""};
            auto result = replacement_token(input);

            CHECK_FALSE(result.has_value());
        }

        SUBCASE("Whitespace") {
            std::istringstream input{"   "};
            auto result = replacement_token(input);

            CHECK_FALSE(result.has_value());
        }
    }
}

TEST_SUITE("Preprocessor Replacement Lists") {
    TEST_CASE("Empty replacement list") {
        std::istringstream input{""};
        auto result = replacement_list(input);

        REQUIRE(result.has_value());
        CHECK(result->empty());
    }

    TEST_CASE("Single replacement token") {
        SUBCASE("Identifier") {
            std::istringstream input{"FOO"};
            auto result = replacement_list(input);

            REQUIRE(result.has_value());
            CHECK(*result == std::vector<std::string>{"FOO"});
        }

        SUBCASE("Number") {
            std::istringstream input{"42"};
            auto result = replacement_list(input);

            REQUIRE(result.has_value());
            CHECK(result.value().size() == 1);
        }

        SUBCASE("Punctuator") {
            std::istringstream input{"+"};
            auto result = replacement_list(input);

            REQUIRE(result.has_value());
            CHECK(*result == std::vector<std::string>{"+"});
        }

        SUBCASE("String literal") {
            std::istringstream input{"\"hello\""};
            auto result = replacement_list(input);

            REQUIRE(result.has_value());
            CHECK(*result == std::vector<std::string>{"\"hello\""});
        }
    }

    TEST_CASE("Multiple replacement tokens") {
        SUBCASE("Adjacent identifiers") {
            std::istringstream input{"FOO BAR"};
            auto result = replacement_list(input);

            REQUIRE(result.has_value());
            CHECK(*result == std::vector<std::string>{
                                 "FOO", "BAR"});
        }

        SUBCASE("Identifier and number") {
            std::istringstream input{"FOO 123"};
            auto result = replacement_list(input);

            REQUIRE(result.has_value());
            CHECK(*result == std::vector<std::string>{"FOO", "123"});
        }

        SUBCASE("Arithmetic expression") {
            std::istringstream input{"5 + 3 - 4"};
            auto result = replacement_list(input);

            REQUIRE(result.has_value());
            CHECK(*result == std::vector<std::string>{"5", "+", "3", "-", "4"});
        }

        SUBCASE("Mixed token types") {
            std::istringstream input{"FOO + 123 * \"hello\""};
            auto result = replacement_list(input);

            REQUIRE(result.has_value());
            CHECK(*result == std::vector<std::string>{
                                 "FOO", "+", "123",
                                 "*", "\"hello\""});
        }
    }

    TEST_CASE("Whitespace handling") {
        SUBCASE("Leading whitespace") {
            std::istringstream input{"   FOO"};
            auto result = replacement_list(input);

            REQUIRE(result.has_value());
            CHECK(*result == std::vector<std::string>{"FOO"});
        }

        SUBCASE("Trailing whitespace") {
            std::istringstream input{"FOO   "};
            auto result = replacement_list(input);

            REQUIRE(result.has_value());
            CHECK(*result == std::vector<std::string>{"FOO"});
        }

        SUBCASE("Leading and trailing whitespace") {
            std::istringstream input{"   FOO   "};
            auto result = replacement_list(input);

            REQUIRE(result.has_value());
            CHECK(*result == std::vector<std::string>{"FOO"});
        }

        SUBCASE("Multiple spaces between tokens") {
            std::istringstream input{"FOO     BAR"};
            auto result = replacement_list(input);

            REQUIRE(result.has_value());
            CHECK(*result == std::vector<std::string>{"FOO", "BAR"});
        }

        SUBCASE("Tabs between tokens") {
            std::istringstream input{"FOO\tBAR"};
            auto result = replacement_list(input);

            REQUIRE(result.has_value());
            CHECK(*result == std::vector<std::string>{"FOO", "BAR"});
        }
    }

    TEST_CASE("Complex replacement expressions") {
        SUBCASE("Arithmetic expression") {
            std::istringstream input{"(5 + 3) * 2"};
            auto result = replacement_list(input);

            REQUIRE(result.has_value());
            CHECK(*result == std::vector<std::string>{"(", "5", "+", "3", ")", "*", "2"});
        }

        SUBCASE("Comparison expression") {
            std::istringstream input{"VALUE >= 100"};
            auto result = replacement_list(input);

            REQUIRE(result.has_value());
            CHECK(*result == std::vector<std::string>{"VALUE", ">=", "100"});
        }

        SUBCASE("Function-like expression") {
            std::istringstream input{"MAX(a, b)"};
            auto result = replacement_list(input);

            REQUIRE(result.has_value());
            CHECK(*result == std::vector<std::string>{
                                 "MAX", "(", "a", ",", "b", ")"});
        }
        SUBCASE("Optional whitespace") {
            std::istringstream input{" 10"};

            auto result = utils::unit::opt_white_space(input);

            REQUIRE(result.has_value());
            CHECK(result.value() == " ");
            CHECK(input.peek() == '1');
        }

        SUBCASE("Number after whitespace") {
            std::istringstream input{" 10"};

            auto whitespace = utils::unit::opt_white_space(input);
            REQUIRE(whitespace.has_value());

            auto result = utils::unit::number(input);

            REQUIRE(result.has_value());
            CHECK(result.value() == "10");
        }

        SUBCASE("Replacement token after whitespace") {
            std::istringstream input{" 10"};

            auto whitespace = utils::unit::opt_white_space(input);
            REQUIRE(whitespace.has_value());

            auto result = replacement_token(input);

            REQUIRE(result.has_value());
            CHECK(result.value() == "10");
        }
        SUBCASE("Mixed identifiers, numbers and punctuators") {
            std::istringstream input{"FOO123\t\t+= BAR_456      * 10"};
            auto result = replacement_list(input);

            REQUIRE(result.has_value());
            CHECK(result.value().size() == 5);
            CHECK(*result == std::vector<std::string>{"FOO123", "+=", "BAR_456", "*", "10"});
        }
    }

    TEST_CASE("Preprocessor Parameter Lists") {
        SUBCASE("Single parameter") {
            std::istringstream input{"x"};

            auto result = params_list(input);

            REQUIRE(result.has_value());
            REQUIRE(result->size() == 1);
            CHECK((*result)[0] == "x");
        }

        SUBCASE("Multiple parameters") {
            std::istringstream input{"x,y,z"};

            auto result = params_list(input);

            REQUIRE(result.has_value());
            REQUIRE(result->size() == 3);
            CHECK((*result)[0] == "x");
            CHECK((*result)[1] == "y");
            CHECK((*result)[2] == "z");
        }

        SUBCASE("Parameters with whitespace") {
            std::istringstream input{"x , y , z"};

            auto result = params_list(input);

            REQUIRE(result.has_value());
            REQUIRE(result->size() == 3);
            CHECK((*result)[0] == "x");
            CHECK((*result)[1] == "y");
            CHECK((*result)[2] == "z");
        }

        SUBCASE("Parameters with tabs") {
            std::istringstream input{"x\t,\ty\t,\tz"};

            auto result = params_list(input);

            REQUIRE(result.has_value());
            REQUIRE(result->size() == 3);
            CHECK((*result)[0] == "x");
            CHECK((*result)[1] == "y");
            CHECK((*result)[2] == "z");
        }

        SUBCASE("Parameters with underscores") {
            std::istringstream input{"_first, second_value, third_"};

            auto result = params_list(input);

            REQUIRE(result.has_value());
            REQUIRE(result->size() == 3);
            CHECK((*result)[0] == "_first");
            CHECK((*result)[1] == "second_value");
            CHECK((*result)[2] == "third_");
        }

        SUBCASE("Preserves following token") {
            std::istringstream input{"x, y)"};

            auto result = params_list(input);

            REQUIRE(result.has_value());
            REQUIRE(result->size() == 2);
            CHECK((*result)[0] == "x");
            CHECK((*result)[1] == "y");
            CHECK(input.peek() == ')');
        }

        SUBCASE("Rejects empty input") {
            std::istringstream input{""};

            CHECK_FALSE(params_list(input).has_value());
        }

        SUBCASE("Rejects leading comma") {
            std::istringstream input{",x"};

            CHECK_FALSE(params_list(input).has_value());
        }

        SUBCASE("Accepts trailing comma") {
            std::istringstream input{"x,"};

            CHECK(params_list(input).has_value());
        }

        SUBCASE("Rejects consecutive commas") {
            std::istringstream input{"x,,y"};

            auto result = params_list(input);

            REQUIRE(result.has_value());
            REQUIRE(result->size() == 1);
            CHECK((*result)[0] == "x");
            CHECK(input.peek() == ',');
        }
    }

    TEST_CASE("Optional Preprocessor Parameter Lists") {
        SUBCASE("Empty parameter list") {
            std::istringstream input{""};

            auto result = opt_params_list(input);

            REQUIRE(result.has_value());
            CHECK(result->empty());
        }

        SUBCASE("Single parameter") {
            std::istringstream input{"x"};

            auto result = opt_params_list(input);

            REQUIRE(result.has_value());
            REQUIRE(result->size() == 1);
            CHECK((*result)[0] == "x");
        }

        SUBCASE("Multiple parameters") {
            std::istringstream input{"x, y, z"};

            auto result = opt_params_list(input);

            REQUIRE(result.has_value());
            REQUIRE(result->size() == 3);
            CHECK((*result)[0] == "x");
            CHECK((*result)[1] == "y");
            CHECK((*result)[2] == "z");
        }

        SUBCASE("Preserves following token") {
            std::istringstream input{"x, y)"};

            auto result = opt_params_list(input);

            REQUIRE(result.has_value());
            REQUIRE(result->size() == 2);
            CHECK((*result)[0] == "x");
            CHECK((*result)[1] == "y");
            CHECK(input.peek() == ')');
        }

        SUBCASE("Preserves entire input when empty") {
            std::istringstream input{")"};

            auto result = opt_params_list(input);

            REQUIRE(result.has_value());
            CHECK(result->empty());
            CHECK(input.peek() == ')');
        }

        SUBCASE("Rejects leading comma") {
            std::istringstream input{",x"};

            auto result = opt_params_list(input);

            REQUIRE(result.has_value());
            CHECK(result->empty());
            CHECK(input.peek() == ',');
        }
    }
}

TEST_SUITE("function_like") {
    TEST_CASE("single parameter") {
        std::istringstream input{"FOO(x) x"};

        auto result = function_like(input);

        REQUIRE(result.has_value());
        REQUIRE(std::holds_alternative<function>(*result));

        const auto& fn = std::get<function>(*result);
        CHECK(fn.identifier == "FOO");
        REQUIRE(fn.parameters.size() == 1);
        CHECK(fn.parameters[0] == "x");

        REQUIRE(fn.replacement.size() == 1);
        CHECK(fn.replacement[0] == "x");

        REQUIRE(fn.parameter_map.size() == 1);
        CHECK(fn.parameter_map.at("x") == 0);
    }

    TEST_CASE("multiple parameters") {
        std::istringstream input{"ADD(a, b) a + b"};

        auto result = function_like(input);

        REQUIRE(result.has_value());
        REQUIRE(std::holds_alternative<function>(*result));

        const auto& fn = std::get<function>(*result);
        CHECK(fn.identifier == "ADD");
        REQUIRE(fn.parameters.size() == 2);
        CHECK(fn.parameters[0] == "a");
        CHECK(fn.parameters[1] == "b");

        REQUIRE(fn.replacement.size() == 3);
        CHECK(fn.replacement[0] == "a");
        CHECK(fn.replacement[1] == "+");
        CHECK(fn.replacement[2] == "b");

        REQUIRE(fn.parameter_map.size() == 2);
        CHECK(fn.parameter_map.at("a") == 0);
        CHECK(fn.parameter_map.at("b") == 1);
    }

    TEST_CASE("three parameters preserve their indices") {
        std::istringstream input{"FUNC(first, second, third) third first second"};

        auto result = function_like(input);

        REQUIRE(result.has_value());
        const auto& fn = std::get<function>(*result);

        REQUIRE(fn.parameters.size() == 3);
        CHECK(fn.parameters[0] == "first");
        CHECK(fn.parameters[1] == "second");
        CHECK(fn.parameters[2] == "third");

        REQUIRE(fn.parameter_map.size() == 3);
        CHECK(fn.parameter_map.at("first") == 0);
        CHECK(fn.parameter_map.at("second") == 1);
        CHECK(fn.parameter_map.at("third") == 2);
    }

    TEST_CASE("empty parameter list") {
        std::istringstream input{"EMPTY() 42"};

        auto result = function_like(input);

        REQUIRE(result.has_value());
        REQUIRE(std::holds_alternative<function>(*result));

        const auto& fn = std::get<function>(*result);
        CHECK(fn.identifier == "EMPTY");
        CHECK(fn.parameters.empty());
        CHECK(fn.parameter_map.empty());

        REQUIRE(fn.replacement.size() == 1);
        CHECK(fn.replacement[0] == "42");
    }

    TEST_CASE("whitespace around parameters") {
        std::istringstream input{"FUNC(  a , b , c  ) a + b + c"};

        auto result = function_like(input);

        REQUIRE(result.has_value());

        const auto& fn = std::get<function>(*result);
        CHECK(fn.identifier == "FUNC");

        REQUIRE(fn.parameters.size() == 3);
        CHECK(fn.parameters[0] == "a");
        CHECK(fn.parameters[1] == "b");
        CHECK(fn.parameters[2] == "c");

        CHECK(fn.parameter_map.at("a") == 0);
        CHECK(fn.parameter_map.at("b") == 1);
        CHECK(fn.parameter_map.at("c") == 2);
    }

    TEST_CASE("replacement contains numbers and punctuators") {
        std::istringstream input{"CALC(a, b) (a * 10) + b - 5"};

        auto result = function_like(input);

        REQUIRE(result.has_value());

        const auto& fn = std::get<function>(*result);
        CHECK(fn.identifier == "CALC");

        REQUIRE(fn.replacement.size() == 9);
        CHECK(fn.replacement[0] == "(");
        CHECK(fn.replacement[1] == "a");
        CHECK(fn.replacement[2] == "*");
        CHECK(fn.replacement[3] == "10");
        CHECK(fn.replacement[4] == ")");
        CHECK(fn.replacement[5] == "+");
        CHECK(fn.replacement[6] == "b");
        CHECK(fn.replacement[7] == "-");
        CHECK(fn.replacement[8] == "5");
    }

    TEST_CASE("replacement contains string literals") {
        std::istringstream input{R"(MESSAGE(x) "hello" x)"};

        auto result = function_like(input);

        REQUIRE(result.has_value());

        const auto& fn = std::get<function>(*result);
        CHECK(fn.identifier == "MESSAGE");

        REQUIRE(fn.replacement.size() == 2);
        CHECK(fn.replacement[0] == R"("hello")");
        CHECK(fn.replacement[1] == "x");
    }

    TEST_CASE("replacement can be empty") {
        std::istringstream input{"EMPTY(x)"};

        auto result = function_like(input);

        REQUIRE(result.has_value());

        const auto& fn = std::get<function>(*result);
        CHECK(fn.identifier == "EMPTY");
        CHECK(fn.parameters == std::vector<std::string>{"x"});
        CHECK(fn.replacement.empty());
        CHECK(fn.parameter_map.at("x") == 0);
    }

    TEST_CASE("parameter names are preserved in declaration order") {
        std::istringstream input{"ORDER(z, a, m) z a m"};

        auto result = function_like(input);

        REQUIRE(result.has_value());

        const auto& fn = std::get<function>(*result);

        REQUIRE(fn.parameters.size() == 3);
        CHECK(fn.parameters == std::vector<std::string>{"z", "a", "m"});

        CHECK(fn.parameter_map.at("z") == 0);
        CHECK(fn.parameter_map.at("a") == 1);
        CHECK(fn.parameter_map.at("m") == 2);
    }

    TEST_CASE("missing closing parenthesis fails") {
        std::istringstream input{"FUNC(a, b a + b"};

        auto result = function_like(input);

        CHECK_FALSE(result.has_value());
    }

    TEST_CASE("missing opening parenthesis fails") {
        std::istringstream input{"FUNC a + b"};

        auto result = function_like(input);

        CHECK_FALSE(result.has_value());
    }

    TEST_CASE("missing identifier fails") {
        std::istringstream input{"(a, b) a + b"};

        auto result = function_like(input);

        CHECK_FALSE(result.has_value());
    }

    TEST_CASE("invalid parameter list fails") {
        std::istringstream input{"FUNC(a,,b) a + b"};

        auto result = function_like(input);

        CHECK_FALSE(result.has_value());
    }

    TEST_CASE("trailing comma in parameter list fails") {
        std::istringstream input{"FUNC(a, b,) a + b"};

        auto result = function_like(input);

        CHECK_FALSE(result.has_value());
    }

    TEST_CASE("Space between ident and params fails") {
        std::istringstream input{"FUNC  (a, b, c) a + b"};
        auto result = function_like(input);
        CHECK_FALSE(result.has_value());
    }

    TEST_SUITE("object_like") {
        TEST_CASE("simple identifier replacement") {
            std::istringstream input{"FOO 42"};

            auto result = object_like(input);

            REQUIRE(result.has_value());
            REQUIRE(std::holds_alternative<object>(*result));

            const auto& obj = std::get<object>(*result);
            CHECK(obj.identifier == "FOO");

            REQUIRE(obj.replacement.size() == 1);
            CHECK(obj.replacement[0] == "42");
        }

        TEST_CASE("multiple replacement tokens") {
            std::istringstream input{"ADD a + b"};

            auto result = object_like(input);

            REQUIRE(result.has_value());

            const auto& obj = std::get<object>(*result);
            CHECK(obj.identifier == "ADD");

            REQUIRE(obj.replacement.size() == 3);
            CHECK(obj.replacement[0] == "a");
            CHECK(obj.replacement[1] == "+");
            CHECK(obj.replacement[2] == "b");
        }

        TEST_CASE("replacement with numbers and operators") {
            std::istringstream input{"VALUE 10 * 20 + 5"};

            auto result = object_like(input);

            REQUIRE(result.has_value());

            const auto& obj = std::get<object>(*result);
            CHECK(obj.identifier == "VALUE");

            REQUIRE(obj.replacement.size() == 5);
            CHECK(obj.replacement[0] == "10");
            CHECK(obj.replacement[1] == "*");
            CHECK(obj.replacement[2] == "20");
            CHECK(obj.replacement[3] == "+");
            CHECK(obj.replacement[4] == "5");
        }

        TEST_CASE("replacement with a single identifier") {
            std::istringstream input{"VALUE OTHER"};

            auto result = object_like(input);

            REQUIRE(result.has_value());

            const auto& obj = std::get<object>(*result);
            CHECK(obj.identifier == "VALUE");

            REQUIRE(obj.replacement.size() == 1);
            CHECK(obj.replacement[0] == "OTHER");
        }

        TEST_CASE("replacement with string literal") {
            std::istringstream input{R"(MESSAGE "hello world")"};

            auto result = object_like(input);

            REQUIRE(result.has_value());

            const auto& obj = std::get<object>(*result);
            CHECK(obj.identifier == "MESSAGE");

            REQUIRE(obj.replacement.size() == 1);
            CHECK(obj.replacement[0] == R"("hello world")");
        }

        TEST_CASE("replacement with parentheses") {
            std::istringstream input{"VALUE (10 + 20)"};

            auto result = object_like(input);

            REQUIRE(result.has_value());

            const auto& obj = std::get<object>(*result);
            REQUIRE(obj.replacement.size() == 5);

            CHECK(obj.replacement[0] == "(");
            CHECK(obj.replacement[1] == "10");
            CHECK(obj.replacement[2] == "+");
            CHECK(obj.replacement[3] == "20");
            CHECK(obj.replacement[4] == ")");
        }

        TEST_CASE("replacement with empty list") {
            std::istringstream input{"EMPTY"};

            auto result = object_like(input);

            REQUIRE(result.has_value());

            const auto& obj = std::get<object>(*result);
            CHECK(obj.identifier == "EMPTY");
            CHECK(obj.replacement.empty());
        }

        TEST_CASE("identifier with underscores and numbers") {
            std::istringstream input{"MY_VALUE_123 100"};

            auto result = object_like(input);

            REQUIRE(result.has_value());

            const auto& obj = std::get<object>(*result);
            CHECK(obj.identifier == "MY_VALUE_123");

            REQUIRE(obj.replacement.size() == 1);
            CHECK(obj.replacement[0] == "100");
        }

        TEST_CASE("leading whitespace") {
            std::istringstream input{"   VALUE 42"};

            auto result = object_like(input);

            REQUIRE(result.has_value());

            const auto& obj = std::get<object>(*result);
            CHECK(obj.identifier == "VALUE");
            REQUIRE(obj.replacement.size() == 1);
            CHECK(obj.replacement[0] == "42");
        }

        TEST_CASE("multiple whitespace characters") {
            std::istringstream input{"VALUE     a   +    b"};

            auto result = object_like(input);

            REQUIRE(result.has_value());

            const auto& obj = std::get<object>(*result);
            CHECK(obj.identifier == "VALUE");

            REQUIRE(obj.replacement.size() == 3);
            CHECK(obj.replacement[0] == "a");
            CHECK(obj.replacement[1] == "+");
            CHECK(obj.replacement[2] == "b");
        }

        TEST_CASE("missing identifier fails") {
            std::istringstream input{"42"};

            auto result = object_like(input);

            CHECK_FALSE(result.has_value());
        }

        TEST_CASE("empty input fails") {
            std::istringstream input{""};

            auto result = object_like(input);

            CHECK_FALSE(result.has_value());
        }

        TEST_CASE("invalid identifier fails") {
            std::istringstream input{"123ABC value"};

            auto result = object_like(input);

            CHECK_FALSE(result.has_value());
        }

        TEST_CASE("punctuation instead of identifier fails") {
            std::istringstream input{"@define VALUE"};

            auto result = object_like(input);

            CHECK_FALSE(result.has_value());
        }

        TEST_CASE("object-like is not function-like") {
            std::istringstream input{"CALL(x) x"};
            auto result = object_like(input);
            CHECK_FALSE(result.has_value());
        }
    }
}