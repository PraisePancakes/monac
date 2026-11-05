#pragma once
#include <istream>
#include <mona/mona.hpp>

#include "../cfg/version.hpp"
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

#define FLAG_ENTRY_LIST                     \
    X(FLAG_HELP, "help", 'h', 1 << 0)       \
    X(FLAG_VERSION, "version", 'v', 1 << 1) \
    X(FLAG_OUTPUT, "output", 'o', 1 << 2)   \
    X(FLAG_INPUT, "input", 'i', 1 << 3)

class args {
    using flag_t = std::uint8_t;
    std::stringstream m_arguments;
    std::string m_usage;
    enum class CLI_FLAGS : flag_t {
#define X(e, _, __, v) e = v,
        FLAG_ENTRY_LIST
#undef X
    };
    flag_t m_flags{0};

    struct cli_info {
        std::vector<std::string> input_files;
        std::string output;
    } m_info;
    void set_flag(CLI_FLAGS flag) {
        m_flags |= static_cast<flag_t>(flag);
    };
    bool is_set(CLI_FLAGS flag) {
        return (m_flags & static_cast<flag_t>(flag));
    }
    void parse_FLAG_INPUT() {
        set_flag(CLI_FLAGS::FLAG_INPUT);
        auto files = mona::_many_or_one(parsers::file_parser)(m_arguments);
        if (!files.has_value()) {
            glob::logger::instance().log(glob::log_info{.type = glob::log_info::LOG_TYPE::FATAL, .what = files.error()}, std::cerr);
            throw cli_except("Cli couldn't parse input files");
        }
        m_info.input_files = files.value();
    };
    void parse_FLAG_OUTPUT() {
        set_flag(CLI_FLAGS::FLAG_OUTPUT);
        parsers::skip_whitespace(m_arguments);
        auto op = parsers::string_parser(m_arguments);
        if (!op.has_value()) {
            glob::logger::instance().log(glob::log_info{.type = glob::log_info::LOG_TYPE::FATAL, .what = "Missing output files"}, std::cerr);
            throw cli_except("Cli couldn't parse output files");
        }
        m_info.output = op.value();
    };
    void parse_FLAG_HELP() {
        set_flag(CLI_FLAGS::FLAG_HELP);
    };
    void parse_FLAG_VERSION() {
        set_flag(CLI_FLAGS::FLAG_VERSION);
    }

    void parse_commands() {
        // don't touch this, x-macro automates this portion, all you need is to define the parse functions in the form of parse_FLAG_<FLAG>
        if (mona::_either(mona::_check(mona::_match("-")), mona::_check(mona::_match("--")))(m_arguments)) {
            while (mona::_either(mona::_check(mona::_match("-")), mona::_check(mona::_match("--")))(m_arguments)) {
                auto dflag = parsers::double_flag_parser(m_arguments);
                auto sflag = parsers::single_flag_parser(m_arguments);
                if (dflag.has_value()) {
                    auto flag = dflag.value();
#define X(e, ln, _, __) \
    if (flag == ln) parse_##e();
                    FLAG_ENTRY_LIST
#undef X
                } else if (sflag.has_value()) {
                    auto flag = sflag.value();
#define X(e, _, sn, __) \
    if (flag == sn) parse_##e();
                    FLAG_ENTRY_LIST
#undef X
                }
                parsers::skip_whitespace(m_arguments);
            };
        } else
            set_flag(CLI_FLAGS::FLAG_HELP);
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

    cli_info execute() {
        if (is_set(CLI_FLAGS::FLAG_HELP)) {
            glob::logger::instance().log(glob::log_info{.type = glob::log_info::LOG_TYPE::INFO, .what = m_usage}, std::cout);
            throw cli_except("help exception");
        }

        if (is_set(CLI_FLAGS::FLAG_VERSION)) {
            glob::logger::instance().log(glob::log_info{.type = glob::log_info::LOG_TYPE::INFO, .what = MONAC_VERSION}, std::cout);
            throw cli_except("version exception");
        }

        return m_info;
    };
};
};  // namespace cli