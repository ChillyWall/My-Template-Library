#include <fstream>
#include <test_mtl/myutils.h>
#include <test_mtl/test_deque.h>

using namespace mtl;

void test_push_pop(ostream& os) {
    deque<int> deq;
    for (int i = 0; i < 100; ++i) {
        deq.push_back(i);
    }
    print(os, deq);
}

int main() {
    std::ofstream ofs("test_deque.txt");
    test_push_pop(ofs);
    return 0;
}
