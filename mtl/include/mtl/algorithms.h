#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <iostream>

namespace mtl {
    /*  count how many elements between two iterators
        type: Iterator, which must provides ++ and != operators */
    template <typename Iterator>
    size_t count_length(Iterator begin, Iterator end);

    /* sort the array in the original place (it will change the array directly)
       type Iterator, which should provides ++, -- and != operators 
       the ranges is [begin, end), return the iterator to the first element of the second group */
    template <typename Iterator>
    void inplace_quicksort(Iterator begin, Iterator end);

    template <typename Iterator>
    Iterator partition(Iterator begin, Iterator end) noexcept;

    template <typename T>
    void swap(T& a, T& b) noexcept;

    template <typename Iterator>
    size_t count_length(Iterator begin, Iterator end) {
        size_t len = 0;
        while (begin != end) {
            ++begin;
            ++len;
        }

        return len;
    }

    template <typename Iterator>
    void inplace_quicksort(Iterator begin, Iterator end) {
        if (begin != end) {
            auto mid = partition(begin, end);
            inplace_quicksort(begin, mid);
            inplace_quicksort(mid + 1, end);
        }
    }

    template <typename Iterator>
    Iterator partition(Iterator begin, Iterator end) noexcept {
        // the pivot
        auto pivot = std::move(*begin);
        while (begin != end) {
            do {
                --end;
            } while (begin != end && pivot < *end);
            if (begin == end) {
                break;
            }
            *begin = std::move(*end);

            do {
                ++begin;
            } while (begin != end && pivot > *begin);

            if (begin != end) {
                *end = std::move(*begin);
            }
        }
        *begin = std::move(pivot);
        return begin;
    }

    template <typename T>
    void swap(T& a, T& b) noexcept {
        auto c = std::move(a);
        a = std::move(b);
        b = std::move(c);
    }
}

#endif