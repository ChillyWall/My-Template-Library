#ifndef MTL_QUICK_SORT_H
#define MTL_QUICK_SORT_H

#include <mtl/mtldefs.h>

namespace mtl {

/* sort the array in place in ascending order (it will change the array
 * directly) the ranges is [begin, end) */
template <Iterator Iter>
void inplace_quicksort(Iter begin, Iter end);

/* perform partition for the sequence in range [begin, end)
 * all the elements smaller than the pivot are on the left side and thus the
 * ones greater on the right side. return the iterator to the first element of
 * the second group (the pivot) */
template <Iterator Iter>
Iter partition(Iter begin, Iter end) noexcept;

template <Iterator Iter>
void inplace_quicksort(Iter begin, Iter end) {
    if (begin != end) {
        auto mid = partition(begin, end);
        inplace_quicksort(begin, mid);
        ++mid;
        inplace_quicksort(mid, end);
    }
}

template <Iterator Iter>
Iter partition(Iter begin, Iter end) noexcept {
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

}  // namespace mtl

#endif
