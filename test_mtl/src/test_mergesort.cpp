#include <gtest/gtest.h>
#include <mtl/merge_sort.h>
#include <mtl/vector.h>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>

TEST(TestMergeSort, TestMergeSortRecursive) {
    mtl::size_t size = 1000000;
    mtl::vector<int> v(size);
    std::vector<int> vec(size);

    std::random_device rd;
    std::default_random_engine e(rd());
    std::uniform_int_distribution<> uid(-size * 10, size * 10);
    for (mtl::size_t i = 0; i < size; ++i) {
        int tmp = uid(e);
        vec.push_back(tmp);
        v.push_back(tmp);
    }

    auto begin = std::chrono::high_resolution_clock::now();
    mtl::inplace_mergesort(v.begin(), v.end());
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << "Time taken for recursive merge sort: " << duration.count()
              << " microseconds" << std::endl;

    std::sort(vec.begin(), vec.end());

    for (size_t i = 0; i < size; ++i) {
        EXPECT_EQ(vec[i], v[i]);
    }
}

TEST(TestMergeSort, TestMergeSortIterative) {
    mtl::size_t size = 1000000;
    mtl::vector<int> v(size);
    std::vector<int> vec(size);

    std::random_device rd;
    std::default_random_engine e(rd());
    std::uniform_int_distribution<> uid(-size * 10, size * 10);
    for (mtl::size_t i = 0; i < size; ++i) {
        int tmp = uid(e);
        vec.push_back(tmp);
        v.push_back(tmp);
    }

    auto begin = std::chrono::high_resolution_clock::now();
    mtl::inplace_mergesort_iterative(v.begin(), v.end());
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << "Time taken for iterative merge sort: " << duration.count()
              << " microseconds" << std::endl;

    std::sort(vec.begin(), vec.end());

    for (size_t i = 0; i < size; ++i) {
        EXPECT_EQ(vec[i], v[i]);
    }
}

int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
