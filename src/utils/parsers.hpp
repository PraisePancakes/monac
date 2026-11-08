#ifndef PARSERS_HPP
#define PARSERS_HPP
#include <filesystem>
#include <mona/mona.hpp>

namespace utils::parsers {
namespace _internals {
inline auto skip_wspace_parser = mona::_many_or_none(mona::_either(mona::_match(' '), mona::_match('\t')));
inline auto single_flag_parser = (mona::_match('-') >> mona::_letter());
inline auto double_flag_parser = (mona::_match("--") >> mona::_alnums());
inline auto identifier_parser = mona::_sequence(mona::_letter(), mona::_symbols_ex(' ', '\n', '\r', '+', '!', '@', '#', '%', '^', '&', '(', ')', '-', '=', '+', '*'));

}  // namespace _internals
inline auto skip_wspace = mona::_transform([](auto) -> std::string { return ""; }, _internals::skip_wspace_parser);
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

inline auto file = mona::_symbols_ex(' ');

inline auto identifier = mona::_transform([](auto tup) -> std::string {
    std::string s{""};
    auto [l, rest] = tup;
    s += l;
    s.append(rest);
    return s;
},
                                          _internals::identifier_parser);

inline auto define_parser = mona::_sequence(mona::_match("#define"), skip_wspace, identifier, skip_wspace, mona::_symbols_ex('\r', '\n'));
inline auto define = mona::_transform([](auto tup) -> std::pair<std::string, std::string> {
    auto [_, _, ident, _, text] = tup;
    return std::make_pair(ident, text);
},
                                      define_parser);

};  // namespace utils::parsers
#endif