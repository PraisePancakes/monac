#ifndef PREP_HPP
#define PREP_HPP
#include <iostream>
#include <istream>
#include <mona/mona.hpp>

#include "../../utils/overload.hpp"
#include "normalizer.hpp"
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

        return m_info;
    };
};
};  // namespace monac::prep

#endif