#include <gtest/gtest.h>
#include <mtl/mtlutils.h>
#include <mtl/vector.h>

TEST(TestMTLUtils, TestMinMax) {
    int max_res = mtl::max(1, 2);
    EXPECT_EQ(max_res, 2);
    int min_res = mtl::min(1, 2);
    EXPECT_EQ(min_res, 1);
    int max3_res = mtl::max(1, 3, 2);
    EXPECT_EQ(max3_res, 3);
    int min3_res = mtl::min(3, 1, 2);
    EXPECT_EQ(min3_res, 1);
}

TEST(TestMTLUtils, TestSumProd) {
    int sum_res = mtl::sum(1, 2, 3, 4, 5);
    EXPECT_EQ(sum_res, 15);
    int prod_res = mtl::prod(1, 2, 3, 4, 5);
    EXPECT_EQ(prod_res, 120);
}

TEST(TestMTLUtils, TestMaxMinWithConstexpr) {
    constexpr int constexpr_max = mtl::max(1, 3, 2, 7, 4);
    EXPECT_EQ(constexpr_max, 7);
    constexpr int constexpr_min = mtl::min(5, 10, -1, 0, 8);
    EXPECT_EQ(constexpr_min, -1);
}

TEST(TestMTLUtils, TestFindMidAdvanceAndDistance) {
    mtl::vector<int> v({0, 1, 2, 3, 4, 5, 6, 7});
    auto mid = mtl::find_mid(v.begin(), v.end());
    EXPECT_EQ(*mid, 4);
    auto v5 = mtl::advance(v.begin(), 5);
    EXPECT_EQ(*v5, 5);
    auto diff = mtl::distance(v.begin(), v.end());
    EXPECT_EQ(diff, 8);
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
