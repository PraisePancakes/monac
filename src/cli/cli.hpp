#ifndef CLI_HPP
#define CLI_HPP
#include <istream>
#include <mona/mona.hpp>
#include <unordered_set>

#include "../cfg/version.hpp"
#include "../globals/logger.hpp"
#include "../utils/directory_manager.hpp"
#include "../utils/unit_parsers.hpp"
#include "parsers.hpp"

namespace cli {
using namespace utils;
class cli_except final : public std::exception {
    std::string _m_what;

   public:
    cli_except() : _m_what{""} {};
    cli_except(const std::string& what) : _m_what(std::format("[MONAC-CLI-EXCEPT]: {}", what)) {};
    [[nodiscard]] const char* what() const noexcept override {
        return _m_what.c_str();
    };
    ~cli_except() = default;
};

#define FLAG_ENTRY_LIST                     \
    X(FLAG_HELP, "help", 'h', 1 << 0)       \
    X(FLAG_VERSION, "version", 'v', 1 << 1) \
    X(FLAG_OUTPUT, "output", 'o', 1 << 2)   \
    X(FLAG_NORM, "norm", 'N', 1 << 5)       \
    X(FLAG_PREP, "prep", 'P', 1 << 6)       \
    X(FLAG_LEX, "lex", 'l', 1 << 7)

class args final {
    using flag_t = std::uint32_t;
    std::stringstream _m_arguments;
    std::string _m_usage;

   public:
    enum class CLI_FLAGS : flag_t {
#define X(e, _, __, v) e = v,
        FLAG_ENTRY_LIST
#undef X
    };
   private:
    flag_t _m_flags{0};

