#ifndef MTL_VECTOR_H
#define MTL_VECTOR_H

#include <mtl/mtldefs.h>
#include <mtl/mtlutils.h>
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <utility>

// The namespace where the ADTs are.
namespace mtl {

/* The vector ADT, it can expand its data array to double size when space is
 * not enough. */
template <typename T, typename Alloc = std::allocator<T>>
class vector {
public:
    using self_t = vector<T, Alloc>;

private:
    // the length of the array
    size_t capacity_;

    // the array contain the data
    T* data_;

    Alloc allocator_;

    size_t size_;

    void check_empty() const {
        if (empty()) {
            throw EmptyContainer();
        }
    }

    /* allocate a new array with length size and return the pointer to it */
    [[nodiscard]] T* allocate(size_t new_capacity) {
        return allocator_.allocate(new_capacity);
    }

    void deallocate(T* array, size_t length) {
        allocator_.deallocate(array, length);
    }

    template <typename Ref, typename Ptr>
    class vector_iterator;

public:
    using const_iterator = vector_iterator<const T&, const T*>;
    using iterator = vector_iterator<T&, T*>;

    // the default constructor
    vector() : capacity_(0), size_(0), data_(nullptr) {}

    // construct the vector with particular size
    explicit vector(size_t init_size)
        : capacity_(init_size), size_(init_size), data_(allocate(init_size)) {
        construct_all(data_, init_size);
    }

    vector(size_t init_size, const T& init_val)
        : capacity_(init_size), size_(init_size), data_(allocate(capacity_)) {
        for (int i = 0; i < init_size; ++i) {
            construct(data_, i, init_val);
        }
    }

    /* construct from initializer list, the size will be the same with the
     * il. */
    vector(std::initializer_list<T> il) noexcept
        : capacity_(il.size()), size_(il.size()), data_(allocate(il.size())) {
        auto itr = il.begin();
        for (int i = 0; i < il.size(); ++i, ++itr) {
            construct(data_, i, std::move_if_noexcept(*itr));
        }
    }

    // copy constructor
    vector(const self_t& rhs)
        : capacity_(rhs.size()),
          size_(rhs.size()),
          data_(allocate(rhs.size())) {
        std::uninitialized_copy_n(rhs.data_, rhs.size_, data_);
    }

    // moving constructor
    vector(self_t&& rhs) noexcept
        : data_(rhs.data_), capacity_(rhs.capacity_), size_(rhs.size_) {
        rhs.data_ = nullptr;
        rhs.capacity_ = 0;
        rhs.size_ = 0;
    }

    // the destructor
    ~vector() noexcept {
        destroy_all(data_, size_);
        deallocate(data_, capacity_);
    }

    /* return the reference to the element at position index
     * it don't check the boundary */
    const T& operator[](size_t index) const {
        return data()[index];
    }

    // the const version
    T& operator[](size_t index) {
        return const_cast<T&>(
            static_cast<const self_t*>(this)->operator[](index));
    }

    const T& at(size_t index) const {
        if (index >= capacity_) {
            throw std::out_of_range("The index is out of range.");
        }
        return data()[index];
    }

    T& at(size_t index) {
        return const_cast<T&>(static_cast<const self_t*>(this)->at(index));
    }

    [[nodiscard]] size_t capacity() const {
        return capacity_;
    }

    [[nodiscard]] size_t size() const {
        return size_;
    }

    // the interface for derived classes to get data_
    const T* data() const {
        return data_;
    }

    // the interface for derived classes to get data_
    T* data() {
        return const_cast<T*>(static_cast<const self_t*>(this)->data());
    }

    // return whether the vector is empty
    [[nodiscard]] bool empty() const {
        return size() == 0;
    }

    void clear() noexcept {
        destroy_all(data_, size_);
        deallocate(data_, capacity_);
        capacity_ = 0;
        size_ = 0;
        data_ = nullptr;
    }

    void shrink_to_fit() noexcept;

    void reserve(size_t capacity);

    void resize(size_t new_size) noexcept;

    const T& front() const {
        return at(0);
    }

    const T& back() const {
        return at(size() - 1);
    }

    T& front() {
        return const_cast<T&>(static_cast<const self_t*>(this)->front());
    }

    T& back() {
        return const_cast<T&>(static_cast<const self_t*>(this)->back());
    }

    template <typename V>
    void push_back(V&& elem) {
        // emplace_back(std::forward<V>(elem));
        if (size() + 1 > capacity()) {
            reserve(capacity() == 0 ? 1 : capacity() * 2);
        }
        construct(data_, size(), std::forward<V>(elem));
        ++size_;
    }

    void pop_back() {
        check_empty();
        destroy(data_, size() - 1);
        --size_;
    }

    template <typename... Args>
    void emplace_back(Args&&... args) {
        if (size() + 1 > capacity()) {
            reserve(capacity() == 0 ? 1 : capacity() * 2);
        }
        construct(data_, size(), std::forward<Args>(args)...);
        ++size_;
    }

