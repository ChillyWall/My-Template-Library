#include <gtest/gtest.h>

import mtl.core;
import mtl.map;

using namespace mtl;

// ============================================================
// Basic operations
// ============================================================

TEST(MapTest, DefaultConstructor) {
    map<int, std::string> m;
    EXPECT_TRUE(m.empty());
    EXPECT_EQ(m.size(), 0);
}

TEST(MapTest, InsertAndAccess) {
    map<int, std::string> m;
    m.insert(1, std::string("one"));
    m.insert(2, std::string("two"));
    m.insert(3, std::string("three"));

    EXPECT_EQ(m.size(), 3);
    EXPECT_FALSE(m.empty());
    EXPECT_TRUE(m.contains(1));
    EXPECT_TRUE(m.contains(2));
    EXPECT_TRUE(m.contains(3));
    EXPECT_FALSE(m.contains(4));
}

TEST(MapTest, InsertDuplicateKey) {
    map<int, std::string> m;
    m.insert(1, std::string("first"));
    m.insert(1, std::string("second"));

    // Duplicate key should be ignored
    EXPECT_EQ(m.size(), 1);
    auto it = m.find(1);
    EXPECT_EQ(it->second, "first");
}

TEST(MapTest, EraseByKey) {
    map<int, std::string> m;
    m.insert(1, std::string("one"));
    m.insert(2, std::string("two"));

    EXPECT_EQ(m.erase(2), 1);
    EXPECT_EQ(m.size(), 1);
    EXPECT_FALSE(m.contains(2));

    EXPECT_EQ(m.erase(100), 0);
    EXPECT_EQ(m.size(), 1);
}

TEST(MapTest, EraseByIterator) {
    map<int, std::string> m;
    m.insert(1, std::string("one"));
    m.insert(2, std::string("two"));
    m.insert(3, std::string("three"));

    auto it = m.find(2);
    auto next = m.erase(it);
    EXPECT_EQ(m.size(), 2);
    EXPECT_FALSE(m.contains(2));
    EXPECT_EQ(next->first, 3);
}

TEST(MapTest, Find) {
    map<int, std::string> m;
    m.insert(1, std::string("one"));
    m.insert(2, std::string("two"));

    auto it = m.find(1);
    EXPECT_NE(it, m.end());
    EXPECT_EQ(it->first, 1);
    EXPECT_EQ(it->second, "one");

    auto it2 = m.find(100);
    EXPECT_EQ(it2, m.end());
}

TEST(MapTest, Clear) {
    map<int, std::string> m;
    m.insert(1, std::string("one"));
    m.insert(2, std::string("two"));
    m.clear();

    EXPECT_TRUE(m.empty());
    EXPECT_EQ(m.size(), 0);
    EXPECT_FALSE(m.contains(1));
}

// ============================================================
// operator[]
// ============================================================

TEST(MapTest, OperatorBracketAccess) {
    map<int, std::string> m;
    m.insert(1, std::string("one"));

    EXPECT_EQ(m[1], "one");

    // Modify existing value
    m[1] = "modified";
    EXPECT_EQ(m[1], "modified");
}

TEST(MapTest, OperatorBracketInsertDefault) {
    map<int, int> m;
    // operator[] should insert default-constructed value
    m[42] = 100;

    EXPECT_EQ(m.size(), 1);
    EXPECT_EQ(m[42], 100);

    // Access non-existing key → insert default (0)
    EXPECT_EQ(m[99], 0);
    EXPECT_TRUE(m.contains(99));
    m[99] = 200;
    EXPECT_EQ(m[99], 200);
}

// ============================================================
// at()
// ============================================================

TEST(MapTest, AtSuccess) {
    map<int, std::string> m;
    m.insert(1, std::string("one"));

    EXPECT_EQ(m.at(1), "one");

    m.at(1) = "modified";
    EXPECT_EQ(m.at(1), "modified");
}

TEST(MapTest, AtThrowsOnMissingKey) {
    map<int, std::string> m;
    EXPECT_THROW(m.at(42), EmptyContainer);

    const auto& cm = m;
    EXPECT_THROW(cm.at(42), EmptyContainer);
}

// ============================================================
// Iteration (sorted by key)
// ============================================================

TEST(MapTest, IterationSortedByKey) {
    map<int, std::string> m;
    m.insert(30, std::string("thirty"));
    m.insert(10, std::string("ten"));
    m.insert(20, std::string("twenty"));

    int expected_keys[] = {10, 20, 30};
    int i = 0;
    for (auto it = m.begin(); it != m.end(); ++it, ++i) {
        EXPECT_EQ(it->first, expected_keys[i]);
    }
    EXPECT_EQ(i, 3);
}

TEST(MapTest, ModifyThroughIterator) {
    map<int, int> m;
    m.insert(1, 10);
    m.insert(2, 20);

    for (auto it = m.begin(); it != m.end(); ++it) {
        it->second *= 2;
    }

    EXPECT_EQ(m[1], 20);
    EXPECT_EQ(m[2], 40);
}

// ============================================================
// Copy and move
// ============================================================

TEST(MapTest, CopyConstructor) {
    map<int, std::string> m1;
    m1.insert(1, std::string("one"));
    m1.insert(2, std::string("two"));

    map<int, std::string> m2(m1);
    EXPECT_EQ(m2.size(), 2);
    EXPECT_EQ(m2[1], "one");
    EXPECT_EQ(m2[2], "two");

    // Original unchanged
    EXPECT_EQ(m1.size(), 2);
}

TEST(MapTest, MoveConstructor) {
    map<int, std::string> m1;
    m1.insert(1, std::string("one"));
    m1.insert(2, std::string("two"));

    map<int, std::string> m2(std::move(m1));
    EXPECT_EQ(m2.size(), 2);
    EXPECT_EQ(m2[1], "one");
    EXPECT_EQ(m2[2], "two");
}

TEST(MapTest, MoveAssignment) {
    map<int, std::string> m1;
    m1.insert(1, std::string("one"));
    m1.insert(2, std::string("two"));

    map<int, std::string> m2;
    m2 = std::move(m1);
    EXPECT_EQ(m2.size(), 2);
    EXPECT_EQ(m2[1], "one");
    EXPECT_EQ(m2[2], "two");
}

// ============================================================
// Custom comparator
// ============================================================

TEST(MapTest, CustomComparator) {
    map<int, std::string, std::greater<int>> m;
    m.insert(10, std::string("ten"));
    m.insert(20, std::string("twenty"));
    m.insert(5, std::string("five"));

    // Descending order
    int expected[] = {20, 10, 5};
    int i = 0;
    for (auto it = m.begin(); it != m.end(); ++it, ++i) {
        EXPECT_EQ(it->first, expected[i]);
    }
    EXPECT_EQ(i, 3);
}

// ============================================================
// Large number of elements
// ============================================================

TEST(MapTest, LargeNumberOfElements) {
    map<int, int> m;
    constexpr int N = 5000;
    for (int i = 0; i < N; ++i) {
        m.insert(i, i * 10);
    }
    EXPECT_EQ(m.size(), static_cast<size_t>(N));

    for (int i = 0; i < N; ++i) {
        EXPECT_TRUE(m.contains(i));
        EXPECT_EQ(m[i], i * 10);
    }

    for (int i = 0; i < N; ++i) {
        m.erase(i);
    }
    EXPECT_TRUE(m.empty());
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
