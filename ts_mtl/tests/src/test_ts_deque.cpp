#include <gtest/gtest.h>

import std;
import mtl.core;
import ts_mtl.deque;

using mtl::ts_deque;

// ============================================================
// Single-threaded basic functionality tests
// ============================================================

TEST(TestTSDequeConstructor, DefaultConstructor) {
    ts_deque<int> d;
    EXPECT_TRUE(d.empty());
    EXPECT_EQ(d.size(), 0);

    int val;
    EXPECT_THROW(d.pop_front(val), mtl::EmptyContainer);
    EXPECT_THROW(d.pop_back(val), mtl::EmptyContainer);
}

TEST(TestTSDequeOperation, PushFrontAndPopFront) {
    ts_deque<int> d;
    d.push_front(1);
    d.push_front(2);
    d.push_front(3);

    EXPECT_EQ(d.size(), 3);
    EXPECT_FALSE(d.empty());

    int val;
    d.pop_front(val);
    EXPECT_EQ(val, 3);
    d.pop_front(val);
    EXPECT_EQ(val, 2);
    d.pop_front(val);
    EXPECT_EQ(val, 1);

    EXPECT_TRUE(d.empty());
    EXPECT_EQ(d.size(), 0);
}

TEST(TestTSDequeOperation, PushBackAndPopBack) {
    ts_deque<int> d;
    d.push_back(1);
    d.push_back(2);
    d.push_back(3);

    EXPECT_EQ(d.size(), 3);

    int val;
    d.pop_back(val);
    EXPECT_EQ(val, 3);
    d.pop_back(val);
    EXPECT_EQ(val, 2);
    d.pop_back(val);
    EXPECT_EQ(val, 1);

    EXPECT_TRUE(d.empty());
}

TEST(TestTSDequeOperation, PushFrontPopBack) {
    ts_deque<int> d;
    d.push_front(1);
    d.push_front(2);
    d.push_front(3);

    int val;
    d.pop_back(val);
    EXPECT_EQ(val, 1);
    d.pop_back(val);
    EXPECT_EQ(val, 2);
    d.pop_back(val);
    EXPECT_EQ(val, 3);
}

TEST(TestTSDequeOperation, PushBackPopFront) {
    ts_deque<int> d;
    d.push_back(1);
    d.push_back(2);
    d.push_back(3);

    int val;
    d.pop_front(val);
    EXPECT_EQ(val, 1);
    d.pop_front(val);
    EXPECT_EQ(val, 2);
    d.pop_front(val);
    EXPECT_EQ(val, 3);
}

TEST(TestTSDequeOperation, MixedOperations) {
    ts_deque<int> d;
    d.push_front(2);
    d.push_back(3);
    d.push_front(1);

    // deque: 1, 2, 3

    int val;
    d.pop_front(val);
    EXPECT_EQ(val, 1);
    d.pop_back(val);
    EXPECT_EQ(val, 3);
    d.pop_front(val);
    EXPECT_EQ(val, 2);

    EXPECT_TRUE(d.empty());
}

TEST(TestTSDequeOperation, PopFrontFromEmptyThrows) {
    ts_deque<int> d;
    int val;
    EXPECT_THROW(d.pop_front(val), mtl::EmptyContainer);

    d.push_front(42);
    d.pop_front(val);
    EXPECT_EQ(val, 42);
    EXPECT_THROW(d.pop_front(val), mtl::EmptyContainer);
}

TEST(TestTSDequeOperation, PopBackFromEmptyThrows) {
    ts_deque<int> d;
    int val;
    EXPECT_THROW(d.pop_back(val), mtl::EmptyContainer);

    d.push_back(42);
    d.pop_back(val);
    EXPECT_EQ(val, 42);
    EXPECT_THROW(d.pop_back(val), mtl::EmptyContainer);
}

TEST(TestTSDequeConstructor, CopyConstructor) {
    ts_deque<int> d1;
    d1.push_back(10);
    d1.push_back(20);
    d1.push_back(30);

    ts_deque<int> d2(d1);
    EXPECT_EQ(d2.size(), 3);

    int val;
    d2.pop_front(val);
    EXPECT_EQ(val, 10);
    d2.pop_front(val);
    EXPECT_EQ(val, 20);
    d2.pop_front(val);
    EXPECT_EQ(val, 30);
    EXPECT_TRUE(d2.empty());

    // Original unchanged
    EXPECT_EQ(d1.size(), 3);
    EXPECT_FALSE(d1.empty());
}

TEST(TestTSDequeConstructor, MoveConstructor) {
    ts_deque<int> d1;
    d1.push_back(100);
    d1.push_back(200);

    ts_deque<int> d2(std::move(d1));
    EXPECT_EQ(d2.size(), 2);

    int val;
    d2.pop_front(val);
    EXPECT_EQ(val, 100);
    d2.pop_front(val);
    EXPECT_EQ(val, 200);
    EXPECT_TRUE(d2.empty());
}

TEST(TestTSDequeOperation, MoveAssignment) {
    ts_deque<int> d1;
    d1.push_back(1);
    d1.push_back(2);
    d1.push_back(3);

    ts_deque<int> d2;
    d2 = std::move(d1);
    EXPECT_EQ(d2.size(), 3);

    int val;
    d2.pop_front(val);
    EXPECT_EQ(val, 1);
    d2.pop_front(val);
    EXPECT_EQ(val, 2);
    d2.pop_front(val);
    EXPECT_EQ(val, 3);
    EXPECT_TRUE(d2.empty());
}

