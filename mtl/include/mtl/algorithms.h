#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <iostream>

namespace mtl {
    /*  count how many elements between two iterators
        type: Iteator, which must provides ++ and != operator
    */
    template <typename Iterator>
    size_t count_length(Iterator begin, Iterator end);

    template <typename Iterator>
    size_t count_length(Iterator begin, Iterator end) {
        size_t size = 0;
        while (begin != end) {
            ++begin;
            ++size;
        }

        return size;
    }
}

#endif