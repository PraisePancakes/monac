#pragma once
#include <iostream>
#include <mona/mona.hpp>

#include "../../utils/unit_parsers.hpp"
namespace monac::prep::parsers {
struct macro_definition {
    std::string identifier;
    std::vector<std::string> arguments;
    std::vector<std::string> replacement;
    bool function_like = false;
};

inline auto replacement_token = mona::_any(utils::unit::identifier, utils::unit::string_literal, mona::_symbols_ex('\n', '\r', ' '));
inline auto replacement_list = mona::_transform([](auto vec) -> std::vector<std::string> {
    std::vector<std::string> ret;
    for (const auto [pre, tok, post] : vec) {
        if (!pre.empty()) ret.push_back(pre);
        ret.push_back(tok);
        if (!post.empty()) ret.push_back(post);
    }
    return ret;
},
                                                mona::_many_or_none(mona::_sequence(utils::unit::opt_white_space, replacement_token, utils::unit::opt_white_space)));
inline auto args_list = mona::_transform(
    [](auto tup) -> std::vector<std::string> {
        auto [first, rest] = tup;
        std::vector<std::string> args;
        args.push_back(first);
        for (auto& arg : rest) {
            auto [_, _, _, v, _] = arg;
            args.push_back(v);
        }
        return args;
    },
    mona::_sequence(utils::unit::identifier, mona::_many_or_none(mona::_sequence(utils::unit::opt_white_space, mona::_match(','), utils::unit::opt_white_space, utils::unit::identifier, utils::unit::opt_white_space))));
inline auto opt_args_list = mona::_transform([](auto nested_vec) -> std::vector<std::string> {
    std::vector<std::string> s;
    for (auto v : nested_vec) {
        for (auto nv : v) {
            s.push_back(nv);
        }
    }
    return s;
},
                                             mona::_many_or_none(args_list));

inline auto object_like = mona::_transform(
    [](auto tup) -> struct macro_definition {
        auto [name, _, replacement] = tup;
        return {
            .identifier = name,
            .arguments = {},
            .replacement = replacement,
            .function_like = false};
    },
    mona::_sequence(utils::unit::identifier, utils::unit::opt_white_space, replacement_list));
inline auto function_like = mona::_transform([](auto tup) -> struct macro_definition {
    auto [name, _, args, _, replacement] = tup;
    return {
        .identifier = name,
        .arguments = args,
        .replacement = replacement,
        .function_like = true};
},
                                             mona::_sequence(utils::unit::identifier, mona::_match("("), opt_args_list, mona::_match(")"), replacement_list));
inline auto macro_def = mona::_either(function_like, object_like);

inline auto define = mona::_transform([](auto tup) -> struct macro_definition {
    auto [_, _, md] = tup;
    return md;
},
                                      mona::_sequence(mona::_match("@define"), utils::unit::at_least_space, macro_def));

};  // namespace monac::prep::parsers