export module mtl.core:defs;

import std;

export namespace mtl {
/**
 * @brief Alias for size type.
 */
using std::size_t;

/**
 * @brief Alias for signed difference type.
 */
using difference_t = std::ptrdiff_t;

/**
 * @brief Exception type for null iterator access.
 */
struct NullIterator : public std::exception {
    const char* msg_;

    /**
     * @brief Construct with a default message.
     */
    NullIterator() : msg_("This iterator is null.") {}
    /**
     * @brief Copy-construct the exception.
     * @param rhs Exception to copy from.
     */
    NullIterator(const NullIterator&) = default;
    /**
     * @brief Move construction is disabled.
     */
    NullIterator(NullIterator&&) = delete;
    /**
     * @brief Copy-assign the exception.
     * @return Reference to this exception.
     */
    NullIterator& operator=(const NullIterator&) = default;
    /**
     * @brief Move assignment is disabled.
     * @return Reference to this exception.
     */
    NullIterator& operator=(NullIterator&&) = delete;
    /**
     * @brief Construct with a custom message.
     * @param msg Message to store.
     */
    explicit NullIterator(const char* msg) : msg_(msg) {}
    /**
     * @brief Destroy the exception.
     */
    ~NullIterator() noexcept override = default;
    /**
     * @brief Return the stored message.
     * @return The stored message text.
     */
    virtual const char* what() {
        return msg_;
    }
};

/**
 * @brief Exception type for operations on empty containers.
 */
struct EmptyContainer : public std::exception {
    const char* msg_;

    /**
     * @brief Construct with a default message.
     */
    EmptyContainer() : msg_("This container is empty.") {}
    /**
     * @brief Copy-construct the exception.
     * @param rhs Exception to copy from.
     */
    EmptyContainer(const EmptyContainer&) = default;
    /**
     * @brief Move construction is disabled.
     */
    EmptyContainer(EmptyContainer&&) = delete;
    /**
     * @brief Copy-assign the exception.
     * @return Reference to this exception.
     */
    EmptyContainer& operator=(const EmptyContainer&) = default;
    /**
     * @brief Move assignment is disabled.
     * @return Reference to this exception.
     */
    EmptyContainer& operator=(EmptyContainer&&) = delete;
    /**
     * @brief Construct with a custom message.
     * @param msg Message to store.
     */
    explicit EmptyContainer(const char* msg) : msg_(msg) {}
    /**
     * @brief Destroy the exception.
     */
    ~EmptyContainer() noexcept override = default;
    /**
     * @brief Return the stored message.
     * @return The stored message text.
     */
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

/**
 * @brief Concept that checks for basic iterator operations.
 *
 * @tparam T The iterator type to test.
 */
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

/**
 * @brief Concept that checks for random-access iterator operations.
 *
 * @tparam T The iterator type to test.
 */
template <typename T>
concept RandomIterator = Iterator<T> && requires(T a, T b) {
    { b - a };
    { a + 1 };
};

}  // namespace mtl
