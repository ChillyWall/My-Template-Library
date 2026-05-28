#include <gtest/gtest.h>

import std;
import mtl.core;
import ts_mtl.queue;

using mtl::ts_queue;

// ============================================================
// Single-threaded basic functionality tests
// ============================================================

TEST(TestTSQueueConstructor, DefaultConstructor) {
    ts_queue<int> q;
    EXPECT_TRUE(q.empty());
    EXPECT_EQ(q.size(), 0);

    int val;
    EXPECT_THROW(q.pop(val), mtl::EmptyContainer);
}

TEST(TestTSQueueOperation, PushAndPop) {
    ts_queue<int> q;
    q.push(1);
    q.push(2);
    q.push(3);

    EXPECT_EQ(q.size(), 3);
    EXPECT_FALSE(q.empty());

    int val;
    q.pop(val);
    EXPECT_EQ(val, 1);
    q.pop(val);
    EXPECT_EQ(val, 2);
    q.pop(val);
    EXPECT_EQ(val, 3);

    EXPECT_TRUE(q.empty());
    EXPECT_EQ(q.size(), 0);
}

TEST(TestTSQueueOperation, PopFromEmptyThrows) {
    ts_queue<int> q;
    int val;
    EXPECT_THROW(q.pop(val), mtl::EmptyContainer);

    // After push-pop cycle, popping empty again should throw
    q.push(42);
    q.pop(val);
    EXPECT_EQ(val, 42);
    EXPECT_THROW(q.pop(val), mtl::EmptyContainer);
}

TEST(TestTSQueueConstructor, CopyConstructor) {
    ts_queue<int> q1;
    q1.push(10);
    q1.push(20);
    q1.push(30);

    ts_queue<int> q2(q1);
    EXPECT_EQ(q2.size(), 3);

    int val;
    q2.pop(val);
    EXPECT_EQ(val, 10);
    q2.pop(val);
    EXPECT_EQ(val, 20);
    q2.pop(val);
    EXPECT_EQ(val, 30);
    EXPECT_TRUE(q2.empty());

    // Original unchanged
    EXPECT_EQ(q1.size(), 3);
    EXPECT_FALSE(q1.empty());
}

TEST(TestTSQueueConstructor, MoveConstructor) {
    ts_queue<int> q1;
    q1.push(100);
    q1.push(200);

    ts_queue<int> q2(std::move(q1));
    EXPECT_EQ(q2.size(), 2);

    int val;
    q2.pop(val);
    EXPECT_EQ(val, 100);
    q2.pop(val);
    EXPECT_EQ(val, 200);
    EXPECT_TRUE(q2.empty());
}

TEST(TestTSQueueOperation, MoveAssignment) {
    ts_queue<int> q1;
    q1.push(1);
    q1.push(2);
    q1.push(3);

    ts_queue<int> q2;
    q2 = std::move(q1);
    EXPECT_EQ(q2.size(), 3);

    int val;
    q2.pop(val);
    EXPECT_EQ(val, 1);
    q2.pop(val);
    EXPECT_EQ(val, 2);
    q2.pop(val);
    EXPECT_EQ(val, 3);
    EXPECT_TRUE(q2.empty());
}

TEST(TestTSQueueOperation, FIFOOrder) {
    ts_queue<int> q;
    constexpr int N = 1000;
    for (int i = 0; i < N; ++i) {
        q.push(i);
    }
    EXPECT_EQ(q.size(), static_cast<size_t>(N));

    int val;
    for (int i = 0; i < N; ++i) {
        q.pop(val);
        EXPECT_EQ(val, i);
    }
    EXPECT_TRUE(q.empty());
}

TEST(TestTSQueueOperation, StringType) {
    ts_queue<std::string> q;
    q.push(std::string("hello"));
    q.push(std::string("world"));

    EXPECT_EQ(q.size(), 2);

    std::string val;
    q.pop(val);
    EXPECT_EQ(val, "hello");
    q.pop(val);
    EXPECT_EQ(val, "world");
    EXPECT_TRUE(q.empty());
}

