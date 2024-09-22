#ifndef TEST_VECTOR_H
#define TEST_VECTOR_H

#include <iostream>
#include <mtl/vector.h>

void test_constructor(std::ostream& os);
void test_push_pop_shrink(std::ostream& os);
void test_insert_remove(std::ostream& os);

template <typename T>
void print_vector(std::ostream& os, const typename mtl::vector<T>& vec);

template <typename T>
void print_vector(std::ostream& os, const typename mtl::vector<T>& vec) {
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
