#pragma once
#include <iostream>
#include <mona/mona.hpp>
#include <unordered_map>
#include <variant>

#include "../../utils/unit_parsers.hpp"
namespace monac::prep::parsers {

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
inline auto params_list = mona::_transform(
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
inline auto opt_params_list = mona::_transform([](auto nested_vec) -> std::vector<std::string> {
    std::vector<std::string> s;
    for (auto v : nested_vec) {
        for (auto nv : v) {
            s.push_back(nv);
        }
    }
    return s;
},
                                               mona::_many_or_none(params_list));

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
    mona::_sequence(mona::_symbols_ex(',', ')', '('), mona::_many_or_none(mona::_sequence(utils::unit::opt_white_space, mona::_match(','), utils::unit::opt_white_space, mona::_symbols_ex(',', ')', '('), utils::unit::opt_white_space))));
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
    [](auto tup) -> macro_definition {
        auto [name, _, replacement] = tup;
        return object{.identifier = name, .replacement = replacement};
    },
    mona::_sequence(utils::unit::identifier, utils::unit::opt_white_space, replacement_list));
inline auto function_like = mona::_transform([](auto tup) -> macro_definition {
    auto [name, _, _, params, _, _, replacement] = tup;
    std::unordered_map<std::string, unsigned int> param_map;
    for (std::size_t i{0}; i < params.size(); i++) param_map[params[i]] = i;
    return function{.identifier = name, .parameters = params, .replacement = replacement, .parameter_map = param_map};
},
                                             mona::_sequence(utils::unit::identifier, mona::_match("("), utils::unit::opt_white_space, opt_params_list, utils::unit::opt_white_space, mona::_match(")"), replacement_list));

inline auto call_like = mona::_transform([](auto tup) -> macro_invocation {
    auto [name, _, _, args, _, _] = tup;
    return call{.identifier = name, .arguments = args};
},
                                         mona::_sequence(utils::unit::identifier, mona::_match("("), utils::unit::opt_white_space, opt_args_list, utils::unit::opt_white_space, mona::_match(")")));
inline auto ident_like = mona::_transform([](auto p) -> macro_invocation {
    return identifiable{.identifier = p};
},
                                          utils::unit::identifier);

inline auto macro_def = mona::_either(function_like, object_like);

inline auto define = mona::_transform([](auto tup) -> macro_definition {
    auto [_, _, md] = tup;
    return md;
},
                                      mona::_sequence(mona::_match("@define"), utils::unit::at_least_space, macro_def));
inline auto value = mona::_transform([](auto v) -> macro_invocation {
    return v;
},
                                     mona::_either(call_like, ident_like));

};  // namespace monac::prep::parsers