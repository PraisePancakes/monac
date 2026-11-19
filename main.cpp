#include <fstream>
#include <iostream>
#include <sstream>

#include "src/cfg/dev.hpp"
#include "src/cli/cli.hpp"
#include "src/globals/logger.hpp"
#include "src/monac/prep/normalizer.hpp"
#include "src/monac/prep/prep.hpp"
#include "src/utils/directory_manager.hpp"

template <typename ProcessCallback>
bool atomic_process(const std::vector<std::filesystem::path>& to_process, ProcessCallback&& callback) {
    for (const auto& path : to_process) {
        utils::fs::directory_manager dm{path};
        auto files = dm.resolve_directory();
        if (!utils::fs::directory_manager::validate_extension(files, ".mm", ".mx")) {
            glob::log_info li{.type = glob::log_info::LOG_TYPE::FATAL, .what = std::format("Error validating monac file extension in path: {} => expected a .mm (module) / .mx(source) extension", path.c_str())};
            glob::out.log(li, std::cerr);
            return EXIT_FAILURE;
        };
        for (const auto& f : files) {
            {
                std::ifstream ifs{f};
                if (ifs.is_open()) {
                    std::stringstream ss;
                    ss << ifs.rdbuf();
                    callback(ss, f);
                } else {
                    glob::log_info li{.type = glob::log_info::LOG_TYPE::FATAL, .what = std::format("Error opening file for I/O: {}", f.c_str())};
                    glob::out.log(li, std::cerr);
                    return EXIT_FAILURE;
                }
            }
        };
    }
    return EXIT_SUCCESS;
};

int main(int argc, char** argv) {
    if (argc < 2) {
        glob::log_info li{.type = glob::log_info::LOG_TYPE::FATAL, .what = "No arguments passed to parse: try `monac --help` for more."};
        glob::out.log(li, std::cerr);
        return EXIT_FAILURE;
    }
    std::stringstream args;
    for (int i = 1; i < argc; i++) {
        args << argv[i] << ' ';
    }
    try {
        cli::args cli{std::move(args)};
        cli.parse();
        auto cli_info = cli.execute();
#if defined(DEV_PEDANTIC) && DEV_PEDANTIC
        std::cout << "modules: " << std::endl;
        for (const auto& m : cli_info.module_paths) {
            std::cout << m << std::endl;
        }
#endif

#if defined(DEV_PEDANTIC) && DEV_PEDANTIC
        std::cout << "inputs (source): " << std::endl;
        for (const auto& i : cli_info.input_paths) {
            std::cout << i << std::endl;
        }
#endif
        if (cli.is_set_or(cli::args::CLI_FLAGS::FLAG_PREP, cli::args::CLI_FLAGS::FLAG_NORM)) {
            bool passed{true};
            if (cli.is_set(cli::args::CLI_FLAGS::FLAG_NORM))
                passed = atomic_process(cli_info.to_normalize, [&](std::stringstream& ss, const std::filesystem::path path) {
                    const std::string normalized = monac::prep::normalizer{ss}.process();
                    glob::log_info li{.type = glob::log_info::LOG_TYPE::INFO,
                                      .what = std::format("Normalized {} with contents:\n\n{}", path.c_str(), normalized)};
                    glob::out.log(li, std::cerr);
                });
            if (cli.is_set(cli::args::CLI_FLAGS::FLAG_PREP)) {
                passed = atomic_process(cli_info.to_preprocess, [&](std::stringstream& ss, const std::filesystem::path path) {
                    monac::prep::preprocessor prep{monac::prep::normalizer{ss}.process()};
                    auto prep_info = prep.process();
                    glob::log_info li{.type = glob::log_info::LOG_TYPE::INFO,
                                      .what = std::format("Preprocess {} with contents:\n\n{}", path.c_str(), prep_info.expanded)};
                    glob::out.log(li, std::cerr);
                });
            }

            return passed;

        } else {
            //////////////////////////////// FULL COMPILATION ////////////////////////////////
            // preprocess (go through every file .mm/.mx and expand macro defs)
            for (const auto& path : cli_info.all_paths) {
                utils::fs::directory_manager dm{path};
                auto files = dm.resolve_directory();
                if (!utils::fs::directory_manager::validate_extension(files, ".mm", ".mx")) {
                    glob::log_info li{.type = glob::log_info::LOG_TYPE::FATAL, .what = std::format("Error validating monac file extension in path: {} => expected a .mm (module) / .mx(source) extension", path.c_str())};
                    glob::out.log(li, std::cerr);
                    return EXIT_FAILURE;
                };
                for (const auto& f : files) {
                    {
                        std::ifstream ifs{f};
                        if (ifs.is_open()) {
                            std::stringstream ss;
                            ss << ifs.rdbuf();
                            monac::prep::preprocessor prep{monac::prep::normalizer{ss}.process()};
                            auto prep_info = prep.process();

                        } else {
                            glob::log_info li{.type = glob::log_info::LOG_TYPE::FATAL, .what = std::format("Error opening file for I/O: {}", f.c_str())};
                            glob::out.log(li, std::cerr);
                            return EXIT_FAILURE;
                        }
                    }
                }
            }

            // mds (module dependancy schedular)
            //  * go through all modules
            //  * set up the DAG, sort them topologically
            // bmi (built module interface) keep in outer scope so once we build source files we can retrieve symbols from here
            //  * recieve the sorted DAG
            //  * go through each module and build them (set up symbol table {exports, imports, sealed, etc..})
            // lex
            // parse
            // static analysis
            // whatever the f*ck else
            // generate asm
            // link
        }

    } catch (cli::cli_except& e) {
        std::cerr << e.get_reason() << std::endl;
        return EXIT_FAILURE;
    }

    glob::out.report();
    return EXIT_SUCCESS;
};