#include <filesystem>
#include <fstream>
#include <iostream>
#include <mtl/vector.h>
#include <test_mtl/myutils.h>

using mtl::vector;
using std::endl;
using std::ofstream;
using std::ostream;

void test_constructor(ostream& os) {
    os << "1. The default constructor: " << endl;
    vector<int> vec1;
    print_vector(os, vec1);

    os << "\n2. vector(size) (size = 10 < DEFAULT_CAPACITY)\n";
    vector<int> vec2(10);
    print_vector(os, vec2);

    os << "\n3. vector(size) (size = 200 > DEFAULT_CAPACITY)\n";
    vector<int> vec3(200);
    print_vector(os, vec3);

    os << "\n4. vector(initializer_list)\n";
    vector<int> vec4({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
    print_vector(os, vec4);

    os << "\n5. copy (the 4)\n";
    vector<int> vec5(vec4);
    print_vector(os, vec5);

    os << "\n6. moving (the 4)\n";
    vector<int> vec6(std::move(vec4));
    os << "The 6:\n";
    print_vector(os, vec6);
    os << "The 4 after moving\n";
    print_vector(os, vec4);
}

void test_push_pop_shrink(ostream& os) {
    vector<int> vec;
    os << "The original: " << endl;
    print_vector(os, vec);

    os << "push back a right-value and a normal variable" << endl;
    vec.push_back(0);
    int num = 1;
    vec.push_back(num);
    print_vector(os, vec);

    for (int i = 2; i < 201; ++i) {
        vec.push_back(i);
    }
    os << " after pushing back 201 numbers" << endl;
    print_vector(os, vec);

    for (int i = 0; i < 50; ++i) {
        vec.pop_back();
    }
    os << "after popping back 50 numbers" << endl;
    print_vector(os, vec);

    vec.shrink();
    os << "after shrinking" << endl;
    print_vector(os, vec);

    while (!vec.empty()) {
        vec.pop_back();
    }

    try {
        vec.pop_back();
    } catch (const std::exception& exc) {
        os << "when an empty vector trying to pop out: " << exc.what() << endl;
    }
}

void test_iterator(ostream& os) {
    vector<int> vec;
    for (int i = 0; i < 100; ++i) {
        vec.push_back(i);
    }

    print_vector(os, vec);
    os << "print the vector with iterator: \n";
    for (auto itr = vec.begin(); itr != vec.end(); ++itr) {
        os << *itr << ", ";
    }

    os << "\nprint the elements with even indices: \n";
    for (auto itr = vec.begin(); itr < vec.end(); itr += 2) {
        os << *itr << ", ";
    }

    os << "\nprint the vector in reversed order: \n";
    for (auto itr = vec.end() - 1ULL; itr >= vec.begin(); --itr) {
        os << *itr << ", ";
    }
}

void test_insert_remove(ostream& os) {
    vector<int> vec;
    for (int i = 0; i < 10; ++i) {
        vec.push_back(i);
    }

    os << "The original vector: " << endl;
    print_vector(os, vec);

    auto itr = vec.insert(vec.begin() + 5, 10);
    os << "after inserting 10 at position 5: " << endl;
    print_vector(os, vec);
    os << "the returned iterator points to: " << *itr << endl;

    vector<int> vec1({11, 12, 13, 14});

    itr = vec.insert(vec.begin() + 2, vec1.begin(), vec1.end());
    os << "after inserting {11, 12, 13, 14} at position 2" << endl;
    print_vector(os, vec);
    os << "the returned iterator points to: " << *itr << endl;

    itr = vec.remove(vec.begin() + 4);
    os << "after removing at position 4" << endl;
    print_vector(os, vec);
    os << "the returned iterator points to: " << *itr << endl;

    itr = vec.remove(vec.begin() + 2, vec.begin() + 7);
    os << "after removing range [2, 7)" << endl;
    print_vector(os, vec);
    os << "the returned iterator points to: " << *itr << endl;
}

int main() {
    std::filesystem::create_directories("vector");
    ofstream ofs1("vector/test_constructor.txt");
    test_constructor(ofs1);

    ofstream ofs2("vector/test_push_pop_shrink.txt");
    test_push_pop_shrink(ofs2);

    ofstream ofs3("vector/test_iterator.txt");
    test_iterator(ofs3);

    ofstream ofs4("vector/test_insert_remove.txt");
    test_insert_remove(ofs4);

    return 0;
}
