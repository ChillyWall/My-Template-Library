#ifndef MTL_VECTOR_H
#define MTL_VECTOR_H

#include <mtl/basic_vector.h>
#include <mtl/types.h>
#include <initializer_list>
#include <stdexcept>

// The namespace where the ADTs are.
namespace mtl {
/* The vector ADT, it can expand its data array to double size when space is
 * not enough. */
template <typename T, typename Alloc = std::allocator<T>>
class vector : public basic_vector<T, Alloc> {
private:
    void check_empty() const {
        if (empty()) {
            throw EmptyContainer();
        }
    }

    template <typename Ref, typename Ptr>
    class vector_iterator;

    using const_iterator = vector_iterator<const T&, const T*>;
    using iterator = vector_iterator<T&, T*>;

public:
    // the default constructor
    vector() {}

    // construct the vector with particular size
    explicit vector(size_t init_size) : basic_vector<T, Alloc>(init_size) {
        for (int i = 0; i < init_size; ++i) {
            basic_vector<T, Alloc>::construct(i);
        }
        basic_vector<T, Alloc>::size_ = init_size;
    }

    vector(size_t init_size, const T& init_val)
        : basic_vector<T, Alloc>(init_size) {
        for (int i = 0; i < init_size; ++i) {
            basic_vector<T, Alloc>::construct(i, init_val);
        }
        basic_vector<T, Alloc>::size_ = init_size;
    }

    /* construct from initializer list, the size will be the same with the
     * il. */
    vector(std::initializer_list<T>&& il) noexcept;

    // copy constructor
    vector(const vector<T, Alloc>& rhs) : basic_vector<T, Alloc>(rhs) {}

    // moving copy constructor
    vector(vector<T, Alloc>&& rhs) noexcept
        : basic_vector<T, Alloc>(std::move(rhs)) {}

    // the destructor
    ~vector() override = default;

    // return whether the vector is empty
    bool empty() const {
        return basic_vector<T, Alloc>::size() == 0;
    }

    void shrink() {
        basic_vector<T, Alloc>::resize(basic_vector<T, Alloc>::size());
    }

    void resize(size_t new_size) {}

    // return a vector contains the elements [begin, stop)
    vector<T, Alloc> splice(size_t begin, size_t stop);

    const T& front() const {
        check_empty();
        return basic_vector<T, Alloc>::data()[0];
    }

    const T& back() const {
        check_empty();
        return basic_vector<T, Alloc>::at(basic_vector<T, Alloc>::size() - 1);
    }

    T& front() {
        return const_cast<T&>(
            static_cast<const vector<T, Alloc>*>(this)->front());
    }

