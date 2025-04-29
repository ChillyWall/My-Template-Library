#ifndef MTL_STACK_H
#define MTL_STACK_H

#include <mtl/deque.h>
#include <mtl/mtldefs.h>
#include <memory>

namespace mtl {
template <typename T, typename Alloc = std::allocator<T>>
class stack {
public:
    using self_t = stack<T, Alloc>;

private:
    using container_type = deque<T, Alloc>;
    deque<T> data_;

public:
    stack() = default;
    stack(const self_t& rhs) = default;
    stack(self_t&& rhs) noexcept = default;
    virtual ~stack() = default;

    bool empty() const {
        return data_.empty();
    }

    size_t size() const {
        return data_.size();
    }

    template <typename V>
    void push(V&& elem) {
        data_.push_back(std::forward<V>(elem));
    }

    void pop() {
        data_.pop_back();
    }

    const T& top() const {
        data_.back();
    }

    T& top() {
        return data_.back();
    }
};

}  // namespace mtl
#endif
