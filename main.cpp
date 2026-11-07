#include <fstream>
#include <iostream>
#include <sstream>

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
        for (const auto& i : info.input_files) {
            
        }
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
    }

    glob::out.report();
    return EXIT_SUCCESS;
};