#ifndef MTL_DEFS_H
#define MTL_DEFS_H

#include <cstddef>
#include <exception>

namespace mtl {
using std::size_t;
using difference_t = std::ptrdiff_t;

struct NullIterator : public std::exception {
    const char* msg_;

    NullIterator() : msg_("This iterator is null.") {}
    NullIterator(const NullIterator&) = default;
    NullIterator(NullIterator&&) = delete;
    NullIterator& operator=(const NullIterator&) = default;
    NullIterator& operator=(NullIterator&&) = delete;
    explicit NullIterator(const char* msg) : msg_(msg) {}
    ~NullIterator() noexcept override = default;
    virtual const char* what() {
        return msg_;
    }
};

struct EmptyContainer : public std::exception {
    const char* msg_;

    EmptyContainer() : msg_("This container is empty.") {}
    EmptyContainer(const EmptyContainer&) = default;
    EmptyContainer(EmptyContainer&&) = delete;
    EmptyContainer& operator=(const EmptyContainer&) = default;
    EmptyContainer& operator=(EmptyContainer&&) = delete;
    explicit EmptyContainer(const char* msg) : msg_(msg) {}
    ~EmptyContainer() noexcept override = default;
    virtual const char* what() {
        return msg_;
    }
};

/**
 * @brief to constrain the type of T to be the same as normal_t and V to be the
 * same with const_t
 * @tparam T The first type
 * @tparam V the second type
 * @tparam normal_t the normal type
 * @tparam const_t the const type
 */
template <typename T, typename V, typename normal_t, typename const_t>
concept normal_to_const =
    std::is_same_v<T, normal_t> && std::is_same_v<V, const_t>;

/**
 * @brief to constrain the type of T to be the same as V1 or V2
 * @tparam T the target type
 * @tparam V1 the first type
 * @tparam V2 the second type
 */
template <typename T, typename V1, typename V2>
concept is_one_of = std::is_same_v<T, V1> || std::is_same_v<T, V2>;

template <typename T>
concept Iterator = requires(T a) {
    { *a };
    { a++ };
    { a-- };
    { ++a };
    { --a };
    { a == a };
    { a != a };
};

}  // namespace mtl

#endif
