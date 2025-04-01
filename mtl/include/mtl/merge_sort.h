#ifndef MTL_MERGE_SORT_H
#define MTL_MERGE_SORT_H

#include <mtl/mtldefs.h>

namespace mtl {

/* merge sort the sequence with range [begin, end) in ascending order in place*/
template <Iterator Iter>
void inplace_mergesort(Iter begin, Iter end) {
    if (begin != end && begin != end - 1) {
        auto mid = find_mid(begin, end);
        inplace_mergesort(begin, mid);
        inplace_mergesort(mid, end);
        inplace_merge(begin, mid, end);
    }
}

/* merge two sorted sequences [begin, mid) and [mid, end) in place in ascending
 * order. a temporary buffer will be requested by new. */
template <Iterator Iter>
void inplace_merge(Iter begin, Iter mid, Iter end) noexcept {
    using T = typename std::remove_reference_t<decltype(*begin)>;

    difference_t len1 = mid - begin;
    difference_t len2 = end - mid;

    auto buf = new T[len1 + len2];

    auto buf_begin1 = buf;
    auto buf_begin2 = buf + len1;

    auto buf_end1 = buf_begin2;
    auto buf_end2 = buf_end1 + len2;

    move_ranges(buf_begin1, buf_end2, begin, end);

    for (auto itr = begin; itr < end; ++itr) {
        if (buf_begin1 == buf_end1) {
            move_ranges(itr, end, buf_begin2, buf_end2);
            break;
        }
        if (buf_begin2 == buf_end2) {
            move_ranges(itr, end, buf_begin1, buf_end1);
            break;
        }
        if (*buf_begin2 > *buf_begin1) {
            *itr = std::move(*(buf_begin2++));
        } else {
            *itr = std::move(*(buf_begin1++));
        }
    }

    delete[] buf;
}

}  // namespace mtl

#endif