TEST(TestTSDequeOperation, StringType) {
    ts_deque<std::string> d;
    d.push_front(std::string("hello"));
    d.push_back(std::string("world"));

    EXPECT_EQ(d.size(), 2);

    std::string val;
    d.pop_front(val);
    EXPECT_EQ(val, "hello");
    d.pop_back(val);
    EXPECT_EQ(val, "world");
    EXPECT_TRUE(d.empty());
}

TEST(TestTSDequeOperation, LargeNumberOfElements) {
    ts_deque<int> d;
    constexpr int N = 100000;
    for (int i = 0; i < N; ++i) {
        d.push_back(i);
    }
    EXPECT_EQ(d.size(), static_cast<size_t>(N));

    int val;
    for (int i = 0; i < N; ++i) {
        d.pop_front(val);
        EXPECT_EQ(val, i);
    }
    EXPECT_TRUE(d.empty());
}

// ============================================================
// Multi-threaded tests
// ============================================================

TEST(TestTSDequeThreaded, ConcurrentPushFrontAndPushBack) {
    ts_deque<int> d;
    constexpr int THREADS = 4;
    constexpr int OPS_PER_THREAD = 10000;
    std::vector<std::thread> threads;

    // Two threads push front, two threads push back
    for (int t = 0; t < THREADS; ++t) {
        threads.emplace_back([&d, t]() {
            for (int i = 0; i < OPS_PER_THREAD; ++i) {
                if (t % 2 == 0) {
                    d.push_front(i);
                } else {
                    d.push_back(i);
                }
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    EXPECT_EQ(d.size(), static_cast<size_t>(THREADS * OPS_PER_THREAD));
}

TEST(TestTSDequeThreaded, ConcurrentPopFrontAndPopBack) {
    ts_deque<int> d;
    constexpr int THREADS = 4;
    constexpr int OPS_PER_THREAD = 5000;
    constexpr int TOTAL = THREADS * OPS_PER_THREAD;

    for (int i = 0; i < TOTAL; ++i) {
        d.push_back(i);
    }
    ASSERT_EQ(d.size(), static_cast<size_t>(TOTAL));

    std::atomic<int> total_popped {0};
    std::vector<std::thread> threads;

    for (int t = 0; t < THREADS; ++t) {
        threads.emplace_back([&d, &total_popped, t]() {
            int val;
            for (int i = 0; i < OPS_PER_THREAD; ++i) {
                if (t % 2 == 0) {
                    d.pop_front(val);
                } else {
                    d.pop_back(val);
                }
                total_popped.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    EXPECT_TRUE(d.empty());
    EXPECT_EQ(total_popped.load(), TOTAL);
}

TEST(TestTSDequeThreaded, InterleavedFrontAndBackOps) {
    ts_deque<int> d;
    constexpr int THREADS = 4;
    constexpr int OPS_PER_THREAD = 5000;
    std::atomic<int> total_pushed {0};
    std::atomic<int> total_popped {0};
    std::vector<std::thread> threads;

    for (int t = 0; t < THREADS; ++t) {
        threads.emplace_back([&d, &total_pushed, &total_popped, t]() {
            int val;
            for (int i = 0; i < OPS_PER_THREAD; ++i) {
                if (t == 0) {
                    d.push_front(i);
                    total_pushed.fetch_add(1, std::memory_order_relaxed);
                } else if (t == 1) {
                    d.push_back(i);
                    total_pushed.fetch_add(1, std::memory_order_relaxed);
                } else {
                    try {
                        if (t % 2 == 0) {
                            d.pop_front(val);
                        } else {
                            d.pop_back(val);
                        }
                        total_popped.fetch_add(1, std::memory_order_relaxed);
                    } catch (const mtl::EmptyContainer&) {
                        // Expected under contention
                    }
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
            d.pop_front(val);
            total_popped.fetch_add(1, std::memory_order_relaxed);
        } catch (const mtl::EmptyContainer&) {
            break;
        }
    }

    EXPECT_EQ(total_pushed.load(), total_popped.load());
    EXPECT_TRUE(d.empty());
}

TEST(TestTSDequeThreaded, ProducerConsumerFrontBack) {
    ts_deque<int> d;
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
        producers.emplace_back([&d, &total_pushed, t]() {
            for (int i = 0; i < ELEMS_PER_PRODUCER; ++i) {
                if (t % 2 == 0) {
                    d.push_front(i);
                } else {
                    d.push_back(i);
                }
                total_pushed.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    for (int t = 0; t < CONSUMERS; ++t) {
        consumers.emplace_back([&d, &total_popped, &producers_done, t]() {
            int val;
            while (true) {
                try {
                    if (t % 2 == 0) {
                        d.pop_front(val);
                    } else {
                        d.pop_back(val);
                    }
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
            d.pop_front(val);
            total_popped.fetch_add(1, std::memory_order_relaxed);
        } catch (const mtl::EmptyContainer&) {
            break;
        }
    }

    EXPECT_EQ(total_pushed.load(), total_popped.load());
    EXPECT_EQ(total_pushed.load(), TOTAL);
    EXPECT_TRUE(d.empty());
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
