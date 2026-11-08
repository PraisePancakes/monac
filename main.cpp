#include <fstream>
#include <iostream>
#include <sstream>

#include "src/cfg/dev.hpp"
#include "src/cli/cli.hpp"
#include "src/globals/logger.hpp"
#include "src/monac/prep/prep.hpp"

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
        // remember input files will always be source files.
        // preprocess
        // module dependency graph (imports and exports)
        // lex
        // parse
        // type check
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