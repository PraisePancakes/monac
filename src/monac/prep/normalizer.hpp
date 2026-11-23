#ifndef NORMALIZER_HPP
#define NORMALIZER_HPP
#include <iostream>
#include <istream>
#include <mona/mona.hpp>

#include "../../utils/unit_parsers.hpp"
namespace monac::prep {

class normalizer {
    std::istream& _m_stream;

   public:
    normalizer(std::istream& is) : _m_stream{is} {};
    normalizer() = delete;
    normalizer(const normalizer&) = default;
    normalizer(normalizer&&) = default;
    normalizer& operator=(normalizer&&) = delete;
    normalizer& operator=(const normalizer&) = delete;
    ~normalizer() = default;

    std::string process() {
        std::string ret{""};
        while (!mona::_eof()(_m_stream)) {
            auto skippable_parses = mona::_any(mona::_match(" "), mona::_match("\\\r\n"), mona::_match("\\\n"));
            if (skippable_parses(_m_stream)) {
                while (skippable_parses(_m_stream)) {
                };
                ret += ' ';
                continue;
            }
            ret += _m_stream.get();
        };
        return ret;
    };
};

}  // namespace monac::prep

#endif