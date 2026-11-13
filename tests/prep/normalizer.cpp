#include "../../src/monac/prep/normalizer.hpp"

#include <sstream>

#include "../doctest.h"

TEST_SUITE("Prep Normalizer") {
    TEST_CASE("Normalizer") {
        SUBCASE("Normalizer normalizes basic stream") {
            std::stringstream ss{"\\\r\nabc\\\r\ndef\\\r\n\\\r\nwxyz"};
            monac::prep::normalizer norm{ss};
            std::string result = norm.process();
            CHECK(result == " abc def wxyz");
        }
    }
}
