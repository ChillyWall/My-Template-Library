#include <gtest/gtest.h>

import mtl.set;

using namespace mtl;

// ============================================================
// Basic operations
// ============================================================

TEST(SetTest, DefaultConstructor) {
    set<int> s;
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0);
}

TEST(SetTest, InsertAndContains) {
    set<int> s;
    s.insert(10);
    s.insert(20);
    s.insert(5);

    EXPECT_EQ(s.size(), 3);
    EXPECT_FALSE(s.empty());
    EXPECT_TRUE(s.contains(10));
    EXPECT_TRUE(s.contains(20));
    EXPECT_TRUE(s.contains(5));
    EXPECT_FALSE(s.contains(100));
}

TEST(SetTest, InsertDuplicate) {
    set<int> s;
    s.insert(10);
    s.insert(10);

    EXPECT_EQ(s.size(), 1);
    EXPECT_TRUE(s.contains(10));
}

TEST(SetTest, EraseByValue) {
    set<int> s;
    s.insert(10);
    s.insert(20);
    s.insert(5);

    EXPECT_EQ(s.erase(20), 1);
    EXPECT_EQ(s.size(), 2);
    EXPECT_FALSE(s.contains(20));

    EXPECT_EQ(s.erase(100), 0);
    EXPECT_EQ(s.size(), 2);
}

TEST(SetTest, EraseByIterator) {
    set<int> s;
    s.insert(10);
    s.insert(20);
    s.insert(5);

    auto it = s.find(10);
    auto next = s.erase(it);
    EXPECT_EQ(s.size(), 2);
    EXPECT_FALSE(s.contains(10));
    // next should point to the next element (20)
    EXPECT_EQ(*next, 20);
}

TEST(SetTest, Find) {
    set<int> s;
    s.insert(10);
    s.insert(20);

    auto it = s.find(10);
    EXPECT_NE(it, s.end());
    EXPECT_EQ(*it, 10);

    auto it2 = s.find(100);
    EXPECT_EQ(it2, s.end());
}

TEST(SetTest, Clear) {
    set<int> s;
    s.insert(10);
    s.insert(20);
    s.clear();

    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0);
    EXPECT_FALSE(s.contains(10));
}

// ============================================================
// Iteration (sorted order)
// ============================================================

TEST(SetTest, IterationSorted) {
    set<int> s;
    s.insert(30);
    s.insert(10);
    s.insert(20);
    s.insert(40);
    s.insert(5);

    int expected[] = {5, 10, 20, 30, 40};
    int i = 0;
    for (auto it = s.begin(); it != s.end(); ++it, ++i) {
        EXPECT_EQ(*it, expected[i]);
    }
    EXPECT_EQ(i, 5);
}

TEST(SetTest, ConstIteration) {
    set<int> s;
    s.insert(10);
    s.insert(20);

    const auto& cs = s;
    int sum = 0;
    for (auto it = cs.begin(); it != cs.end(); ++it) {
        sum += *it;
    }
    EXPECT_EQ(sum, 30);
}

TEST(SetTest, ReverseIteration) {
    set<int> s;
    s.insert(10);
    s.insert(20);
    s.insert(30);

    auto it = s.find(30);
    EXPECT_EQ(*it, 30);
    --it;
    EXPECT_EQ(*it, 20);
    --it;
    EXPECT_EQ(*it, 10);
}

// ============================================================
// Copy and move
// ============================================================

TEST(SetTest, CopyConstructor) {
    set<int> s1;
    s1.insert(10);
    s1.insert(20);

    set<int> s2(s1);
    EXPECT_EQ(s2.size(), 2);
    EXPECT_TRUE(s2.contains(10));
    EXPECT_TRUE(s2.contains(20));

    // Original unchanged
    EXPECT_EQ(s1.size(), 2);
}

TEST(SetTest, MoveConstructor) {
    set<int> s1;
    s1.insert(10);
    s1.insert(20);

    set<int> s2(std::move(s1));
    EXPECT_EQ(s2.size(), 2);
    EXPECT_TRUE(s2.contains(10));
    EXPECT_TRUE(s2.contains(20));
}

TEST(SetTest, MoveAssignment) {
    set<int> s1;
    s1.insert(10);
    s1.insert(20);

    set<int> s2;
    s2 = std::move(s1);
    EXPECT_EQ(s2.size(), 2);
    EXPECT_TRUE(s2.contains(10));
    EXPECT_TRUE(s2.contains(20));
}

// ============================================================
// Custom comparator
// ============================================================

TEST(SetTest, CustomComparator) {
    set<int, std::greater<int>> s;
    s.insert(10);
    s.insert(20);
    s.insert(5);

    // Should iterate in descending order
    int expected[] = {20, 10, 5};
    int i = 0;
    for (auto it = s.begin(); it != s.end(); ++it, ++i) {
        EXPECT_EQ(*it, expected[i]);
    }
    EXPECT_EQ(i, 3);
}

// ============================================================
// String type
// ============================================================

TEST(SetTest, StringType) {
    set<std::string> s;
    s.insert(std::string("world"));
    s.insert(std::string("hello"));
    s.insert(std::string("apple"));

    // Sorted lexicographically
    auto it = s.begin();
    EXPECT_EQ(*it++, "apple");
    EXPECT_EQ(*it++, "hello");
    EXPECT_EQ(*it++, "world");
}

// ============================================================
// Large number of elements
// ============================================================

TEST(SetTest, LargeNumberOfElements) {
    set<int> s;
    constexpr int N = 5000;
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
