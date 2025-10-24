#ifndef MTL_UTILS_H
#define MTL_UTILS_H

#include <mtl/mtldefs.h>

namespace mtl {

using std::move;
using std::swap;

template <Iterator Iter>
inline void iter_swap(Iter itr1, Iter itr2) {
    swap(*itr1, *itr2);
}

template <typename T>
inline T abs(const T& x) {
    return x < 0 ? -x : x;
}

template <typename T>
[[nodiscard]] constexpr const T& max(const T& a) {
    return a;
}

template <typename T>
[[nodiscard]] constexpr const T& min(const T& a) {
    return a;
}

template <typename T, typename... Args>
[[nodiscard]] constexpr const T& max(const T& a, const Args&... args) {
    const T& b = max(args...);
    return (a < b) ? b : a;
}

template <typename T, typename... Args>
[[nodiscard]] constexpr const T& min(const T& a, const Args&... args) {
    const T& b = min(args...);
    return (b < a) ? b : a;
}

template <typename... Args>
[[nodiscard]] constexpr auto sum(Args... args) {
    return (... + args);
}

template <typename... Args>
[[nodiscard]] constexpr auto prod(Args... args) {
    return (... * args);
}

inline bool is_prime(size_t num) {
    if (num <= 1) {
        return false;
    }
    if (num <= 3) {
        return true;
    }
    if (num % 2 == 0 || num % 3 == 0) {
        return false;
    }
    for (size_t i = 5; i * i <= num; i += 6) {
        if (num % i == 0 || num % (i + 2) == 0) {
            return false;
        }
    }
    return true;
}

inline size_t next_prime(size_t n) {
    if (n <= 1) {
        return 2;
    }
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

template <RandomIterator Iter>
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

template <RandomIterator Iter>
difference_t distance(Iter first, Iter last) {
    difference_t n = 0;
    while (first != last) {
        ++first;
        ++n;
    }
    return n;
}

/* move the sequence [src_begin, src_end) to dest, note that it uses move, so
 * you should ensure the length of the two ranges are the same, and it won't be
 * checked */
template <Iterator Iter1, Iterator Iter2>
void move_ranges(Iter1 begin, Iter1 end, Iter2 dest) noexcept {
    while (begin != end) {
        *(dest++) = move(*(begin++));
    }
}

/* copy the sequence [begin, end) into the sequence beginning with output,
 * You should ensure there is enough space in the output sequence , and it won't
 * be checked */
template <Iterator Iter1, Iterator Iter2>
void copy(Iter1 begin, Iter1 end, Iter2 output) {
    while (begin != end) {
        *(output++) = *(begin++);
    }
}

/* find the middle point of a sequence by fast and slow pointers
 * if there are even numbers of elements, the smaller one will be returned */
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

/* find the middle of a sequence by random access */
template <RandomIterator Iter>
Iter find_mid(Iter begin, Iter end) {
    return begin + ((end - begin) / 2);
}

}  // namespace mtl

#endif
