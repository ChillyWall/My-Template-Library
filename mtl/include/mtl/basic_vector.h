#ifndef MTL_BASIC_VECTOR_H
#define MTL_BASIC_VECTOR_H

#include <mtl/mtldefs.h>
#include <memory>

namespace mtl {
template <typename T, typename Alloc = std::allocator<T>>
class basic_vector {
public:
    using self_t = basic_vector<T, Alloc>;

private:
    // the length of the array
    size_t capacity_;

    // the array contain the data
    T* data_;

protected:
    size_t size_;

    Alloc allocator_;
    /* allocate a new array with length size it don't delete the original array
     */
    void allocate(size_t new_capacity) {
        capacity_ = new_capacity;
        data_ = allocator_.allocate(new_capacity);
    }

    template <typename... Args>
    void construct(size_t index, Args&&... args) {
        std::construct_at(data_ + index, std::forward<Args>(args)...);
    }

    void destroy(size_t index) {
        std::destroy_at(data_ + index);
    }

    void destroy_all() {
        for (size_t i = 0; i < size_; ++i) {
            destroy(i);
        }
    }

    void deallocate() {
        allocator_.deallocate(data_, capacity_);
        data_ = nullptr;
        capacity_ = 0;
    }

public:
    basic_vector();
    explicit basic_vector(size_t s);
    basic_vector(const self_t& rhs);
    basic_vector(self_t&& rhs) noexcept;
    virtual ~basic_vector();

    basic_vector& operator=(const self_t& rhs);
    basic_vector& operator=(self_t&& rhs) noexcept;

    /* return the reference to the element at position index
     * it don't check the boundary */
    const T& operator[](size_t index) const {
        return data_[index];
    }

    // the const version
    T& operator[](size_t index) {
        return const_cast<T&>(
            static_cast<const self_t*>(this)->operator[](index));
    }

    const T& at(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("The index is out of range.");
        }
        return data_[index];
    }

    T& at(size_t index) {
        return const_cast<T&>(static_cast<const self_t*>(this)->at(index));
    }

    size_t size() const {
        return size_;
    }

    size_t capacity() const {
        return capacity_;
    }

    // To expand the array to the new capacity
    void expand(size_t new_capacity) noexcept;

    // To shrink the array to the new capacity
    void shrink(size_t new_capacity) noexcept;

    template <typename V>
    void push_back(V&& elem) {
        if (size_ == capacity_) {
            expand(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        construct(size_, std::forward<V>(elem));
        ++size_;
    }

    template <typename... Args>
    void emplace_back(Args&&... args) {
        if (size_ == capacity_) {
            expand(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        construct(size_, std::forward<Args>(args)...);
        ++size_;
    }

    void pop_back() {
        if (size_ == 0) {
            throw EmptyContainer();
        }
        destroy(size_);
        --size_;
    }

    void clear() {
        destroy_all();
        deallocate();
        size_ = 0;
    }

    // the interface for derived classes to get data_
    const T* data() const {
        return data_;
    }

    // the interface for derived classes to get data_
    T* data() {
        return data_;
    }
};

template <typename T, typename Alloc>
basic_vector<T, Alloc>::basic_vector()
    : capacity_(0), data_(nullptr), size_(0) {}

template <typename T, typename Alloc>
basic_vector<T, Alloc>::basic_vector(size_t s) : capacity_(s), size_(0) {
    allocate(capacity_);
}

template <typename T, typename Alloc>
basic_vector<T, Alloc>::basic_vector(const self_t& rhs) : size_(rhs.size_) {
    allocate(rhs.capacity_);
    for (size_t i = 0; i < size_; ++i) {
        construct(i, rhs.data_[i]);
    }
}

template <typename T, typename Alloc>
basic_vector<T, Alloc>::basic_vector(self_t&& rhs) noexcept
    : data_(rhs.data_), capacity_(rhs.capacity_), size_(rhs.size_) {
    rhs.data_ = nullptr;
    rhs.capacity_ = 0;
    rhs.size_ = 0;
}

template <typename T, typename Alloc>
basic_vector<T, Alloc>::~basic_vector() {
    destroy_all();
    deallocate();
}

template <typename T, typename Alloc>
void basic_vector<T, Alloc>::expand(size_t new_capacity) noexcept {
    if (new_capacity <= capacity_) {
        return;
    }

    auto old_data = data_;
    auto old_capacity = capacity_;

    allocate(new_capacity);

    for (size_t i = 0; i < size_; ++i) {
        construct(i, std::move(old_data[i]));
        std::destroy_at(old_data + i);
    }

    allocator_.deallocate(old_data, old_capacity);
}

template <typename T, typename Alloc>
void basic_vector<T, Alloc>::shrink(size_t new_capacity) noexcept {
    if (new_capacity >= capacity_) {
        return;
    }

    T* old_data = data_;
    T* old_capacity = capacity_;

    allocate(new_capacity);

    if (new_capacity >= size_) {
        for (size_t i = 0; i < size_; ++i) {
            construct(i, std::move(old_data[i]));
            std::destroy_at(old_data + i);
        }
    } else {
        for (size_t i = 0; i < new_capacity; ++i) {
            construct(i, std::move(old_data[i]));
            std::destroy_at(old_data + i);
        }
        for (size_t i = new_capacity; i < size_; ++i) {
            std::destroy_at(old_data + i);
        }
        size_ = new_capacity;
    }

    allocator_.deallocate(old_data, old_capacity);
}

template <typename T, typename Alloc>
typename basic_vector<T, Alloc>::self_t&
basic_vector<T, Alloc>::operator=(const self_t& rhs) {
    if (this == &rhs) {
        return *this;
    }
    destroy_all();
    deallocate();

    // copy the object
    size_ = rhs.size_;
    allocate(rhs.capacity_);

    for (int i = 0; i < size_; ++i) {
        construct(i, rhs[i]);
    }

    return *this;
}

template <typename T, typename Alloc>
basic_vector<T, Alloc>::self_t&
basic_vector<T, Alloc>::operator=(self_t&& vec) noexcept {
    if (this == &vec) {
        return *this;
    }
    destroy_all();
    deallocate();

    // copy the object
    capacity_ = vec.capacity_;
    data_ = vec.data_;
    size_ = vec.size_;

    vec.data_ = nullptr;
    vec.capacity_ = 0;
    vec.size_ = 0;
    return *this;
}
}  // namespace mtl

#endif
