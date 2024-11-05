#include <fstream>
#include <mtl/list.h>
#include <test_mtl/myutils.h>
#include <test_mtl/test_list.h>

using mtl::list;
using std::endl;
using std::ofstream;
using std::ostream;

void test_constructor(ostream& os) {
    os << "1. The default constructor: " << endl;
    list<int> ls1;
    print(os, ls1);
    os << endl;

    os << "2. list(initializer_list)";
    list<int> ls2({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
    print(os, ls2);
    os << endl;

    os << "\n3. copy (the 2)\n";
    list<int> ls3(ls2);
    print(os, ls3);
    os << endl;

    os << "\n4. moving (the 2)\n";
    list<int> ls4(std::move(ls2));
    os << "The 4:\n";
    print(os, ls4);
    os << endl;
    os << "The 2 after moving\n";
    print(os, ls2);
    os << endl;
}

void test_push_pop(ostream& os) {
    list<int> ls;
    os << "The original: " << endl;
    print(os, ls);

    os << "push back a right-value and a normal variable" << endl;
    ls.push_back(0);
    int num = 1;
    ls.push_back(num);
    print(os, ls);

    for (int i = 2; i < 201; ++i) {
        ls.push_back(i);
    }
    os << " after pushing back 201 numbers" << endl;
    print(os, ls);

    for (int i = 0; i < 50; ++i) {
        ls.pop_back();
    }
    os << "after popping back 50 numbers" << endl;
    print(os, ls);

    while (!ls.empty()) {
        ls.pop_back();
    }

    try {
        ls.pop_back();
    } catch (const std::exception& exc) {
        os << "when an empty list trying to pop out: " << exc.what() << endl;
    }
}

void test_iterator(ostream& os) {
    list<int> ls;
    for (int i = 0; i < 100; ++i) {
        ls.push_back(i);
    }

    print(os, ls);
    os << "print the list with iterator: \n";
    for (auto itr = ls.begin(); itr != ls.end(); ++itr) {
        os << *itr << ", ";
    }

    os << "\nprint the list in reversed order: \n";
    for (auto itr = ls.end() - 1ULL; itr != ls.head(); --itr) {
        os << *itr << ", ";
    }
}

void test_insert_remove(ostream& os) {
    list<int> ls;
    for (int i = 0; i < 10; ++i) {
        ls.push_back(i);
    }

    os << "The original list: " << endl;
    print(os, ls);

    auto itr = ls.insert(ls.begin() + 5ull, 10);
    os << "after inserting 10 at position 5: " << endl;
    print(os, ls);
    os << "the returned iterator points to: " << *itr << endl;

    list<int> ls1({11, 12, 13, 14});

    itr = ls.insert(ls.begin() + 2, ls1.begin(), ls1.end());
    os << "after inserting {11, 12, 13, 14} at position 2" << endl;
    print(os, ls);
    os << "the returned iterator points to: " << *itr << endl;

    itr = ls.remove(ls.begin() + 4);
    os << "after removing at position 4" << endl;
    print(os, ls);
    os << "the returned iterator points to: " << *itr << endl;

    itr = ls.remove(ls.begin() + 2, ls.begin() + 7);
    os << "after removing range [2, 7)" << endl;
    print(os, ls);
    os << "the returned iterator points to: " << *itr << endl;
}

int main() {
    ofstream ofs1("list_test_constructor.txt");
    if (ofs1.is_open())
        test_constructor(ofs1);

    ofstream ofs2("list_test_push_pop.txt");
    if (ofs2.is_open())
        test_push_pop(ofs2);

    ofstream ofs3("list_test_iterator.txt");
    if (ofs3.is_open())
        test_iterator(ofs3);

    ofstream ofs4("list_test_insert_remove.txt");
    if (ofs4.is_open())
        test_insert_remove(ofs4);
}
