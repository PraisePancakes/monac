#pragma once
#include <mona/mona.hpp>
namespace cli::parsers {
inline auto file_parser = mona::_sequence(mona::_alnum("expected filename"), mona::_match('.', "expected file separator"), mona::_alnum("expected file extension"));
inline auto skip_whitespace = mona::_many_or_none(mona::_match(' '));

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
};  // namespace cli::parsers