#ifndef PARSER_HPP
#define PARSER_HPP
#include "../arena.hpp"
#include "../fsm.hpp"
#include "../tokens.hpp"
namespace monac::semantic {

class parser final {
    monac::memory::arena_list _m_arenas;
    std::vector<token> _m_tokens;
    worm_machine<std::vector<token>> _m_fsm;

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