    /* insert an element at position index,
     * return an iterator pointing to the next cell */
    template <typename V>
    iterator insert(iterator index, V&& elem);

    /* insert another from another container (deep copy) with iterators
     * which provide ++, --, ==, and != operators */
    template <typename InputIterator>
    iterator insert(iterator index, InputIterator begin, InputIterator end);

    /* remove the elements at position index,
     * return iterator to the next position */
    iterator remove(iterator index) noexcept;

    // remove the range [begin, stop)
    iterator remove(iterator begin, iterator stop) noexcept;

    /* return whether two vector is the same vector (whether the data_ is
     * equal) */
    bool operator==(const self_t& vec) const {
        return data() == vec.data();
    }

    // the copy assignment operator
    self_t& operator=(const self_t& rhs) {
        if (&rhs == this) {
            return *this;
        }
        clear();
        reserve(rhs.size());
        size_ = rhs.size();
        for (size_t i = 0; i < size_; ++i) {
            construct(data_, i, rhs[i]);
        }

        return *this;
    }

    // the moving assignment operator
    self_t& operator=(self_t&& rhs) noexcept {
        if (this == &rhs) {
            return *this;
        }
        destroy_all(data_, size_);
        deallocate(data_, capacity_);

        // copy the object
        capacity_ = rhs.capacity_;
        data_ = rhs.data_;
        size_ = rhs.size_;

        rhs.data_ = nullptr;
        rhs.capacity_ = 0;
        rhs.size_ = 0;

        return *this;
    }

    // return a vector_iterator pointing to the position 0
    const_iterator cbegin() const {
        return const_iterator(const_cast<T*>(data()));
    }

    /* return a vector_iterator pointing to the position after the last
     * element */
    const_iterator cend() const {
        return const_iterator(
            data() == nullptr ? nullptr : const_cast<T*>(data()) + size());
    }

    // return an iterator pointing to the first element
    iterator begin() {
        return iterator(data());
    }

    // return an iterator pointing to the element behind the last one
    iterator end() {
        return iterator(data() == nullptr ? nullptr : data() + size());
    }

    // return a vector_iterator pointing to the position 0
    const_iterator begin() const {
        return cbegin();
    }

    /* return a vector_iterator pointing to the position after the last
     * element */
    const_iterator end() const {
        return cend();
    }
};

template <typename T, typename Alloc>
template <typename V>
typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(iterator index,
                                                             V&& elem) {
    if (size() + 1 > capacity()) {
        size_t pre_len = index ? index - begin() : size_;
        size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
        auto new_data = allocate(new_capacity);

        std::uninitialized_move_n(data_, pre_len, new_data);
        construct(new_data, pre_len, std::forward<V>(elem));
        std::uninitialized_move_n(data_ + pre_len, size_ - pre_len,
                                  new_data + pre_len + 1);

        destroy_all(data_, size_);
        deallocate(data_, capacity_);

        data_ = new_data;
        capacity_ = new_capacity;

        index = this->begin() + pre_len;
    } else {
        auto itr1 = this->begin() + size_ - 1;
        auto itr2 = itr1 - 1;
        // the position of index in data_
        construct(data_, size(), std::move_if_noexcept(*itr1));
        while (itr1 > index) {
            *itr1 = std::move_if_noexcept(*itr2);
            --itr1;
            --itr2;
        }
        *index = std::forward<V>(elem);
    }

    ++size_;
    return index;
}

template <typename T, typename Alloc>
template <typename InputIterator>
typename vector<T, Alloc>::iterator
vector<T, Alloc>::insert(iterator index, InputIterator begin,
                         InputIterator end) {
    size_t len = end - begin;

    // check whether the capacity is big enough
    if (size_ + len > capacity()) {
        size_t pre_len = index ? index - this->begin() : size_;

        size_t new_capacity = capacity_ == 0 ? len : capacity_ * 2;
        while (new_capacity < size_ + len) {
            new_capacity = size_ + len;
        }
        auto new_data = allocate(new_capacity);

        std::uninitialized_move_n(data_, pre_len, new_data);
        for (size_t i = 0; i < len; ++i, ++begin) {
            construct(new_data, pre_len + i, *begin);
        }
        std::uninitialized_move_n(data_ + pre_len, size_ - pre_len,
                                  new_data + pre_len + len);
        destroy_all(data_, size_);
        deallocate(data_, capacity_);
        data_ = new_data;
        capacity_ = new_capacity;
        index = this->begin() + pre_len;
    } else {
        // move elements backward
        for (size_t i = size_ + len - 1; i >= size_; --i) {
            construct(data_, i, std::move(data_[i - len]));
        }
        auto itr1 = this->begin() + size_ - 1;
        auto itr2 = itr1 - len;
        while (itr1 >= index + len) {
            *itr1 = std::move(*itr2);
            --itr1;
            --itr2;
        }
        for (auto itr = index; begin != end; ++itr, ++begin) {
            *itr = *begin;
        }
    }

    size_ += len;
    return index;
}

template <typename T, typename Alloc>
typename vector<T, Alloc>::iterator
vector<T, Alloc>::remove(iterator index) noexcept {
    // move the following elements
    for (auto itr1 = index, itr2 = index + 1; itr2 != this->end();
         ++itr1, ++itr2) {
        *itr1 = std::move_if_noexcept(*itr2);
    }
    destroy(data_, size() - 1);
    --size_;

    return index;
}

template <typename T, typename Alloc>
typename vector<T, Alloc>::iterator
vector<T, Alloc>::remove(iterator begin, iterator stop) noexcept {
    if (begin == stop) {
        return begin;
    }

    // move the elements
    difference_t wid = stop - begin;

    auto itr1 = begin;
    auto itr2 = stop;
    while (itr2 != this->end()) {
        *(itr1++) = std::move(*(itr2++));
    }
    size_t size = this->size();
    for (size_t i = size - 1; i >= size - wid; --i) {
        destroy(data_, i);
    }

    size_ -= wid;
    return begin;
}

template <typename T, typename Alloc>
void vector<T, Alloc>::reserve(size_t capacity) {
    if (capacity <= capacity_) {
        return;
    }

    auto new_data = allocate(capacity);

    if (data_ != nullptr) {
        std::uninitialized_move_n(data_, size_, new_data);
        destroy_all(data_, size_);
        deallocate(data_, capacity_);
    }

    data_ = new_data;
    capacity_ = capacity;
}

template <typename T, typename Alloc>
void vector<T, Alloc>::shrink_to_fit() noexcept {
    if (size_ == capacity_) {
        return;
    }

    auto new_data = allocate(size_);
    capacity_ = size_;

    std::uninitialized_move_n(data_, size_, new_data);
    destroy_all(data_, size_);
    deallocate(data_, capacity_);

    data_ = new_data;
    capacity_ = size_;
}

template <typename T, typename Alloc>
template <typename Ref, typename Ptr>
class vector<T, Alloc>::vector_iterator {
private:
    T* elem_;  // pointer to the element
    using self_t = vector_iterator<Ref, Ptr>;
    friend const_iterator;

public:
    vector_iterator() : elem_(nullptr) {}

