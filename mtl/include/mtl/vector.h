#ifndef MTL_VECTOR_H
#define MTL_VECTOR_H

#include <mtl/basic_vector.h>
#include <mtl/mtldefs.h>
#include <initializer_list>
#include <stdexcept>

// The namespace where the ADTs are.
namespace mtl {

/* The vector ADT, it can expand its data array to double size when space is
 * not enough. */
template <typename T, typename Alloc = std::allocator<T>>
class vector : public basic_vector<T, Alloc> {
public:
    using self_t = vector<T, Alloc>;

private:
    using base_type = basic_vector<T, Alloc>;
    void check_empty() const {
        if (empty()) {
            throw EmptyContainer();
        }
    }

    template <typename Ref, typename Ptr>
    class vector_iterator;

public:
    using const_iterator = vector_iterator<const T&, const T*>;
    using iterator = vector_iterator<T&, T*>;

    // the default constructor
    vector() = default;

    // construct the vector with particular size
    explicit vector(size_t init_size) : base_type(init_size) {
        for (int i = 0; i < init_size; ++i) {
            base_type::construct(i);
        }
        base_type::size_ = init_size;
    }

    vector(size_t init_size, const T& init_val) : base_type(init_size) {
        for (int i = 0; i < init_size; ++i) {
            base_type::construct(i, init_val);
        }
        base_type::size_ = init_size;
    }

    /* construct from initializer list, the size will be the same with the
     * il. */
    vector(std::initializer_list<T>&& il) noexcept;

    // copy constructor
    vector(const self_t& rhs) : base_type(rhs) {}

    // moving copy constructor
    vector(self_t&& rhs) noexcept : base_type(std::move(rhs)) {}

    // the destructor
    ~vector() override = default;

    // return whether the vector is empty
    bool empty() const {
        return base_type::size() == 0;
    }

    void shrink() {
        base_type::resize(base_type::size());
    }

    void resize(size_t new_size) {}

    // return a vector contains the elements [begin, stop)
    self_t splice(size_t begin, size_t stop);

    const T& front() const {
        check_empty();
        return base_type::data()[0];
    }

    const T& back() const {
        check_empty();
        return base_type::at(base_type::size() - 1);
    }

    T& front() {
        return const_cast<T&>(static_cast<const self_t*>(this)->front());
    }

    T& back() {
        return const_cast<T&>(static_cast<const self_t*>(this)->back());
    }

    // push a new element to front, with perfect forwarding
    template <typename V>
    void push_front(V&& elem) {
        insert(begin(), std::forward<V>(elem));
    }

    // remove the first element.
    void pop_front() {
        if (!remove(begin())) {
            throw std::out_of_range("There's no element to be popped out.");
        }
    }

    /* insert an element at position index,
     * return an iterator pointing to the next cell */
    template <typename V>
    iterator insert(iterator index, V&& elem) noexcept;

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
        return base_type::data() == vec.data();
    }

    // the copy assignment operator
    self_t& operator=(const self_t& rhs) = default;

    // the moving assignment operator
    self_t& operator=(self_t&& rhs) noexcept {
        base_type::operator=(std::move(rhs));
        return *this;
    }

    // return a vector_iterator pointing to the position 0
    const_iterator cbegin() const {
        return const_iterator(const_cast<T*>(base_type::data()));
    }

    /* return a vector_iterator pointing to the position after the last
     * element */
    const_iterator cend() const {
        return const_iterator(const_cast<T*>(base_type::data()) +
                              base_type::size());
    }

    // return an iterator pointing to the first element
    iterator begin() {
        return iterator(base_type::data());
    }

