#include <gtest/gtest.h>

import mtl.core;
import mtl.hash_map;

using namespace mtl;

// ============================================================
// Basic operations
// ============================================================

TEST(HashMapTest, DefaultConstructor) {
    hash_map<int, std::string> m;
    EXPECT_TRUE(m.empty());
    EXPECT_EQ(m.size(), 0);
}

TEST(HashMapTest, InsertAndAccess) {
    hash_map<int, std::string> m;
    EXPECT_TRUE(m.insert(1, std::string("one")));
    EXPECT_TRUE(m.insert(2, std::string("two")));
    EXPECT_TRUE(m.insert(3, std::string("three")));

    EXPECT_EQ(m.size(), 3);
    EXPECT_FALSE(m.empty());
    EXPECT_TRUE(m.contains(1));
    EXPECT_TRUE(m.contains(2));
    EXPECT_TRUE(m.contains(3));
    EXPECT_FALSE(m.contains(4));
}

TEST(HashMapTest, InsertDuplicateKey) {
    hash_map<int, std::string> m;
    EXPECT_TRUE(m.insert(1, std::string("first")));
    EXPECT_FALSE(m.insert(1, std::string("second")));

    EXPECT_EQ(m.size(), 1);
    auto it = m.find(1);
    EXPECT_EQ(it->second, "first");
}

TEST(HashMapTest, EraseByKey) {
    hash_map<int, std::string> m;
    m.insert(1, std::string("one"));
    m.insert(2, std::string("two"));

    EXPECT_EQ(m.erase(2), 1);
    EXPECT_EQ(m.size(), 1);
    EXPECT_FALSE(m.contains(2));

    EXPECT_EQ(m.erase(100), 0);
    EXPECT_EQ(m.size(), 1);
}

TEST(HashMapTest, EraseByIterator) {
    hash_map<int, std::string> m;
    m.insert(1, std::string("one"));
    m.insert(2, std::string("two"));
    m.insert(3, std::string("three"));

    auto it = m.find(2);
    EXPECT_TRUE(m.erase(it));
    EXPECT_EQ(m.size(), 2);
    EXPECT_FALSE(m.contains(2));
}

TEST(HashMapTest, EraseEndIterator) {
    hash_map<int, std::string> m;
    m.insert(1, std::string("one"));
    EXPECT_FALSE(m.erase(m.end()));
    EXPECT_EQ(m.size(), 1);
}

TEST(HashMapTest, Find) {
    hash_map<int, std::string> m;
    m.insert(1, std::string("one"));
    m.insert(2, std::string("two"));

    auto it = m.find(1);
    EXPECT_NE(it, m.end());
    EXPECT_EQ(it->first, 1);
    EXPECT_EQ(it->second, "one");

    auto it2 = m.find(100);
    EXPECT_EQ(it2, m.end());
}

TEST(HashMapTest, Clear) {
    hash_map<int, std::string> m;
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

TEST(HashMapTest, OperatorBracketAccess) {
    hash_map<int, std::string> m;
    m.insert(1, std::string("one"));

    EXPECT_EQ(m[1], "one");

    m[1] = "modified";
    EXPECT_EQ(m[1], "modified");
}

TEST(HashMapTest, OperatorBracketInsertDefault) {
    hash_map<int, int> m;
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

TEST(HashMapTest, AtSuccess) {
    hash_map<int, std::string> m;
    m.insert(1, std::string("one"));

    EXPECT_EQ(m.at(1), "one");

    m.at(1) = "modified";
    EXPECT_EQ(m.at(1), "modified");
}

TEST(HashMapTest, AtThrowsOnMissingKey) {
    hash_map<int, std::string> m;
    EXPECT_THROW(m.at(42), EmptyContainer);

    const auto& cm = m;
    EXPECT_THROW(cm.at(42), EmptyContainer);
}

// ============================================================
// Iteration
// ============================================================

TEST(HashMapTest, IterationCoversAllEntries) {
    hash_map<int, int> m;
    m.insert(1, 100);
    m.insert(2, 200);
    m.insert(3, 300);

    int count = 0;
    int sum_keys = 0;
    int sum_vals = 0;
    for (auto it = m.begin(); it != m.end(); ++it) {
        ++count;
        sum_keys += it->first;
        sum_vals += it->second;
    }
    EXPECT_EQ(count, 3);
    EXPECT_EQ(sum_keys, 6);    // 1 + 2 + 3
    EXPECT_EQ(sum_vals, 600);  // 100 + 200 + 300
}

TEST(HashMapTest, ModifyThroughIterator) {
    hash_map<int, int> m;
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

TEST(HashMapTest, CopyConstructor) {
    hash_map<int, std::string> m1;
    m1.insert(1, std::string("one"));
    m1.insert(2, std::string("two"));

    hash_map<int, std::string> m2(m1);
    EXPECT_EQ(m2.size(), 2);
    EXPECT_EQ(m2[1], "one");
    EXPECT_EQ(m2[2], "two");

    EXPECT_EQ(m1.size(), 2);
}

TEST(HashMapTest, MoveConstructor) {
    hash_map<int, std::string> m1;
    m1.insert(1, std::string("one"));
    m1.insert(2, std::string("two"));

    hash_map<int, std::string> m2(std::move(m1));
    EXPECT_EQ(m2.size(), 2);
    EXPECT_EQ(m2[1], "one");
    EXPECT_EQ(m2[2], "two");
}

TEST(HashMapTest, MoveAssignment) {
    hash_map<int, std::string> m1;
    m1.insert(1, std::string("one"));
    m1.insert(2, std::string("two"));

    hash_map<int, std::string> m2;
    m2 = std::move(m1);
    EXPECT_EQ(m2.size(), 2);
    EXPECT_EQ(m2[1], "one");
    EXPECT_EQ(m2[2], "two");
}

// ============================================================
// Custom hash and equality
// ============================================================

struct CIHash {
    size_t operator()(const std::string& s) const {
        size_t h = 0;
        for (char c : s)
            h = h * 31 + static_cast<size_t>(std::tolower(c));
        return h;
    }
};

struct CIEqual {
    bool operator()(const std::string& a, const std::string& b) const {
        if (a.size() != b.size())
            return false;
        for (size_t i = 0; i < a.size(); ++i) {
            if (std::tolower(a[i]) != std::tolower(b[i]))
                return false;
        }
        return true;
    }
};

TEST(HashMapTest, CustomHashAndEqual) {
    hash_map<std::string, int, CIHash, CIEqual> m;

    EXPECT_TRUE(m.insert("Hello", 42));
    EXPECT_FALSE(m.insert("hello", 99));  // duplicate (case-insensitive)

    EXPECT_EQ(m.size(), 1);
    EXPECT_TRUE(m.contains("HELLO"));
    EXPECT_EQ(m["hello"], 42);
}

// ============================================================
// Large number of elements
// ============================================================

TEST(HashMapTest, LargeNumberOfElements) {
    hash_map<int, int> m;
    constexpr int N = 5000;
    for (int i = 0; i < N; ++i) {
        EXPECT_TRUE(m.insert(i, i * 10));
    }
    EXPECT_EQ(m.size(), static_cast<size_t>(N));

    for (int i = 0; i < N; ++i) {
        EXPECT_TRUE(m.contains(i));
        EXPECT_EQ(m[i], i * 10);
    }

    for (int i = 0; i < N; ++i) {
        EXPECT_EQ(m.erase(i), 1);
    }
    EXPECT_TRUE(m.empty());
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
