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

    token create_token(type t) {
        return token{.location{m_x, m_y}, .type = t, .value = stringify()};
    }

    token lex_rest(const char c) {
        if (peek_prev() == '.' && std::isdigit(c)) {
            while (std::isdigit(peek())) {
                m_x++;
                advance();
            }
            return create_token(type::TK_NUMBER);
        } else if (std::isdigit(c)) {
            while (std::isdigit(peek())) {
                m_x++;
                advance();
            }
            if (peek() == '.') {
                m_x++;
                advance();
            }
            while (std::isdigit(peek())) {
                m_x++;
                advance();
            }
            return create_token(type::TK_NUMBER);
        } else if (c == '.') {
            while (std::isdigit(peek())) {
                m_x++;
                advance();
            }
            return create_token(type::TK_NUMBER);
        } else if (std::isalpha(c)) {
            while (std::isalnum(peek())) {
                m_x++;
                advance();
            }
            return create_token(type::TK_IDENTIFIER);
        }
        return create_token(type::TK_UNKNOWN);
        // ident
    };
    token lex_token(const char c) {
        switch (c) {
            case ';':
                return create_token(type::TK_SEMI);
            case ':':
                return create_token(type::TK_COLON);
            case '.': {
                if (std::isdigit(peek())) return lex_rest(c);
                return create_token(type::TK_DOT);
            }
            case '$':
                return create_token(type::TK_DOLLA);
            case '/':
                return create_token(type::TK_SLASH);
            case '*': {
                if (peek() == '=') {
                    m_x++;
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
                    m_x++;
                    advance();
                    return create_token(type::TK_EQUALITY);
                };
                return create_token(type::TK_EQ);
            };
            case '+': {
                if (peek() == '+') {
                    m_x++;
                    advance();
                    return create_token(type::TK_INC);
                };
                if (peek() == '=') {
                    m_x++;
                    advance();
                    return create_token(type::TK_PLUS_EQ);
                }
                return create_token(type::TK_PLUS);
            }
            case '-': {
                if (peek() == '-') {
                    m_x++;
                    advance();
                    return create_token(type::TK_DEC);
                };
                if (peek() == '=') {
                    m_x++;
                    advance();
                    return create_token(type::TK_MINUS_EQ);
                }
                if (std::isdigit(peek())) return lex_rest(c);

                return create_token(type::TK_MINUS);
            }
            case '!': {
                if (peek() == '=') {
                    m_x++;
                    advance();
                    return create_token(type::TK_BANG_EQ);
                }
                return create_token(type::TK_BANG);
            }
            case '<': {
                if (peek() == '=') {
                    m_x++;
                    advance();
                    return create_token(type::TK_LTE);
                }
                return create_token(type::TK_LT);
            }
            case '>': {
                if (peek() == '=') {
                    m_x++;
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