#ifndef NORMALIZER_HPP
#define NORMALIZER_HPP
#include <iostream>
#include <istream>
#include <mona/mona.hpp>

#include "parsers.hpp"
namespace monac::prep {

class normalizer {
    std::istream& m_stream;

   public:
    normalizer(std::istream& is) : m_stream{is} {};
    normalizer() = delete;
    normalizer(const normalizer&) = default;
    normalizer(normalizer&&) = default;
    normalizer& operator=(normalizer&&) = delete;
    normalizer& operator=(const normalizer&) = delete;
    ~normalizer() = default;

    std::string process() {
        std::string ret{""};
        while (!mona::_eof()(m_stream)) {
            if (mona::_any(mona::_match(" "), mona::_match("\\\r\n"), mona::_match("\\\n"))(m_stream)) {
                while (mona::_any(mona::_match(" "), mona::_match("\\\r\n"), mona::_match("\\\n"))(m_stream)) {
                };
                ret += ' ';
                continue;
            }
            ret += m_stream.get();
        };
        return ret;
    };
};
}  // namespace monac::prep

#endif