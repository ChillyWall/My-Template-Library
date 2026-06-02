# ts_hash_set and ts_hash_map Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add `ts_hash_set` and `ts_hash_map` thread-safe wrappers (shared_mutex) to ts_mtl.

**Architecture:** Two new `.cppm` module files, each wrapping the existing `mtl::hash_set`/`mtl::hash_map` with a `std::shared_mutex`. Read ops take shared locks, write ops take exclusive locks. No iterators exposed for traversal — only `find()` which returns an ephemeral iterator.

**Tech Stack:** C++23 modules, `std::shared_mutex`, `std::shared_lock`, `std::lock_guard`, GTest, CMake

---

### Task 1: Create ts_hash_set module

**Files:**
- Create: `ts_mtl/src/ts_hash_set.cppm`
- Modify: `ts_mtl/src/ts_mtl.cppm`

- [ ] **Step 1: Write ts_hash_set.cppm**

```cpp
export module ts_mtl.hash_set;

import mtl.hash_set;
import std;

export namespace mtl {

/**
 * @brief Thread-safe unordered unique-key set backed by a hopscotch hash
 *        table, protected by a shared mutex for read/write locking.
 *
 * Read operations (empty, size, contains, find) acquire a shared lock.
 * Write operations (insert, erase, clear) acquire an exclusive lock.
 *
 * @tparam T        Element type.
 * @tparam Hash     Hash function type. Defaults to std::hash<T>.
 * @tparam KeyEqual Equality comparison type. Defaults to std::equal_to<T>.
 * @tparam Alloc    Allocator type.
 */
template <typename T, typename Hash = std::hash<T>,
          typename KeyEqual = std::equal_to<T>,
          typename Alloc = std::allocator<T>>
class ts_hash_set {
public:
    using self_t = ts_hash_set<T, Hash, KeyEqual, Alloc>;

private:
    using inner_t = hash_set<T, Hash, KeyEqual, Alloc>;
    inner_t data_;
    mutable std::shared_mutex mtx_;

public:
    using iterator = typename inner_t::iterator;
    using const_iterator = typename inner_t::const_iterator;

    ts_hash_set() = default;

    ts_hash_set(const self_t& rhs) {
        std::shared_lock lock(rhs.mtx_);
        data_ = rhs.data_;
    }

    ts_hash_set(self_t&& rhs) noexcept {
        std::lock_guard lock(rhs.mtx_);
        data_ = std::move(rhs.data_);
    }

    ~ts_hash_set() noexcept = default;

    self_t& operator=(const self_t& rhs) = delete;

    self_t& operator=(self_t&& rhs) noexcept {
        if (this != &rhs) {
            std::unique_lock lock_this(mtx_, std::defer_lock);
            std::unique_lock lock_rhs(rhs.mtx_, std::defer_lock);
            std::lock(lock_this, lock_rhs);

            data_ = std::move(rhs.data_);
        }
        return *this;
    }

    [[nodiscard]] bool empty() const {
        std::shared_lock lock(mtx_);
        return data_.empty();
    }

    [[nodiscard]] size_t size() const {
        std::shared_lock lock(mtx_);
        return data_.size();
    }

    void clear() {
        std::lock_guard lock(mtx_);
        data_.clear();
    }

    bool insert(const T& elem) {
        std::lock_guard lock(mtx_);
        return data_.insert(elem);
    }

    size_t erase(const T& elem) {
        std::lock_guard lock(mtx_);
        return data_.erase(elem);
    }

    bool erase(iterator itr) {
        std::lock_guard lock(mtx_);
        return data_.erase(itr);
    }

    iterator find(const T& elem) {
        std::shared_lock lock(mtx_);
        return data_.find(elem);
    }

    const_iterator find(const T& elem) const {
        std::shared_lock lock(mtx_);
        return data_.find(elem);
    }

    [[nodiscard]] bool contains(const T& elem) const {
        std::shared_lock lock(mtx_);
        return data_.contains(elem);
    }
};

}  // namespace mtl
```

