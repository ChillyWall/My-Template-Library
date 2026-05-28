# AGENTS.md

## Build / test (CMake presets)
- Configure (clang debug default): `cmake --preset clang-debug`
- Build: `cmake --build --preset clang-debug`
- Run all tests: `cmake --test --preset clang-test`
- Full workflow (configure+build+test): `cmake --workflow --preset clang-debug-workflow`
- GCC workflow: `cmake --workflow --preset gcc-debug-workflow`
- Single tests run directly from build dir, e.g. `./build/clang-debug/mtl/tests/test_vector`

## Project shape / quirks
- C++23 library using C++20 modules (`.cppm`) with `CMAKE_CXX_MODULE_STD ON`.
- Umbrella module `mtl` re-exports modules in `mtl/src/*.cppm` (see CLAUDE.md for hierarchy).
- `ts_mtl` subdirectory exists but is commented out in root `CMakeLists.txt` (not built).
- Tests are GTest executables defined in `mtl/tests/CMakeLists.txt` and registered via `add_test`.

## Existing instructions
- See `CLAUDE.md` for detailed module hierarchy and design patterns.
