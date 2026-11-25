#ifndef LEXER_HPP
#define LEXER_HPP
#include <vector>

#include "../fsm.hpp"
#include "../tokens.hpp"

namespace monac::semantic {
using type = token::TOKEN_TYPE;

class lex_error final : public std::exception {
    std::string _m_what;

   public:
    lex_error() : _m_what{""} {};
    lex_error(const lex_error&) = default;
    lex_error(lex_error&&) = default;
    lex_error& operator=(const lex_error&) = default;
    lex_error& operator=(lex_error&&) = default;
    ~lex_error() = default;
    lex_error(const std::string& what, unsigned int x, unsigned int y) : _m_what{std::format("[MONAC-LEXER-EXCEPT]: {} at line {}:{}", what, y, x)} {};
    [[nodiscard]] const char* what() const noexcept override {
        return _m_what.c_str();
    };
};
class lexer final {
    std::string _m_source;
    worm_machine<std::string> _m_fsm;
    unsigned int _m_x{0};  // pos
    unsigned int _m_y{1};  // line
    std::vector<token> _m_tokens;
    std::string stringify() {
        std::string ret{""};
        for (const auto c : _m_fsm) ret += c;
        return ret;
    };

    token create_token(type t) {
        return token{.location{_m_x, _m_y}, .type = t, .value = stringify()};
    }

    token lex_partial_number() {
        while (std::isdigit(_m_fsm.peek())) {
            _m_x++;
            _m_fsm.advance();
        }
        return create_token(type::TK_NUMBER);
    };

    token lex_number() {
        while (std::isdigit(_m_fsm.peek())) {
            _m_x++;
            _m_fsm.advance();
        }
        if (_m_fsm.peek() == '.') {
            _m_x++;
            _m_fsm.advance();
        }
        while (std::isdigit(_m_fsm.peek())) {
            _m_x++;
            _m_fsm.advance();
        }
        return create_token(type::TK_NUMBER);
    }

    token lex_string_lit() {
        auto start_x = _m_x;
        auto start_y = _m_y;

        while (_m_fsm.peek() != '\"') {
            if (_m_fsm.is_end()) throw lex_error("non terminated string literal, expected \" to close string", start_x, start_y);
            _m_x++;
            _m_fsm.advance();
        }
        _m_fsm.advance();
        return create_token(type::TK_STRING_LIT);
    };

    token lex_rest(const char c) {
        if (std::isdigit(c)) {
            return lex_number();
        } else if (std::isalpha(c) || _m_fsm.peek() == '_') {
            while (std::isalnum(_m_fsm.peek()) || _m_fsm.peek() == '_') {
                _m_x++;
                _m_fsm.advance();
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
                if (std::isdigit(_m_fsm.peek())) return lex_partial_number();
                return create_token(type::TK_DOT);
            }
            case '$':
                return create_token(type::TK_DOLLA);
            case '/':
                return create_token(type::TK_SLASH);
            case '*': {
                if (_m_fsm.peek() == '=') {
                    _m_x++;
                    _m_fsm.advance();
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
                if (_m_fsm.peek() == '=') {
                    _m_x++;
                    _m_fsm.advance();
                    return create_token(type::TK_EQUALITY);
                };
                return create_token(type::TK_EQ);
            };
            case '+': {
                if (_m_fsm.peek() == '+') {
                    _m_x++;
                    _m_fsm.advance();
                    return create_token(type::TK_INC);
                };
                if (_m_fsm.peek() == '=') {
                    _m_x++;
                    _m_fsm.advance();
                    return create_token(type::TK_PLUS_EQ);
                }
                return create_token(type::TK_PLUS);
            }
            case '-': {
                if (_m_fsm.peek() == '-') {
                    _m_x++;
                    _m_fsm.advance();
                    return create_token(type::TK_DEC);
                };
                if (_m_fsm.peek() == '=') {
                    _m_x++;
                    _m_fsm.advance();
                    return create_token(type::TK_MINUS_EQ);
                }
                if (std::isdigit(_m_fsm.peek())) return lex_number();
                if (_m_fsm.peek() == '.') {
                    _m_fsm.advance();
                    _m_x++;
                    return lex_partial_number();
                }

                return create_token(type::TK_MINUS);
            }
            case '!': {
                if (_m_fsm.peek() == '=') {
                    _m_x++;
                    _m_fsm.advance();
                    return create_token(type::TK_BANG_EQ);
                }
                return create_token(type::TK_BANG);
            }
            case '<': {
                if (_m_fsm.peek() == '=') {
                    _m_x++;
                    _m_fsm.advance();
                    return create_token(type::TK_LTE);
                }
                return create_token(type::TK_LT);
            }
            case '>': {
                if (_m_fsm.peek() == '=') {
                    _m_x++;
                    _m_fsm.advance();
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
    lexer(const std::string& s) : _m_source{s}, _m_fsm{_m_source} {};
    lexer(std::string&& s) : _m_source{std::move(s)}, _m_fsm{_m_source} {};

    std::vector<token> lex() {
        while (!this->_m_fsm.is_end()) {
            while (std::isspace(static_cast<unsigned char>(_m_fsm.peek()))) {
                const char c = _m_fsm.advance();
                if (c == '\n') {
                    _m_x = 0;
                    _m_y++;
                } else if (c == '\r') {
                    if (_m_fsm.peek() == '\n') _m_fsm.advance();
                    _m_x = 0;
                    _m_y++;
                } else
                    _m_x++;
                this->_m_fsm.finish_unit();
            }
            const char c = _m_fsm.advance();
            _m_x++;
            _m_tokens.push_back(lex_token(c));
            this->_m_fsm.finish_unit();
        };
        return _m_tokens;
    };
};
};  // namespace monac::semantic

#endif