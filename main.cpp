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
    for (int i = 0; i < argc; i++) {
        args << argv[i] << ' ';
    }
    cli::args cli{std::move(args)};
    cli.parse();
    glob::logger::instance().report();
    return EXIT_SUCCESS;
};