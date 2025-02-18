#ifndef MTL_PRIORITY_QUEUE_H
#define MTL_PRIORITY_QUEUE_H

#include <mtl/basic_vector.h>
#include <mtl/types.h>

namespace mtl {
/* The priority queue ADT, implemented by basic_vector so that it could
 * dynamicly expand its capacity. */
template <typename T>
class priority_queue {
private:
    /* the number of elements, note that the first element is at index 1 */
    basic_vector<T> data_;

    // check whether the queue is empty, if true, throw a out_of_range exception
    void check_empty() const {
        if (empty()) {
            throw EmptyContainer("There's no element.");
        }
    }

    /* to percolate up from the last element, to ensure the heap order after
     * push */
    void percolate_up() noexcept;
    // to percolate down from position 1, to ensure the heap order after pop
    void percolate_down() noexcept;

public:
    priority_queue();
    priority_queue(size_t n);
    priority_queue(const priority_queue<T>& rhs);
    priority_queue(priority_queue<T>&& rhs) noexcept;
    virtual ~priority_queue() = default;

    // clear the queue
    virtual void clear() {
        data_.clear();
    }

    size_t size() const {
        return data_.size() - 1;
    }

    bool empty() const {
        return size() <= 0;
    }

    priority_queue<T>& operator=(const priority_queue<T>& rhs) {
        data_ = rhs.data_;
        return *this;
    }

    priority_queue<T>& operator=(priority_queue<T>&& rhs) noexcept {
        data_ = std::move(rhs.data_);
        return *this;
    }

    // push a new element
    template <typename V>
    void push(V&& elem) noexcept {
        data_.push_back(std::forward<V>(elem));
        percolate_up();
    }

    // pop the minimum element
    void pop() {
        check_empty();
        percolate_down();
    }

    // return the minimum element
    const T& top() const {
        check_empty();
        return data_[1];
    }

    T& top() {
        return const_cast<T&>(
            static_cast<const priority_queue<T>*>(this)->top());
    }
};

template <typename T>
priority_queue<T>::priority_queue() : data_(1) {
    data_.push_back(T());
}

template <typename T>
priority_queue<T>::priority_queue(size_t capacity) : data_(capacity) {
    data_.push_back(T());
}

template <typename T>
priority_queue<T>::priority_queue(const priority_queue<T>& rhs)
    : data_(rhs.data_) {}

template <typename T>
priority_queue<T>::priority_queue(priority_queue<T>&& rhs) noexcept
    : data_(std::move(rhs.data_)) {}

template <typename T>
void priority_queue<T>::percolate_up() noexcept {
    size_t pos = size();
    T temp = std::move(data_[pos]);

    while (temp < data_[pos / 2]) {
        // move the parent down
        data_[pos] = std::move(data_[pos / 2]);
        pos /= 2;
    }
    data_[pos] = std::move(temp);
}

template <typename T>
void priority_queue<T>::percolate_down() noexcept {
    T temp = std::move(data_[size()]);
    data_.pop_back();
    size_t pos = 1;
    while ((pos * 2) <= size()) {
        size_t child = pos * 2;
        // choose the smaller child
        if (child + 1 <= size()) {
            child = data_[child] > data_[child + 1] ? child + 1 : child;
        }
        // move the child up
        if (data_[child] < temp) {
            data_[pos] = std::move(data_[child]);
            pos = child;
        } else {
            break;
        }
    }
    data_[pos] = std::move(temp);
}
}  // namespace mtl
#endif
