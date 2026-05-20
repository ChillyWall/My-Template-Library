export module mtl.algorithms:merge_sort;

import mtl.core;
import mtl.deque;
import mtl.pair;
import mtl.stack;
import mtl.vector;
import std;

namespace mtl {
using std::tuple;
}

export namespace mtl {

/**
 * @brief Sort the range [begin, end) in ascending order in place.
 * @tparam Iter Iterator type.
 * @param begin Range start iterator.
 * @param end Range end iterator.
 */
template <Iterator Iter>
void inplace_mergesort(Iter begin, Iter end) {
    if (begin != end && begin != advance(end, -1)) {
        auto mid = find_mid(begin, end);
        inplace_mergesort(begin, mid);
        inplace_mergesort(mid, end);
        inplace_merge(begin, mid, end);
    }
}

/**
 * @brief Sort the range [begin, end) in ascending order using iterative merge sort.
 * @tparam Iter Iterator type.
 * @param begin Range start iterator.
 * @param end Range end iterator.
 */
template <Iterator Iter>
void inplace_mergesort_iterative(Iter begin, Iter end) {
    using itr3 = tuple<Iter, Iter, Iter>;
    using node = pair<itr3, bool>;
    stack<node> sta;
    sta.push(node(itr3(begin, find_mid(begin, end), end), 0));
    while (!sta.empty()) {
        auto& cur = sta.top();
        if (cur.second) {
            auto& [begin, mid, end] = cur.first;
            inplace_merge(begin, mid, end);
            sta.pop();
        } else {
            cur.second = 1;
            auto& [begin, mid, end] = cur.first;
            if (begin != end && begin != advance(end, -1)) {
                sta.push(node(itr3(begin, find_mid(begin, mid), mid), 0));
                sta.push(node(itr3(mid, find_mid(mid, end), end), 0));
            } else {
                sta.pop();
            }
        }
    }
}

/**
 * @brief Sort the range [begin, end) in ascending order using parallel merge sort.
 * @tparam Iter Iterator type.
 * @param begin Range start iterator.
 * @param end Range end iterator.
 */
template <Iterator Iter>
void parallel_inplace_mergesort(Iter begin, Iter end) {
    if (begin != end && begin != advance(end, -1)) {
        if (mtl::distance(begin, end) < 1024) {
            inplace_mergesort(begin, end);
        } else {
            auto mid = find_mid(begin, end);
            auto left =
                std::async(std::launch::async, parallel_inplace_mergesort<Iter>,
                           begin, mid);
            auto right = std::async(std::launch::async,
                                    parallel_inplace_mergesort<Iter>, mid, end);
            left.wait();
            right.wait();
            inplace_merge(begin, mid, end);
        }
    }
}

/**
 * @brief Merge sorted ranges [begin, mid) and [mid, end) in place.
 * @tparam Iter Iterator type.
 * @param begin Range start iterator.
 * @param mid Middle iterator separating the two ranges.
 * @param end Range end iterator.
 */
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
