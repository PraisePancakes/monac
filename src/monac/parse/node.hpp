#ifndef NODE_HPP
#define NODE_HPP
#include <iostream>
#include <variant>
#include <vector>

#include "../tokens.hpp"
namespace monac::semantic {

#define EXPR_NODE_LIST \
    X(literal)         \
    X(variable)        \
    X(binary)          \
    X(unary)           \
    X(call)

#define X(t) struct t;
EXPR_NODE_LIST
#undef X

#define STMT_NODE_LIST \
    S(function)        \
    S(expr)            \
    S(if_s)            \
    S(else_s)

#define S(t) struct t;
STMT_NODE_LIST
#undef S

using stmt_t = std::variant<std::monostate
#define S(t) , t*
                                STMT_NODE_LIST
#undef S
                            >;
using value_t = std::variant<std::string, float, int, double, bool, char, std::nullopt_t>;
using expr_t = std::variant<std::monostate
#define X(t) , t*
                                EXPR_NODE_LIST
#undef X
                            >;

struct literal {
    value_t value;
};

struct variable {
    std::string ident;
};
struct unary {
    token op;
    expr_t rhs;
};
struct binary {
    expr_t lhs;
    token op;
    expr_t rhs;
};

struct expr {
    expr_t expression;
};

struct call {
    std::string ident;
    std::vector<expr_t> args;
};

};  // namespace monac::semantic

#endif