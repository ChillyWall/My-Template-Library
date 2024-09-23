#ifndef TEST_VECTOR_H
#define TEST_VECTOR_H

#include <iostream>
#include <mtl/vector.h>

using std::ostream;

void test_constructor(ostream& os);
void test_push_pop_shrink(ostream& os);
void test_iterator(ostream& os);
void test_insert_remove(ostream& os);

template <typename T>
void print_vector(ostream& os, const typename mtl::vector<T>& vec);

template <typename T>
void print_vector(ostream& os, const typename mtl::vector<T>& vec) {
    using std::endl;
    os << "The size: " << vec.size() << endl
       << "The capacity: " << vec.capacity() << endl
       << "The elements: ";

    for (auto i = vec.begin(); i != vec.end(); ++i) {
        os << *i << ", ";
    }

    os << std::endl;
}

#endif
