#ifndef MTL_QUEUE_H
#define MTL_QUEUE_H

#include <mtl/deque.h>
#include <mtl/types.h>

namespace mtl {
template <typename T, template <typename> typename allocator>
class queue {
private:
    allocator<T>* data_;

public:
    queue();
    explicit queue(size_t s);
    queue(const queue<T, allocator>& rhs);
    queue(queue<T, allocator>&& rhs);
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

template <typename T, template <typename> typename allocator>
queue<T, allocator>::queue() : data_(new allocator()) {}

template <typename T, template <typename> typename allocator>
queue<T, allocator>::queue(size_t s) : data_(new allocator(s)) {}

template <typename T, template <typename> typename allocator>
queue<T, allocator>::queue(const queue<T, allocator>& rhs)
    : data_(new allocator(*rhs.data_)) {}

template <typename T, template <typename> typename allocator>
queue<T, allocator>::queue(queue<T, allocator>&& rhs) : data_(rhs.data_) {
    rhs.data_ = new allocator;
}

template <typename T, template <typename> typename allocator>
queue<T, allocator>::~queue() noexcept {
    delete data_;
}
}  // namespace mtl

#endif
