#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <mtl/types.h>
#include <algorithm>
#include <utility>

namespace mtl {
/* type Iterator: this type should overload ++, --, ==, != and *
 * operators, operator*() is used to dereference and return type is T&  */

template <typename T>
concept Iterator = requires(T a) {
    { a.operator*() };
    { a.operator->() };
    { a++ } -> std::same_as<T>;
    { a-- } -> std::same_as<T>;
    { ++a } -> std::same_as<T&>;
    { --a } -> std::same_as<T&>;
    { a == a } -> std::same_as<bool>;
    { a != a } -> std::same_as<bool>;
};

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

/* merge sort the sequence with range [begin, end) in ascending order in place*/
template <Iterator Iter>
void inplace_mergesor(Iter begin, Iter end);

/* merge two sorted sequences [begin, mid) and [mid, end) in place in ascending
 * order. a temporary buffer will be requested by new. */
template <Iterator Iter>
void inplace_merge(Iter begin, Iter mid, Iter end) noexcept;

/* find the middle point of a sequence by fast and slow pointers
 * if there are even numbers of elements, the smaller one will be returned */
template <Iterator Iter>
Iter find_mid(Iter begin, Iter end);

/* swap two object, note that it uses std::move */
template <typename T>
inline void swap(T& a, T& b) noexcept {
    auto c = std::move(a);
    a = std::move(b);
    b = std::move(c);
}

template <Iterator Iter>
void iter_swap(Iter itr1, Iter itr2) {
    swap(*itr1, *itr2);
}

template <Iterator Iter>
Iter advance(Iter itr, difference_t n);

template <typename T>
inline T abs(const T& x) {
    return x < 0 ? -x : x;
}

template <typename T>
[[nodiscard]] constexpr const T& max(const T& a, const T& b) {
    return a < b ? b : a;
}

template <typename T>
[[nodiscard]] constexpr const T& min(const T& a, const T& b) {
    return a > b ? b : a;
}

/* move the sequence [begin2, end2) to [begin1, end1), note that it uses
 * std::move, so you should ensure the length of the two ranges are the same,
 * and it won't be checked */
template <Iterator Iter1, Iterator Iter2>
void move_ranges(Iter1 dest_first, Iter1 dest_end, Iter2 src) noexcept;

/* copy the sequence [begin, end) into the sequence beginning with output,
 * You should ensure there is enough space in the output sequence , and it won't
 * be checked */
template <Iterator Iter1, Iterator Iter2>
void copy(Iter1 begin, Iter1 end, Iter2 output);

inline bool is_prime(size_t num) {
    if (num <= 1)
        return false;
    if (num <= 3)
        return true;
    if (num % 2 == 0 || num % 3 == 0)
        return false;
    for (size_t i = 5; i * i <= num; i += 6) {
        if (num % i == 0 || num % (i + 2) == 0)
            return false;
    }
    return true;
}

inline size_t next_prime(size_t n) {
    if (n <= 1)
        return 2;
    size_t prime = n;
    bool found = false;

    while (!found) {
        prime++;
        if (is_prime(prime)) {
            found = true;
        }
    }
    return prime;
}

template <Iterator Iter>
Iter advance(Iter itr, difference_t n) {
    if (n > 0) {
        while (n-- > 0) {
            ++itr;
        }
    } else {
        while (n++ < 0) {
            --itr;
        }
    }
    return itr;
}

template <Iterator Iter>
difference_t distance(Iter first, Iter last) {
    difference_t n = 0;
    while (first != last) {
        ++first;
        ++n;
    }
    return n;
}

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

template <Iterator Iter1, Iterator Iter2>
void move_ranges(Iter1 dest_begin, Iter1 dest_end, Iter2 src) noexcept {
    while (dest_begin != dest_end) {
        *(dest_begin++) = std::move(*(src++));
    }
}

template <Iterator Iter1, Iterator Iter2>
void copy(Iter1 begin, Iter1 end, Iter2 output) {
    while (begin != end) {
        *(output++) = *(begin++);
    }
}

template <Iterator Iter>
void inplace_mergesort(Iter begin, Iter end) {
    if (begin != end && begin != end - 1) {
        auto mid = find_mid(begin, end);
        inplace_mergesort(begin, mid);
        inplace_mergesort(mid, end);
        inplace_merge(begin, mid, end);
    }
}

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

template <Iterator Iter>
Iter find_mid(Iter begin, Iter end) {
    auto fast = begin;
    auto slow = begin;
    while (fast != end) {
        ++fast;
        if (fast == end) {
            break;
        }
        ++fast;
        ++slow;
    }
    return slow;
}
}  // namespace mtl

#endif
