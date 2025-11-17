#ifndef TS_MTL_STACK_H
#define TS_MTL_STACK_H

#include <mtl/stack.h>
#include <mutex>

namespace mtl {

template <typename T, typename Alloc = std::allocator<T>>
class ts_stack {
public:
    using self_t = ts_stack<T, Alloc>;

private:
    stack<T, Alloc> data_;
    mutable std::mutex m;

public:
    ts_stack() = default;

    ts_stack(const self_t& rhs) {
        std::lock_guard<std::mutex> lock(rhs.m);
        data_ = rhs.data_;
    }

    ts_stack(self_t&& rhs) noexcept {
        std::lock_guard<std::mutex> lock(rhs.m);
        data_ = std::move(rhs.data_);
    }

    ~ts_stack() noexcept = default;

    self_t& operator=(const self_t& rhs) = delete;

    self_t& operator=(self_t&& rhs) noexcept {
        std::unique_lock<std::mutex> lock_this(m, std::defer_lock);
        std::unique_lock<std::mutex> lock_rhs(rhs.m, std::defer_lock);
        std::lock(lock_this, lock_rhs);

        data_ = std::move(rhs.data_);
        return *this;
    }

    [[nodiscard]] bool empty() const {
        std::lock_guard<std::mutex> lock(m);
        return data_.empty();
    }

    [[nodiscard]] size_t size() const {
        std::lock_guard<std::mutex> lock(m);
        return data_.size();
    }

    template <typename V>
    void push(V&& elem) {
        std::lock_guard<std::mutex> lock(m);
        data_.push(std::forward<V>(elem));
    }

    void pop(T& value) {
        std::lock_guard<std::mutex> lock(m);
        if (data_.empty()) {
            throw EmptyContainer("This ts_stack is empty.");
        }
        value = data_.top();
        data_.pop();
    }
};

}  // namespace mtl

#endif