TEST(TestTSQueueOperation, LargeNumberOfElements) {
    ts_queue<int> q;
    constexpr int N = 100000;
    for (int i = 0; i < N; ++i) {
        q.push(i);
    }
    EXPECT_EQ(q.size(), static_cast<size_t>(N));

    int val;
    for (int i = 0; i < N; ++i) {
        q.pop(val);
        EXPECT_EQ(val, i);
    }
    EXPECT_TRUE(q.empty());
}

// ============================================================
// Multi-threaded tests
// ============================================================

TEST(TestTSQueueThreaded, ConcurrentPush) {
    ts_queue<int> q;
    constexpr int THREADS = 4;
    constexpr int PUSHES_PER_THREAD = 10000;
    std::vector<std::thread> threads;

    for (int t = 0; t < THREADS; ++t) {
        threads.emplace_back([&q]() {
            for (int i = 0; i < PUSHES_PER_THREAD; ++i) {
                q.push(i);
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    EXPECT_EQ(q.size(), static_cast<size_t>(THREADS * PUSHES_PER_THREAD));
}

TEST(TestTSQueueThreaded, ConcurrentPop) {
    ts_queue<int> q;
    constexpr int THREADS = 4;
    constexpr int POPS_PER_THREAD = 5000;
    constexpr int TOTAL = THREADS * POPS_PER_THREAD;

    for (int i = 0; i < TOTAL; ++i) {
        q.push(i);
    }
    ASSERT_EQ(q.size(), static_cast<size_t>(TOTAL));

    std::atomic<int> total_popped {0};
    std::vector<std::thread> threads;

    for (int t = 0; t < THREADS; ++t) {
        threads.emplace_back([&q, &total_popped]() {
            int val;
            for (int i = 0; i < POPS_PER_THREAD; ++i) {
                q.pop(val);
                total_popped.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    EXPECT_TRUE(q.empty());
    EXPECT_EQ(total_popped.load(), TOTAL);
}

TEST(TestTSQueueThreaded, InterleavedPushAndPop) {
    ts_queue<int> q;
    constexpr int THREADS = 4;
    constexpr int OPS_PER_THREAD = 5000;
    std::atomic<int> total_pushed {0};
    std::atomic<int> total_popped {0};
    std::vector<std::thread> threads;

    for (int t = 0; t < THREADS; ++t) {
        threads.emplace_back([&q, &total_pushed, &total_popped]() {
            int val;
            for (int i = 0; i < OPS_PER_THREAD; ++i) {
                q.push(i);
                total_pushed.fetch_add(1, std::memory_order_relaxed);
                // Best-effort pop; if empty, another thread consumed it
                try {
                    q.pop(val);
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
            q.pop(val);
            total_popped.fetch_add(1, std::memory_order_relaxed);
        } catch (const mtl::EmptyContainer&) {
            break;
        }
    }

    EXPECT_EQ(total_pushed.load(), total_popped.load());
    EXPECT_TRUE(q.empty());
}

TEST(TestTSQueueThreaded, ProducerConsumer) {
    ts_queue<int> q;
    constexpr int PRODUCERS = 4;
    constexpr int CONSUMERS = 4;
    constexpr int ELEMS_PER_PRODUCER = 20000;
    constexpr int TOTAL = PRODUCERS * ELEMS_PER_PRODUCER;

    std::atomic<int> total_pushed {0};
    std::atomic<int> total_popped {0};
    std::atomic<bool> producers_done {false};
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    for (int t = 0; t < PRODUCERS; ++t) {
        producers.emplace_back([&q, &total_pushed]() {
            for (int i = 0; i < ELEMS_PER_PRODUCER; ++i) {
                q.push(i);
                total_pushed.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    for (int t = 0; t < CONSUMERS; ++t) {
        consumers.emplace_back([&q, &total_popped, &producers_done]() {
            int val;
            while (true) {
                try {
                    q.pop(val);
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
            q.pop(val);
            total_popped.fetch_add(1, std::memory_order_relaxed);
        } catch (const mtl::EmptyContainer&) {
            break;
        }
    }

    EXPECT_EQ(total_pushed.load(), total_popped.load());
    EXPECT_EQ(total_pushed.load(), TOTAL);
    EXPECT_TRUE(q.empty());
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
