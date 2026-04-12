#include <gtest/gtest.h>
#include <string>

import mtl.deque;

namespace {

using namespace mtl;

// 测试基础构造与空状态
TEST(DequeTest, BasicConstruction) {
    deque<int> dq;
    EXPECT_TRUE(dq.empty());
    EXPECT_EQ(dq.size(), 0);
}

// 测试 Size 构造函数
TEST(DequeTest, SizeConstruction) {
    size_t n = 40;  // 超过默认 BUF_LEN(16)
    deque<int> dq(n, 100);
    EXPECT_EQ(dq.size(), n);
    for (size_t i = 0; i < n; ++i) {
        EXPECT_EQ(dq[i], 100);
    }
}

// 测试 Initializer List 构造
TEST(DequeTest, InitializerList) {
    deque<int> dq = {1, 2, 3, 4, 5};
    EXPECT_EQ(dq.size(), 5);
    EXPECT_EQ(dq[0], 1);
    EXPECT_EQ(dq[4], 5);
}

// 测试 push_back 和 pop_back (触发多次扩容)
TEST(DequeTest, PushPopBack) {
    deque<int> dq;
    const int count = 100;
    for (int i = 0; i < count; ++i) {
        dq.push_back(i);
    }
    EXPECT_EQ(dq.size(), count);
    EXPECT_EQ(dq.front(), 0);
    EXPECT_EQ(dq.back(), 99);

    for (int i = count - 1; i >= 0; --i) {
        EXPECT_EQ(dq.back(), i);
        dq.pop_back();
    }
    EXPECT_TRUE(dq.empty());
}

// 测试 push_front 和 pop_front
TEST(DequeTest, PushPopFront) {
    deque<std::string> dq;
    dq.push_front("world");
    dq.push_front("hello");

    EXPECT_EQ(dq.size(), 2);
    EXPECT_EQ(dq.front(), "hello");
    EXPECT_EQ(dq.back(), "world");

    dq.pop_front();
    EXPECT_EQ(dq.front(), "world");
    dq.pop_front();
    EXPECT_TRUE(dq.empty());
}

// 测试越界访问 at()
TEST(DequeTest, AtAccess) {
    deque<int> dq = {10, 20};
    EXPECT_EQ(dq.at(0), 10);
    EXPECT_EQ(dq.at(1), 20);
    EXPECT_THROW(dq.at(2), std::out_of_range);
}

// 测试迭代器随机访问
TEST(DequeTest, IteratorArithmetic) {
    deque<int> dq;
    for (int i = 0; i < 50; ++i) {
        dq.push_back(i);
    }

    auto it = dq.begin();
    EXPECT_EQ(*it, 0);

    it += 20;  // 跨越 Node
    EXPECT_EQ(*it, 20);

    it -= 10;
    EXPECT_EQ(*it, 10);

    EXPECT_EQ(*(it + 25), 35);
    EXPECT_EQ(dq.end() - dq.begin(), 50);
}

// 测试拷贝与移动语义
TEST(DequeTest, CopyAndMove) {
    deque<int> original = {1, 2, 3, 4, 5};

    // 拷贝构造
    deque<int> copy_dq(original);
    EXPECT_EQ(copy_dq.size(), 5);
    EXPECT_EQ(copy_dq[2], 3);

    // 移动构造
    deque<int> move_dq(std::move(original));
    EXPECT_EQ(move_dq.size(), 5);
    EXPECT_TRUE(original.empty());  // 源码中移动后原对象被 clear

    // 赋值
    deque<int> assign_dq;
    assign_dq = move_dq;
    EXPECT_EQ(assign_dq.size(), 5);
}

// 测试大量双端操作导致 map 频繁 expand 的稳定性
TEST(DequeTest, HeavyExpansion) {
    deque<int> dq;
    // 反复在两端插入，强制触发 expand(true) 和 expand(false)
    for (int i = 0; i < 500; ++i) {
        dq.push_back(i);
        dq.push_front(-i);
    }
    EXPECT_EQ(dq.size(), 1000);
    EXPECT_EQ(dq.front(), -499);
    EXPECT_EQ(dq.back(), 499);
}

}  // namespace
