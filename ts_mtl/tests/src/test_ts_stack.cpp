#include <gtest/gtest.h>

import std;
import mtl.core;
import ts_mtl.stack;

using mtl::ts_stack;

// ============================================================
// Single-threaded basic functionality tests
// ============================================================

TEST(TestTSStackConstructor, DefaultConstructor) {
    ts_stack<int> s;
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0);

    int val;
    EXPECT_THROW(s.pop(val), mtl::EmptyContainer);
}

TEST(TestTSStackOperation, PushAndPop) {
    ts_stack<int> s;
    s.push(1);
    s.push(2);
    s.push(3);

    EXPECT_EQ(s.size(), 3);
    EXPECT_FALSE(s.empty());

    int val;
    s.pop(val);
    EXPECT_EQ(val, 3);
    s.pop(val);
    EXPECT_EQ(val, 2);
    s.pop(val);
    EXPECT_EQ(val, 1);

    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0);
}

TEST(TestTSStackOperation, PopFromEmptyThrows) {
    ts_stack<int> s;
    int val;
    EXPECT_THROW(s.pop(val), mtl::EmptyContainer);

    // After push-pop cycle, popping empty again should throw
    s.push(42);
    s.pop(val);
    EXPECT_EQ(val, 42);
    EXPECT_THROW(s.pop(val), mtl::EmptyContainer);
}

TEST(TestTSStackConstructor, CopyConstructor) {
    ts_stack<int> s1;
    s1.push(10);
    s1.push(20);
    s1.push(30);

    ts_stack<int> s2(s1);
    EXPECT_EQ(s2.size(), 3);

    int val;
    s2.pop(val);
    EXPECT_EQ(val, 30);
    s2.pop(val);
    EXPECT_EQ(val, 20);
    s2.pop(val);
    EXPECT_EQ(val, 10);
    EXPECT_TRUE(s2.empty());

    // Original unchanged
    EXPECT_EQ(s1.size(), 3);
    EXPECT_FALSE(s1.empty());
}

TEST(TestTSStackConstructor, MoveConstructor) {
    ts_stack<int> s1;
    s1.push(100);
    s1.push(200);

    ts_stack<int> s2(std::move(s1));
    EXPECT_EQ(s2.size(), 2);

    int val;
    s2.pop(val);
    EXPECT_EQ(val, 200);
    s2.pop(val);
    EXPECT_EQ(val, 100);
    EXPECT_TRUE(s2.empty());
}

TEST(TestTSStackOperation, MoveAssignment) {
    ts_stack<int> s1;
    s1.push(1);
    s1.push(2);
    s1.push(3);

    ts_stack<int> s2;
    s2 = std::move(s1);
    EXPECT_EQ(s2.size(), 3);

    int val;
    s2.pop(val);
    EXPECT_EQ(val, 3);
    s2.pop(val);
    EXPECT_EQ(val, 2);
    s2.pop(val);
    EXPECT_EQ(val, 1);
    EXPECT_TRUE(s2.empty());
}

TEST(TestTSStackOperation, LIFOOrder) {
    ts_stack<int> s;
    constexpr int N = 1000;
    for (int i = 0; i < N; ++i) {
        s.push(i);
    }
    EXPECT_EQ(s.size(), static_cast<size_t>(N));

    int val;
    for (int i = N - 1; i >= 0; --i) {
        s.pop(val);
        EXPECT_EQ(val, i);
    }
    EXPECT_TRUE(s.empty());
}

TEST(TestTSStackOperation, StringType) {
    ts_stack<std::string> s;
    s.push(std::string("hello"));
    s.push(std::string("world"));

    EXPECT_EQ(s.size(), 2);

    std::string val;
    s.pop(val);
    EXPECT_EQ(val, "world");
    s.pop(val);
    EXPECT_EQ(val, "hello");
    EXPECT_TRUE(s.empty());
}

TEST(TestTSStackOperation, LargeNumberOfElements) {
    ts_stack<int> s;
    constexpr int N = 100000;
    for (int i = 0; i < N; ++i) {
        s.push(i);
    }
    EXPECT_EQ(s.size(), static_cast<size_t>(N));

    int val;
    for (int i = N - 1; i >= 0; --i) {
        s.pop(val);
        EXPECT_EQ(val, i);
    }
    EXPECT_TRUE(s.empty());
}

// ============================================================
// Multi-threaded tests
// ============================================================