    struct cli_info {
        std::vector<std::filesystem::path> all_paths;
        std::filesystem::path output;
    } _m_info;
    void set_flag(CLI_FLAGS flag) {
        _m_flags |= static_cast<flag_t>(flag);
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

    template <typename ErrorCheckCallback_, typename ProcessCallback_>
    void _for_each_validate_then_process(ErrorCheckCallback_&& check_err, ProcessCallback_&& process) {
        while (!mona::_eof()(_m_arguments)) {
            unit::opt_white_space(_m_arguments);
            auto sng_should_finish = mona::_check(parsers::single_flag)(_m_arguments);
            auto dub_should_finish = mona::_check(parsers::double_flag)(_m_arguments);
            if (sng_should_finish.has_value() || dub_should_finish.has_value()) break;
            auto file = parsers::directory(_m_arguments);
            if (!file.has_value()) {
                glob::log(glob::log_info::LOG_TYPE::FATAL, std::format("File Parser error: {}", file.error()), std::cerr);
                throw cli_except("Cli couldn't find input files");
            }
            std::filesystem::path fp{file.value()};
            check_err(fp);
            process(fp);
            unit::opt_white_space(_m_arguments);
        }
    };

    void parse_FLAG_OUTPUT() {
        set_flag(CLI_FLAGS::FLAG_OUTPUT);
        unit::opt_white_space(_m_arguments);
        auto sng_should_finish = mona::_check(parsers::single_flag)(_m_arguments);
        auto dub_should_finish = mona::_check(parsers::double_flag)(_m_arguments);
        if (sng_should_finish.has_value() || dub_should_finish.has_value()) {
            glob::log(glob::log_info::LOG_TYPE::FATAL, "Missing output files", std::cerr);
            throw cli_except("Cli couldn't parse output files");
        };
        auto op = mona::_symbols_ex(' ')(_m_arguments);
        if (!op.has_value()) {
            glob::log(glob::log_info::LOG_TYPE::FATAL, "Unknown Symbol", std::cerr);
            throw cli_except("Cli couldn't parse output files");
        }
        std::filesystem::path path{op.value()};
        if (!path.has_extension()) throw cli_except("Output path does not have an extension");
        _m_info.output = path;
    };

    void parse_FLAG_HELP() {
        set_flag(CLI_FLAGS::FLAG_HELP);
    };
    void parse_FLAG_VERSION() {
        set_flag(CLI_FLAGS::FLAG_VERSION);
    }

    void parse_FLAG_NORM() {
        set_flag(CLI_FLAGS::FLAG_NORM);
    }

    void parse_FLAG_PREP() {
        set_flag(CLI_FLAGS::FLAG_PREP);
    }

    void parse_FLAG_LEX() {
        set_flag(CLI_FLAGS::FLAG_LEX);
    }

    void parse_command() {
        // don't touch this. X-macro automates this portion, all you need is to define the parse functions in the form of parse_FLAG_<FLAG>
        unit::opt_white_space(_m_arguments);
        // must parse some options
        auto sng = parsers::single_flag(_m_arguments);
        auto dub = parsers::double_flag(_m_arguments);
        if (dub.has_value()) {
            auto flag = dub.value();
            if (!valid_flag(flag)) {
                glob::log(glob::log_info::LOG_TYPE::FATAL, std::format("Command: '{}' is not valid", flag), std::cerr);
                throw cli_except("Unkown command");
            }
#define X(e, ln, _, __) \
    if (flag == ln) parse_##e();
            FLAG_ENTRY_LIST
#undef X
        } else if (sng.has_value()) {
            auto flag = sng.value();
            if (!valid_flag(flag)) {
                glob::log(glob::log_info::LOG_TYPE::FATAL, std::format("Command: '{}' is not valid", flag), std::cerr);
                throw cli_except("Unkown command");
            }
#define X(e, _, sn, __) \
    if (flag == sn) parse_##e();
            FLAG_ENTRY_LIST
#undef X
        } else {
            // parse path
            _for_each_validate_then_process([&](std::filesystem::path check_path) {
                 if (!std::filesystem::exists(check_path)) {
                glob::log(glob::log_info::LOG_TYPE::FATAL, std::format("Undefined input path: {}", check_path.string()), std::cerr);
                throw cli_except("Cli couldn't find input files");
            } },
                                            [&](std::filesystem::path process_path) {
                                                utils::fs::directory_manager dm{process_path};
                                                auto files = dm.resolve_directory();
                                                for (const auto& f : files)
                                                    if (f.extension() == ".mx" || f.extension() == ".mm")
                                                        _m_info.all_paths.push_back(f);
                                                    else
                                                        glob::log(glob::log_info::LOG_TYPE::WARNING, std::format("File format not recognized, skipping: {}", f.string()), std::cerr);
                                            });
        }

        unit::opt_white_space(_m_arguments);
    };

   public:
    args(std::stringstream&& ss)
        : _m_arguments{std::move(ss)} {
        std::ifstream ifs(USAGE_FILE_PATH);
        if (ifs.is_open()) {
            std::stringstream buf;
            buf << ifs.rdbuf();
            _m_usage = buf.str();
            ifs.close();
        } else {
            glob::log(glob::log_info::LOG_TYPE::FATAL, "Missing usage data", std::cerr);
            throw cli_except("Cli couldn't read usage data");
        }
    };

    template <typename... Flags_>
        requires(std::is_same_v<Flags_, CLI_FLAGS> && ...)
    bool is_set_and(Flags_... flags) {
        return (is_set(flags) && ...);
    }

    template <typename... Flags_>
        requires(std::is_same_v<Flags_, CLI_FLAGS> && ...)
    bool is_set_or(Flags_... flags) {
        return (is_set(flags) || ...);
    }

    bool is_set(CLI_FLAGS flag) const {
        return (_m_flags & static_cast<flag_t>(flag));
    }

    void parse() {
        auto entry_parser = mona::_match("monac", "missing entry")(_m_arguments);
        if (!entry_parser.has_value()) {
            glob::log(glob::log_info::LOG_TYPE::FATAL, "Missing monac entry", std::cerr);
            throw cli_except("Cli couldn't find monac entry");
        };
        unit::opt_white_space(_m_arguments);
        while (!mona::_eof()(_m_arguments))
            parse_command();
    };

    cli_info execute() const {
        if (is_set(CLI_FLAGS::FLAG_HELP)) {
            glob::log(glob::log_info::LOG_TYPE::INFO, std::format("\n{}", _m_usage), std::cout);
            throw cli_except("help exception");
        }

        if (is_set(CLI_FLAGS::FLAG_VERSION)) {
            glob::log(glob::log_info::LOG_TYPE::INFO, MONAC_VERSION, std::cout);
            throw cli_except("version exception");
        }
        if (_m_info.all_paths.empty()) {
            glob::log(glob::log_info::LOG_TYPE::FATAL, "no input files\ncompilation halted.", std::cout);
            throw cli_except("Cli couldn't parse any input files, see `monac --help` for more details.");
        }
        return _m_info;
    };
};
};  // namespace cli

#endif