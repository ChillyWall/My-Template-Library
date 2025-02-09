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
    size_t size_;
    basic_vector<T>* data_;

    // check whether the queue is empty, if true, throw a out_of_range exception
    void check_empty() {
        if (size_ == 0) {
            throw EmptyContainer("There's no element.");
        }
    }

    // check whether an expansion is needed.
    void check_expand() {
        if (size_ + 1 >= data_->capacity()) {
            data_->expand(2 * data_->capacity());
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
        size_ = 0;
        data_->clear();
    }

    size_t size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }

    priority_queue<T>& operator=(const priority_queue<T>& rhs) {
        size_ = rhs.size_;
        *data_ = *rhs.data_;
        return *this;
    }

    priority_queue<T>& operator=(priority_queue<T>&& rhs) noexcept {
        size_ = rhs.size_;
        data_ = rhs.data_;
        rhs.data_ = nullptr;
        return *this;
    }

    // push a new element
    template <typename V>
    void push(V&& elem) noexcept {
        check_expand();
        ++size_;
        data_->data()[size_] = std::forward<V>(elem);
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
        return data_->data()[1];
    }

    T& top() {
        return const_cast<T&>(
            static_cast<const priority_queue<T>*>(this)->top());
    }
};

template <typename T>
priority_queue<T>::priority_queue() : size_(0) {}

template <typename T>
priority_queue<T>::priority_queue(size_t size)
    : size_(0), data_(new basic_vector<T>(size)) {}

template <typename T>
priority_queue<T>::priority_queue(const priority_queue<T>& rhs)
    : size_(rhs.size_), data_(new basic_vector<T>(*rhs.data_)) {}

template <typename T>
priority_queue<T>::priority_queue(priority_queue<T>&& rhs) noexcept
    : size_(rhs.size_), data_(rhs.data_) {
    rhs.data_ = new basic_vector<T>();
}

template <typename T>
void priority_queue<T>::percolate_up() noexcept {
    size_t pos = size_;
    auto data = data_->data();
    T temp = std::move(data[pos]);

    while (temp < data[pos >> 1]) {  // pos >> 1 is equivalent to pos / 2.
        // move the parent down
        data[pos] = std::move(data[pos >> 1]);
        pos >>= 1;
    }
    data[pos] = std::move(temp);
}

template <typename T>
void priority_queue<T>::percolate_down() noexcept {
    auto data = data_->data();
    T temp = std::move(data[size_]);
    --size_;
    size_t pos = 1;
    while ((pos << 1) <= size_) {  // pos << 1 is equivalent to pos * 2
        size_t child = pos << 1;
        // choose the smaller child
        if (child + 1 <= size_) {
            child = data[child] > data[child + 1] ? child + 1 : child;
        }
        // move the child up
        if (data[child] < temp) {
            data[pos] = std::move(data[child]);
            pos = child;
        } else {
            break;
        }
    }
    data[pos] = std::move(temp);
}
}  // namespace mtl
#endif
