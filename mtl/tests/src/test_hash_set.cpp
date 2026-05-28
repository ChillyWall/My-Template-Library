#include <gtest/gtest.h>

import mtl.hash_set;

using namespace mtl;

// ============================================================
// Basic operations
// ============================================================

TEST(HashSetTest, DefaultConstructor) {
    hash_set<int> s;
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0);
}

TEST(HashSetTest, InsertAndContains) {
    hash_set<int> s;
    EXPECT_TRUE(s.insert(10));
    EXPECT_TRUE(s.insert(20));
    EXPECT_TRUE(s.insert(5));

    EXPECT_EQ(s.size(), 3);
    EXPECT_FALSE(s.empty());
    EXPECT_TRUE(s.contains(10));
    EXPECT_TRUE(s.contains(20));
    EXPECT_TRUE(s.contains(5));
    EXPECT_FALSE(s.contains(100));
}

TEST(HashSetTest, InsertDuplicate) {
    hash_set<int> s;
    EXPECT_TRUE(s.insert(10));
    EXPECT_FALSE(s.insert(10));

    EXPECT_EQ(s.size(), 1);
    EXPECT_TRUE(s.contains(10));
}

TEST(HashSetTest, EraseByValue) {
    hash_set<int> s;
    s.insert(10);
    s.insert(20);
    s.insert(5);

    EXPECT_EQ(s.erase(20), 1);
    EXPECT_EQ(s.size(), 2);
    EXPECT_FALSE(s.contains(20));

    EXPECT_EQ(s.erase(100), 0);
    EXPECT_EQ(s.size(), 2);
}

TEST(HashSetTest, EraseByIterator) {
    hash_set<int> s;
    s.insert(10);
    s.insert(20);
    s.insert(5);

    auto it = s.find(10);
    EXPECT_TRUE(s.erase(it));
    EXPECT_EQ(s.size(), 2);
    EXPECT_FALSE(s.contains(10));
}

TEST(HashSetTest, EraseEndIterator) {
    hash_set<int> s;
    s.insert(10);

    EXPECT_FALSE(s.erase(s.end()));
    EXPECT_EQ(s.size(), 1);
}

TEST(HashSetTest, Find) {
    hash_set<int> s;
    s.insert(10);
    s.insert(20);

    auto it = s.find(10);
    EXPECT_NE(it, s.end());
    EXPECT_EQ(*it, 10);

    auto it2 = s.find(100);
    EXPECT_EQ(it2, s.end());
}

TEST(HashSetTest, Clear) {
    hash_set<int> s;
    s.insert(10);
    s.insert(20);
    s.clear();

    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0);
    EXPECT_FALSE(s.contains(10));
}

// ============================================================
// Iteration
// ============================================================

TEST(HashSetTest, IterationCoversAllElements) {
    hash_set<int> s;
    s.insert(1);
    s.insert(2);
    s.insert(3);

    int found[4] = {0};
    for (auto it = s.begin(); it != s.end(); ++it) {
        found[*it] = 1;
    }
    EXPECT_EQ(found[1], 1);
    EXPECT_EQ(found[2], 1);
    EXPECT_EQ(found[3], 1);
}

TEST(HashSetTest, ConstIteration) {
    hash_set<int> s;
    s.insert(10);
    s.insert(20);

    const auto& cs = s;
    int sum = 0;
    for (auto it = cs.begin(); it != cs.end(); ++it) {
        sum += *it;
    }
    EXPECT_EQ(sum, 30);
}

// ============================================================
// Copy and move
// ============================================================

TEST(HashSetTest, CopyConstructor) {
    hash_set<int> s1;
    s1.insert(10);
    s1.insert(20);

    hash_set<int> s2(s1);
    EXPECT_EQ(s2.size(), 2);
    EXPECT_TRUE(s2.contains(10));
    EXPECT_TRUE(s2.contains(20));

    // Original unchanged
    EXPECT_EQ(s1.size(), 2);
}

TEST(HashSetTest, MoveConstructor) {
    hash_set<int> s1;
    s1.insert(10);
    s1.insert(20);

    hash_set<int> s2(std::move(s1));
    EXPECT_EQ(s2.size(), 2);
    EXPECT_TRUE(s2.contains(10));
    EXPECT_TRUE(s2.contains(20));
}

TEST(HashSetTest, MoveAssignment) {
    hash_set<int> s1;
    s1.insert(10);
    s1.insert(20);

    hash_set<int> s2;
    s2 = std::move(s1);
    EXPECT_EQ(s2.size(), 2);
    EXPECT_TRUE(s2.contains(10));
    EXPECT_TRUE(s2.contains(20));
}

// ============================================================
// Custom hash and equality
// ============================================================

struct CaseInsensitiveHash {
    size_t operator()(const std::string& s) const {
        size_t h = 0;
        for (char c : s) {
            h = h * 31 + static_cast<size_t>(std::tolower(c));
        }
        return h;
    }
};

struct CaseInsensitiveEqual {
    bool operator()(const std::string& a, const std::string& b) const {
        if (a.size() != b.size()) return false;
        for (size_t i = 0; i < a.size(); ++i) {
            if (std::tolower(a[i]) != std::tolower(b[i])) return false;
        }
        return true;
    }
};

TEST(HashSetTest, CustomHashAndEqual) {
    hash_set<std::string, CaseInsensitiveHash, CaseInsensitiveEqual> s;

    EXPECT_TRUE(s.insert("Hello"));
    // "hello" should be considered a duplicate (case-insensitive)
    EXPECT_FALSE(s.insert("hello"));

    EXPECT_EQ(s.size(), 1);
    EXPECT_TRUE(s.contains("HELLO"));
    EXPECT_TRUE(s.contains("hello"));
}

// ============================================================
// Large number of elements
// ============================================================

TEST(HashSetTest, LargeNumberOfElements) {
    hash_set<int> s;
    constexpr int N = 5000;
    for (int i = 0; i < N; ++i) {
        EXPECT_TRUE(s.insert(i));
    }
    EXPECT_EQ(s.size(), static_cast<size_t>(N));

    for (int i = 0; i < N; ++i) {
        EXPECT_TRUE(s.contains(i));
    }

    for (int i = 0; i < N; ++i) {
        EXPECT_EQ(s.erase(i), 1);
    }
    EXPECT_TRUE(s.empty());
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