- [ ] **Step 2: Update ts_mtl.cppm to export the new module**

In `ts_mtl/src/ts_mtl.cppm`, add two lines after the existing `export import ts_mtl.stack;`:

```cpp
export import ts_mtl.hash_set;
export import ts_mtl.hash_map;
```

- [ ] **Step 3: Build to verify compilation**

```bash
cmake --build --preset clang-debug
```

Expected: Build succeeds with no errors.

- [ ] **Step 4: Commit**

```bash
git add ts_mtl/src/ts_hash_set.cppm ts_mtl/src/ts_mtl.cppm
git commit -m "feat(ts_mtl): add ts_hash_set thread-safe wrapper"
```

---

### Task 2: Create ts_hash_map module

**Files:**
- Create: `ts_mtl/src/ts_hash_map.cppm`

- [ ] **Step 1: Write ts_hash_map.cppm**

```cpp
export module ts_mtl.hash_map;

import mtl.hash_map;
import mtl.core;  // for EmptyContainer exceptions
import std;

export namespace mtl {

/**
 * @brief Thread-safe unordered key-value container backed by a hopscotch
 *        hash table, protected by a shared mutex for read/write locking.
 *
 * Read operations (empty, size, contains, find, at-const) acquire a shared
 * lock. Write operations (insert, erase, clear, operator[], at-non-const)
 * acquire an exclusive lock.
 *
 * @tparam K        Key type.
 * @tparam V        Mapped value type.
 * @tparam Hash     Hash function type for keys. Defaults to std::hash<K>.
 * @tparam KeyEqual Equality comparison type for keys. Defaults to
 *                  std::equal_to<K>.
 * @tparam Alloc    Allocator type.
 */
template <typename K, typename V, typename Hash = std::hash<K>,
          typename KeyEqual = std::equal_to<K>,
          typename Alloc = std::allocator<pair<K, V>>>
class ts_hash_map {
public:
    using key_type = K;
    using mapped_type = V;
    using self_t = ts_hash_map<K, V, Hash, KeyEqual, Alloc>;

private:
    using inner_t = hash_map<K, V, Hash, KeyEqual, Alloc>;
    inner_t data_;
    mutable std::shared_mutex mtx_;

public:
    using iterator = typename inner_t::iterator;
    using const_iterator = typename inner_t::const_iterator;

    ts_hash_map() = default;

    ts_hash_map(const self_t& rhs) {
        std::shared_lock lock(rhs.mtx_);
        data_ = rhs.data_;
    }

    ts_hash_map(self_t&& rhs) noexcept {
        std::lock_guard lock(rhs.mtx_);
        data_ = std::move(rhs.data_);
    }

    ~ts_hash_map() noexcept = default;

    self_t& operator=(const self_t& rhs) = delete;

    self_t& operator=(self_t&& rhs) noexcept {
        if (this != &rhs) {
            std::unique_lock lock_this(mtx_, std::defer_lock);
            std::unique_lock lock_rhs(rhs.mtx_, std::defer_lock);
            std::lock(lock_this, lock_rhs);

            data_ = std::move(rhs.data_);
        }
        return *this;
    }

    [[nodiscard]] bool empty() const {
        std::shared_lock lock(mtx_);
        return data_.empty();
    }

    [[nodiscard]] size_t size() const {
        std::shared_lock lock(mtx_);
        return data_.size();
    }

    void clear() {
        std::lock_guard lock(mtx_);
        data_.clear();
    }

    template <typename Key, typename Val>
    bool insert(Key&& key, Val&& val) {
        std::lock_guard lock(mtx_);
        return data_.insert(std::forward<Key>(key), std::forward<Val>(val));
    }

    size_t erase(const K& key) {
        std::lock_guard lock(mtx_);
        return data_.erase(key);
    }

    bool erase(iterator itr) {
        std::lock_guard lock(mtx_);
        return data_.erase(itr);
    }

    iterator find(const K& key) {
        std::shared_lock lock(mtx_);
        return data_.find(key);
    }

    const_iterator find(const K& key) const {
        std::shared_lock lock(mtx_);
        return data_.find(key);
    }

    [[nodiscard]] bool contains(const K& key) const {
        std::shared_lock lock(mtx_);
        return data_.contains(key);
    }

    /**
     * @brief Access mapped value by key, inserting a default-constructed
     *        value if the key does not exist. Acquires an exclusive lock.
     */
    V& operator[](const K& key) {
        std::lock_guard lock(mtx_);
        return data_[key];
    }

    /**
     * @brief Access mapped value by key, inserting a default-constructed
     *        value if the key does not exist. Acquires an exclusive lock.
     */
    V& operator[](K&& key) {
        std::lock_guard lock(mtx_);
        return data_[std::move(key)];
    }

    /**
     * @brief Access mapped value by key.
     * @throws EmptyContainer if key not found.
     */
    V& at(const K& key) {
        std::lock_guard lock(mtx_);
        return data_.at(key);
    }

    /**
     * @brief Access mapped value by key (const).
     * @throws EmptyContainer if key not found.
     */
    const V& at(const K& key) const {
        std::shared_lock lock(mtx_);
        return data_.at(key);
    }
};

}  // namespace mtl
```

