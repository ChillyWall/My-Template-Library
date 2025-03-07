#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <mtl/types.h>
#include <utility>

namespace mtl {
/* type Iterator: this type should overload +, -, ++, --, ==, != and *
 * operators, operator*() is used to dereference and return type is T&  */

/* sort the array in place in ascending order (it will change the array
 * directly) the ranges is [begin, end) */
template <typename Iterator>
void inplace_quicksort(Iterator begin, Iterator end);

/* perform partition for the sequence in range [begin, end)
 * all the elements smaller than the pivot are on the left side and thus the
 * ones greater on the right side. return the iterator to the first element of
 * the second group (the pivot) */
template <typename Iterator>
Iterator partition(Iterator begin, Iterator end) noexcept;

/* merge sort the sequence with range [begin, end) in ascending order in place*/
template <typename Iterator>
void inplace_mergesort(Iterator begin, Iterator end);

/* merge two sorted sequences [begin, mid) and [mid, end) in place in ascending
 * order. a temporary buffer will be requested by new. */
template <typename Iterator>
void inplace_merge(Iterator begin, Iterator mid, Iterator end) noexcept;

/* find the middle point of a sequence by fast and slow pointers
 * if there are even numbers of elements, the smaller one will be returned */
template <typename Iterator>
Iterator find_mid(Iterator begin, Iterator end);

/* swap two object, note that it uses std::move */
template <typename T>
inline void swap(T& a, T& b) noexcept {
    auto c = std::move(a);
    a = std::move(b);
    b = std::move(c);
}

// return the greater one
template <typename T>
const T& max(const T& x, const T& y) {
    return x < y ? y : x;
}

// return the smaller one
template <typename T>
const T& min(const T& x, const T& y) {
    return x > y ? y : x;
}

template <typename T>
inline T abs(const T& x) {
    return x < 0 ? -x : x;
}

/* replace the sequence [begin1, end1) with [begin2, end2), note that it uses
 * std::move, so you should ensure the length of the two ranges are the same,
 * and it won't be checked */
template <typename Iterator1, typename Iterator2>
void replace(Iterator1 begin1, Iterator1 end1, Iterator2 begin2,
             Iterator2 end2) noexcept;

/* copy the sequence [begin, end) into the sequence beginning with output,
 * You should ensure there is enough space in the output sequence , and it won't
 * be checked */
template <typename Iterator1, typename Iterator2>
void copy(Iterator1 begin, Iterator1 end, Iterator2 output);

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

template <typename Iterator>
Iterator advance(Iterator itr, difference_t n) {
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

template <typename Iterator>
difference_t distance(Iterator first, Iterator last) {
    difference_t n = 0;
    while (first != last) {
        ++first;
        ++n;
    }
    return n;
}

template <typename Iterator>
void inplace_quicksort(Iterator begin, Iterator end) {
    if (begin != end) {
        auto mid = partition(begin, end);
        inplace_quicksort(begin, mid);
        ++mid;
        inplace_quicksort(mid, end);
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

template <typename Iterator1, typename Iterator2>
void replace(Iterator1 begin1, Iterator1 end1, Iterator2 begin2,
             Iterator2 end2) noexcept {
    while (begin1 != end1 && begin2 != end2) {
        *(begin1++) = std::move(*(begin2++));
    }
}

template <typename Iterator1, typename Iterator2>
void copy(Iterator1 begin, Iterator1 end, Iterator2 output) {
    while (begin != end) {
        *(output++) = *(begin++);
    }
}

template <typename Iterator>
void inplace_mergesort(Iterator begin, Iterator end) {
    if (begin != end && begin != end - 1) {
        auto mid = find_mid(begin, end);
        inplace_mergesort(begin, mid);
        inplace_mergesort(mid, end);
        inplace_merge(begin, mid, end);
    }
}

template <typename Iterator>
void inplace_merge(Iterator begin, Iterator mid, Iterator end) noexcept {
    using T = typename std::remove_reference_t<decltype(*begin)>;

    difference_t len1 = mid - begin;
    difference_t len2 = end - mid;

    auto buf = new T[len1 + len2];

    auto buf_begin1 = buf;
    auto buf_begin2 = buf + len1;

    auto buf_end1 = buf_begin2;
    auto buf_end2 = buf_end1 + len2;

    replace(buf_begin1, buf_end2, begin, end);

    for (auto itr = begin; itr < end; ++itr) {
        if (buf_begin1 == buf_end1) {
            replace(itr, end, buf_begin2, buf_end2);
            break;
        }
        if (buf_begin2 == buf_end2) {
            replace(itr, end, buf_begin1, buf_end1);
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

template <typename Iterator>
Iterator find_mid(Iterator begin, Iterator end) {
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
