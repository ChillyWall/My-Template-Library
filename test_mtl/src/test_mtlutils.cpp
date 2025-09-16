#include <gtest/gtest.h>
#include <mtl/mtlutils.h>

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

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
