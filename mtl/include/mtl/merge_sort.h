#ifndef MTL_MERGE_SORT_H
#define MTL_MERGE_SORT_H

#include <mtl/mtldefs.h>
#include <mtl/mtlutils.h>
#include <mtl/pair.h>
#include <mtl/stack.h>

namespace mtl {

using std::tuple;

/* merge sort the sequence with range [begin, end) in ascending order in place*/
template <Iterator Iter>
void inplace_mergesort(Iter begin, Iter end) {
    if (begin != end && begin != advance(end, -1)) {
        auto mid = find_mid(begin, end);
        inplace_mergesort(begin, mid);
        inplace_mergesort(mid, end);
        inplace_merge(begin, mid, end);
    }
}

template <Iterator Iter>
void inplace_mergesort_iterative(Iter begin, Iter end) {
    using itr3 = tuple<Iter, Iter, Iter>;
    using node = pair<itr3, bool>;
    stack<node> st;
    st.push(node(itr3(begin, find_mid(begin, end), end), 0));
    while (!st.empty()) {
        auto& cur = st.top();
        if (cur.second) {
            auto& [begin, mid, end] = cur.first;
            inplace_merge(begin, mid, end);
            st.pop();
        } else {
            cur.second = 1;
            auto& [begin, mid, end] = cur.first;
            if (begin != end && begin != advance(end, -1)) {
                st.push(node(itr3(begin, find_mid(begin, mid), mid), 0));
                st.push(node(itr3(mid, find_mid(mid, end), end), 0));
            } else {
                st.pop();
            }
        }
    }
}

/* merge two sorted sequences [begin, mid) and [mid, end) in place in ascending
 * order. a temporary buffer will be requested by new. */
template <Iterator Iter>
void inplace_merge(Iter begin, Iter mid, Iter end) noexcept {
    using T = typename std::remove_reference_t<decltype(*begin)>;

    difference_t len1 = distance(begin, mid);
    difference_t len2 = distance(mid, end);

    auto buf = new T[len1];

    auto buf_begin1 = buf;
    auto buf_end1 = buf + len1;

    auto buf_begin2 = mid;
    auto buf_end2 = end;

    move_ranges(begin, mid, buf_begin1);

    for (auto itr = begin; itr < end; ++itr) {
        if (buf_begin1 == buf_end1) {
            move_ranges(buf_begin2, buf_end2, itr);
            break;
        }
        if (buf_begin2 == buf_end2) {
            move_ranges(buf_begin1, buf_end1, itr);
            break;
        }
        if (*buf_begin2 < *buf_begin1) {
            *itr = std::move(*(buf_begin2++));
        } else {
            *itr = std::move(*(buf_begin1++));
        }
    }

    delete[] buf;
}

}  // namespace mtl

#endif
