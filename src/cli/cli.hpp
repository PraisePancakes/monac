#pragma once
#include <istream>
namespace cli {
class args {
    std::stringstream arguments;

   public:
    args(std::stringstream&& ss) : arguments{std::move(ss)} {};
    void parse() {};
};
};  // namespace cli