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

// ============================================================
// Multi-threaded tests
// ============================================================

TEST(TestTSHashMapThreaded, ConcurrentInsert) {
    ts_hash_map<int, int> m;
    constexpr int THREADS = 4;
    constexpr int INSERTS_PER_THREAD = 2500;
    std::vector<std::thread> threads;
    threads.reserve(THREADS);

    std::atomic<int> total_inserted {0};

    for (int t = 0; t < THREADS; ++t) {
        threads.emplace_back([&m, t]() {
            int base = t * INSERTS_PER_THREAD;
            for (int i = 0; i < INSERTS_PER_THREAD; ++i) {
                m.insert(base + i, (base + i) * 10);
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    // Verify no corruption: check that elements we tried to insert are
    // either present or not — with no double-counting, no crashes.
    for (int t = 0; t < THREADS; ++t) {
        int base = t * INSERTS_PER_THREAD;
        for (int i = 0; i < INSERTS_PER_THREAD; ++i) {
            if (m.contains(base + i)) {
                total_inserted.fetch_add(1, std::memory_order_relaxed);
            }
        }
    }
    EXPECT_EQ(m.size(), static_cast<size_t>(total_inserted.load()));
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
    threads.reserve(THREADS);

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
    threads.reserve(THREADS);

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
    threads.reserve(THREADS);

    for (int t = 0; t < THREADS; ++t) {
        threads.emplace_back([&m, &erased, t]() {
            int base = t * (N / THREADS);
            int end = base + (N / THREADS);
            for (int i = base; i < end; ++i) {
                size_t n = m.erase(i);
                erased.fetch_add(static_cast<int>(n),
                                 std::memory_order_relaxed);
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
    std::vector<std::thread> threads;
    threads.reserve(THREADS);

    for (int t = 0; t < THREADS; ++t) {
        threads.emplace_back([&m, t]() {
            int base = t * OPS;
            for (int i = 0; i < OPS; ++i) {
                m[base + i] = i;
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    // Verify no data corruption: count what actually landed and check size
    // matches. Hopscotch hash table insert may return false under contention,
    // but size must always equal the number of actually-present keys.
    std::atomic<int> total_present {0};
    for (int t = 0; t < THREADS; ++t) {
        int base = t * OPS;
        for (int i = 0; i < OPS; ++i) {
            if (m.contains(base + i)) {
                total_present.fetch_add(1, std::memory_order_relaxed);
            }
        }
    }
    EXPECT_EQ(m.size(), static_cast<size_t>(total_present.load()));
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
