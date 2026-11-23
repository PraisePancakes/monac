#ifndef FORMATS_HPP
#define FORMATS_HPP
#include <format>
#include <iostream>

#include "../lex/lexer.hpp"
#include "../prep/prep.hpp"

template <>
struct std::formatter<decltype(std::declval<monac::prep::preprocessor>().process())> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }
    auto format(const decltype(std::declval<monac::prep::preprocessor>().process())& prep_info, std::format_context& ctx) const {
        std::string ret{""};
        std::stringstream ss{prep_info.expanded};
        while (!mona::_eof()(ss)) {
            auto skippable_parses = mona::_any(mona::_match(" "), mona::_match("\\\r\n"), mona::_match("\\\n"));
            if (skippable_parses(ss)) {
                while (skippable_parses(ss)) {
                };
                ret += ' ';
                continue;
            }
            if (utils::unit::new_line(ss)) {
                while (utils::unit::new_line(ss)) {
                };
                ret += '\n';
                continue;
            }
            ret += ss.get();
        };
        return std::format_to(ctx.out(), "{}", ret);
    }
};

template <>
struct std::formatter<decltype(std::declval<monac::semantic::lexer>().lex())> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }
    auto format(const decltype(std::declval<monac::semantic::lexer>().lex())& toks, std::format_context& ctx) const {
        std::stringstream ss;
        for (const auto& t : toks) ss << t << std::endl;
        return std::format_to(ctx.out(), "{}", ss.str());
    }
};

#endif