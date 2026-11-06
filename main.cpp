#include <fstream>
#include <iostream>
#include <sstream>

#include "src/cli/cli.hpp"
#include "src/globals/logger.hpp"

int main(int argc, char** argv) {
    if (argc < 2) {
        glob::log_info li{.type = glob::log_info::LOG_TYPE::FATAL, .what = "No arguments passed to parse: try `monac --help` for more."};
        glob::logger::instance().log(li, std::cerr);
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
        for (const auto& f : info.input_files) {
            std::ifstream is{f};
            if (is.is_open()) {
                // preprocess
                // module dependency graph (imports and exports)
            }
        };

        // lex
        // parse
        // type check
        // whatever the f*ck else
        // generate asm
        // link
    } catch (cli::cli_except& e) {
        std::cerr << e.get_reason() << std::endl;
    }

    glob::logger::instance().report();
    return EXIT_SUCCESS;
};