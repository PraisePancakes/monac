#ifndef NODE_HPP
#define NODE_HPP
#include <iostream>
#include <variant>
namespace monac::semantic {
struct literal;
struct variable;
struct binary;

using value_t = std::variant<std::string, float, int, double, bool, char>;
using expr_t = std::variant<literal, variable, binary>;

struct literal {
    value_t value;
};

struct variable {
    std::string ident;
};

struct binary {
    
};

};  // namespace monac::semantic

#endif