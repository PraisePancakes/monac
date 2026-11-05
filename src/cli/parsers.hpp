#pragma once
#include <mona/mona.hpp>
namespace cli::parsers {

inline auto skip_whitespace = mona::_transform([](auto v) -> std::string { return " "; }, mona::_many_or_none(mona::_match(' ')));

inline auto file_parser = mona::_transform([](auto tup) -> std::string { auto [_, n, d, e, _] = tup; return n + d + e; },
                                           mona::_sequence(skip_whitespace, mona::_alnum("expected filename"), mona::_match('.', "expected file separator"), mona::_alnum("expected file extension"), skip_whitespace));
inline auto double_flag_parser = mona::_transform([](auto tup) -> std::string {
    std::string flag{""};
    auto [_, vec] = tup;
    for (const auto& c : vec) {
        flag.append(c);
    }
    return flag;
},
                                                  mona::_sequence(mona::_match("--"), mona::_many_or_one(mona::_alnum())));

inline auto single_flag_parser = mona::_transform([](auto tup) -> char {
    auto [_, c] = tup;
    return c;
},
                                                  mona::_sequence(mona::_match("-"), mona::_letter()));

inline auto string_parser = mona::_transform([](auto vec) -> std::string {
    std::string res{""};
    for (const auto& c : vec) res.append(c);
    return res;
},
                                             mona::_many_or_one(mona::_alnum()));
};  // namespace cli::parsers