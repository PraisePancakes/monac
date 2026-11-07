#ifndef PARSERS_HPP
#define PARSERS_HPP
#include <filesystem>
#include <mona/mona.hpp>
namespace utils::parsers {
namespace _internals {
inline auto skip_wspace_parser = mona::_many_or_none(mona::_either(mona::_match(' '), mona::_match('\t')));
inline auto single_flag_parser = (mona::_match('-') >> mona::_letter());
inline auto double_flag_parser = (mona::_match("--") >> mona::_alnums());

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

};  // namespace utils::parsers
#endif