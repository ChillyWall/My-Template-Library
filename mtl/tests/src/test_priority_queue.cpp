#include <gtest/gtest.h>

import mtl.priority_queue;

TEST(TestPriorityQueue, TestDefaultConstructor) {
    mtl::priority_queue<int> pq;
    EXPECT_EQ(pq.size(), 0);
    EXPECT_TRUE(pq.empty());
}

TEST(TestPriorityQueue, TestPushPop) {
    mtl::priority_queue<int> pq;
    for (int i = 20; i > 0; --i) {
        pq.push(i);
    }
    EXPECT_EQ(pq.size(), 20);
    for (int i = 1; i <= 20; ++i) {
        EXPECT_EQ(pq.top(), i);
        pq.pop();
    }
}

TEST(TestPriorityQueue, TestCopyConstructor) {
    mtl::priority_queue<int> pq1;
    pq1.push(1);
    pq1.push(2);
    pq1.push(3);
    mtl::priority_queue<int> pq2(pq1);
    EXPECT_EQ(pq2.size(), pq1.size());
    for (int i = 0; i < 3; ++i) {
        EXPECT_EQ(pq2.top(), pq1.top());
        pq1.pop();
        pq2.pop();
    }
}

TEST(TestPriorityQueue, TestMoveConstructor) {
    mtl::priority_queue<int> pq1;
    pq1.push(1);
    pq1.push(2);
    pq1.push(3);
    mtl::priority_queue<int> pq2(std::move(pq1));
    EXPECT_EQ(pq2.size(), 3);
    for (int i = 0; i < 3; ++i) {
        EXPECT_EQ(pq2.top(), i + 1);
        pq2.pop();
    }
}

TEST(TestPriorityQueue, TestDuplicateValues) {
    mtl::priority_queue<int> pq;
    pq.push(5);
    pq.push(1);
    pq.push(3);
    pq.push(3);
    pq.push(2);

    EXPECT_EQ(pq.size(), 5);
    EXPECT_EQ(pq.top(), 1);
    pq.pop();
    EXPECT_EQ(pq.top(), 2);
    pq.pop();
    EXPECT_EQ(pq.top(), 3);
    pq.pop();
    EXPECT_EQ(pq.top(), 3);
    pq.pop();
    EXPECT_EQ(pq.top(), 5);
}

TEST(TestPriorityQueue, TestReserveAndPush) {
    mtl::priority_queue<int> pq;
    pq.reserve(64);
    for (int i = 50; i >= 1; --i) {
        pq.push(i);
    }
    EXPECT_EQ(pq.size(), 50);
    EXPECT_EQ(pq.top(), 1);
}

TEST(TestPriorityQueue, TestCopyAssignment) {
    mtl::priority_queue<int> pq1;
    pq1.push(4);
    pq1.push(1);
    pq1.push(9);

    mtl::priority_queue<int> pq2;
    pq2 = pq1;
    EXPECT_EQ(pq2.size(), pq1.size());
    EXPECT_EQ(pq2.top(), 1);

    pq1.pop();
    EXPECT_EQ(pq2.top(), 1);
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
