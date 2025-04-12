#ifndef MTL_PAIR_H
#define MTL_PAIR_H

#include <mtl/mtldefs.h>

namespace mtl {

template <typename T, typename V>
struct pair {
    T first;
    V second;

    using self_t = pair<T, V>;

    pair() = default;

    template <typename TR, typename VR>
    pair(TR&& fir, VR&& sec)
        : first(std::forward<T>(fir)), second(std::forward<V>(sec)) {}

    pair(const self_t& rhs) = default;
    pair(self_t&& rhs) noexcept = default;

    ~pair() = default;

    self_t& operator=(const self_t& rhs) = default;
    self_t& operator=(self_t&& rhs) = default;
};

}  // namespace mtl

#endif
