# AGENTS.md

## Build and test (use CMake presets)
- Configure: `cmake --preset clang-debug` or `cmake --preset gcc-debug` (presets are defined in `CMakePresets.json`).
- Build: `cmake --build --preset clang-debug` (binary dir is `build/<preset>`).
- Test: `ctest --preset clang-test` or `ctest --preset gcc-test` (GTest is required by `mtl/tests`).
- Single test: `ctest --preset clang-test -R test_vector` (test names are defined in `mtl/tests/CMakeLists.txt`).

## Project layout and entrypoints
- Root `CMakeLists.txt` only adds `mtl/` (the `ts_mtl/` subproject exists but is commented out and not built by default).
- Core library modules live in `mtl/src/*.cppm`; the public module entrypoint is `mtl/src/mtl.cppm`.

## Toolchain quirks
- Presets hardcode compiler paths (`/usr/bin/clang++-22`, `/usr/bin/clang-22`, `/usr/bin/g++-15`, `/usr/bin/gcc-15`).
- The root `CMakeLists.txt` enables C++23 modules and sets `CMAKE_EXPERIMENTAL_CXX_IMPORT_STD`.
