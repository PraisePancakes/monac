#ifndef TOKENS_HPP
#define TOKENS_HPP
#include <iostream>
namespace monac::semantic {
#define TOKEN_TABLE               \
    TOKEN(TK_SEMI, ';')           \
    TOKEN(TK_DOT, '.')            \
    TOKEN(TK_COLON, ':')          \
    TOKEN(TK_COMMA, ',')          \
    TOKEN(TK_SLASH, '/')          \
    TOKEN(TK_MINUS, '-')          \
    TOKEN(TK_PLUS, '+')           \
    TOKEN(TK_STAR, '*')           \
    TOKEN(TK_LPAREN, '(')         \
    TOKEN(TK_RPAREN, ')')         \
    TOKEN(TK_LBRACE, '[')         \
    TOKEN(TK_RBRACE, ']')         \
    TOKEN(TK_LCURL, '{')          \
    TOKEN(TK_RCURL, '}')          \
    TOKEN(TK_EQ, '=')             \
    TOKEN(TK_DOLLA, '$')          \
    TOKEN(TK_LT, '<')             \
    TOKEN(TK_GT, '>')             \
    TOKEN(TK_LTE, 0x80)           \
    TOKEN(TK_GTE, 0x81)           \
    TOKEN(TK_EQUALITY, 0x82)      \
    TOKEN(TK_IDENTIFIER, 0x83)    \
    TOKEN(TK_NUMBER, 0x84)        \
    TOKEN(TK_RESERVED, 0x85)      \
    TOKEN(TK_RESERVED_TYPE, 0x86) \
    TOKEN(TK_STRING_LIT, 0x87)    \
    TOKEN(TK_UNKNOWN, 0xFF)

struct token {
    enum class TOKEN_TYPE : std::uint8_t {
#define TOKEN(ENUM, VALUE) ENUM = VALUE,
        TOKEN_TABLE
#undef TOKEN
    } type;
    std::pair<unsigned int, unsigned int> location;
    std::string value;

    static std::string type_to_string(const TOKEN_TYPE token) {
        switch (token) {
#define TOKEN(ENUM, _)                     \
    case TOKEN_TYPE::ENUM:                 \
        return std::format("[{}]", #ENUM); \
        break;
            TOKEN_TABLE
#undef TOKEN
        };
    };
};

};  // namespace monac::semantic

#endif