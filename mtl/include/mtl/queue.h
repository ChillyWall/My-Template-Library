#ifndef MTL_QUEUE_H
#define MTL_QUEUE_H

#include <mtl/deque.h>
#include <mtl/mtldefs.h>
#include <memory>

namespace mtl {
template <typename T, typename Alloc = std::allocator<T>>
class queue {
public:
    using self_t = queue<T, Alloc>;

private:
    using container_t = deque<T, Alloc>;
    deque<T> data_;

public:
    queue() = default;
    queue(const self_t& rhs) = default;
    queue(self_t&& rhs) = default;
    queue& operator=(const self_t& rhs) = default;
    queue& operator=(self_t&& rhs) = default;
    ~queue() noexcept = default;

    size_t size() const {
        return data_->size();
    }

    bool empty() const {
        return data_->empty();
    }

    void clear() {
        data_->clear();
    }

    template <typename V>
    void push(V&& elem) noexcept {
        data_->push_back(std::forward<V>(elem));
    }

    void pop() {
        data_->pop_front();
    }

    const T& front() const {
        return data_->front();
    }

    const T& back() const {
        return data_->back();
    }

    T& front() {
        return data_->front();
    }

    T& back() {
        return data_->back();
    }
};

}  // namespace mtl

#endif