    ~vector_iterator() = default;

    // construct from pointer
    explicit vector_iterator(T* elem) : elem_(elem) {}

    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    vector_iterator(const Iter& rhs) : elem_(rhs.elem_) {}

    vector_iterator(const self_t& rhs) = default;
    vector_iterator(self_t&& rhs) noexcept = default;

    // return a reference to the element
    Ref operator*() const {
        return *elem_;
    }

    Ptr operator->() const {
        return elem_;
    }

    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator>(const self_t& lhs, const Iter& rhs) {
        return lhs.elem_ > rhs.elem_;
    }

    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator<(const self_t& lhs, const Iter& rhs) {
        return lhs.elem_ < rhs.elem_;
    }

    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator<=(const self_t& lhs, const Iter& rhs) {
        return lhs.elem_ <= rhs.elem_;
    }

    // compare the pointer
    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator>=(const self_t& lhs, const Iter& rhs) {
        return lhs.elem_ >= rhs.elem_;
    }

    // compare the pointer
    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator==(const self_t& lhs, const Iter& rhs) {
        return lhs.elem_ == rhs.elem_;
    }

    // compare the pointer
    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator!=(const self_t& lhs, const Iter& rhs) {
        return lhs.elem_ != rhs.elem_;
    }

    explicit operator bool() const {
        return elem_;
    }

    self_t& operator=(const self_t& rhs) = default;
    self_t& operator=(self_t&& rhs) noexcept = default;

    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    self_t& operator=(const Iter& rhs) {
        elem_ = rhs.elem_;
        return *this;
    }

    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    self_t& operator=(Iter&& rhs) noexcept {
        elem_ = rhs.elem_;
        return *this;
    }

    // move n items next, it don't check the boundary
    self_t operator+(difference_t n) const {
        auto new_itr = *this;
        new_itr += n;
        return new_itr;
    }

    // move n items next, it don't check the boundary
    self_t& operator+=(difference_t n) {
        elem_ += n;
        return *this;
    }

    // move n items previous, it don't check the boundary
    self_t operator-(difference_t n) const {
        auto new_itr = *this;
        new_itr -= n;
        return new_itr;
    }

    template <is_one_of<iterator, const_iterator> Iter>
    friend difference_t operator-(const self_t& lhs, const Iter& rhs) {
        return lhs.elem_ - rhs.elem_;
    }

    // move n items previous, it don't check the boundary
    self_t& operator-=(difference_t n) {
        elem_ -= n;
        return *this;
    }

    // prefix increment
    self_t& operator++() {
        ++elem_;
        return *this;
    }

    // postfix increment
    self_t operator++(int) {
        auto new_itr = *this;
        ++elem_;
        return new_itr;
    }

    // prefix decrement
    self_t& operator--() {
        --elem_;
        return *this;
    }

    // postfix decrement
    self_t operator--(int) {
        auto new_itr = *this;
        --elem_;
        return new_itr;
    }
};

}  // namespace mtl
#endif  // MTL_VECTOR_H
