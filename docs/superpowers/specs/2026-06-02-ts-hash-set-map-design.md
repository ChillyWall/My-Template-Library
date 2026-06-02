# ts_hash_set and ts_hash_map Design

## Summary

Add `ts_hash_set` and `ts_hash_map` to the `ts_mtl` library — thread-safe wrappers
around the existing `mtl::hash_set` and `mtl::hash_map` containers, using
`std::shared_mutex` for read/write locking.

## Motivation

The `mtl` library already provides `hash_set` (unordered unique-key set) and
`hash_map` (unordered key-value map) backed by hopscotch hashing. The `ts_mtl`
sub-library provides thread-safe wrappers (`ts_queue`, `ts_deque`, `ts_stack`)
around single-threaded containers. Adding `ts_hash_set` and `ts_hash_map`
completes the thread-safe collection offerings.

## Design

### Locking Strategy

A single `std::shared_mutex` per container instance. Read operations
(`empty`, `size`, `contains`, `find`) acquire a shared (read) lock. Write
operations (`insert`, `erase`, `clear`, `operator[]`) acquire an exclusive
(write) lock. Copy construction acquires a shared lock on the source. Move
construction/assignment acquire exclusive locks on both sides.

### No Iterator Exposure

`begin()`/`end()` are not exposed. Iterators returned by `find()` are valid
only while the caller holds no other lock on the container — the caller must
consume the iterator immediately and not hold it across other operations. This
is the standard caveat for coarse-grained thread-safe containers.

### Files

| File | Purpose |
|------|---------|
| `ts_mtl/src/ts_hash_set.cppm` | `export module ts_mtl.hash_set;` — `ts_hash_set<T, Hash, KeyEqual, Alloc>` |
| `ts_mtl/src/ts_hash_map.cppm` | `export module ts_mtl.hash_map;` — `ts_hash_map<K, V, Hash, KeyEqual, Alloc>` |
| `ts_mtl/src/ts_mtl.cppm` | Add `export import ts_mtl.hash_set;` and `export import ts_mtl.hash_map;` |
| `ts_mtl/tests/src/test_ts_hash_set.cpp` | GTest tests for ts_hash_set |
| `ts_mtl/tests/src/test_ts_hash_map.cpp` | GTest tests for ts_hash_map |
| `ts_mtl/tests/CMakeLists.txt` | Add test executables |

### ts_hash_set Interface

```cpp
template <typename T, typename Hash = std::hash<T>,
          typename KeyEqual = std::equal_to<T>,
          typename Alloc = std::allocator<T>>
class ts_hash_set {
    // Constructors
    ts_hash_set();                          // default
    ts_hash_set(const self_t&);             // copy — shared_lock on src
    ts_hash_set(self_t&&) noexcept;         // move — exclusive lock on src
    ~ts_hash_set() noexcept;

    // Assignment — copy deleted, move supported
    self_t& operator=(const self_t&) = delete;
    self_t& operator=(self_t&&) noexcept;

    // Read — shared lock
    [[nodiscard]] bool empty() const;
    [[nodiscard]] size_t size() const;
    [[nodiscard]] bool contains(const T&) const;
    iterator find(const T&);                // shared lock
    const_iterator find(const T&) const;    // shared lock

    // Write — exclusive lock
    void clear();
    bool insert(const T&);
    size_t erase(const T&);
    bool erase(iterator);
};
```

### ts_hash_map Interface

```cpp
template <typename K, typename V, typename Hash = std::hash<K>,
          typename KeyEqual = std::equal_to<K>,
          typename Alloc = std::allocator<pair<K, V>>>
class ts_hash_map {
    // Constructors
    ts_hash_map();
    ts_hash_map(const self_t&);
    ts_hash_map(self_t&&) noexcept;
    ~ts_hash_map() noexcept;

    // Assignment
    self_t& operator=(const self_t&) = delete;
    self_t& operator=(self_t&&) noexcept;

    // Read — shared lock
    [[nodiscard]] bool empty() const;
    [[nodiscard]] size_t size() const;
    [[nodiscard]] bool contains(const K&) const;
    iterator find(const K&);
    const_iterator find(const K&) const;
    const V& at(const K&) const;            // shared lock

    // Write — exclusive lock
    void clear();
    template <typename Key, typename Val> bool insert(Key&&, Val&&);
    size_t erase(const K&);
    bool erase(iterator);
    V& operator[](const K&);                // may insert, exclusive lock
    V& operator[](K&&);                     // may insert, exclusive lock
    V& at(const K&);                        // may throw, exclusive lock
};
```

### Testing

Single-threaded tests mirror the existing `test_ts_queue` pattern:
- Default construction, empty/size invariants
- Insert/find/contains/erase cycles
- Copy and move construction/assignment
- String and large-N stress

Multi-threaded tests:
- Concurrent inserts (N threads, M inserts each, verify count)
- Concurrent reads during insert (reader threads spin on `contains`/`size`)
- Concurrent erase workload
- Producer-consumer pattern with insert+find+erase

### Code Conventions

Follow existing `ts_mtl` patterns:
- Use `std::shared_lock` for read, `std::lock_guard` for write
- `operator=(const&) = delete` matching ts_queue/ts_stack
- Delegate all work to the underlying container after acquiring the lock
- No `#include` directives — pure module imports only (no global module fragment)
- Comment style matches existing containers (Doxygen `@brief`, `@tparam`, `@param`)
