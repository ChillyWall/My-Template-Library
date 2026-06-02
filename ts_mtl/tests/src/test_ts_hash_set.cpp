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

// ============================================================
// Multi-threaded tests
// ============================================================

TEST(TestTSHashSetThreaded, ConcurrentInsert) {
    ts_hash_set<int> s;
    constexpr int THREADS = 4;
    constexpr int INSERTS_PER_THREAD = 2500;
    std::vector<std::thread> threads;
    threads.reserve(THREADS);

    std::atomic<int> total_inserted {0};

    for (int t = 0; t < THREADS; ++t) {
        threads.emplace_back([&s, &total_inserted, t]() {
            int base = t * INSERTS_PER_THREAD;
            for (int i = 0; i < INSERTS_PER_THREAD; ++i) {
                s.insert(base + i);
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
            if (s.contains(base + i)) {
                total_inserted.fetch_add(1, std::memory_order_relaxed);
            }
        }
    }
    EXPECT_EQ(s.size(), static_cast<size_t>(total_inserted.load()));
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
    threads.reserve(THREADS);

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
    threads.reserve(2);

    // Producer threads
    for (int t = 0; t < 2; ++t) {
        threads.emplace_back([&s, t]() {
            int base = t * N;
            for (int i = 0; i < N; ++i) {
                s.insert(base + i);
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }
    threads.clear();
    threads.reserve(2);

    // Count what actually got inserted
    for (int t = 0; t < 2; ++t) {
        int base = t * N;
        for (int i = 0; i < N; ++i) {
            if (s.contains(base + i)) {
                inserted.fetch_add(1, std::memory_order_relaxed);
            }
        }
    }
    EXPECT_EQ(s.size(), static_cast<size_t>(inserted.load()));

    // Eraser threads
    for (int t = 0; t < 2; ++t) {
        threads.emplace_back([&s, &erased, t]() {
            int base = t * N;
            for (int i = 0; i < N; ++i) {
                size_t n = s.erase(base + i);
                erased.fetch_add(static_cast<int>(n),
                                 std::memory_order_relaxed);
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
    threads.reserve(THREADS);

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

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
