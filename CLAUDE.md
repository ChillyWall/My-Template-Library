# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build & Test Commands

```bash
# Configure (clang debug is active default)
cmake --preset clang-debug

# Build
cmake --build --preset clang-debug

# Run all tests
cmake --test --preset clang-test

# Run a single test binary directly
./build/clang-debug/mtl/tests/test_vector
./build/clang-debug/mtl/tests/test_avl

# Full workflow (configure + build + test)
cmake --workflow --preset clang-debug-workflow

# Using GCC instead
cmake --workflow --preset gcc-debug-workflow
```

Available presets: `gcc-release`, `gcc-debug`, `clang-release`, `clang-debug`. Debug workflows include tests.

## Architecture

A C++23 data structures and algorithms library using C++20 modules (`.cppm` files). Also includes a small TypeScript wrapper (`ts_mtl`, header-only, currently disabled in the build).

### Module hierarchy

```
mtl (umbrella module, re-exports everything)
  └─ mtl.core
       ├─ mtl.core:defs    — exception types, iterator concepts (Iterator, RandomIterator)
       └─ mtl.core:utils   — construct/destroy helpers, swap, iter_swap, min/max/sum/prod, is_prime, advance/distance, move_ranges, copy, find_mid
  ├─ mtl.vector            — dynamic array with Iterator/RandomIterator, allocator-aware
  ├─ mtl.deque             — double-ended queue (map-of-blocks architecture)
  ├─ mtl.list              — doubly linked list
  ├─ mtl.algorithms (umbrella)
  │    ├─ mtl.algorithms:quick_sort
  │    └─ mtl.algorithms:merge_sort
  ├─ mtl.avl_tree          — AVL self-balancing BST
  ├─ mtl.hashing           — Hopscotch hash table
  ├─ mtl.pair              — simple pair struct
  ├─ mtl.priority_queue    — heap-based priority queue
  ├─ mtl.queue             — queue adapter
  └─ mtl.stack             — stack adapter
```

### Key patterns

- **Namespace**: Everything lives in `namespace mtl`.
- **Iterator design**: Each container defines a private `container_iterator<Ref, Ptr>` template class, with public `iterator` and `const_iterator` aliases. Iterators support conversion from normal to const via the `normal_to_const` concept.
- **Allocator**: Containers accept `Alloc = std::allocator<T>`, rebinding for internal node types via `std::allocator_traits`.
- **Construct/destroy**: Uses `std::construct_at` / `std::destroy_at` wrappers (`mtl::construct`, `mtl::destroy`, `mtl::construct_all`, `mtl::destroy_all`) instead of direct assignment.
- **Test framework**: Google Test (GTest). Each test file includes `<gtest/gtest.h>`, imports the relevant module, and defines its own `main()` calling `InitGoogleTest` + `RUN_ALL_TESTS`.
- **C++23**: Uses `import std;` (standard library modules), CMake C++ module support (`CMAKE_CXX_MODULE_STD ON`). Compiler: clang++.
- **Currently active**: `mtl` subdirectory. `ts_mtl` (TypeScript C++ wrapper, header-only) is commented out in the root CMakeLists.txt.
