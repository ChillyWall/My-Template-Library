#ifndef MYUTILS_H
#define MYUTILS_H

#include <mtl/vector.h>
#include <iostream>

using std::ostream;

template <typename Container>
void print(ostream& os, const Container& c);

template <typename Container>
void print(ostream& os, const Container& c) {
    using std::endl;
    os << "The size: " << c.size() << endl
       << "The capacity: " << c.capacity() << endl
       << "The elements: ";

    for (auto i = c.begin(); i != c.end(); ++i) {
        os << *i << ", ";
    }

    os << std::endl;
}

#endif