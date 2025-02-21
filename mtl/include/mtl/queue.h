#ifndef MTL_QUEUE_H
#define MTL_QUEUE_H

#include <mtl/deque.h>
#include <mtl/types.h>
#include <memory>

namespace mtl {
template <typename T, template <typename> typename Alloc = std::allocator>
class queue {
private:
    using container_type = deque<T, Alloc>;
    deque<T>* data_;

public:
    queue();
    explicit queue(size_t s);
    queue(const queue<T, Alloc>& rhs);
    queue(queue<T, Alloc>&& rhs);
    ~queue() noexcept;

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

template <typename T, template <typename> typename Alloc>
queue<T, Alloc>::queue() : data_(new container_type()) {}

template <typename T, template <typename> typename Alloc>
queue<T, Alloc>::queue(size_t s) : data_(new container_type(s)) {}

template <typename T, template <typename> typename Alloc>
queue<T, Alloc>::queue(const queue<T, Alloc>& rhs)
    : data_(new container_type(*rhs.data_)) {}

template <typename T, template <typename> typename Alloc>
queue<T, Alloc>::queue(queue<T, Alloc>&& rhs) : data_(rhs.data_) {
    rhs.data_ = new container_type;
}

template <typename T, template <typename> typename Alloc>
queue<T, Alloc>::~queue() noexcept {
    delete data_;
}
}  // namespace mtl

#endif
