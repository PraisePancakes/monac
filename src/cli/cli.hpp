#ifndef CLI_HPP
#define CLI_HPP
#include <istream>
#include <mona/mona.hpp>

#include "../cfg/version.hpp"
#include "../globals/logger.hpp"
#include "../utils/parsers.hpp"

namespace cli {
using namespace utils;
class cli_except final : public std::exception {
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

class args final {
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
    bool any_set() {
#define X(e, _, __, v) \
    if (is_set(CLI_FLAGS::e)) return true;
        FLAG_ENTRY_LIST
#undef X
        return false;
    };
    bool valid_flag(const std::string& s) {
#define X(_, ln, __, ___) \
    if (s == ln) return true;
        FLAG_ENTRY_LIST
#undef X
        return false;
    };
    bool valid_flag(const char c) {
#define X(_, __, sn, ___) \
    if (c == sn) return true;
        FLAG_ENTRY_LIST
#undef X
        return false;
    };
    void parse_FLAG_INPUT() {
        set_flag(CLI_FLAGS::FLAG_INPUT);
        while (!mona::_eof()(m_arguments)) {
            parsers::skip_wspace(m_arguments);
            auto sng = mona::_check(parsers::single_flag)(m_arguments);
            auto dub = mona::_check(parsers::double_flag)(m_arguments);
            if (sng.has_value() || dub.has_value()) break;
            auto file = parsers::file(m_arguments);
            if (file.has_value()) m_info.input_files.push_back(file.value());
            parsers::skip_wspace(m_arguments);
        }
    };
    void parse_FLAG_OUTPUT() {
        set_flag(CLI_FLAGS::FLAG_OUTPUT);
        parsers::skip_wspace(m_arguments);
        auto sng = mona::_check(parsers::single_flag)(m_arguments);
        auto dub = mona::_check(parsers::double_flag)(m_arguments);
        if (sng.has_value() || dub.has_value()) {
            glob::logger::instance().log(glob::log_info{.type = glob::log_info::LOG_TYPE::FATAL, .what = "Missing output files"}, std::cerr);
            throw cli_except("Cli couldn't parse output files");
        };
        auto op = mona::_symbols_ex(' ')(m_arguments);
        if (!op.has_value()) {
            glob::logger::instance().log(glob::log_info{.type = glob::log_info::LOG_TYPE::FATAL, .what = "Unknown Symbol"}, std::cerr);
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

    void parse_command() {
        // don't touch this. X-macro automates this portion, all you need is to define the parse functions in the form of parse_FLAG_<FLAG>
        parsers::skip_wspace(m_arguments);
        auto before = m_arguments.tellg();
        // must parse some options
        auto sng = parsers::single_flag(m_arguments);
        auto dub = parsers::double_flag(m_arguments);
        if (dub.has_value()) {
            auto flag = dub.value();
            if (!valid_flag(flag)) {
                glob::logger::instance().log(glob::log_info{.type = glob::log_info::LOG_TYPE::FATAL, .what = std::format("Command: '{}' is not valid", flag)}, std::cerr);
                throw cli_except("Unkown command");
            }
#define X(e, ln, _, __) \
    if (flag == ln) parse_##e();
            FLAG_ENTRY_LIST
#undef X
        } else if (sng.has_value()) {
            auto flag = sng.value();
            if (!valid_flag(flag)) {
                glob::logger::instance().log(glob::log_info{.type = glob::log_info::LOG_TYPE::FATAL, .what = std::format("Command: '{}' is not valid", flag)}, std::cerr);
                throw cli_except("Unkown command");
            }
#define X(e, _, sn, __) \
    if (flag == sn) parse_##e();
            FLAG_ENTRY_LIST
#undef X
        }

        parsers::skip_wspace(m_arguments);
    };

   public:
    args(std::stringstream&& ss)
        : m_arguments{std::move(ss)} {
        std::ifstream ifs(USAGE_FILE_PATH);
        if (ifs.is_open()) {
            std::stringstream buf;
            buf << ifs.rdbuf();
            m_usage = buf.str();
            ifs.close();
        } else {
            glob::out.log(glob::log_info{.type = glob::log_info::LOG_TYPE::FATAL, .what = "Missing usage data"}, std::cerr);
            throw cli_except("Cli couldn't read usage data");
        }
    };
    void parse() {
        auto entry_parser = mona::_match("monac", "missing entry")(m_arguments);
        if (!entry_parser.has_value()) {
            glob::out.log(glob::log_info{.type = glob::log_info::LOG_TYPE::FATAL, .what = "Missing monac entry"}, std::cerr);
            throw cli_except("Cli couldn't find monac entry");
        };
        parsers::skip_wspace(m_arguments);
        while (!mona::_eof()(m_arguments))
            parse_command();
    };

    cli_info execute() {
        if (!any_set()) {
            glob::out.log(glob::log_info{.type = glob::log_info::LOG_TYPE::FATAL, .what = "No commands see 'monac --help' for more."}, std::cerr);
            throw cli_except("Cli couldn't parse commands");
        }

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

#endif