- [ ] **Step 2: Build to verify compilation**

```bash
cmake --build --preset clang-debug
```

Expected: Build succeeds with no errors.

- [ ] **Step 3: Commit**

```bash
git add ts_mtl/src/ts_hash_map.cppm
git commit -m "feat(ts_mtl): add ts_hash_map thread-safe wrapper"
```

---

### Task 3: Add test for ts_hash_set (single-threaded)

**Files:**
- Create: `ts_mtl/tests/src/test_ts_hash_set.cpp`
- Modify: `ts_mtl/tests/CMakeLists.txt`

- [ ] **Step 1: Write the test file**

```cpp
#include <gtest/gtest.h>

import std;
import mtl.core;
import ts_mtl.hash_set;

using mtl::ts_hash_set;

// ============================================================
// Single-threaded basic functionality tests
// ============================================================

TEST(TestTSHashSetConstructor, DefaultConstructor) {
    ts_hash_set<int> s;
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0);
    EXPECT_FALSE(s.contains(0));
    EXPECT_EQ(s.find(0), s.end());
}

TEST(TestTSHashSetOperation, InsertAndContains) {
    ts_hash_set<int> s;
    EXPECT_TRUE(s.insert(1));
    EXPECT_TRUE(s.insert(2));
    EXPECT_TRUE(s.insert(3));
    EXPECT_FALSE(s.insert(1));  // duplicate

    EXPECT_EQ(s.size(), 3);
    EXPECT_FALSE(s.empty());
    EXPECT_TRUE(s.contains(1));
    EXPECT_TRUE(s.contains(2));
    EXPECT_TRUE(s.contains(3));
    EXPECT_FALSE(s.contains(0));
}

TEST(TestTSHashSetOperation, InsertAndFind) {
    ts_hash_set<int> s;
    s.insert(42);
    s.insert(100);

    auto it = s.find(42);
    EXPECT_NE(it, s.end());
    EXPECT_EQ(*it, 42);

    auto cit = s.find(100);
    EXPECT_NE(cit, s.end());
    EXPECT_EQ(*cit, 100);

    EXPECT_EQ(s.find(0), s.end());
}

TEST(TestTSHashSetOperation, EraseByValue) {
    ts_hash_set<int> s;
    s.insert(10);
    s.insert(20);
    EXPECT_EQ(s.size(), 2);

    EXPECT_EQ(s.erase(10), 1);
    EXPECT_EQ(s.size(), 1);
    EXPECT_FALSE(s.contains(10));
    EXPECT_TRUE(s.contains(20));

    // Erase non-existent
    EXPECT_EQ(s.erase(99), 0);
    EXPECT_EQ(s.size(), 1);
}

TEST(TestTSHashSetOperation, EraseByIterator) {
    ts_hash_set<int> s;
    s.insert(1);
    s.insert(2);

    auto it = s.find(1);
    EXPECT_TRUE(s.erase(it));
    EXPECT_EQ(s.size(), 1);
    EXPECT_FALSE(s.contains(1));
}

TEST(TestTSHashSetOperation, Clear) {
    ts_hash_set<int> s;
    s.insert(1);
    s.insert(2);
    s.insert(3);
    EXPECT_EQ(s.size(), 3);

    s.clear();
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0);
    EXPECT_FALSE(s.contains(1));
}

TEST(TestTSHashSetConstructor, CopyConstructor) {
    ts_hash_set<int> s1;
    s1.insert(10);
    s1.insert(20);
    s1.insert(30);

    ts_hash_set<int> s2(s1);
    EXPECT_EQ(s2.size(), 3);
    EXPECT_TRUE(s2.contains(10));
    EXPECT_TRUE(s2.contains(20));
    EXPECT_TRUE(s2.contains(30));

    // Original unchanged
    EXPECT_EQ(s1.size(), 3);
    EXPECT_FALSE(s1.empty());
}

TEST(TestTSHashSetConstructor, MoveConstructor) {
    ts_hash_set<int> s1;
    s1.insert(100);
    s1.insert(200);

    ts_hash_set<int> s2(std::move(s1));
    EXPECT_EQ(s2.size(), 2);
    EXPECT_TRUE(s2.contains(100));
    EXPECT_TRUE(s2.contains(200));
}

TEST(TestTSHashSetOperation, MoveAssignment) {
    ts_hash_set<int> s1;
    s1.insert(1);
    s1.insert(2);

    ts_hash_set<int> s2;
    s2 = std::move(s1);
    EXPECT_EQ(s2.size(), 2);
    EXPECT_TRUE(s2.contains(1));
    EXPECT_TRUE(s2.contains(2));
}

TEST(TestTSHashSetOperation, StringType) {
    ts_hash_set<std::string> s;
    EXPECT_TRUE(s.insert(std::string("hello")));
    EXPECT_TRUE(s.insert(std::string("world")));
    EXPECT_EQ(s.size(), 2);
    EXPECT_TRUE(s.contains("hello"));
    EXPECT_TRUE(s.contains("world"));
    EXPECT_FALSE(s.contains("nope"));

    EXPECT_EQ(s.erase("hello"), 1);
    EXPECT_EQ(s.size(), 1);
    EXPECT_FALSE(s.contains("hello"));
}

TEST(TestTSHashSetOperation, LargeNumberOfElements) {
    ts_hash_set<int> s;
    constexpr int N = 10000;
    for (int i = 0; i < N; ++i) {
        s.insert(i);
    }
    EXPECT_EQ(s.size(), static_cast<size_t>(N));
    for (int i = 0; i < N; ++i) {
        EXPECT_TRUE(s.contains(i));
    }
    for (int i = 0; i < N; ++i) {
        s.erase(i);
    }
    EXPECT_TRUE(s.empty());
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
```

