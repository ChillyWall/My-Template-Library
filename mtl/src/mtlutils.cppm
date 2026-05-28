export module mtl.core:utils;

import :defs;
import std;

export namespace mtl {

using std::swap;

/**
 * @brief Swap the values pointed to by two iterators.
 *
 * @tparam Iter Iterator type pointing to swappable values.
 * @param itr1 First iterator.
 * @param itr2 Second iterator.
 */
template <Iterator Iter>
inline void iter_swap(Iter itr1, Iter itr2) {
    swap(*itr1, *itr2);
}

/**
 * @brief Compute the absolute value of x.
 *
 * @tparam T Numeric type supporting comparison with 0 and unary negation.
 * @param x Value to take absolute of.
 * @return Absolute value of x.
 */
template <typename T>
inline T abs(const T& x) {
    return x < 0 ? -x : x;
}

/**
 * @brief Return the single-element maximum (identity overload).
 *
 * @tparam T Element type.
 * @param a The only element.
 * @return The same element reference a.
 */
template <typename T>
[[nodiscard]] constexpr const T& max(const T& a) {
    return a;
}

/**
 * @brief Return the single-element minimum (identity overload).
 *
 * @tparam T Element type.
 * @param a The only element.
 * @return The same element reference a.
 */
template <typename T>
[[nodiscard]] constexpr const T& min(const T& a) {
    return a;
}

/**
 * @brief Return the maximum of multiple values.
 *
 * @tparam T Element type.
 * @tparam Args Additional argument types (same as T).
 * @param a First value.
 * @param args Remaining values.
 * @return Reference to the largest value among the arguments.
 */
template <typename T, typename... Args>
[[nodiscard]] constexpr const T& max(const T& a, const Args&... args) {
    const T& b = max(args...);
    return (a < b) ? b : a;
}

/**
 * @brief Return the minimum of multiple values.
 *
 * @tparam T Element type.
 * @tparam Args Additional argument types (same as T).
 * @param a First value.
 * @param args Remaining values.
 * @return Reference to the smallest value among the arguments.
 */
template <typename T, typename... Args>
[[nodiscard]] constexpr const T& min(const T& a, const Args&... args) {
    const T& b = min(args...);
    return (b < a) ? b : a;
}

/**
 * @brief Sum a parameter pack of values using fold expression.
 *
 * @tparam Args Types of values to sum.
 * @param args Values to sum.
 * @return Sum of all arguments.
 */
template <typename... Args>
[[nodiscard]] constexpr auto sum(Args... args) {
    return (... + args);
}

/**
 * @brief Multiply a parameter pack of values using fold expression.
 *
 * @tparam Args Types of values to multiply.
 * @param args Values to multiply.
 * @return Product of all arguments.
 */
template <typename... Args>
[[nodiscard]] constexpr auto prod(Args... args) {
    return (... * args);
}

/**
 * @brief Test whether a number is prime.
 *
 * @param num Number to test.
 * @return true if num is prime, false otherwise.
 */
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

/**
 * @brief Find the next prime strictly greater than n.
 *
 * @param n Starting number.
 * @return The smallest prime > n (or 2 if n <= 1).
 */
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

/**
 * @brief Advance an input iterator by n steps.
 *
 * @tparam Iter Iterator type supporting increment/decrement.
 * @param itr Iterator to advance.
 * @param n Number of steps to advance (negative to move backwards).
 * @return Iterator after advancing n steps.
 */
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

/**
 * @brief Advance a random-access iterator by n steps.
 *
 * Behaves like the input iterator overload but optimized for random access.
 *
 * @tparam Iter Random-access iterator type.
 * @param itr Iterator to advance.
 * @param n Number of steps to advance (negative to move backwards).
 * @return Iterator after advancing n steps.
 */
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

/**
 * @brief Compute distance between two input iterators by walking.
 *
 * @tparam Iter Input iterator type.
 * @param first Begin iterator.
 * @param last End iterator.
 * @return Number of increments from first to last.
 */
template <Iterator Iter>
difference_t distance(Iter first, Iter last) {
    difference_t n = 0;
    while (first != last) {
        ++first;
        ++n;
    }
    return n;
}

/**
 * @brief Compute distance between two random-access iterators.
 *
 * Implementation walks the range; could be replaced with subtraction.
 *
 * @tparam Iter Random-access iterator type.
 * @param first Begin iterator.
 * @param last End iterator.
 * @return Number of increments from first to last.
 */
template <RandomIterator Iter>
difference_t distance(Iter first, Iter last) {
    difference_t n = 0;
    while (first != last) {
        ++first;
        ++n;
    }
    return n;
}

/**
 * @brief Move elements from [begin, end) into the destination range.
 *
 * @tparam Iter1 Source iterator type.
 * @tparam Iter2 Destination iterator type.
 * @param begin Begin of source range.
 * @param end End of source range.
 * @param dest Begin of destination range.
 * @note The caller must ensure the destination has enough space; lengths are
 * not checked.
 */
template <Iterator Iter1, Iterator Iter2>
void move_ranges(Iter1 begin, Iter1 end, Iter2 dest) noexcept {
    while (begin != end) {
        *(dest++) = std::move(*(begin++));
    }
}

/**
 * @brief Copy elements from [begin, end) to an output range.
 *
 * @tparam Iter1 Source iterator type.
 * @tparam Iter2 Output iterator type.
 * @param begin Begin of source range.
 * @param end End of source range.
 * @param output Begin of output range.
 * @note The caller must ensure the output range has sufficient space.
 */
template <Iterator Iter1, Iterator Iter2>
void copy(Iter1 begin, Iter1 end, Iter2 output) {
    while (begin != end) {
        *output = *begin;
        ++begin;
        ++output;
    }
}

/**
 * @brief Find the middle iterator of a sequence using fast/slow pointers.
 *
 * For even-length ranges returns the earlier (lower) middle.
 *
 * @tparam Iter Iterator type supporting increment.
 * @param begin Begin iterator.
 * @param end End iterator.
 * @return Iterator pointing to the middle element.
 */
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

/**
 * @brief Find the middle iterator of a random-access sequence.
 *
 * @tparam Iter Random-access iterator type.
 * @param begin Begin iterator.
 * @param end End iterator.
 * @return Iterator pointing to the middle element (lower for even lengths).
 */
template <RandomIterator Iter>
Iter find_mid(Iter begin, Iter end) {
    return begin + ((end - begin) / 2);
}

/**
 * @brief Construct an element in-place at data[index] with forwarded arguments.
 *
 * @tparam T Element type.
 * @tparam Args Constructor argument types.
 * @param data Pointer to element storage.
 * @param index Index at which to construct.
 * @param args Arguments forwarded to T's constructor.
 */
template <typename T, typename... Args>
void construct(T* data, size_t index, Args&&... args) {
    std::construct_at(data + index, std::forward<Args>(args)...);
}

/**
 * @brief Default-construct a contiguous sequence of objects.
 *
 * @tparam T Element type.
 * @param data Pointer to storage begin.
 * @param size Number of elements to default-construct.
 */
template <typename T>
void construct_all(T* data, size_t size) {
    std::uninitialized_default_construct_n(data, size);
}

/**
 * @brief Destroy the object at data[index].
 *
 * @tparam T Element type.
 * @param data Pointer to storage.
 * @param index Index of element to destroy.
 */
template <typename T>
void destroy(T* data, size_t index) noexcept {
    std::destroy_at(data + index);
}

/**
 * @brief Destroy a contiguous sequence of objects.
 *
 * @tparam T Element type.
 * @param data Pointer to storage begin.
 * @param size Number of elements to destroy.
 */
template <typename T>
void destroy_all(T* data, size_t size) noexcept {
    std::destroy_n(data, size);
}

}  // namespace mtl