    T& back() {
        return const_cast<T&>(
            static_cast<const vector<T, Alloc>*>(this)->back());
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
    bool operator==(const vector<T, Alloc>& vec) const {
        return basic_vector<T, Alloc>::data() == vec.data();
    }

    // the copy assignment operator
    vector<T, Alloc>& operator=(const vector<T, Alloc>& rhs) = default;

    // the moving assignment operator
    vector<T, Alloc>& operator=(vector<T, Alloc>&& rhs) noexcept {
        basic_vector<T, Alloc>::operator=(std::move(rhs));
        return *this;
    }

    // return a vector_iterator pointing to the position 0
    const_iterator cbegin() const {
        return const_iterator(const_cast<T*>(basic_vector<T, Alloc>::data()));
    }

    /* return a vector_iterator pointing to the position after the last
     * element */
    const_iterator cend() const {
        return const_iterator(const_cast<T*>(basic_vector<T, Alloc>::data()) +
                              basic_vector<T, Alloc>::size());
    }

    // return an iterator pointing to the first element
    iterator begin() {
        return iterator(basic_vector<T, Alloc>::data());
    }

    // return an iterator pointing to the element behind the last one
    iterator end() {
        return iterator(basic_vector<T, Alloc>::data() +
                        basic_vector<T, Alloc>::size());
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
    : basic_vector<T, Alloc>(il.size()) {
    basic_vector<T, Alloc>::size_ = il.size();
    auto itr = il.begin();
    auto data = basic_vector<T, Alloc>::data();
    for (int i = 0; i < il.size(); ++i) {
        basic_vector<T, Alloc>::construct(i, std::move(*(itr++)));
    }
}

template <typename T, typename Alloc>
vector<T, Alloc> vector<T, Alloc>::splice(size_t begin, size_t stop) {
    size_t size = stop - begin;
    vector<T, Alloc> vec(size);
    vec.size_ = size;
    for (size_t i = 0; i < size; ++i) {
        vec[i] = basic_vector<T, Alloc>::data()[begin + i];
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
    if (basic_vector<T, Alloc>::size() + 1 >
        basic_vector<T, Alloc>::capacity()) {
        size_t len = index - begin();
        auto old_data = basic_vector<T, Alloc>::data();
        auto old_capacity = basic_vector<T, Alloc>::capacity();

        basic_vector<T, Alloc>::allocate(old_capacity == 0 ? 1
                                                           : old_capacity * 2);
        for (int i = 0; i < len; ++i) {
            basic_vector<T, Alloc>::construct(i, std::move(old_data[i]));
            std::destroy_at(old_data + i);
        }
        basic_vector<T, Alloc>::construct(len, std::forward<V>(elem));
        for (int i = len; i < basic_vector<T, Alloc>::size(); ++i) {
            basic_vector<T, Alloc>::construct(i + 1, std::move(old_data[i]));
            std::destroy_at(old_data + i);
        }
        basic_vector<T, Alloc>::allocator_.deallocate(old_data, old_capacity);
        index = this->begin() + len + 1;
    } else {
        auto itr1 = this->end() - 1;
        auto itr2 = itr1 - 1;
        basic_vector<T, Alloc>::construct(basic_vector<T, Alloc>::size(),
                                          std::move(*itr1));
        while (itr1 > index) {
            *(itr1--) = std::move(*(itr2--));
        }
        *(index++) = std::forward<V>(elem);
    }

    ++basic_vector<T, Alloc>::size_;
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
    if (basic_vector<T, Alloc>::size() + len >
        basic_vector<T, Alloc>::capacity()) {
        size_t len2 = index - this->begin();

        auto old_data = basic_vector<T, Alloc>::data();
        auto old_capacity = basic_vector<T, Alloc>::capacity();

        size_t new_capacity = old_capacity * 2;
        while (new_capacity < old_capacity + len) {
            new_capacity *= 2;
        }
        basic_vector<T, Alloc>::allocate(new_capacity);

        for (int i = 0; i < len2; ++i) {
            basic_vector<T, Alloc>::construct(i, std::move(old_data[i]));
            std::destroy_at(old_data + i);
        }
        for (int i = 0; i < len; ++i) {
            basic_vector<T, Alloc>::construct(len2 + i, *begin);
            ++begin;
        }
        for (int i = len2; i < basic_vector<T, Alloc>::size(); ++i) {
            basic_vector<T, Alloc>::construct(i + len, std::move(old_data[i]));
            std::destroy_at(old_data + i);
        }
        basic_vector<T, Alloc>::allocator_.deallocate(old_data, old_capacity);
        index = this->begin() + len2 + len;
    } else {
        // move elements backward
        size_t old_size = basic_vector<T, Alloc>::size();
        auto data = basic_vector<T, Alloc>::data();
        for (int i = old_size + len - 1; i >= old_size; --i) {
            basic_vector<T, Alloc>::construct(i, std::move(data[i - len]));
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

    basic_vector<T, Alloc>::size_ += len;
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
    basic_vector<T, Alloc>::destroy(this->size() - 1);
    --basic_vector<T, Alloc>::size_;

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
    size_t size = basic_vector<T, Alloc>::size();
    for (int i = size - 1; i >= size - wid; --i) {
        basic_vector<T, Alloc>::destroy(i);
    }

    basic_vector<T, Alloc>::size_ -= wid;
    return begin;
}

template <typename T, typename Alloc>
template <typename Ref, typename Ptr>
class vector<T, Alloc>::vector_iterator {
private:
    T* elem_;  // pointer to the element
    using self_t = vector_iterator<Ref, Ptr>;

public:
    vector_iterator() : elem_(nullptr) {}

    ~vector_iterator() = default;

    // construct from pointer
    explicit vector_iterator(T* elem) : elem_(elem) {}

    template <typename Iter,
              typename = std::enable_if_t<
                  std::is_same<self_t, const_iterator>::value &&
                  std::is_same<Iter, iterator>::value>>
    vector_iterator(const Iter& rhs) : elem_(rhs.elem_) {}

    vector_iterator(const vector_iterator& rhs) : elem_(rhs.elem_) {}

    // return a reference to the element
    Ref operator*() const {
        return *elem_;
    }

    Ptr operator->() const {
        return elem_;
    }

    // compare the pointer
    friend bool operator>(const vector_iterator& lhs,
                          const vector_iterator& rhs) {
        return lhs.elem_ > rhs.elem_;
    }

    // compare the pointer
    friend bool operator<(const vector_iterator& lhs,
                          const vector_iterator& rhs) {
        return lhs.elem_ < rhs.elem_;
    }

    // compare the pointer
    friend bool operator<=(const vector_iterator& lhs,
                           const vector_iterator& rhs) {
        return lhs.elem_ <= rhs.elem_;
    }

    // compare the pointer
    friend bool operator>=(const vector_iterator& lhs,
                           const vector_iterator& rhs) {
        return lhs.elem_ >= rhs.elem_;
    }

    // compare the pointer
    friend bool operator==(const vector_iterator& lhs,
                           const vector_iterator& rhs) {
        return lhs.elem_ == rhs.elem_;
    }

    // compare the pointer
    friend bool operator!=(const vector_iterator& lhs,
                           const vector_iterator& rhs) {
        return lhs.elem_ != rhs.elem_;
    }

    explicit operator bool() const {
        return elem_;
    }

    self_t& operator=(const vector_iterator& rhs) = default;

    template <typename Iter,
              typename = std::enable_if_t<
                  std::is_same<self_t, const_iterator>::value &&
                  std::is_same<Iter, iterator>::value>>
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

    friend difference_t operator-(const vector_iterator& lhs,
                                  const vector_iterator& rhs) {
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
