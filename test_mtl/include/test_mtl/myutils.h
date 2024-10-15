#ifndef MYUTILS_H
#define MYUTILS_H

#include <mtl/vector.h>
#include <iostream>

using namespace mtl;
using std::ostream;

/* To print the basic information fo a vector and all of its elements. 
   Type T: it should overload the operator<<.
*/
template <typename T>
void print_vector(ostream& os, const vector<T>& v);

/* To print the basic information of a container and all of its elements. 
    Type Container: it should provide begin() and end(), which return a iterator.
    The type of elements should overload operator<< */
template <typename Container>
void print(ostream& os, const Container& c);

template <typename T>
void print_vector(ostream& os, const vector<T>& v) {
    using std::endl;
    os << "The size: " << v.size()
       << "\nThe capacity: " << v.capacity()
       << "\nThe elements: ";

    for (auto i = v.begin(); i != v.end(); ++i) {
        os << *i << ", ";
    }

    os << std::endl;
}


/* To print the basic information of a container and all of its elements. 
    Type Container: it should provide begin() and end(), which return a iterator.
    The type of elements should overload operator<< */
template <typename Container>
void print(ostream& os, const Container& c) {
    using std::endl;
    os << "The size: " << c.size()
       << "\nThe elements: ";
    
    for (auto i = c.begin(); i != c.end(); ++i) {
        os << *i << ", ";
    }
    os << std::endl;
}

#endif