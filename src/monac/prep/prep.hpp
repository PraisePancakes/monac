#ifndef PREP_HPP
#define PREP_HPP
#include <iostream>
#include <istream>
#include <mona/mona.hpp>

#include "../../utils/unit_parsers.hpp"
#include "normalizer.hpp"
#include "parsers.hpp"
namespace monac::prep {
using namespace utils;
// takes an input stream of normalized contents, retains dependencies required by this compilation unit, expands macros, and returns some information needed for the module's dependency phase.
// the lifetime of this object should never outlive the lifetime of the input stream.
// the returned info will be the basis of all read only lexical analysis operations (string_view) and parsing, let RVO handle the return contents on return.
// **NOTE**: must take in a normalized string of contents before expansion and parsing.
class preprocessor {
    std::string normalized;

    struct prep_info {
        std::string expanded;
    } m_info;

    std::unordered_map<std::string, prep::parsers::macro_definition> define_map;

   public:
    preprocessor() = delete;
    preprocessor(const std::string& normalized) : normalized{normalized} {};
    preprocessor(const preprocessor&) = default;
    preprocessor(preprocessor&&) = default;
    preprocessor& operator=(preprocessor&&) = delete;
    preprocessor& operator=(const preprocessor&) = delete;
    ~preprocessor() = default;

    prep_info process() {
        std::stringstream stream{normalized};
        // don't worry about import/exports. This class will only handle macro replacements (for now) and eventually expansions.
        while (!mona::_eof()(stream)) {
            auto define = parsers::define(stream);
            auto ident = utils::unit::identifier(stream);
            if (define.has_value()) {
                auto v = define.value();
                define_map[v.identifier] = v;
            } else if (ident.has_value()) {
                if (define_map.contains(ident.value()))
                    if (define_map[ident.value()].function_like) {
                        // handle function unwrap
                    } else {
                        for (const auto& s : define_map[ident.value()].replacement) m_info.expanded.append(s);
                    }
                else
                    m_info.expanded += ident.value();
            } else {
                m_info.expanded += static_cast<char>(stream.get());
            }
        };

        return m_info;
    };
};
};  // namespace monac::prep

#endif