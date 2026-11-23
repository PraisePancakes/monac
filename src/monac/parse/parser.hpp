#ifndef PARSER_HPP
#define PARSER_HPP
#include "../arena.hpp"
#include "../fsm.hpp"
#include "../tokens.hpp"
namespace monac::semantic {

class parser final : public worm_machine<std::span<token>> {
    monac::memory::arena<> _m_arena;

   public:
    parser(const std::span<token>& tokens) : worm_machine(tokens) {};
};
};  // namespace monac::semantic

#endif