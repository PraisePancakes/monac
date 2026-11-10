#ifndef PREP_HPP
#define PREP_HPP
#include <iostream>
#include <istream>
#include <mona/mona.hpp>

#include "../../utils/parsers.hpp"
namespace monac::prep {
using namespace utils;
// takes an input stream of contents, retains dependencies required by this compilation unit, expands macros, and returns some information needed for the module's dependency phase.
// the lifetime of this object should never outlive the lifetime of the input stream.
// the returned info will be the basis of all read only lexical analysis operations (string_view) and parsing, let RVO handle the return contents on return.
class preprocessor {
    std::istream& m_stream;

    struct prep_info {
        std::string expanded;
    } m_info;

    std::unordered_map<std::string, std::string> define_map;

   public:
    preprocessor() = delete;
    preprocessor(std::istream& contents) : m_stream{contents} {};
    preprocessor(const preprocessor&) = default;
    preprocessor(preprocessor&&) = default;
    preprocessor& operator=(preprocessor&&) = delete;
    preprocessor& operator=(const preprocessor&) = delete;
    ~preprocessor() = default;

    prep_info process() {
        // don't worry about import/exports. This class will only handle macro replacements (for now) and eventually expansions.
        while (!mona::_eof()(m_stream)) {
            auto define = parsers::define(m_stream);
            auto ident = parsers::identifier(m_stream);
            if (define.has_value()) {
                auto v = define.value();
                define_map[v.first] = v.second;
            } else if (ident.has_value()) {
                if (define_map.contains(ident.value()))
                    m_info.expanded += define_map[ident.value()];
                else
                    m_info.expanded += ident.value();
            } else {
                m_info.expanded += static_cast<char>(m_stream.get());
            }
        };

        return m_info;
    };
};
};  // namespace monac::prep

#endif