- [ ] **Step 2: Add test executable to CMakeLists.txt**

In `ts_mtl/tests/CMakeLists.txt`, add after the existing `test_ts_deque` block:

```cmake
add_executable(test_ts_hash_set src/test_ts_hash_set.cpp)
add_test(NAME test_ts_hash_set COMMAND test_ts_hash_set)
```

- [ ] **Step 3: Build and run tests**

```bash
cmake --build --preset clang-debug
./build/clang-debug/ts_mtl/tests/test_ts_hash_set
```

Expected: All tests pass.

- [ ] **Step 4: Commit**

```bash
git add ts_mtl/tests/src/test_ts_hash_set.cpp ts_mtl/tests/CMakeLists.txt
git commit -m "test(ts_mtl): add single-threaded tests for ts_hash_set"
```

---

### Task 4: Add test for ts_hash_map (single-threaded)

**Files:**
- Create: `ts_mtl/tests/src/test_ts_hash_map.cpp`
- Modify: `ts_mtl/tests/CMakeLists.txt`

- [ ] **Step 1: Write the test file**

```cpp
#include <gtest/gtest.h>

import std;
import mtl.core;
import ts_mtl.hash_map;

using mtl::ts_hash_map;

// ============================================================
// Single-threaded basic functionality tests
// ============================================================

TEST(TestTSHashMapConstructor, DefaultConstructor) {
    ts_hash_map<int, std::string> m;
    EXPECT_TRUE(m.empty());
    EXPECT_EQ(m.size(), 0);
    EXPECT_FALSE(m.contains(0));
    EXPECT_EQ(m.find(0), m.end());
}

TEST(TestTSHashMapOperation, InsertAndFind) {
    ts_hash_map<int, std::string> m;
    EXPECT_TRUE(m.insert(1, std::string("one")));
    EXPECT_TRUE(m.insert(2, std::string("two")));
    EXPECT_FALSE(m.insert(1, std::string("again")));  // duplicate key

    EXPECT_EQ(m.size(), 2);

    auto it = m.find(1);
    EXPECT_NE(it, m.end());
    EXPECT_EQ(it->first, 1);
    EXPECT_EQ(it->second, "one");

    EXPECT_FALSE(m.contains(0));
    EXPECT_TRUE(m.contains(2));
}

TEST(TestTSHashMapOperation, OperatorBracket) {
    ts_hash_map<int, std::string> m;

    // Insert via operator[]
    m[1] = "one";
    EXPECT_EQ(m.size(), 1);
    EXPECT_EQ(m[1], "one");

    // Overwrite
    m[1] = "ONE";
    EXPECT_EQ(m[1], "ONE");

    // Default insert
    EXPECT_EQ(m[2], "");  // default-constructed string
    EXPECT_EQ(m.size(), 2);
}

TEST(TestTSHashMapOperation, At) {
    ts_hash_map<int, std::string> m;
    m.insert(1, std::string("hello"));

    EXPECT_EQ(m.at(1), "hello");

    // Non-existent should throw
    EXPECT_THROW(m.at(2), mtl::EmptyContainer);

    // Const at on const map
    const auto& cm = m;
    EXPECT_EQ(cm.at(1), "hello");
    EXPECT_THROW(cm.at(99), mtl::EmptyContainer);
}

TEST(TestTSHashMapOperation, EraseByKey) {
    ts_hash_map<int, std::string> m;
    m.insert(10, std::string("ten"));
    m.insert(20, std::string("twenty"));
    EXPECT_EQ(m.size(), 2);

    EXPECT_EQ(m.erase(10), 1);
    EXPECT_EQ(m.size(), 1);
    EXPECT_FALSE(m.contains(10));

    // Erase non-existent
    EXPECT_EQ(m.erase(99), 0);
}

TEST(TestTSHashMapOperation, EraseByIterator) {
    ts_hash_map<int, std::string> m;
    m.insert(1, std::string("one"));

    auto it = m.find(1);
    EXPECT_TRUE(m.erase(it));
    EXPECT_EQ(m.size(), 0);
    EXPECT_FALSE(m.contains(1));
}

TEST(TestTSHashMapOperation, Clear) {
    ts_hash_map<int, int> m;
    m.insert(1, 10);
    m.insert(2, 20);
    EXPECT_EQ(m.size(), 2);

    m.clear();
    EXPECT_TRUE(m.empty());
    EXPECT_EQ(m.size(), 0);
}

TEST(TestTSHashMapConstructor, CopyConstructor) {
    ts_hash_map<int, std::string> m1;
    m1.insert(1, std::string("a"));
    m1.insert(2, std::string("b"));

    ts_hash_map<int, std::string> m2(m1);
    EXPECT_EQ(m2.size(), 2);
    EXPECT_EQ(m2[1], "a");
    EXPECT_EQ(m2[2], "b");

    // Original unchanged
    EXPECT_EQ(m1.size(), 2);
}

TEST(TestTSHashMapConstructor, MoveConstructor) {
    ts_hash_map<int, int> m1;
    m1.insert(1, 100);

    ts_hash_map<int, int> m2(std::move(m1));
    EXPECT_EQ(m2.size(), 1);
    EXPECT_EQ(m2[1], 100);
}

TEST(TestTSHashMapOperation, MoveAssignment) {
    ts_hash_map<int, int> m1;
    m1.insert(1, 42);

    ts_hash_map<int, int> m2;
    m2 = std::move(m1);
    EXPECT_EQ(m2.size(), 1);
    EXPECT_EQ(m2[1], 42);
}

TEST(TestTSHashMapOperation, MoveKeyOperatorBracket) {
    ts_hash_map<std::string, int> m;
    std::string key = "hello";
    m.insert(key, 42);

    // operator[] with rvalue key
    m[std::string("world")] = 100;
    EXPECT_EQ(m.size(), 2);
    EXPECT_EQ(m["hello"], 42);
    EXPECT_EQ(m["world"], 100);
}

TEST(TestTSHashMapOperation, LargeNumberOfElements) {
    ts_hash_map<int, int> m;
    constexpr int N = 10000;
    for (int i = 0; i < N; ++i) {
        m.insert(i, i * 10);
    }
    EXPECT_EQ(m.size(), static_cast<size_t>(N));
    for (int i = 0; i < N; ++i) {
        EXPECT_TRUE(m.contains(i));
        EXPECT_EQ(m[i], i * 10);
    }
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
```

