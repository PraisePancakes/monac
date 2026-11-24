#ifndef LEXER_HPP
#define LEXER_HPP
#include <vector>

#include "../fsm.hpp"
#include "../tokens.hpp"
namespace monac::semantic {
using type = token::TOKEN_TYPE;
class lexer final : public worm_machine<std::string_view> {
    unsigned int _m_x{0};  // pos
    unsigned int _m_y{1};  // line
    std::vector<token> _m_tokens;
    std::string stringify() {
        std::string ret{""};
        for (const auto c : *this) ret += c;
        return ret;
    };

    token create_token(type t) {
        return token{.location{_m_x, _m_y}, .type = t, .value = stringify()};
    }

    token lex_partial_number() {
        while (std::isdigit(peek())) {
            _m_x++;
            advance();
        }
        return create_token(type::TK_NUMBER);
    };

    token lex_number() {
        while (std::isdigit(peek())) {
            _m_x++;
            advance();
        }
        if (peek() == '.') {
            _m_x++;
            advance();
        }
        while (std::isdigit(peek())) {
            _m_x++;
            advance();
        }
        return create_token(type::TK_NUMBER);
    }

    token lex_string_lit() {
        while (peek() != '\"') {
            _m_x++;
            advance();
        }
        advance();
        return create_token(type::TK_STRING_LIT);
    };

    token lex_rest(const char c) {
        if (std::isdigit(c)) {
            return lex_number();
        } else if (std::isalpha(c) || peek() == '_') {
            while (std::isalnum(peek()) || peek() == '_') {
                _m_x++;
                advance();
            }
            return create_token(type::TK_IDENTIFIER);
        }
        return create_token(type::TK_UNKNOWN);
        // ident
    };
    token lex_token(const char c) {
        switch (c) {
            case '\"':
                return lex_string_lit();
            case ';':
                return create_token(type::TK_SEMI);
            case ':':
                return create_token(type::TK_COLON);
            case '.': {
                if (std::isdigit(peek())) return lex_partial_number();
                return create_token(type::TK_DOT);
            }
            case '$':
                return create_token(type::TK_DOLLA);
            case '/':
                return create_token(type::TK_SLASH);
            case '*': {
                if (peek() == '=') {
                    _m_x++;
                    advance();
                    return create_token(type::TK_STAR_EQ);
                }
                return create_token(type::TK_STAR);
            }
            case ',':
                return create_token(type::TK_COMMA);
            case '(':
                return create_token(type::TK_LPAREN);
            case ')':
                return create_token(type::TK_RPAREN);
            case '[':
                return create_token(type::TK_LBRACE);
            case ']':
                return create_token(type::TK_RBRACE);
            case '{':
                return create_token(type::TK_LCURL);
            case '}':
                return create_token(type::TK_RCURL);
            case '=': {
                if (peek() == '=') {
                    _m_x++;
                    advance();
                    return create_token(type::TK_EQUALITY);
                };
                return create_token(type::TK_EQ);
            };
            case '+': {
                if (peek() == '+') {
                    _m_x++;
                    advance();
                    return create_token(type::TK_INC);
                };
                if (peek() == '=') {
                    _m_x++;
                    advance();
                    return create_token(type::TK_PLUS_EQ);
                }
                return create_token(type::TK_PLUS);
            }
            case '-': {
                if (peek() == '-') {
                    _m_x++;
                    advance();
                    return create_token(type::TK_DEC);
                };
                if (peek() == '=') {
                    _m_x++;
                    advance();
                    return create_token(type::TK_MINUS_EQ);
                }
                if (std::isdigit(peek())) return lex_number();
                if (peek() == '.') {
                    advance();
                    _m_x++;
                    return lex_partial_number();
                }

                return create_token(type::TK_MINUS);
            }
            case '!': {
                if (peek() == '=') {
                    _m_x++;
                    advance();
                    return create_token(type::TK_BANG_EQ);
                }
                return create_token(type::TK_BANG);
            }
            case '<': {
                if (peek() == '=') {
                    _m_x++;
                    advance();
                    return create_token(type::TK_LTE);
                }
                return create_token(type::TK_LT);
            }
            case '>': {
                if (peek() == '=') {
                    _m_x++;
                    advance();
                    return create_token(type::TK_GTE);
                }
                return create_token(type::TK_GT);
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
                    _m_x = 0;
                    _m_y++;
                } else if (c == '\r') {
                    if (peek() == '\n') advance();
                    _m_x = 0;
                    _m_y++;
                } else
                    _m_x++;
                this->finish_unit();
            }
            const char c = advance();
            _m_x++;
            _m_tokens.push_back(lex_token(c));
            this->finish_unit();
        };
        return _m_tokens;
    };
};
};  // namespace monac::semantic

#endif