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
        : first(std::forward<T>(fir)), second(std::forward<V>(sec)) {}

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

}  // namespace mtl