- [ ] **Step 2: Add test executable to CMakeLists.txt**

In `ts_mtl/tests/CMakeLists.txt`, add after the `test_ts_hash_set` block from Task 3:

```cmake
add_executable(test_ts_hash_map src/test_ts_hash_map.cpp)
add_test(NAME test_ts_hash_map COMMAND test_ts_hash_map)
```

- [ ] **Step 3: Build and run tests**

```bash
cmake --build --preset clang-debug
./build/clang-debug/ts_mtl/tests/test_ts_hash_map
```

Expected: All tests pass.

- [ ] **Step 4: Commit**

```bash
git add ts_mtl/tests/src/test_ts_hash_map.cpp ts_mtl/tests/CMakeLists.txt
git commit -m "test(ts_mtl): add single-threaded tests for ts_hash_map"
```

---

### Task 5: Add multi-threaded tests for ts_hash_set

**Files:**
- Modify: `ts_mtl/tests/src/test_ts_hash_set.cpp` (append multi-threaded tests)

- [ ] **Step 1: Append multi-threaded test code**

Append the following before `int main()` at the end of `test_ts_hash_set.cpp`:

```cpp
// ============================================================
// Multi-threaded tests
// ============================================================

TEST(TestTSHashSetThreaded, ConcurrentInsert) {
    ts_hash_set<int> s;
    constexpr int THREADS = 4;
    constexpr int INSERTS_PER_THREAD = 2500;
    std::vector<std::thread> threads;

    for (int t = 0; t < THREADS; ++t) {
        threads.emplace_back([&s, t]() {
            int base = t * INSERTS_PER_THREAD;
            for (int i = 0; i < INSERTS_PER_THREAD; ++i) {
                s.insert(base + i);
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    EXPECT_EQ(s.size(), static_cast<size_t>(THREADS * INSERTS_PER_THREAD));
    // Spot-check some values
    EXPECT_TRUE(s.contains(0));
    EXPECT_TRUE(s.contains(INSERTS_PER_THREAD));
    EXPECT_TRUE(s.contains(THREADS * INSERTS_PER_THREAD - 1));
}

TEST(TestTSHashSetThreaded, ConcurrentContains) {
    ts_hash_set<int> s;
    constexpr int N = 5000;
    for (int i = 0; i < N; ++i) {
        s.insert(i);
    }

    std::atomic<int> found {0};
    constexpr int THREADS = 4;
    std::vector<std::thread> threads;

    for (int t = 0; t < THREADS; ++t) {
        threads.emplace_back([&s, &found, t]() {
            int base = t * (N / THREADS);
            int end = base + (N / THREADS);
            for (int i = base; i < end; ++i) {
                if (s.contains(i)) {
                    found.fetch_add(1, std::memory_order_relaxed);
                }
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    EXPECT_EQ(found.load(), N);
}

TEST(TestTSHashSetThreaded, ConcurrentInsertAndErase) {
    ts_hash_set<int> s;
    constexpr int N = 2000;
    std::atomic<int> inserted {0};
    std::atomic<int> erased {0};
    std::vector<std::thread> threads;

    // Producer threads
    for (int t = 0; t < 2; ++t) {
        threads.emplace_back([&s, &inserted, t]() {
            int base = t * N;
            for (int i = 0; i < N; ++i) {
                s.insert(base + i);
                inserted.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    // Wait for producers to finish
    for (auto& th : threads) {
        th.join();
    }
    threads.clear();

    // Eraser threads
    for (int t = 0; t < 2; ++t) {
        threads.emplace_back([&s, &erased, t]() {
            int base = t * N;
            for (int i = 0; i < N; ++i) {
                size_t n = s.erase(base + i);
                erased.fetch_add(n, std::memory_order_relaxed);
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    EXPECT_EQ(erased.load(), inserted.load());
    EXPECT_TRUE(s.empty());
}

TEST(TestTSHashSetThreaded, ConcurrentFind) {
    ts_hash_set<int> s;
    constexpr int N = 1000;
    for (int i = 0; i < N; ++i) {
        s.insert(i);
    }

    std::atomic<int> found_count {0};
    constexpr int THREADS = 4;
    std::vector<std::thread> threads;

    for (int t = 0; t < THREADS; ++t) {
        threads.emplace_back([&s, &found_count]() {
            for (int i = 0; i < N; ++i) {
                auto it = s.find(i);
                if (it != s.end()) {
                    found_count.fetch_add(1, std::memory_order_relaxed);
                }
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    EXPECT_EQ(found_count.load(), THREADS * N);
}
```

