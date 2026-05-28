export module mtl.pair;

import std;

export namespace mtl {

/**
 * @brief Simple pair of two values.
 * @tparam T First value type.
 * @tparam V Second value type.
 */
template <typename T, typename V>
struct pair {
    T first;
    V second;

    using self_t = pair<T, V>;

    /**
     * @brief Construct a default-initialized pair.
     */
    pair() = default;

    /**
     * @brief Construct a pair from two values.
     * @tparam TR First value argument type.
     * @tparam VR Second value argument type.
     * @param fir First value.
     * @param sec Second value.
     */
    template <typename TR, typename VR>
    pair(TR&& fir, VR&& sec)
        : first(static_cast<TR&&>(fir)), second(static_cast<VR&&>(sec)) {}

    /**
     * @brief Copy-construct a pair.
     * @param rhs Pair to copy from.
     */
    pair(const self_t& rhs) = default;
    /**
     * @brief Move-construct a pair.
     * @param rhs Pair to move from.
     */
    pair(self_t&& rhs) noexcept = default;

    /**
     * @brief Destroy the pair.
     */
    ~pair() = default;

    /**
     * @brief Copy-assign from another pair.
     * @param rhs Pair to copy from.
     * @return Reference to this pair.
     */
    self_t& operator=(const self_t& rhs) = default;
    /**
     * @brief Move-assign from another pair.
     * @param rhs Pair to move from.
     * @return Reference to this pair.
     */
    self_t& operator=(self_t&& rhs) = default;
};

/**
 * @brief Equality comparison for pair.
 */
template <typename T, typename V>
bool operator==(const pair<T, V>& lhs, const pair<T, V>& rhs) {
    return lhs.first == rhs.first && lhs.second == rhs.second;
}

/**
 * @brief Inequality comparison for pair.
 */
template <typename T, typename V>
bool operator!=(const pair<T, V>& lhs, const pair<T, V>& rhs) {
    return !(lhs == rhs);
}

/**
 * @brief Less-than comparison for pair (lexicographic).
 */
template <typename T, typename V>
bool operator<(const pair<T, V>& lhs, const pair<T, V>& rhs) {
    if (lhs.first < rhs.first) return true;
    if (rhs.first < lhs.first) return false;
    return lhs.second < rhs.second;
}

/**
 * @brief Greater-than comparison for pair.
 */
template <typename T, typename V>
bool operator>(const pair<T, V>& lhs, const pair<T, V>& rhs) {
    return rhs < lhs;
}

/**
 * @brief Less-or-equal comparison for pair.
 */
template <typename T, typename V>
bool operator<=(const pair<T, V>& lhs, const pair<T, V>& rhs) {
    return !(rhs < lhs);
}

/**
 * @brief Greater-or-equal comparison for pair.
 */
template <typename T, typename V>
bool operator>=(const pair<T, V>& lhs, const pair<T, V>& rhs) {
    return !(lhs < rhs);
}

}  // namespace mtl
