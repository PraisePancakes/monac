#ifndef PREP_PARSERS_HPP
#define PREP_PARSERS_HPP
#include <iostream>
#include <variant>
#include <vector>

#include "../../utils/unit_parsers.hpp"
namespace monac::prep::parsers {

namespace _internal {
inline auto identifier_separator = mona::_sequence(utils::unit::opt_white_space, mona::_match(','), utils::unit::opt_white_space, utils::unit::identifier);

};  // namespace _internal
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
inline auto text_line = mona::_transform([](const auto seq) -> std::vector<std::string> {
    std::vector<std::string> ret{};
    auto [vec, _] = seq;
    for (const auto tok : vec) ret.push_back(tok);
    return ret;
},
                                         mona::_sequence(mona::_many_or_none(utils::unit::preprocessing_token), utils::unit::new_line));

struct variable {
    std::string id;
    std::vector<std::string> replacement_list;
};
struct function {
    std::string id;
    std::unordered_map<std::string, unsigned int> parameters_map;
    std::vector<std::string> replacement_list;
};

using macro_node_t = std::variant<variable, function>;

namespace _internal_ctrl {
inline auto define_identifier = mona::_transform([](const auto seq) -> macro_node_t {
    const auto [_, _, id, _, repl, _] = seq;
    return variable{.id = id, .replacement_list = repl};
},
                                                 mona::_sequence(mona::_match("@define"), utils::unit::at_least_space, utils::unit::identifier, utils::unit::at_least_space, replacement_list, utils::unit::new_line));
inline auto define_function = mona::_transform([](const auto seq) -> macro_node_t {
    const auto [_, _, id, _, _, params, _, _, _, repl, _] = seq;
    return function{.id = id, .parameters_map = params, .replacement_list = repl};
},
                                               mona::_sequence(mona::_match("@define"), utils::unit::at_least_space, utils::unit::identifier, mona::_match("("), utils::unit::opt_white_space, mona::_transform([](const auto vec) -> std::unordered_map<std::string, unsigned int> {
                                                                   std::unordered_map<std::string, unsigned int> parameters_map;
                                                                   for (const auto v : vec)
                                                                       for (std::size_t i{0}; i < v.size(); i++) {
                                                                           parameters_map[v[i]] = i;
                                                                       }
                                                                   return parameters_map;
                                                               },
                                                                                                                                                                                                                mona::_many_or_none(identifier_list)),
                                                               utils::unit::opt_white_space, mona::_match(")"), utils::unit::opt_white_space, replacement_list, utils::unit::new_line));
}  // namespace _internal_ctrl

inline auto control_line = mona::_any(_internal_ctrl::define_function, _internal_ctrl::define_identifier);

};  // namespace monac::prep::parsers
#endif