    // return an iterator pointing to the element behind the last one
    iterator end() {
        return iterator(base_type::data() + base_type::size());
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
vector<T, Alloc>::vector(std::initializer_list<T>&& il) noexcept
    : base_type(il.size()) {
    base_type::size_ = il.size();
    auto itr = il.begin();
    auto data = base_type::data();
    for (int i = 0; i < il.size(); ++i) {
        base_type::construct(i, std::move(*(itr++)));
    }
}

template <typename T, typename Alloc>
typename vector<T, Alloc>::self_t vector<T, Alloc>::splice(size_t begin,
                                                           size_t stop) {
    size_t size = stop - begin;
    self_t vec(size);
    vec.size_ = size;
    for (size_t i = 0; i < size; ++i) {
        vec[i] = base_type::data()[begin + i];
    }

    return vec;
}

template <typename T, typename Alloc>
template <typename V>
typename vector<T, Alloc>::iterator
vector<T, Alloc>::insert(iterator index, V&& elem) noexcept {
    if (index > this->end()) {
        return iterator();
    }
    if (base_type::size() + 1 > base_type::capacity()) {
        size_t len = index - begin();
        auto old_data = base_type::data();
        auto old_capacity = base_type::capacity();

        base_type::allocate(old_capacity == 0 ? 1 : old_capacity * 2);
        for (int i = 0; i < len; ++i) {
            base_type::construct(i, std::move(old_data[i]));
            std::destroy_at(old_data + i);
        }
        base_type::construct(len, std::forward<V>(elem));
        for (int i = len; i < base_type::size(); ++i) {
            base_type::construct(i + 1, std::move(old_data[i]));
            std::destroy_at(old_data + i);
        }
        base_type::allocator_.deallocate(old_data, old_capacity);
        index = this->begin() + len + 1;
    } else {
        auto itr1 = this->end() - 1;
        auto itr2 = itr1 - 1;
        base_type::construct(base_type::size(), std::move(*itr1));
        while (itr1 > index) {
            *(itr1--) = std::move(*(itr2--));
        }
        *(index++) = std::forward<V>(elem);
    }

    ++base_type::size_;
    return index;
}

template <typename T, typename Alloc>
template <typename InputIterator>
typename vector<T, Alloc>::iterator
vector<T, Alloc>::insert(iterator index, InputIterator begin,
                         InputIterator end) {
    // check the validity of index
    if (index > this->end()) {
        return iterator();
    }

    size_t len = end - begin;

    // check whether the capacity is big enough
    if (base_type::size() + len > base_type::capacity()) {
        size_t len2 = index - this->begin();

        auto old_data = base_type::data();
        auto old_capacity = base_type::capacity();

        size_t new_capacity = old_capacity * 2;
        while (new_capacity < old_capacity + len) {
            new_capacity *= 2;
        }
        base_type::allocate(new_capacity);

        for (int i = 0; i < len2; ++i) {
            base_type::construct(i, std::move(old_data[i]));
            std::destroy_at(old_data + i);
        }
        for (int i = 0; i < len; ++i) {
            base_type::construct(len2 + i, *begin);
            ++begin;
        }
        for (int i = len2; i < base_type::size(); ++i) {
            base_type::construct(i + len, std::move(old_data[i]));
            std::destroy_at(old_data + i);
        }
        base_type::allocator_.deallocate(old_data, old_capacity);
        index = this->begin() + len2 + len;
    } else {
        // move elements backward
        size_t old_size = base_type::size();
        auto data = base_type::data();
        for (int i = old_size + len - 1; i >= old_size; --i) {
            base_type::construct(i, std::move(data[i - len]));
        }
        auto itr1 = this->end() - 1;
        auto itr2 = itr1 - len;
        while (itr1 >= index + len) {
            *(itr1--) = std::move(*(itr2--));
        }
        while (begin != end) {
            *(index++) = *(begin++);
        }
    }

    base_type::size_ += len;
    return index;
}

template <typename T, typename Alloc>
typename vector<T, Alloc>::iterator
vector<T, Alloc>::remove(iterator index) noexcept {
    // check whether the position is valid
    if (index >= this->end()) {
        return iterator();
    }

    // move the following elements
    auto itr1 = index;
    auto itr2 = index + 1;
    while (itr2 != this->end()) {
        *(itr1++) = std::move(*(itr2++));
    }
    base_type::destroy(this->size() - 1);
    --base_type::size_;

    return index;
}

template <typename T, typename Alloc>
typename vector<T, Alloc>::iterator
vector<T, Alloc>::remove(iterator begin, iterator stop) noexcept {
    // check whether the range is valid
    if (begin >= stop || begin >= this->end()) {
        return iterator();
    }

    // move the elements
    difference_t wid = stop - begin;
    auto itr1 = begin;
    auto itr2 = stop;
    while (itr2 != this->end()) {
        *(itr1++) = std::move(*(itr2++));
    }
    size_t size = base_type::size();
    for (int i = size - 1; i >= size - wid; --i) {
        base_type::destroy(i);
    }

    base_type::size_ -= wid;
    return begin;
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

    vector_iterator(const self_t& rhs) : elem_(rhs.elem_) {}

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

    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    self_t& operator=(const Iter& rhs) {
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

template <typename T>
vector<T>::const_iterator advance(typename vector<T>::const_iterator iter,
                                  difference_t n) {
    return iter + n;
}

template <typename T>
vector<T>::const_iterator distance(typename vector<T>::const_iterator first,
                                   typename vector<T>::const_iterator last) {
    return last - first;
}

template <typename T>
vector<T>::iterator advance(typename vector<T>::iterator iter, difference_t n) {
    return iter + n;
}

template <typename T>
vector<T>::iterator distance(typename vector<T>::iterator first,
                             typename vector<T>::iterator last) {
    return last - first;
}

}  // namespace mtl
#endif  // MTL_VECTOR_H
