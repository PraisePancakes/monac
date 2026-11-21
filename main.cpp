#include <fstream>
#include <iostream>
#include <sstream>

#include "src/cfg/dev.hpp"
#include "src/cli/cli.hpp"
#include "src/globals/logger.hpp"
#include "src/monac/prep/normalizer.hpp"
#include "src/monac/prep/prep.hpp"
#include "src/utils/directory_manager.hpp"
using namespace cli;
template <typename ProcessCallback>
bool mx_atomic_process(const std::vector<std::filesystem::path>& to_process, ProcessCallback&& callback) {
    for (const auto& path : to_process) {
        utils::fs::directory_manager dm{path};
        auto files = dm.resolve_directory();
        if (!utils::fs::directory_manager::validate_extension(files, ".mm", ".mx")) {
            glob::log_info li{};
            glob::log(glob::log_info::LOG_TYPE::FATAL, std::format("Error validating monac file extension in path: {} => expected a .mm (module) / .mx(source) extension", path.c_str()), std::cerr);
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
                    glob::log(glob::log_info::LOG_TYPE::FATAL, std::format("Error opening file for I/O: {}", f.c_str()), std::cerr);
                    return EXIT_FAILURE;
                }
            }
        };
    }
    return EXIT_SUCCESS;
};

int main(int argc, char** argv) {
    if (argc < 2) {
        glob::log(glob::log_info::LOG_TYPE::FATAL, "No arguments passed to parse: try `monac --help` for more.", std::cerr);
        return EXIT_FAILURE;
    }
    std::stringstream args;
    for (int i = 1; i < argc; i++) {
        args << argv[i] << ' ';
    }

    bool MONAC__PROCESS__COMBINATION__STATUS = {EXIT_SUCCESS};
    try {
        cli::args cli{std::move(args)};
        cli.parse();
        auto cli_info = cli.execute();

#if defined(DEV_PEDANTIC) && DEV_PEDANTIC
        std::cout << "inputs (source): " << std::endl;
        for (const auto& i : cli_info.all_paths) {
            std::cout << i << std::endl;
        }
#endif
        if (cli.is_set_or(args::CLI_FLAGS::FLAG_PREP, args::CLI_FLAGS::FLAG_NORM)) {
            if (cli.is_set(args::CLI_FLAGS::FLAG_NORM))
                MONAC__PROCESS__COMBINATION__STATUS = mx_atomic_process(cli_info.all_paths, [&](std::stringstream& ss, const std::filesystem::path path) {
                    const std::string normalized = monac::prep::normalizer{ss}.process();
                    std::string contents = std::format("Normalized {} with contents:\n\n{}", path.c_str(), normalized);
                    if (cli.is_set(args::CLI_FLAGS::FLAG_OUTPUT)) {
                        {
                            const std::string io_error = std::format("Error opening file for I/O: {}", cli_info.output.c_str());
                            std::ofstream ofs{cli_info.output};
                            if (!ofs.is_open()) return (glob::log(glob::log_info::LOG_TYPE::FATAL, io_error, std::cerr), EXIT_FAILURE);
                            ofs << contents;
                        };
                    } else
                        glob::log(glob::log_info::LOG_TYPE::INFO, contents, std::clog);
                    return EXIT_SUCCESS;
                });
            if (cli.is_set(args::CLI_FLAGS::FLAG_PREP)) {
                MONAC__PROCESS__COMBINATION__STATUS = mx_atomic_process(cli_info.all_paths, [&](std::stringstream& ss, const std::filesystem::path path) {
                    monac::prep::preprocessor prep{monac::prep::normalizer{ss}.process()};
                    auto prep_info = prep.process();
                    std::stringstream to_normalize{prep_info.expanded};
                    const std::string normalized_expansion = monac::prep::normalizer{to_normalize}.process();
                    const std::string contents = std::format("Preprocess {} with contents:\n\n{}", path.c_str(), normalized_expansion);
                    if (cli.is_set(args::CLI_FLAGS::FLAG_OUTPUT)) {
                        {
                            const std::string io_error = std::format("Error opening file for I/O: {}", cli_info.output.c_str());
                            std::ofstream ofs{cli_info.output};
                            if (!ofs.is_open()) return (glob::log(glob::log_info::LOG_TYPE::FATAL, io_error, std::cerr), EXIT_FAILURE);
                            ofs << contents;
                        };
                    } else
                        glob::log(glob::log_info::LOG_TYPE::INFO, contents, std::clog);
                    return EXIT_SUCCESS;
                });
            }

        } else {
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //                                                                                                                                        ||
            //    ▄██▀▄▀█▄  ▄██▀█▄  ▄██▀█▄  ▄██▀█▄  ███▀██       ███▀██  ▄██▀█▄  ▄██▀▄▀█▄  ███▀█▄  ███  ███     ▄██▀█▄ █▀███▀██  ███  ▄██▀█▄  ▄██▀█▄  ||
            //    ███ █ ██ ▀███ ██ ▀███ ██  ███ ██ ▄███         ▄███    ▀███ ██  ███ █ ██ ▀███▀▀▀  ███ ▀███     ███ ██   ███     ███ ▀███ ██ ▀███ ██  ||
            //   ▀███   ██  ███ ██  ███ ██ ▀███▀██  ███ ▄▄       ███ ▄▄  ███ ██ ▀███   ██  ███    ▀███  ███ ▄▄ ▀███▀██   ███    ▀███  ███ ██  ███ ██  ||
            //    ▀▀▀    ▀  ▀▀▀▀▀   ▀▀▀ ▀▀  ▀▀▀ ▀▀   ▀▀▀▀▀        ▀▀▀▀▀  ▀▀▀▀▀   ▀▀▀    ▀  ▀▀▀     ▀▀▀   ▀▀▀▀▀  ▀▀▀ ▀▀   ▀▀▀     ▀▀▀  ▀▀▀▀▀   ▀▀▀ ▀▀  ||                                                     //
            //========================================================================================================================================||
            //   0) preprocess (go through every file .mm/.mx and expand macro defs)                                                                  ||
            //   1) mds (module dependancy scheduler)                                                                                                 ||
            //      * go through all modules                                                                                                          ||
            //      * set up the DAG, sort them topologically                                                                                         ||
            //   2) bmi (built module interface) keep in outer scope so once we build source files we can retrieve symbols from here                  ||
            //      * recieve the sorted DAG                                                                                                          ||
            //      * go through each module and build them (set up symbol table {exports, imports, sealed, etc..})                                   ||
            //   3) lex                                                                                                                               ||
            //   4) parse                                                                                                                             ||
            //   5) static analysis                                                                                                                   ||
            //   6) whatever the f*ck else                                                                                                            ||
            //   7) generate asm                                                                                                                      ||
            //   9) optimization passes                                                                                                               ||
            //   8) link                                                                                                                              ||
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            MONAC__PROCESS__COMBINATION__STATUS = mx_atomic_process(cli_info.all_paths, [&](std::stringstream& ss, const std::filesystem::path path) {
                glob::log(glob::log_info::LOG_TYPE::INFO, std::format("Beginning Compilation of Translation Unit {}", path.c_str()), std::clog);
                monac::prep::preprocessor prep{monac::prep::normalizer{ss}.process()};
                glob::log(glob::log_info::LOG_TYPE::INFO, std::format("Preprocessing...\t\t{}", path.c_str()), std::clog);
                const auto prep_info = prep.process();
                
            });
        }

    } catch (cli::cli_except& e) {
        std::cerr << e.get_reason() << std::endl;
        return MONAC__PROCESS__COMBINATION__STATUS;
    }

    glob::report();
    return MONAC__PROCESS__COMBINATION__STATUS;
};