TEST(TestTSStackThreaded, ConcurrentPush) {
    ts_stack<int> s;
    constexpr int THREADS = 4;
    constexpr int PUSHES_PER_THREAD = 10000;
    std::vector<std::thread> threads;

    for (int t = 0; t < THREADS; ++t) {
        threads.emplace_back([&s]() {
            for (int i = 0; i < PUSHES_PER_THREAD; ++i) {
                s.push(i);
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    EXPECT_EQ(s.size(), static_cast<size_t>(THREADS * PUSHES_PER_THREAD));
}

TEST(TestTSStackThreaded, ConcurrentPop) {
    ts_stack<int> s;
    constexpr int THREADS = 4;
    constexpr int POPS_PER_THREAD = 5000;
    constexpr int TOTAL = THREADS * POPS_PER_THREAD;

    for (int i = 0; i < TOTAL; ++i) {
        s.push(i);
    }
    ASSERT_EQ(s.size(), static_cast<size_t>(TOTAL));

    std::atomic<int> total_popped{0};
    std::vector<std::thread> threads;

    for (int t = 0; t < THREADS; ++t) {
        threads.emplace_back([&s, &total_popped]() {
            int val;
            for (int i = 0; i < POPS_PER_THREAD; ++i) {
                s.pop(val);
                total_popped.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    EXPECT_TRUE(s.empty());
    EXPECT_EQ(total_popped.load(), TOTAL);
}

TEST(TestTSStackThreaded, InterleavedPushAndPop) {
    ts_stack<int> s;
    constexpr int THREADS = 4;
    constexpr int OPS_PER_THREAD = 5000;
    std::atomic<int> total_pushed{0};
    std::atomic<int> total_popped{0};
    std::vector<std::thread> threads;

    for (int t = 0; t < THREADS; ++t) {
        threads.emplace_back([&s, &total_pushed, &total_popped]() {
            int val;
            for (int i = 0; i < OPS_PER_THREAD; ++i) {
                s.push(i);
                total_pushed.fetch_add(1, std::memory_order_relaxed);
                // Best-effort pop; if empty, another thread consumed it
                try {
                    s.pop(val);
                    total_popped.fetch_add(1, std::memory_order_relaxed);
                } catch (const mtl::EmptyContainer&) {
                    // Expected under contention
                }
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    // Drain remaining
    int val;
    while (true) {
        try {
            s.pop(val);
            total_popped.fetch_add(1, std::memory_order_relaxed);
        } catch (const mtl::EmptyContainer&) {
            break;
        }
    }

    EXPECT_EQ(total_pushed.load(), total_popped.load());
    EXPECT_TRUE(s.empty());
}

TEST(TestTSStackThreaded, ProducerConsumer) {
    ts_stack<int> s;
    constexpr int PRODUCERS = 4;
    constexpr int CONSUMERS = 4;
    constexpr int ELEMS_PER_PRODUCER = 20000;
    constexpr int TOTAL = PRODUCERS * ELEMS_PER_PRODUCER;

    std::atomic<int> total_pushed{0};
    std::atomic<int> total_popped{0};
    std::atomic<bool> producers_done{false};
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    for (int t = 0; t < PRODUCERS; ++t) {
        producers.emplace_back([&s, &total_pushed]() {
            for (int i = 0; i < ELEMS_PER_PRODUCER; ++i) {
                s.push(i);
                total_pushed.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    for (int t = 0; t < CONSUMERS; ++t) {
        consumers.emplace_back([&s, &total_popped, &producers_done]() {
            int val;
            while (true) {
                try {
                    s.pop(val);
                    total_popped.fetch_add(1, std::memory_order_relaxed);
                } catch (const mtl::EmptyContainer&) {
                    if (producers_done.load(std::memory_order_acquire)) {
                        break;
                    }
                    std::this_thread::yield();
                }
            }
        });
    }

    for (auto& th : producers) {
        th.join();
    }
    producers_done.store(true, std::memory_order_release);

    for (auto& th : consumers) {
        th.join();
    }

    // Drain stragglers
    int val;
    while (true) {
        try {
            s.pop(val);
            total_popped.fetch_add(1, std::memory_order_relaxed);
        } catch (const mtl::EmptyContainer&) {
            break;
        }
    }

    EXPECT_EQ(total_pushed.load(), total_popped.load());
    EXPECT_EQ(total_pushed.load(), TOTAL);
    EXPECT_TRUE(s.empty());
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
