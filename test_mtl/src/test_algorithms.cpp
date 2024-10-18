#include <test_mtl/test_algorithms.h>
#include <test_mtl/myutils.h>
#include <mtl/algorithms.h>
#include <mtl/vector.h>
#include <fstream>
#include <random>
#include <chrono>

void test_quicksort(ostream& os) {
    mtl::vector<int> vec;
    using namespace std::chrono;

    std::random_device rd;
    std::default_random_engine e(rd());
    std::uniform_int_distribution<> uid(-100000, 100000);
    for (size_t i = 0; i < 10000; ++i) {
        vec.push_back(uid(e));
    }

    os << "the original: \n";
    print(os, vec);

    auto start = system_clock::now();
    mtl::inplace_quicksort(vec.begin(), vec.end());
    auto end = system_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    os << "Time costs: " << double(duration.count()) * microseconds::period::num / microseconds::period::den << "\n";

    os << "after sorting: \n";
    print(os, vec);
}

void test_mergesort(ostream& os) {
    mtl::vector<int> vec;
    using namespace std::chrono;

    std::random_device rd;
    std::default_random_engine e(rd());
    std::uniform_int_distribution<> uid(-100000, 100000);
    for (size_t i = 0; i < 10000; ++i) {
        vec.push_back(uid(e));
    }

    os << "the original: \n";
    print(os, vec);

    auto start = system_clock::now();
    mtl::inplace_mergesort(vec.begin(), vec.end());
    auto end = system_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    os << "Time costs: " << double(duration.count()) * microseconds::period::num / microseconds::period::den << "\n";

    os << "after sorting: \n";
    print(os, vec);
}

int main() {
    using std::ofstream;
    ofstream ofs("test_mergesort.txt");
    test_mergesort(ofs);

    return 0;
}