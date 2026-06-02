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

A C++23 data structures and algorithms library using C++20 modules (`.cppm` files). Also includes a thread-safe wrapper library (`ts_mtl`) using `std::shared_mutex` and `std::mutex` for concurrent access.

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
  ├─ mtl.pair              — simple pair struct with comparison operators
  ├─ mtl.set               — ordered set adapter over avl_tree
  ├─ mtl.map               — ordered map adapter over avl_tree (key-only comparison)
  ├─ mtl.hash_set          — unordered set adapter over hashing
  ├─ mtl.hash_map          — unordered map adapter over hashing (key-only hash/comparison)
  ├─ mtl.priority_queue    — heap-based priority queue
  ├─ mtl.queue             — queue adapter
  └─ mtl.stack             — stack adapter

### ts_mtl (thread-safe wrappers)

ts_mtl wraps mtl containers with locks for concurrent use:

| Module | Underlying | Lock strategy |
|--------|-----------|---------------|
| ts_mtl.deque | list_node (two-lock linked list) | head/tail mutexes |
| ts_mtl.queue | list_node (two-lock linked list) | head/tail mutexes |
| ts_mtl.stack | mtl.stack | single std::mutex |
| ts_mtl.hash_set | mtl.hash_set | std::shared_mutex |
| ts_mtl.hash_map | mtl.hash_map | std::shared_mutex |

- **ts_mtl.cppm** (umbrella) re-exports all ts_mtl modules.
- Read ops (empty, size, contains, find) use shared locks; write ops use exclusive locks.
- No `begin()`/`end()` exposed by ts_hash_set/ts_hash_map for traversal — iterators returned by `find()` are ephemeral.
- `operator=(const&)` is deleted on all ts_* containers (non-copyable pattern).
- Multi-threaded tests verify internal consistency, not that every concurrent insert succeeds — hopscotch hash table `insert()` can fail under contention (MAX_DIST=32). `hashing::insert()` retries with expand up to 3 times before returning false.
```

### Key patterns

- **Namespace**: Everything lives in `namespace mtl`.
- **Iterator design**: Each container defines a private `container_iterator<Ref, Ptr>` template class, with public `iterator` and `const_iterator` aliases. Iterators support conversion from normal to const via the `normal_to_const` concept.
- **Allocator**: Containers accept `Alloc = std::allocator<T>`, rebinding for internal node types via `std::allocator_traits`.
- **Construct/destroy**: Uses `std::construct_at` / `std::destroy_at` wrappers (`mtl::construct`, `mtl::destroy`, `mtl::construct_all`, `mtl::destroy_all`) instead of direct assignment.
- **Test framework**: Google Test (GTest). Each test file includes `<gtest/gtest.h>`, imports the relevant module, and defines its own `main()` calling `InitGoogleTest` + `RUN_ALL_TESTS`.
- **C++23**: Uses `import std;` (standard library modules), CMake C++ module support (`CMAKE_CXX_MODULE_STD ON`). Compiler: clang++.
- **Currently active**: Both `mtl` and `ts_mtl` subdirectories.

## Code Style

```bash
# Format all C++ source files (run from repo root)
clang-format -i mtl/src/*.cppm mtl/tests/src/*.cpp ts_mtl/src/*.cppm ts_mtl/tests/src/*.cpp
```

- **clang-format**: Uses `.clang-format` at repo root (BasedOnStyle: Google, C++20, IndentWidth: 4).
- **Never format CMakeLists.txt** with clang-format — it corrupts CMake syntax.
- Always use fully qualified `std::forward` and `std::move`, never replace them with `static_cast` or unqualified names.
- The global module fragment (`module;` at the top of a `.cppm` file) is only needed when the file uses `#include` directives to bring in headers. It has nothing to do with name visibility for `import std;`.
