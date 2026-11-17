#ifndef PREP_PARSERS_HPP
#define PREP_PARSERS_HPP
#include <iostream>
#include <variant>
#include <vector>

#include "../../utils/unit_parsers.hpp"
namespace monac::prep::parsers {

namespace _internal {
inline auto identifier_separator = mona::_sequence(utils::unit::opt_white_space, mona::_match(','), utils::unit::opt_white_space, utils::unit::identifier);

};
inline auto identifier_list = mona::_transform(
    [](const auto tup) -> std::vector<std::string> {
        const auto& [first, rest] = tup;
        std::vector<std::string> ret{first};
        for (const auto& [_, _, _, id] : rest) ret.push_back(id);
        return ret;
    },
    mona::_sequence(utils::unit::identifier, mona::_many_or_none(_internal::identifier_separator)));

inline auto replacement_list = mona::_transform([](const auto vec) -> std::vector<std::string> {
    std::vector<std::string> ret{};
    for (const auto [tok, _] : vec) ret.push_back(tok);
    return ret;
},
                                                mona::_many_or_none(mona::_sequence(utils::unit::preprocessing_token, utils::unit::opt_white_space)));
};  // namespace monac::prep::parsers
#endif