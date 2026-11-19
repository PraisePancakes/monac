# ENTRY 

From the build directory run :

`./MONAC monac --help`

To see some current outputs from the build directory run :

`./MONAC monac -P ../examples/example_source.mx ../examples/example_xmacro.mx`

# REQUIREMENTS

Monac is built on a compiler that supports at least C++23. 
Recommended Compiler Specs. include Clang 20.1.2 x86_64 and GCC 14.2.0 x86_64. 
This project requires CMake version 3.28 or later.

# BUILDING
`cmake -S . -B build` 

`cmake --build build`

To build tests/examples/benchmarks navigate to the CMakeLists.txt file in the root directory of Forge and locate option(FORGE_BUILD_EXAMPLES "Build Forge examples" OFF) option(FORGE_BUILD_TESTS "Build Forge tests" OFF) option(FORGE_BUILD_BENCHMARK "Build Forge benchmarks" OFF) you can then switch from OFF to ON which will build the options.

