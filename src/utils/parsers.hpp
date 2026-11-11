#ifndef PARSERS_HPP
#define PARSERS_HPP
#include <filesystem>
#include <mona/mona.hpp>

namespace utils::parsers {
namespace _internals {
inline auto wspace_parser = mona::_either(mona::_match(' '), mona::_match('\t'));
inline auto define_newline_parser = mona::_either(mona::_match("\\\n\r"), mona::_match("\\\n"));

inline auto single_flag_parser = (mona::_match('-') >> mona::_letter());
inline auto double_flag_parser = (mona::_match("--") >> mona::_alnums());
inline auto single_ident_parser = mona::_transform([](auto c) -> std::string {
    std::string s{""};
    s += c;
    return s;
},
                                                   mona::_letter());
inline auto multi_ident_parser = mona::_transform([](auto seq) -> std::string {
    std::string s{""};
    auto [letter, syms] = seq;
    s += letter;
    s.append(syms);
    return s;
},
                                                  mona::_sequence(mona::_letter(), mona::_symbols_ex(' ', '\n', '\r', '+', '!', '@', '#', '%', '^', '&', '(', ')', '-', '=', '+', '*', ';', '\\')));
inline auto identifier_parser = mona::_either(multi_ident_parser, single_ident_parser);

}  // namespace _internals
inline auto skip_wspace = mona::_transform([](auto) -> std::string { return ""; }, _many_or_none(_internals::wspace_parser));
inline auto define_continuation = mona::_transform([](auto) -> std::string {
    return " ";
},
                                                   mona::_sequence(mona::_many_or_none(_internals::wspace_parser), _internals::define_newline_parser, mona::_many_or_none(_internals::wspace_parser)));

inline auto define_value_parser = mona::_many_or_one(mona::_any(define_continuation, mona::_symbols_ex('\r', '\n', '\\'), mona::_match("\\")));

inline auto single_flag = mona::_transform([](auto pair) -> char {
    return pair.second;
},
                                           _internals::single_flag_parser);
inline auto double_flag = mona::_transform([](auto pair) -> std::string {
    std::string s{""};
    s += pair.second;
    return s;
},
                                           _internals::double_flag_parser);

inline auto directory = mona::_symbols_ex(' ');

inline auto identifier = _internals::identifier_parser;
inline auto define_value = mona::_transform([](auto vec) -> std::string {
    std::string ret;
    for (const auto& v : vec)
        ret.append(v);
    return ret;
},
                                            define_value_parser);

inline auto define_parser = mona::_sequence(mona::_match("@define"), skip_wspace, identifier, skip_wspace, define_value);
inline auto define = mona::_transform([](auto tup) -> std::pair<std::string, std::string> {
    auto [_, _, ident, _, text] = tup;
    return std::make_pair(ident, text);
},
                                      define_parser);

inline auto export_module = mona::_transform([](auto tup) -> std::string {
    auto [_, _, _, _, ident] = tup;
    return ident;
},
                                             mona::_sequence(mona::_match("export"), skip_wspace, mona::_match("module"), skip_wspace, identifier));

inline auto import_module = mona::_transform([](auto tup) -> std::string {
    auto [_, _, _, _, ident] = tup;
    return ident;
},
                                             mona::_sequence(mona::_match("import"), skip_wspace, mona::_match("module"), skip_wspace, identifier));

};  // namespace utils::parsers
#endif