#ifndef UNIT_PARSERS_HPP
#define UNIT_PARSERS_HPP
#include <filesystem>
#include <mona/mona.hpp>

namespace utils::unit {
inline auto white_space = mona::_either(mona::_match(' '), mona::_match('\t'));
inline auto at_least_space = mona::_transform([](auto) -> std::string { return " "; }, mona::_many_or_one(white_space));
inline auto opt_white_space = mona::_transform([](auto v) -> std::string {
    if (v.empty()) return "";
    return " ";
},
                                               mona::_many_or_none(white_space));
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
inline auto string_character = mona::_transform([](auto c) -> std::string {
    std::string s{""};
    s += c;
    return s;
},
                                                mona::_symbol_ex('\"', '\\', '\n'));
inline auto escape_character = mona::_any(mona::_match("n"), mona::_match("t"), mona::_match("r"), mona::_match("0"), mona::_match("\\"), mona::_match("\""), mona::_match("'"));
inline auto escape_sequence = mona::_transform([](auto tup) -> std::string {
    auto [f, r] = tup;
    return f + r;
},
                                               mona::_sequence(mona::_match("\\"), escape_character));
inline auto triple_unit_punctuator = mona::_any(mona::_match("..."), mona::_match("<<="), mona::_match(">>="));

inline auto double_unit_punctuator = mona::_any(mona::_match("++"), mona::_match("--"), mona::_match("->"),
                                                mona::_match("&&"), mona::_match("||"), mona::_match("=="),
                                                mona::_match("!="), mona::_match("<="), mona::_match(">="),
                                                mona::_match("+="), mona::_match("-="), mona::_match("*="),
                                                mona::_match("/="), mona::_match("%="), mona::_match("&="),
                                                mona::_match("|="), mona::_match("^="), mona::_match(">>"),
                                                mona::_match("<<"), mona::_match("##"));

inline auto single_unit_punctuator = mona::_any(mona::_match("#"), mona::_match("("), mona::_match(")"),
                                                mona::_match("["), mona::_match("]"), mona::_match("{"),
                                                mona::_match("}"), mona::_match("."), mona::_match(","),
                                                mona::_match(";"), mona::_match(":"), mona::_match("?"),
                                                mona::_match("+"), mona::_match("-"), mona::_match("*"),
                                                mona::_match("%"), mona::_match("&"), mona::_match("|"),
                                                mona::_match("^"), mona::_match("~"), mona::_match("!"),
                                                mona::_match("="), mona::_match("<"), mona::_match(">"));

inline auto punctuator = mona::_any(triple_unit_punctuator, double_unit_punctuator, single_unit_punctuator);

inline auto string_content = mona::_many_or_none(mona::_any(string_character, escape_sequence));
inline auto string_literal = mona::_transform([](auto tup) -> std::string {
    std::string res{""};
    auto [_, s, _] = tup;
    for (const auto& str : s) res.append(str);
    return res;
},
                                              mona::_sequence(mona::_match('\"'), string_content, mona::_match('\"')));
};  // namespace utils::unit
#endif