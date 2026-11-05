#pragma once
#include <istream>
#include <mona/mona.hpp>

#include "../globals/logger.hpp"
#include "parsers.hpp"
namespace cli {
class cli_except : public std::exception {
    std::string m_what;

   public:
    cli_except() : m_what{""} {};
    cli_except(const std::string& what) : m_what(what) {};
    std::string get_reason() const {
        return "[MONAC-CLI-EXCEPT]: " + m_what;
    };
    ~cli_except() = default;
};

class args {
    using flag_t = std::uint8_t;
    std::stringstream m_arguments;
    std::string m_usage;
    enum class CLI_FLAGS : flag_t {
        FLAG_HELP = 1 << 0,
        FLAG_VERSION = 1 << 1
    };
    flag_t m_flags{0};

    void parse_commands() {
        while (mona::_either(mona::_check(mona::_match("-")), mona::_check(mona::_match("--")))(m_arguments)) {
            auto dflag = parsers::double_flag_parser(m_arguments);
            auto sflag = parsers::single_flag_parser(m_arguments);
            if (dflag.has_value()) {
                auto flag = dflag.value();
            } else if (sflag.has_value()) {
                auto flag = sflag.value();
            }
            parsers::skip_whitespace(m_arguments);
        };
    };

   public:
    args(std::stringstream&& ss)
        : m_arguments{std::move(ss)} {
        std::ifstream ifs("../config/usage.txt");  // hardcoded for now, TO-DO: either use filesystem or have path in same dir as build.
        if (ifs.is_open()) {
            std::stringstream buf;
            buf << ifs.rdbuf();
            m_usage = buf.str();
            ifs.close();
        } else {
            glob::logger::instance().log(glob::log_info{.type = glob::log_info::LOG_TYPE::FATAL, .what = "Missing usage data"}, std::cerr);
            throw cli_except("Cli couldn't read usage data");
        }
    };
    void parse() {
        auto entry_parser = mona::_match("monac", "missing entry")(m_arguments);
        if (!entry_parser.has_value()) {
            glob::logger::instance().log(glob::log_info{.type = glob::log_info::LOG_TYPE::FATAL, .what = "Missing monac entry"}, std::cerr);
            throw cli_except("Cli couldn't find monac entry");
        };
        parsers::skip_whitespace(m_arguments);
        parse_commands();
    };
};
};  // namespace cli