#include <gtest/gtest.h>
#include <mtl/algorithms.h>
#include <mtl/hashing.h>

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

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
