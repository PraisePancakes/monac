include(FetchContent)

FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.11
)

FetchContent_MakeAvailable(doctest)

add_executable(monac_tests
main.cpp
unit_parsers.cpp
)

target_compile_options(monac_tests PRIVATE
    -fsanitize=address
    -fno-omit-frame-pointer
)
target_compile_features(monac_tests INTERFACE cxx_std_23)

target_link_options(monac_tests PRIVATE
    -fsanitize=address
)

target_link_libraries(monac_tests PRIVATE mona::mona)
include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(monac_tests)