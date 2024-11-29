#include <filesystem>
#include <fstream>
#include <test_mtl/myutils.h>
#include <test_mtl/test_deque.h>

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

void test_iterator(ostream& os) {

}

int main() {
    std::filesystem::create_directories("deque");
    std::ofstream ofs("deque/test_deque.txt");
    test_push_pop(ofs);
    return 0;
}
