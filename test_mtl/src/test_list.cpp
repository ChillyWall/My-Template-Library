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
    for (int i = 1; i <= 3; ++i) {
        EXPECT_EQ(l.front(), i);
    }
}

TEST(TestListOperation, TestListPushBack) {
    list<int> l;
    l.push_back(1);
    l.push_back(2);
    l.push_back(3);
    for (int i = 1; i <= 3; ++i) {
        EXPECT_EQ(l.front(), i);
        l.pop_front();
    }
}

TEST(TestListConstructor, TestListCopyConstructor) {
    list<int> l1;
    l1.push_back(1);
    l1.push_back(2);
    l1.push_back(3);
    list<int> l2(l1);
    EXPECT_EQ(l2.size(), 3);
    EXPECT_EQ(l2.front(), 1);
    EXPECT_EQ(l2.back(), 3);
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
