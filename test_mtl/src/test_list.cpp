#include <gtest/gtest.h>
#include <mtl/list.h>
#include <test_mtl/myutils.h>

using mtl::list;

TEST(TestListConstructor, TestListDefaultConstructor) {
    list<int> l;
    EXPECT_EQ(l.size(), 0);
    EXPECT_THROW(l.front(), mtl::EmptyContainer);
    EXPECT_THROW(l.back(), mtl::EmptyContainer);
}

TEST(TestConstructor, TestListInitListConstructor) {
    list<int> l = {1, 2, 3};
    EXPECT_EQ(l.size(), 3);
    auto itr = l.begin();
    for (int i = 1; i <= 3; ++i) {
        EXPECT_EQ(*itr, i);
        ++itr;
    }
}

TEST(TestListConstructor, TestListCopyConstructor) {
    list<int> l1 = {1, 2, 3};
    list<int> l2(l1);
    EXPECT_EQ(l2.size(), 3);
    auto itr = l2.begin();
    for (int i = 1; i <= 3; ++i) {
        EXPECT_EQ(*itr, i);
        ++itr;
    }
}

TEST(TestListOperation, TestListPushBack) {
    list<int> l;
    l.push_back(1);
    l.push_back(2);
    l.push_back(3);
    auto itr = l.begin();
    for (int i = 1; i <= 3; ++i) {
        EXPECT_EQ(*itr, i);
        ++itr;
    }
}

TEST(TestListOperation, TestListPushFront) {
    list<int> l;
    l.push_front(3);
    l.push_front(2);
    l.push_front(1);
    auto itr = l.begin();
    for (int i = 1; i <= 3; ++i) {
        EXPECT_EQ(*itr, i);
        ++itr;
    }
}

TEST(TestListOperation, TestListPopBack) {
    list<int> l = {1, 2, 3};
    l.pop_back();
    EXPECT_EQ(l.size(), 2);
    auto itr = l.begin();
    EXPECT_EQ(*itr, 1);
    ++itr;
    EXPECT_EQ(*itr, 2);
}

TEST(TestListOperation, TestListPopFront) {
    list<int> l = {1, 2, 3};
    l.pop_front();
    EXPECT_EQ(l.size(), 2);
    auto itr = l.begin();
    EXPECT_EQ(*itr, 2);
    ++itr;
    EXPECT_EQ(*itr, 3);
}

TEST(TestListOperation, TestListInsert) {
    list<int> l = {1, 2, 3};
    auto itr = l.begin();
    ++itr;
    l.insert(itr, 4);
    EXPECT_EQ(l.size(), 4);
    itr = l.begin();
    EXPECT_EQ(*itr, 1);
    ++itr;
    EXPECT_EQ(*itr, 4);
    ++itr;
    EXPECT_EQ(*itr, 2);
    ++itr;
    EXPECT_EQ(*itr, 3);
}

TEST(TestListOperation, TestListRemove) {
    list<int> l = {1, 2, 3};
    auto itr = l.begin();
    ++itr;
    l.remove(itr);
    EXPECT_EQ(l.size(), 2);
    itr = l.begin();
    EXPECT_EQ(*itr, 1);
    ++itr;
    EXPECT_EQ(*itr, 3);
}

TEST(TestListOperation, TestListRemoveRange) {
    list<int> l = {1, 2, 3, 4, 5};
    l.remove(mtl::advance(l.begin(), 1), mtl::advance(l.begin(), 3));
    EXPECT_EQ(l.size(), 3);
    auto itr = l.begin();
    EXPECT_EQ(*itr, 1);
    ++itr;
    EXPECT_EQ(*itr, 4);
    ++itr;
    EXPECT_EQ(*itr, 5);
}

TEST(TestListOperation, TestListInsertRange) {
    list<int> l = {1, 2, 3};
    list<int> l2 = {4, 5};
    l.insert(l.end(), l2.begin(), l2.end());
    EXPECT_EQ(l.size(), 5);
    auto itr = l.begin();
    for (int i = 1; i <= 5; ++i) {
        EXPECT_EQ(*(itr++), i);
    }
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
