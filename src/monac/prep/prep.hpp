#ifndef PREP_HPP
#define PREP_HPP
#include <iostream>
#include <istream>
#include <mona/mona.hpp>
namespace monac::prep {
// takes an input stream of contents, retains dependencies required by this compilation unit, expands macros, and returns some information needed for the module's dependency phase.
// the lifetime of this object should never outlive the lifetime of the input stream.
// the returned string will be the basis of all read only lexical analysis operations (string_view), let RVO handle the string contents on return.
class preprocessor {
    std::istream& m_stream;
    struct prep_info {
        std::vector<std::string> imports;
        std::vector<std::string> exports;
    };

   public:
    preprocessor() = delete;
    preprocessor(std::istream& contents) : m_stream{contents} {};
    preprocessor(const preprocessor&) = default;
    preprocessor(preprocessor&&) = default;
    preprocessor& operator=(preprocessor&&) = default;
    preprocessor& operator=(const preprocessor&) = default;
    ~preprocessor() = default;

    std::string process() {
        std::string expanded{""};

        return expanded;
    };
};
};  // namespace monac::prep

#endif