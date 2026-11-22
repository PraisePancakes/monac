#ifndef UNIT_PARSERS_HPP
#define UNIT_PARSERS_HPP
#include <filesystem>
#include <mona/mona.hpp>

namespace utils::unit {
inline auto white_space = mona::_either(mona::_match(' '), mona::_match('\t'));
inline auto at_least_space = mona::_transform([](auto) -> std::string { return " "; }, mona::_many_or_one(white_space));
inline auto opt_white_space = mona::_transform([](const auto v) -> std::string {
    if (v.empty()) return "";
    return " ";
},
                                               mona::_many_or_none(white_space));
inline auto new_line = mona::_any(mona::_match("\r\n"), mona::_match("\n"), mona::_match("\r"));

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
                                                mona::_match("}"), mona::_match("."), mona::_match(";"),
                                                mona::_match(":"), mona::_match("?"), mona::_match("+"),
                                                mona::_match("-"), mona::_match("*"), mona::_match("%"),
                                                mona::_match("&"), mona::_match("|"), mona::_match("^"),
                                                mona::_match("~"), mona::_match("!"), mona::_match("="),
                                                mona::_match("<"), mona::_match(">"), mona::_match(","));

inline auto preprocessing_op_or_punc = mona::_any(triple_unit_punctuator, double_unit_punctuator, single_unit_punctuator);
inline auto escape_sequence = mona::_transform(
    [](const auto tup) -> std::string {
        const auto [slash, escaped] = tup;
        return std::string{slash, escaped};
    },
    mona::_sequence(
        mona::_match('\\'),
        mona::_any(
            mona::_match('\''),
            mona::_match('"'),
            mona::_match('\\'),
            mona::_match('n'),
            mona::_match('t'),
            mona::_match('r'))));
// Character literal content: cannot contain an unescaped single quote.
inline auto character = mona::_either(
    mona::_symbols_ex('\\', '\r', '\n', '\''),
    escape_sequence);

inline auto char_sequence = mona::_transform(
    [](const auto vec) -> std::string {
        std::string seq{""};
        for (const auto& c : vec) seq.append(c);
        return seq;
    },
    mona::_many_or_one(character));

inline auto character_literal = mona::_transform([](const auto tup) -> std::string {
    std::string ret{""};
    const auto [p, seq, po] = tup;
    ret += p;
    ret.append(seq);
    ret += po;
    return ret;
},
                                                 mona::_sequence(mona::_match("\'"), char_sequence, mona::_match("\'")));

inline auto string_character = mona::_either(
    mona::_symbols_ex('\\', '\r', '\n', '"'),
    escape_sequence);

inline auto string_char_sequence = mona::_transform(
    [](const auto vec) -> std::string {
        std::string seq{""};
        for (const auto& c : vec) seq.append(c);
        return seq;
    },
    mona::_many_or_one(string_character));
inline auto string_literal = mona::_transform([](const auto tup) -> std::string {
    std::string ret{""};
    const auto [p, seq, po] = tup;
    ret += p;
    for (const auto s : seq) ret.append(s);
    ret += po;
    return ret;
},
                                              mona::_sequence(mona::_match('"'), mona::_many_or_none(string_char_sequence), mona::_match('"')));

inline auto integer = mona::_transform([](const auto vec) -> std::string {
    std::string ret{""};
    for (const auto d : vec) ret += d + '0';
    return ret;
},
                                       mona::_many_or_one(mona::_digit()));
inline auto negative_integer = mona::_transform([](const auto p) -> std::string {
    return p.first + p.second;
},
                                                mona::_both(mona::_match("-"), integer));

namespace _internal {
inline auto full_floating_point = mona::_transform([](auto tup) -> std::string {
    auto [p, n, po] = tup;
    return p + n + po;
},
                                                   mona::_sequence(integer, mona::_match("."), integer));
inline auto post_floating_point = mona::_transform([](auto p) -> std::string {
    return p.first + p.second;
},
                                                   mona::_both(mona::_match("."), integer));
inline auto pre_floating_point = mona::_transform([](auto p) -> std::string {
    return p.first + p.second;
},
                                                  mona::_both(integer, mona::_match(".")));
};  // namespace _internal
inline auto floating_point = mona::_any(_internal::full_floating_point, _internal::post_floating_point, _internal::pre_floating_point);
inline auto negative_floating_point = mona::_transform([](const auto p) -> std::string {
    return p.first + p.second;
},
                                                       mona::_both(mona::_match("-"), floating_point));
inline auto digit = mona::_transform([](const auto d) -> char {
    return d + '0';
},
                                     mona::_digit());
inline auto pp_number = mona::_any(floating_point, negative_floating_point, integer, negative_integer);
inline auto identifier_nondigit = mona::_any(mona::_letter(), mona::_match('_'));
inline auto identifier = mona::_transform([](const auto tup) -> std::string {
    std::string ret{""};
    const auto [f, rest] = tup;
    ret += f;
    for (const auto v : rest) ret += v;
    return ret;
},
                                          mona::_sequence(identifier_nondigit, mona::_many_or_none(mona::_either(identifier_nondigit, digit))));
inline auto preprocessing_token = mona::_any(identifier, pp_number, character_literal, string_literal, preprocessing_op_or_punc, mona::_symbols_ex(' ', '\n', '\r', '\t'));

inline auto args_value = mona::_any(identifier, pp_number, character_literal, string_literal);

};  // namespace utils::unit
#endif