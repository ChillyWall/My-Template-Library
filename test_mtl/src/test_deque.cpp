#include <mtl/deque.h>
#include <test_mtl/myutils.h>
#include <filesystem>
#include <fstream>

using namespace mtl;

void test_push_pop(ostream& os) {
    os << "test push and pop\n";
    deque<int> deq;
    for (int i = 0; i < 100; ++i) {
        deq.push_back(i);
        deq.push_front(-i - 1);
    }
    print(os, deq);
    for (int i = 0; i < 50; ++i) {
        deq.pop_front();
        deq.pop_back();
    }
    print(os, deq);
}

void test_random_access(ostream& os) {
    os << "test random access\n";
    deque<int> deq;
    for (int i = 0; i < 100; ++i) {
        deq.push_back(i);
    }
    print(os, deq);
    for (int i = 0; i < 100; ++i) {
        os << deq[i] << ", ";
    }
}

int main() {
    std::filesystem::create_directories("deque");
    std::ofstream ofs("deque/test_deque.txt");
    test_random_access(ofs);
    return 0;
}
