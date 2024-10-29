#ifndef MTL_BASIC_VECTOR_H
#define MTL_BASIC_VECTOR_H

#include <utility>
#include "types.h"

namespace mtl {
    template <typename T>
    class basic_vector {
    private:
        // the array contain the data
        T* data_;

        // the length of the array
        size_t capacity_;

        // the default capacity, basic_vector ensures that the capacity won't be smaller than it
        const static size_t DEFAULT_CAPACITY = 128;

        /* allocate a new array with length capacity
           it don't delete the original array */
        void allocate(size_t size) {
            data_ = new T [size];
        }

    public:
        basic_vector();
        explicit basic_vector(size_t s);
        basic_vector(const basic_vector<T>& rhs);
        basic_vector(basic_vector<T>&& rhs) noexcept;
        virtual ~basic_vector();

        void expand(size_t new_capacity) noexcept;
        void shrink(size_t new_capacity) noexcept;

        virtual void clear() {
            delete [] data_;
            capacity_ = DEFAULT_CAPACITY;
            allocate(capacity_);
        }

        size_t capacity() const {
            return capacity_;
        }

        basic_vector& operator=(const basic_vector& rhs);
        basic_vector& operator=(basic_vector&& rhs) noexcept;

    protected:
        const T* data() const {
            return data_;
        }

        T* data() {
            return data_;
        }
    };

    template <typename T>
    basic_vector<T>::basic_vector() : capacity_(DEFAULT_CAPACITY) {
        allocate(capacity_);
    }

    template <typename T>
    basic_vector<T>::basic_vector(size_t s) : capacity_(s) {
        capacity_ = capacity_ > DEFAULT_CAPACITY ? capacity_ : DEFAULT_CAPACITY;
        allocate(capacity_);
    }

    template <typename T>
    basic_vector<T>::basic_vector(const basic_vector<T>& vec) : capacity_(vec.capacity_) {
        allocate(capacity_);
        auto vec_data = vec.data_;
        for (size_t i = 0; i < capacity_; ++i) {
            data_[i] = vec_data[i];
        }
    }

    template <typename T>
    basic_vector<T>::basic_vector(basic_vector<T>&& vec) noexcept :
        data_(vec.data_), capacity_(vec.capacity_) {
        vec.data_ = nullptr;
        vec.clear();
    }

    template <typename T>
    basic_vector<T>::~basic_vector() {
        delete[] data_;
    }

    template<typename T>
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

        delete [] old;
    }

    template<typename T>
    void basic_vector<T>::shrink(size_t new_capacity) noexcept {
        if (new_capacity >= capacity_) {
            return;
        }
        // backup the original array
        auto old = data_;

        // create a new array
        capacity_ = new_capacity > DEFAULT_CAPACITY ? new_capacity : DEFAULT_CAPACITY;
        allocate(capacity_);

        // move the elements
        for (size_t i = 0; i < new_capacity; ++i) {
            data_[i] = std::move(old[i]);
        }

        delete [] old;
    }

    template <typename T>
    basic_vector<T>& basic_vector<T>::operator=(const basic_vector<T>& vec) {
        // process the self-assignment
        if (this == &vec) {
            return *this;
        }

        // delete original array
        delete [] data_;

        capacity_ = vec.capacity_;
        data_ = allocate(capacity_);

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
        delete [] data_;

        // copy the object
        capacity_ = vec.capacity_;
        data_ = vec.data_;

        vec.data_ = nullptr;

        return *this;
    }
}

#endif
