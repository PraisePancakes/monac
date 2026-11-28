#ifndef PARSER_HPP
#define PARSER_HPP
#include "../arena.hpp"
#include "../fsm.hpp"
#include "../tokens.hpp"
#include "node.hpp"
namespace monac::semantic {
using tk_t = token::TOKEN_TYPE;
class parser final {
    monac::memory::arena_list _m_arenas;
    std::vector<token> _m_tokens;
    worm_machine<std::vector<token>> _m_fsm;

    expr_t literal() {

    };
    expr_t factor() {
        auto lhs = term();
    }
    expr_t term() {
        auto lhs = factor();
        auto op = _m_fsm.match_any("expected binary operator", tk_t::TK_PLUS, tk_t::TK_MINUS);
        if (!op) throw std::runtime_error(std::format("{} at line {}:{}", op.error(), _m_fsm.peek().location.y, _m_fsm.peek().location.x));
        return _m_arenas.make<binary>(lhs, op.value(), factor());
    };

    expr_t expr() {
        auto lhs = term();
    };

   public:
    parser(const std::vector<token>& tokens) : _m_arenas{}, _m_tokens{tokens}, _m_fsm{_m_tokens} {};
    parser(std::vector<token>&& tokens) : _m_arenas{}, _m_tokens{std::move(tokens)}, _m_fsm{_m_tokens} {};
    parser(const parser&) = default;
    parser(parser&&) = default;
    parser& operator=(parser&&) = default;
    parser& operator=(const parser&) = default;
    parser() = delete;
    ~parser() = default;
};
};  // namespace monac::semantic

#endif