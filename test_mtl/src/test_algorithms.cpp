#include <mtl/algorithms.h>
#include <mtl/vector.h>
#include <test_mtl/myutils.h>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <random>
#include <vector>

void test_quicksort(ostream& os) {
    mtl::size_t size;
    std::cout << "testing quicksort, type the size: ";
    std::cin >> size;
    mtl::vector<int> vec(size);
    using namespace std::chrono;

    std::random_device rd;
    std::default_random_engine e(rd());
    std::uniform_int_distribution<> uid(-size * 10, size * 10);
    for (mtl::size_t i = 0; i < size; ++i) {
        vec.push_back(uid(e));
    }

    os << "the original: \n";
    print(os, vec);

    auto start = system_clock::now();
    mtl::inplace_quicksort(vec.begin(), vec.end());
    auto end = system_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    os << "Time costs: "
       << double(duration.count()) * microseconds::period::num /
            microseconds::period::den
       << "\n";

    os << "after sorting: \n";
    print(os, vec);
}

void test_mergesort(ostream& os) {
    mtl::size_t size;
    std::cout << "testing mergesort, type size: ";
    std::cin >> size;
    mtl::vector<int> vec(size);
    using namespace std::chrono;

    std::random_device rd;
    std::default_random_engine e(rd());
    std::uniform_int_distribution<> uid(-size * 10, size * 10);
    for (mtl::size_t i = 0; i < size; ++i) {
        vec.push_back(uid(e));
    }

    os << "the original: \n";
    print(os, vec);

    auto start = system_clock::now();
    mtl::inplace_mergesort(vec.begin(), vec.end());
    auto end = system_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    os << "Time costs: "
       << double(duration.count()) * microseconds::period::num /
            microseconds::period::den
       << "\n";

    os << "after sorting: \n";
    print(os, vec);
}

void test_stl_sort(ostream& os) {
    mtl::size_t size;
    std::cout << "testing stl sort, type sie: ";
    std::cin >> size;
    std::vector<int> vec(size);
    using namespace std::chrono;

    std::random_device rd;
    std::default_random_engine e(rd());
    std::uniform_int_distribution<> uid(-size * 10, size * 10);
    for (mtl::size_t i = 0; i < size; ++i) {
        vec.push_back(uid(e));
    }

    os << "the original: \n";
    print(os, vec);

    auto start = system_clock::now();
    std::sort(vec.begin(), vec.end());
    auto end = system_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    os << "Time costs: "
       << double(duration.count()) * microseconds::period::num /
            microseconds::period::den
       << "\n";

    os << "after sorting: \n";
    print(os, vec);
}

int main() {
    using std::ofstream;
    std::filesystem::create_directories("algorithm");
    ofstream ofs1("algorithm/test_quicksort.txt");
    test_quicksort(ofs1);
    ofstream ofs2("algorithm/test_mergesort.txt");
    test_mergesort(ofs2);
    return 0;
}
