#ifndef MTL_STACK_H
#define MTL_STACK_H

#include <mtl/deque.h>
#include <mtl/types.h>
#include <utility>

namespace mtl {
template <typename T, template <typename> typename allocator = deque>
class stack {
private:
    allocator<T>* data_;

public:
    stack();
    explicit stack(size_t size_);
    stack(const stack<T, allocator>& rhs);
    stack(stack<T, allocator>&& rhs) noexcept;
    virtual ~stack();

    bool empty() const {
        data_->empty();
    }

    size_t size() const {
        return data_->size();
    }

    template <typename V>
    void push(V&& elem) {
        data_->push_back(std::forward<V>(elem));
    }

    void pop() {
        data_->pop_back();
    }

    const T& top() const {
        data_->back();
    }

    T& top() {
        return data_->back();
    }
};

template <typename T, template <typename> typename allocator>
stack<T, allocator>::stack() : data_(new allocator()) {}

template <typename T, template <typename> typename allocator>
stack<T, allocator>::stack(size_t s) : data_(new allocator(s)) {}

template <typename T, template <typename> typename allocator>
stack<T, allocator>::stack(const stack<T, allocator>& rhs)
    : data_(new allocator(*rhs.data_)) {}

template <typename T, template <typename> typename allocator>
stack<T, allocator>::stack(stack<T, allocator>&& rhs) noexcept
    : data_(rhs.data_) {
    rhs.data_ = nullptr;
}
}  // namespace mtl
#endif
