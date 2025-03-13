#include <gtest/gtest.h>
#include <mtl/algorithms.h>
#include <algorithm>
#include <random>
#include <vector>

TEST(TestAlgorithm, TestMergeSort) {
    mtl::size_t size = 100000;
    std::vector<int> vec(size);
    using namespace std::chrono;

    std::random_device rd;
    std::default_random_engine e(rd());
    std::uniform_int_distribution<> uid(-size * 10, size * 10);
    for (mtl::size_t i = 0; i < size; ++i) {
        vec.push_back(uid(e));
    }

    auto v = vec;

    mtl::inplace_quicksort(vec.begin(), vec.end());
    std::sort(v.begin(), v.end());
    for (size_t i = 0; i < size; ++i) {
        EXPECT_EQ(vec[i], v[i]);
    }
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
