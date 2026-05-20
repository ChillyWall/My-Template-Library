export module mtl.algorithms:quick_sort;

import mtl.core;
import mtl.pair;
import mtl.stack;
import std;

export namespace mtl {

/**
 * @brief Sort the range [begin, end) in ascending order in place.
 * @tparam Iter Iterator type.
 * @param begin Range start iterator.
 * @param end Range end iterator.
 */
template <Iterator Iter>
void inplace_quicksort(Iter begin, Iter end);

/**
 * @brief Partition the range [begin, end) around a pivot.
 * @tparam Iter Iterator type.
 * @param begin Range start iterator.
 * @param end Range end iterator.
 * @return Iterator to the pivot position after partitioning.
 */
template <Iterator Iter>
Iter partition(Iter begin, Iter end) noexcept;

/**
 * @brief Sort the range [begin, end) using recursive quicksort.
 * @tparam Iter Iterator type.
 * @param begin Range start iterator.
 * @param end Range end iterator.
 */
template <Iterator Iter>
void inplace_quicksort(Iter begin, Iter end) {
    if (begin != end) {
        auto mid = partition(begin, end);
        inplace_quicksort(begin, mid);
        inplace_quicksort(++mid, end);
    }
}

/**
 * @brief Sort the range [begin, end) using iterative quicksort.
 * @tparam Iter Iterator type.
 * @param begin Range start iterator.
 * @param end Range end iterator.
 */
template <Iterator Iter>
void inplace_quicksort_iterative(Iter begin, Iter end) {
    stack<pair<Iter, Iter>> st;
    st.push(pair<Iter, Iter>(begin, end));
    while (!st.empty()) {
        auto cur = st.top();
        st.pop();
        if (cur.first == cur.second) {
            continue;
        }
        auto mid = partition(cur.first, cur.second);
        st.push(pair<Iter, Iter>(cur.first, mid));
        st.push(pair<Iter, Iter>(++mid, cur.second));
    }
}

/**
 * @brief Partition the range [begin, end) by moving elements around a pivot.
 * @tparam Iter Iterator type.
 * @param begin Range start iterator.
 * @param end Range end iterator.
 * @return Iterator to the pivot position after partitioning.
 */
template <Iterator Iter>
Iter partition(Iter begin, Iter end) noexcept {
    // the pivot
    auto pivot = std::move(*begin);
    while (begin != end) {
        --end;
        while (begin != end && pivot < *end) {
            --end;
        };
        if (begin == end) {
            break;
        }
        *begin = std::move(*end);

        ++begin;
        while (begin != end && pivot > *begin) {
            ++begin;
        };

        if (begin != end) {
            *end = std::move(*begin);
        }
    }
    *begin = std::move(pivot);
    return begin;
}

}  // namespace mtl
