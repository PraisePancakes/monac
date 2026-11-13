#pragma once
#include <iostream>
#include <mona/mona.hpp>
namespace monac::prep::unit {
struct macro_definition {
    std::string identifier;
    std::vector<std::string> arguments;
    std::vector<std::string> replacement;
    bool function_like = false;
};

inline auto white_space = mona::_either(mona::_match(' '), mona::_match('\t'));
inline auto at_least_space = mona::_transform([](auto) -> std::string { return " "; }, mona::_many_or_one(white_space));
inline auto opt_white_space = mona::_transform([](auto) -> std::string {
    return "";
},
                                               mona::_many_or_none(white_space));
inline auto opt_continuation = mona::_transform([](auto) -> std::string { return ""; }, mona::_sequence(opt_white_space, mona::_either(mona::_match("\\\n\r"), mona::_match("\\\n")), opt_white_space));
inline auto identifier_start = mona::_either(mona::_letter(), mona::_match('_'));
inline auto identifier_continue = mona::_either(identifier_start, mona::_transform([](auto dig) -> char {
                                                    return static_cast<char>(dig + '0');
                                                },
                                                                                   mona::_digit()));
inline auto identifier = mona::_transform([](auto tup) -> std::string {
    std::string ret{""};
    auto [c, vec] = tup;
    ret += c;
    for (const auto& c1 : vec) ret += c1;
    return ret;
},
                                          mona::_sequence(identifier_start, mona::_many_or_none(identifier_continue)));
inline auto string_literal = mona::_transform([](auto tup) -> std::string {
    auto [_, s, _] = tup;
    return s;
},
                                              mona::_sequence(mona::_match('\"'), mona::_symbols_ex('\"', '\\', '\n'), mona::_match('\"')));

inline auto collapse_callable = [](auto vec) -> std::vector<std::string> {
    std::vector<std::string> ret;
    for (std::size_t i{0}; i < vec.size(); i++) {
        if (vec[i].empty()) {
            ret.push_back(" ");
            while (i < vec.size() && vec[i].empty()) i++;
        }
        ret.push_back(vec[i]);
    }
    return ret;
};

inline auto replacement_token = mona::_any(identifier, string_literal, mona::_symbols_ex('\n', '\r', '\\'));
inline auto replacement_list = mona::_transform(collapse_callable, mona::_many_or_none(mona::_any(opt_continuation, replacement_token, mona::_match("\\"))));
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
    mona::_sequence(identifier, mona::_many_or_none(mona::_sequence(opt_white_space, mona::_match(','), opt_white_space, identifier, opt_white_space))));
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
    mona::_sequence(identifier, opt_white_space, replacement_list));
inline auto function_like = mona::_transform([](auto tup) -> struct macro_definition {
    auto [name, _, args, _, replacement] = tup;
    return {
        .identifier = name,
        .arguments = args,
        .replacement = replacement,
        .function_like = true};
},
                                             mona::_sequence(identifier, mona::_match("("), opt_args_list, mona::_match(")"), replacement_list));
inline auto macro_def = mona::_either(function_like, object_like);

inline auto define = mona::_transform([](auto tup) -> struct macro_definition {
    auto [_, _, md] = tup;
    return md;
},
                                      mona::_sequence(mona::_match("@define"), at_least_space, macro_def));

};  // namespace monac::prep::unit