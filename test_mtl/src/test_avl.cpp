#include <filesystem>
#include <fstream>
#include <mtl/avl_tree.h>
#include <test_mtl/myutils.h>

using namespace mtl;

void test_insert_remove(ostream& os) {
    os << "test insert and remove\n";
    avl_tree<int> avl;
    for (int i = 0; i < 100; ++i) {
        avl.insert(i);
    }
    print(os, avl);
    for (int i = 0; i < 50; ++i) {
        avl.remove(i);
    }
    print(os, avl);
}

int main() {
    std::filesystem::create_directories("avl");
    std::ofstream ofs("avl/test_insert_remove.txt");
    test_insert_remove(ofs);
    return 0;
}
