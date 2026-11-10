#include <fstream>
#include <iostream>
#include <sstream>

#include "src/cfg/dev.hpp"
#include "src/cli/cli.hpp"
#include "src/globals/logger.hpp"
#include "src/monac/prep/prep.hpp"
#include "src/utils/directory_manager.hpp"

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
        auto info = cli.execute();
#if defined(DEV_PEDANTIC) && DEV_PEDANTIC
        std::cout << "modules: " << std::endl;
        for (const auto& m : info.module_paths) {
            std::cout << m << std::endl;
        }
#endif

#if defined(DEV_PEDANTIC) && DEV_PEDANTIC
        std::cout << "inputs (source): " << std::endl;
        for (const auto& i : info.input_paths) {
            std::cout << i << std::endl;
        }
#endif

        // preprocess (go through every file .mm/.mx and expand macro defs)
        for (const auto& path : info.module_paths) {
            utils::fs::directory_manager dm{path};
            auto files = dm.resolve_directory();
            if (!utils::fs::directory_manager::validate_extension(files, ".mm", ".mx")) {
                std::cout << "error validating extension." << std::endl;  // handle dis better
            };
            for (const auto& f : files) {
                {
                    std::ifstream ifs{f};
                    if (ifs.is_open()) {
                        std::stringstream ss;
                        ss << ifs.rdbuf();
                        monac::prep::preprocessor prep{ss};
                        auto info = prep.process();
                        std::cout << info.expanded << std::endl;
                    } else {
                    }
                }
            }
        }

        for (const auto& path : info.input_paths) {
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
    } catch (cli::cli_except& e) {
        std::cerr << e.get_reason() << std::endl;
        return EXIT_FAILURE;
    }

    glob::out.report();
    return EXIT_SUCCESS;
};