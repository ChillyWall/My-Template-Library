#ifndef MTL_BASIC_VECTOR_H
#define MTL_BASIC_VECTOR_H

#include <mtl/types.h>
#include <utility>

namespace mtl {
template <typename T>
class basic_vector {
private:
    // the array contain the data
    T* data_;

    // the length of the array
    size_t capacity_;

    /* allocate a new array with length size
       it don't delete the original array */
    void allocate(size_t size) {
        data_ = new T[size];
    }

public:
    basic_vector();
    explicit basic_vector(size_t s);
    basic_vector(const basic_vector<T>& rhs);
    basic_vector(basic_vector<T>&& rhs) noexcept;
    virtual ~basic_vector();

    // To expand the array to the new capacity
    void expand(size_t new_capacity) noexcept;
    // To shrink the array to the new capacaty
    void shrink(size_t new_capacity) noexcept;

    /* delete the old array and allocate a new one with
       size DEFAULT_CAPACITY */
    virtual void clear() {
        delete[] data_;
        capacity_ = 0;
        data_ = nullptr;
    }

    size_t capacity() const {
        return capacity_;
    }

    basic_vector& operator=(const basic_vector& rhs);
    basic_vector& operator=(basic_vector&& rhs) noexcept;

    // the interface for derived classes to get data_
    const T* data() const {
        return data_;
    }

    // the interface for derived classes to get data_
    T* data() {
        return data_;
    }
};

template <typename T>
basic_vector<T>::basic_vector() : capacity_(0), data_(nullptr) {}

template <typename T>
basic_vector<T>::basic_vector(size_t s) : capacity_(s) {
    allocate(capacity_);
}

template <typename T>
basic_vector<T>::basic_vector(const basic_vector<T>& rhs)
    : capacity_(rhs.capacity_) {
    allocate(capacity_);
    auto vec_data = rhs.data_;
    for (size_t i = 0; i < capacity_; ++i) {
        data_[i] = vec_data[i];
    }
}

template <typename T>
basic_vector<T>::basic_vector(basic_vector<T>&& rhs) noexcept
    : data_(rhs.data_), capacity_(rhs.capacity_) {
    rhs.data_ = nullptr;
    rhs.clear();
}

template <typename T>
basic_vector<T>::~basic_vector() {
    delete[] data_;
}

template <typename T>
void basic_vector<T>::expand(size_t new_capacity) noexcept {
    if (new_capacity <= capacity_) {
        return;
    }
    // backup the original array
    auto old = data_;

    // create a new array
    allocate(new_capacity);

    // move the elements
    for (size_t i = 0; i < capacity_; ++i) {
        data_[i] = std::move(old[i]);
    }
    capacity_ = new_capacity;

    delete[] old;
}

template <typename T>
void basic_vector<T>::shrink(size_t new_capacity) noexcept {
    if (new_capacity >= capacity_) {
        return;
    }
    // backup the original array
    auto old = data_;

    // create a new array
    capacity_ = new_capacity;
    allocate(capacity_);

    // move the elements
    for (size_t i = 0; i < new_capacity; ++i) {
        data_[i] = std::move(old[i]);
    }

    delete[] old;
}

template <typename T>
basic_vector<T>& basic_vector<T>::operator=(const basic_vector<T>& vec) {
    // process the self-assignment
    if (this == &vec) {
        return *this;
    }

    // delete original array
    delete[] data_;

    capacity_ = vec.capacity_;
    allocate(capacity_);

    // copy every element
    for (size_t i = 0; i < capacity_; ++i) {
        data_[i] = vec.data_[i];
    }

    return *this;
}

template <typename T>
basic_vector<T>& basic_vector<T>::operator=(basic_vector<T>&& vec) noexcept {
    if (this == &vec) {
        return *this;
    }
    // delete original array
    delete[] data_;

    // copy the object
    capacity_ = vec.capacity_;
    data_ = vec.data_;

    vec.data_ = nullptr;
    vec.clear();
    return *this;
}
} // namespace mtl

#endif
