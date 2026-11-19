#ifndef CLI_HPP
#define CLI_HPP
#include <istream>
#include <mona/mona.hpp>
#include <unordered_set>

#include "../cfg/version.hpp"
#include "../globals/logger.hpp"
#include "../utils/unit_parsers.hpp"
#include "parsers.hpp"

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
    X(FLAG_INPUT, "input", 'i', 1 << 3)     \
    X(FLAG_MODULE, "module", 'm', 1 << 4)   \
    X(FLAG_NORM, "norm", 'N', 1 << 5)       \
    X(FLAG_PREP, "prep", 'P', 1 << 6)

class args final {
    using flag_t = std::uint32_t;
    std::stringstream m_arguments;
    std::string m_usage;

   public:
    enum class CLI_FLAGS : flag_t {
#define X(e, _, __, v) e = v,
        FLAG_ENTRY_LIST
#undef X
    };
   private:
    flag_t m_flags{0};

    struct cli_info {
        std::vector<std::filesystem::path> input_paths;
        std::vector<std::filesystem::path> module_paths;
        std::vector<std::filesystem::path> all_paths;
        std::vector<std::filesystem::path> to_normalize;
        std::vector<std::filesystem::path> to_preprocess;
        std::filesystem::path output;
    } m_info;
    void set_flag(CLI_FLAGS flag) {
        m_flags |= static_cast<flag_t>(flag);
    };

    bool any_set() const {
#define X(e, _, __, ___) \
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

    template <typename ErrorCheckCallback, typename ProcessCallback>
    void _for_each_validate_then_process(ErrorCheckCallback&& check_err, ProcessCallback&& process) {
        while (!mona::_eof()(m_arguments)) {
            unit::opt_white_space(m_arguments);
            auto sng_should_finish = mona::_check(parsers::single_flag)(m_arguments);
            auto dub_should_finish = mona::_check(parsers::double_flag)(m_arguments);
            if (sng_should_finish.has_value() || dub_should_finish.has_value()) break;
            auto file = parsers::directory(m_arguments);
            if (!file.has_value()) {
                glob::logger::instance().log(glob::log_info{.type = glob::log_info::LOG_TYPE::FATAL, .what = std::format("File Parser error: {}", file.error())}, std::cerr);
                throw cli_except("Cli couldn't find input files");
            }
            std::filesystem::path fp{file.value()};
            check_err(fp);
            process(fp);
            unit::opt_white_space(m_arguments);
        }
    };

    void parse_FLAG_INPUT() {
        set_flag(CLI_FLAGS::FLAG_INPUT);
        _for_each_validate_then_process([&](std::filesystem::path check_path) {
                 if (!std::filesystem::exists(check_path) || !check_path.has_extension()) {
                glob::logger::instance().log(glob::log_info{.type = glob::log_info::LOG_TYPE::FATAL, 
                .what = std::format("Undefined input path: {}", check_path.string())}, std::cerr);
                throw cli_except("Cli couldn't find input files");
            }
            if (check_path.extension() != ".mx") {
                glob::logger::instance().log(glob::log_info{.type = glob::log_info::LOG_TYPE::FATAL, 
                .what = std::format("Undefined extension: input file {} requires an .mx extension", check_path.string())}, std::cerr);
                throw cli_except("Cli couldn't find input files");
            } },
                                        [&](std::filesystem::path process_path) {
                                            m_info.input_paths.push_back(process_path);
                                            m_info.all_paths.push_back(process_path);
                                        });
    };
    void parse_FLAG_OUTPUT() {
        set_flag(CLI_FLAGS::FLAG_OUTPUT);
        unit::opt_white_space(m_arguments);
        auto sng_should_finish = mona::_check(parsers::single_flag)(m_arguments);
        auto dub_should_finish = mona::_check(parsers::double_flag)(m_arguments);
        if (sng_should_finish.has_value() || dub_should_finish.has_value()) {
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

    void parse_FLAG_MODULE() {
        set_flag(CLI_FLAGS::FLAG_MODULE);
        _for_each_validate_then_process([&](std::filesystem::path path) {
            if (!std::filesystem::exists(path)) {
                glob::logger::instance().log(glob::log_info{.type = glob::log_info::LOG_TYPE::FATAL, 
                    .what = std::format("Undefined module path: {}", path.string())}, std::cerr);
                throw cli_except("Cli couldn't find input files");
            } }, [&](std::filesystem::path path) {
                  m_info.module_paths.push_back(path);
                m_info.all_paths.push_back(path); });
    };

    void parse_FLAG_NORM() {
        set_flag(CLI_FLAGS::FLAG_NORM);
        _for_each_validate_then_process([&](std::filesystem::path check_path) {
             if (!std::filesystem::exists(check_path) || !check_path.has_extension()) {
                glob::logger::instance().log(glob::log_info{.type = glob::log_info::LOG_TYPE::FATAL, 
                .what = std::format("Undefined input path to normalize: {}", check_path.string())}, std::cerr);
                throw cli_except("Cli couldn't find input files");
            } }, [&](std::filesystem::path path) { m_info.to_normalize.push_back(path); });
    };

    void parse_FLAG_PREP() {
        set_flag(CLI_FLAGS::FLAG_PREP);
        _for_each_validate_then_process([&](std::filesystem::path check_path) {
             if (!std::filesystem::exists(check_path) || !check_path.has_extension()) {
                glob::logger::instance().log(glob::log_info{.type = glob::log_info::LOG_TYPE::FATAL, 
                .what = std::format("Undefined input path to preprocess: {}", check_path.string())}, std::cerr);
                throw cli_except("Cli couldn't find input files");
            } }, [&](std::filesystem::path path) { m_info.to_preprocess.push_back(path); });
    };
    void parse_FLAG_HELP() {
        set_flag(CLI_FLAGS::FLAG_HELP);
    };
    void parse_FLAG_VERSION() {
        set_flag(CLI_FLAGS::FLAG_VERSION);
    }

    void parse_command() {
        // don't touch this. X-macro automates this portion, all you need is to define the parse functions in the form of parse_FLAG_<FLAG>
        unit::opt_white_space(m_arguments);
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

        unit::opt_white_space(m_arguments);
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

    template <typename... Flags>
        requires(std::is_same_v<Flags, CLI_FLAGS> && ...)
    bool is_set_and(Flags... flags) {
        return (is_set(flags) && ...);
    }

    template <typename... Flags>
        requires(std::is_same_v<Flags, CLI_FLAGS> && ...)
    bool is_set_or(Flags... flags) {
        return (is_set(flags) || ...);
    }

    bool is_set(CLI_FLAGS flag) const {
        return (m_flags & static_cast<flag_t>(flag));
    }

    void parse() {
        auto entry_parser = mona::_match("monac", "missing entry")(m_arguments);
        if (!entry_parser.has_value()) {
            glob::out.log(glob::log_info{.type = glob::log_info::LOG_TYPE::FATAL, .what = "Missing monac entry"}, std::cerr);
            throw cli_except("Cli couldn't find monac entry");
        };
        unit::opt_white_space(m_arguments);
        while (!mona::_eof()(m_arguments))
            parse_command();
    };

    cli_info execute() const {
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