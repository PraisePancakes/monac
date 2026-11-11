#include <iostream>

#include "../src/utils/directory_manager.hpp"
#include "doctest.h"
TEST_SUITE("Directory Manager") {
    TEST_CASE("Directory Manager flattens directory to vector of file paths") {
        std::filesystem::path mock_dir{"../tests/mock_dir"};
        REQUIRE(std::filesystem::exists(mock_dir));
        REQUIRE(std::filesystem::is_directory(mock_dir));

        utils::fs::directory_manager dm{mock_dir};
        auto v = dm.resolve_directory();
        REQUIRE(v.size() == 6);
    };
}
