#include <gtest/gtest.h>

import mtl.algorithms;
import mtl.hashing;

TEST(TestHash, TestDefaultConstructor) {
    mtl::hashing<int> set;
    EXPECT_EQ(set.size(), 0);
    EXPECT_EQ(set.max_size(), 101);
}

TEST(TestHash, TestConstructorWithInitSize) {
    mtl::hashing<int> set(1000);
    EXPECT_EQ(set.size(), 0);
    EXPECT_EQ(set.max_size(), 1009);
}

TEST(TestHash, TestIterators) {
    mtl::hashing<int> set;
    for (int i = 0; i < 101; i += 2) {
        set.insert(i);
    }
    int i = 0;
    for (auto itr = set.begin(); itr < set.end(); ++itr) {
        EXPECT_EQ(*itr, i);
        i += 2;
    }
}

TEST(TestHash, TestInsert) {
    mtl::hashing<int> set;
    for (int i = 0; i < 32; ++i) {
        EXPECT_EQ(set.insert(i), true);
    }

    EXPECT_EQ(set.size(), 32);
    EXPECT_EQ(set.max_size(), 101);

    EXPECT_EQ(set.insert(101), true);

    auto itr = set.begin();
    EXPECT_EQ(*itr, 0);
    ++itr;
    EXPECT_EQ(*itr, 101);
    ++itr;
    for (int i = 2; i < 32; ++i, ++itr) {
        EXPECT_EQ(*itr, i);
    }
    EXPECT_EQ(*itr, 1);
}

TEST(TestHash, TestEdgeInsert) {
    mtl::hashing<int> set;
    for (int i = 0; i < 32; ++i) {
        EXPECT_EQ(set.insert(i + 69), true);
    }

    EXPECT_EQ(set.size(), 32);
    EXPECT_EQ(set.max_size(), 101);

    EXPECT_EQ(set.insert(170), true);

    auto itr = set.begin();
    EXPECT_EQ(*itr, 70);
    ++itr;
    EXPECT_EQ(*itr, 69);
    ++itr;
    EXPECT_EQ(*itr, 170);
    ++itr;

    for (int i = 2; i < 32; ++i, ++itr) {
        EXPECT_EQ(*itr, i + 69);
    }
}

TEST(TestHash, TestInsertExpand) {
    mtl::hashing<int> set;
    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(set.insert(i + i * 211), true);
    }
    EXPECT_EQ(set.size(), 100);
    EXPECT_EQ(set.max_size(), 211);
    int i = 0;
    for (auto itr = set.begin(); itr < set.end(); ++itr) {
        EXPECT_EQ(*itr, i + i * 211);
        ++i;
    }
}

TEST(TestHash, TestDuplicateInsert) {
    mtl::hashing<int> set;
    EXPECT_TRUE(set.insert(42));
    EXPECT_FALSE(set.insert(42));
    EXPECT_EQ(set.size(), 1);
}

TEST(TestHash, TestRemove) {
    mtl::hashing<int> set;
    for (int i = 0; i < 50; ++i) {
        set.insert(i * 3);
    }

    EXPECT_TRUE(set.remove(0));
    EXPECT_TRUE(set.remove(6));
    EXPECT_FALSE(set.contains(0));
    EXPECT_FALSE(set.contains(6));
    EXPECT_FALSE(set.remove(6));
    EXPECT_EQ(set.size(), 48);
}

TEST(TestHash, TestClear) {
    mtl::hashing<int> set;
    for (int i = 0; i < 80; ++i) {
        set.insert(i + 1);
    }
    EXPECT_EQ(set.size(), 80);
    set.clear();
    EXPECT_EQ(set.size(), 0);
    EXPECT_EQ(set.max_size(), 101);
    EXPECT_FALSE(set.contains(1));
}

TEST(TestHash, TestCopyAssignment) {
    mtl::hashing<int> set1;
    for (int i = 0; i < 20; ++i) {
        set1.insert(i * 5);
    }

    mtl::hashing<int> set2;
    set2 = set1;
    EXPECT_EQ(set2.size(), set1.size());
    EXPECT_TRUE(set2.contains(0));
    EXPECT_TRUE(set2.contains(95));
}

TEST(TestHash, TestIteratorSkipsEmpty) {
    mtl::hashing<int> set;
    set.insert(0);
    set.insert(100);
    set.insert(201);
    set.insert(305);

    int count = 0;
    for (auto itr = set.begin(); itr < set.end(); ++itr) {
        ++count;
    }
    EXPECT_EQ(count, 4);
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
