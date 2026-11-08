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
        // first we should always check on export module at the top of the file, modules will always have export module at the top.

        return m_info;
    };
};
};  // namespace monac::prep

#endif