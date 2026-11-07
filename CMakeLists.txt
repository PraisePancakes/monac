cmake_minimum_required(VERSION 3.28)
set(MONAC_VERSION_REGEX "#define MONAC_VERSION_.*[ \t]+(.+)")
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/src/cfg/version.hpp" MONAC_VERSION REGEX ${MONAC_VERSION_REGEX})
list(TRANSFORM MONAC_VERSION REPLACE ${MONAC_VERSION_REGEX} "\\1")
string(JOIN "." MONAC_VERSION ${MONAC_VERSION})
set(PROJECT_NAME MONAC)

project(
    ${PROJECT_NAME} 
    VERSION ${MONAC_VERSION} 
    DESCRIPTION "monac source"
    HOMEPAGE_URL "https://github.com/praispancakes/monac"
    LANGUAGES C CXX
)
message("Your C++ compiler supports these C++ features:")
foreach(i ${CMAKE_CXX_COMPILE_FEATURES})
    message("${i}")
endforeach()
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
include(FetchContent)


FetchContent_Declare(
    mona
    GIT_REPOSITORY https://github.com/praisepancakes/mona.git
    GIT_TAG main
)
FetchContent_MakeAvailable(mona)



add_executable(${PROJECT_NAME} 
    main.cpp 
)
target_link_libraries(${PROJECT_NAME}  PRIVATE mona)
message("INFO :")
message("  C: '${CMAKE_C_COMPILER}'")
message("  C++: '${CMAKE_CXX_COMPILER}'")
message("  Source: ${PROJECT_SOURCE_DIR}")
message("  Binary: ${PROJECT_BINARY_DIR}")
message("  Version: ${PROJECT_VERSION}")
message("  Version (alt): ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:Clang>:-Wno-c++26-extensions>
)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_23)
target_compile_definitions(${PROJECT_NAME} PRIVATE
    PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
)
message(STATUS  "*")
message(STATUS  "* ${PROJECT_NAME} v${PROJECT_VERSION} (${CMAKE_BUILD_TYPE})")
message(STATUS  "*")


option(MONAC_BUILD_DEV "Build the developement bed" ON)
if(MONAC_BUILD_DEV)
    option(MONAC_BUILD_TESTS "Build Monac tests" ON)
    if(MONAC_BUILD_TESTS)
        enable_testing()
        add_subdirectory(tests)
    endif()
  
endif()