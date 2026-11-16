#pragma once
#include <iostream>
#include <mona/mona.hpp>
#include <unordered_map>
#include <variant>

#include "../../utils/unit_parsers.hpp"
#define _MONA_DEFER_CHECK_UNIFIED false;
namespace monac::prep::parsers_rewrite {
inline auto replacement_token = mona::_any(utils::unit::punctuator, utils::unit::identifier, utils::unit::string_literal, utils::unit::number);
inline auto replacement_list_single = mona::_sequence(utils::unit::opt_white_space, replacement_token, utils::unit::opt_white_space);
inline auto replacement_list = mona::_transform([](auto vec) -> std::vector<std::string> {
    std::vector<std::string> ret{};
    for (auto tup : vec) {
        auto [_, tok, _] = tup;
        ret.push_back(tok);
    }
    return ret;
},
                                                mona::_many_or_none(replacement_list_single));
inline auto params_list_parser = mona::_sequence(utils::unit::identifier, mona::_many_or_none(mona::_sequence(utils::unit::opt_white_space, mona::_match(","), utils::unit::opt_white_space, utils::unit::identifier)));
inline auto opt_params_list_parser = mona::_many_or_none(params_list_parser);

inline auto params_list = mona::_transform([](auto seq) -> std::vector<std::string> {
    std::vector<std::string> ret{};
    auto [first, rest] = seq;
    ret.push_back(first);
    for (const auto [_, _, _, id] : rest) ret.push_back(id);
    return ret;
},
                                           params_list_parser);
inline auto opt_params_list = mona::_transform([](auto v) -> std::vector<std::string> {
    std::vector<std::string> ret{};
    for (const auto seq : v) {
        auto [first, rest] = seq;
        ret.push_back(first);
        for (const auto [_, _, _, id] : rest) ret.push_back(id);
    };
    return ret;
},
                                               opt_params_list_parser);
struct function {
    std::string identifier;
    std::vector<std::string> parameters;
    std::vector<std::string> replacement;
    std::unordered_map<std::string, unsigned int> parameter_map;
};
struct call {
    std::string identifier;
    std::vector<std::string> arguments;
};

struct identifiable {
    std::string identifier;
};

struct object {
    std::string identifier;
    std::vector<std::string> replacement;
};

using macro_definition = std::variant<function, object>;
using macro_invocation = std::variant<call, identifiable>;

inline auto function_like = mona::_transform([](auto tup) -> macro_definition {
    auto [name, _, _, params, _, _, replacement] = tup;
    std::unordered_map<std::string, unsigned int> param_map;
    for (std::size_t i{0}; i < params.size(); i++) param_map[params[i]] = i;
    return function{.identifier = name, .parameters = params, .replacement = replacement, .parameter_map = param_map};
},
                                             mona::_sequence(utils::unit::identifier, mona::_match("("), utils::unit::opt_white_space, opt_params_list, utils::unit::opt_white_space, mona::_match(")"), replacement_list));
inline auto object_like = mona::_transform(
    [](auto tup) -> macro_definition {
        auto [_, name, _, replacement] = tup;
        return object{.identifier = name, .replacement = replacement};
    },
    mona::_sequence(utils::unit::opt_white_space, utils::unit::identifier, mona::_either(mona::_transform([](auto) -> bool { return _MONA_DEFER_CHECK_UNIFIED; }, mona::_check(mona::_symbols_ex('('))), mona::_eof()), replacement_list));

};  // namespace monac::prep::parsers_rewrite