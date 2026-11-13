#pragma once
#include <mona/mona.hpp>
namespace cli::parsers {
namespace _internals {
inline auto single_flag_parser = (mona::_match('-') >> mona::_letter());
inline auto double_flag_parser = (mona::_match("--") >> mona::_alnums());
inline auto single_ident_parser = mona::_transform([](auto c) -> std::string {
    std::string s{""};
    s += c;
    return s;
},
                                                   mona::_letter());
}  // namespace _internals

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
}  // namespace cli::parsers