- [ ] **Step 2: Build and run tests**

```bash
cmake --build --preset clang-debug
./build/clang-debug/ts_mtl/tests/test_ts_hash_set
```

Expected: All tests (single-threaded + multi-threaded) pass.

- [ ] **Step 3: Commit**

```bash
git add ts_mtl/tests/src/test_ts_hash_set.cpp
git commit -m "test(ts_mtl): add multi-threaded tests for ts_hash_set"
```

---

### Task 6: Add multi-threaded tests for ts_hash_map

**Files:**
- Modify: `ts_mtl/tests/src/test_ts_hash_map.cpp` (append multi-threaded tests)

- [ ] **Step 1: Append multi-threaded test code**

Append the following before `int main()` at the end of `test_ts_hash_map.cpp`:

```cpp
// ============================================================
// Multi-threaded tests
// ============================================================

TEST(TestTSHashMapThreaded, ConcurrentInsert) {
    ts_hash_map<int, int> m;
    constexpr int THREADS = 4;
    constexpr int INSERTS_PER_THREAD = 2500;
    std::vector<std::thread> threads;

    for (int t = 0; t < THREADS; ++t) {
        threads.emplace_back([&m, t]() {
            int base = t * INSERTS_PER_THREAD;
            for (int i = 0; i < INSERTS_PER_THREAD; ++i) {
                m.insert(base + i, base + i * 10);
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    EXPECT_EQ(m.size(), static_cast<size_t>(THREADS * INSERTS_PER_THREAD));
    EXPECT_TRUE(m.contains(0));
    EXPECT_EQ(m[0], 0);
    EXPECT_TRUE(m.contains(INSERTS_PER_THREAD));
}

TEST(TestTSHashMapThreaded, ConcurrentContains) {
    ts_hash_map<int, int> m;
    constexpr int N = 5000;
    for (int i = 0; i < N; ++i) {
        m.insert(i, i * 10);
    }

    std::atomic<int> found {0};
    constexpr int THREADS = 4;
    std::vector<std::thread> threads;

    for (int t = 0; t < THREADS; ++t) {
        threads.emplace_back([&m, &found, t]() {
            int base = t * (N / THREADS);
            int end = base + (N / THREADS);
            for (int i = base; i < end; ++i) {
                if (m.contains(i)) {
                    found.fetch_add(1, std::memory_order_relaxed);
                }
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    EXPECT_EQ(found.load(), N);
}

TEST(TestTSHashMapThreaded, ConcurrentFind) {
    ts_hash_map<int, int> m;
    constexpr int N = 1000;
    for (int i = 0; i < N; ++i) {
        m.insert(i, i * 10);
    }

    std::atomic<int> correct {0};
    constexpr int THREADS = 4;
    std::vector<std::thread> threads;

    for (int t = 0; t < THREADS; ++t) {
        threads.emplace_back([&m, &correct]() {
            for (int i = 0; i < N; ++i) {
                auto it = m.find(i);
                if (it != m.end() && it->second == i * 10) {
                    correct.fetch_add(1, std::memory_order_relaxed);
                }
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    EXPECT_EQ(correct.load(), THREADS * N);
}

TEST(TestTSHashMapThreaded, ConcurrentErase) {
    ts_hash_map<int, int> m;
    constexpr int N = 2000;
    for (int i = 0; i < N; ++i) {
        m.insert(i, i * 10);
    }

    std::atomic<int> erased {0};
    constexpr int THREADS = 4;
    std::vector<std::thread> threads;

    for (int t = 0; t < THREADS; ++t) {
        threads.emplace_back([&m, &erased, t]() {
            int base = t * (N / THREADS);
            int end = base + (N / THREADS);
            for (int i = base; i < end; ++i) {
                size_t n = m.erase(i);
                erased.fetch_add(n, std::memory_order_relaxed);
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    EXPECT_EQ(erased.load(), N);
    EXPECT_TRUE(m.empty());
}

TEST(TestTSHashMapThreaded, ConcurrentOperatorBracket) {
    ts_hash_map<int, int> m;
    constexpr int THREADS = 4;
    constexpr int OPS = 1000;
    std::atomic<int> total {0};
    std::vector<std::thread> threads;

    for (int t = 0; t < THREADS; ++t) {
        threads.emplace_back([&m, &total, t]() {
            int base = t * OPS;
            for (int i = 0; i < OPS; ++i) {
                m[base + i] = i;  // insert or update
                total.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    // Each thread inserts its own key range, so total keys == OPS * THREADS
    EXPECT_EQ(m.size(), static_cast<size_t>(THREADS * OPS));
    EXPECT_EQ(total.load(), THREADS * OPS);
}
```

- [ ] **Step 2: Build and run tests**

```bash
cmake --build --preset clang-debug
./build/clang-debug/ts_mtl/tests/test_ts_hash_map
```

Expected: All tests (single-threaded + multi-threaded) pass.

- [ ] **Step 3: Commit**

```bash
git add ts_mtl/tests/src/test_ts_hash_map.cpp
git commit -m "test(ts_mtl): add multi-threaded tests for ts_hash_map"
```

---

### Task 7: Final verification — run all tests

- [ ] **Step 1: Run the full test suite**

```bash
cmake --test --preset clang-test
```

Expected: All tests pass including all ts_mtl tests (ts_queue, ts_stack, ts_deque, ts_hash_set, ts_hash_map).

- [ ] **Step 2: Commit any fixes if needed**

If any fixes were needed, commit them. Otherwise the work is complete.
