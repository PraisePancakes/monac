#ifndef LEXER_HPP
#define LEXER_HPP
#include <vector>

#include "../fsm.hpp"
#include "../tokens.hpp"
namespace monac::semantic {
using type = token::TOKEN_TYPE;
class lexer final : protected worm_machine<std::string> {
    unsigned int m_x{0};  // pos
    unsigned int m_y{0};  // line
    std::vector<token> m_tokens;
    std::string stringify() {
        std::string ret{""};
        for (const auto c : *this) ret += c;
        return ret;
    };
    token lex_rest(const char c) {
        if (peek_prev() == '.' && std::isdigit(c)) {
            while (std::isdigit(peek())) {
                m_x++;
                advance();
            }
            return token{.location{m_x, m_y}, .type = type::TK_NUMBER, .value = stringify()};
        } else if (std::isdigit(c)) {
            while (std::isdigit(peek())) {
                m_x++;
                advance();
            }
            return token{.location{m_x, m_y}, .type = type::TK_NUMBER, .value = stringify()};
        } else if (c == '.') {
            while (std::isdigit(peek())) {
                m_x++;
                advance();
            }
            return token{.location{m_x, m_y}, .type = type::TK_NUMBER, .value = stringify()};
        } else if (std::isalpha(c)) {
            while (std::isalnum(peek())) {
                m_x++;
                advance();
            }
            return token{.location{m_x, m_y}, .type = type::TK_IDENTIFIER, .value = stringify()};
        }
        return token{.location{m_x, m_y}, .type = type::TK_UNKNOWN, .value = stringify()};
        // ident
    };
    token lex_token(const char c) {
        std::string value(1, c);
        switch (c) {
            case ';':
                return token{.location{m_x, m_y}, .type = type::TK_SEMI, .value = value};
            case ':':
                return token{.location{m_x, m_y}, .type = type::TK_COLON, .value = value};
            case '.': {
                if (std::isdigit(peek())) return lex_rest(c);
                return token{.location{m_x, m_y}, .type = type::TK_DOT, .value = value};
            }
            case '$':
                return token{.location{m_x, m_y}, .type = type::TK_DOLLA, .value = value};
            case '/':
                return token{.location{m_x, m_y}, .type = type::TK_SLASH, .value = value};
            case '*': {
                if (peek() == '=') {
                    m_x++;
                    advance();
                    return token{.location{m_x, m_y}, .type = type::TK_STAR_EQ, .value = stringify()};
                }
                return token{.location{m_x, m_y}, .type = type::TK_STAR, .value = value};
            }
            case ',':
                return token{.location{m_x, m_y}, .type = type::TK_COMMA, .value = value};
            case '(':
                return token{.location{m_x, m_y}, .type = type::TK_LPAREN, .value = value};
            case ')':
                return token{.location{m_x, m_y}, .type = type::TK_RPAREN, .value = value};
            case '[':
                return token{.location{m_x, m_y}, .type = type::TK_LBRACE, .value = value};
            case ']':
                return token{.location{m_x, m_y}, .type = type::TK_RBRACE, .value = value};
            case '{':
                return token{.location{m_x, m_y}, .type = type::TK_LCURL, .value = value};
            case '}':
                return token{.location{m_x, m_y}, .type = type::TK_RCURL, .value = value};
            case '=': {
                if (peek() == '=') {
                    m_x++;
                    advance();
                    return token{.location{m_x, m_y}, .type = type::TK_EQUALITY, .value = stringify()};
                };
                return token{.location{m_x, m_y}, .type = type::TK_EQ, .value = value};
            };
            case '+': {
                if (peek() == '+') {
                    m_x++;
                    advance();
                    return token{.location{m_x, m_y}, .type = type::TK_INC, .value = stringify()};
                };
                if (peek() == '=') {
                    m_x++;
                    advance();
                    return token{.location{m_x, m_y}, .type = type::TK_PLUS_EQ, .value = stringify()};
                }
                return token{.location{m_x, m_y}, .type = type::TK_PLUS, .value = value};
            }
            case '-': {
                if (peek() == '-') {
                    m_x++;
                    advance();
                    return token{.location{m_x, m_y}, .type = type::TK_DEC, .value = stringify()};
                };
                if (peek() == '=') {
                    m_x++;
                    advance();
                    return token{.location{m_x, m_y}, .type = type::TK_MINUS_EQ, .value = stringify()};
                }
                if (std::isdigit(peek())) return lex_rest(c);

                return token{.location{m_x, m_y}, .type = type::TK_MINUS, .value = value};
            }
            case '!': {
                if (peek() == '=') {
                    m_x++;
                    advance();
                    return token{.location{m_x, m_y}, .type = type::TK_BANG_EQ, .value = stringify()};
                }
                return token{.location{m_x, m_y}, .type = type::TK_BANG, .value = value};
            }
            case '<': {
                if (peek() == '=') {
                    m_x++;
                    advance();
                    return token{.location{m_x, m_y}, .type = type::TK_LTE, .value = stringify()};
                }
                return token{.location{m_x, m_y}, .type = type::TK_LT, .value = value};
            }
            case '>': {
                if (peek() == '=') {
                    m_x++;
                    advance();
                    return token{.location{m_x, m_y}, .type = type::TK_GTE, .value = stringify()};
                }
                return token{.location{m_x, m_y}, .type = type::TK_GT, .value = value};
            }
        };
        return lex_rest(c);
    };

   public:
    lexer() = delete;
    lexer(lexer&&) = default;
    lexer(const lexer&) = default;
    lexer& operator=(const lexer&) = default;
    lexer& operator=(lexer&&) = default;
    lexer(const std::string& s) : worm_machine{s} {};

    std::span<token> lex() {
        while (!this->is_end()) {
            while (std::isspace(static_cast<unsigned char>(peek()))) {
                const char c = advance();
                if (c == '\n') {
                    m_x = 0;
                    m_y++;
                } else if (c == '\r') {
                    if (peek() == '\n') advance();
                    m_x = 0;
                    m_y++;
                } else
                    m_x++;
                this->finish_unit();
            }
            const char c = advance();
            m_x++;
            m_tokens.push_back(lex_token(c));
            this->finish_unit();
        };
        return m_tokens;
    };
};
};  // namespace monac::semantic

#endif