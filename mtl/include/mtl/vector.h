#ifndef MTL_VECTOR_H
#define MTL_VECTOR_H

#include <deque>
#include <initializer_list>
#include <mtl/basic_vector.h>
#include <mtl/types.h>
#include <stdexcept>
#include <type_traits>

// The namespace where the ADTs are.
namespace mtl {
/* The vector ADT, it can expand its data array to double size when space is
 * not enough. */
template <typename T>
class vector : public basic_vector<T> {
private:
    // the number of elements
    size_t size_;

    void check_capacity() {
        if (size_ >= basic_vector<T>::capacity()) {
            if (size_ == 0) {
                basic_vector<T>::expand(1);
            } else {
                basic_vector<T>::expand(size_ * 2);
            }
        }
    }

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
    vector() : size_(0) {}

    // construct the vector with particular size
    explicit vector(size_t capa) : basic_vector<T>(capa), size_(0) {}

    /* construct from initializer list, the size will be the same with the
     * il. */
    vector(std::initializer_list<T>&& il) noexcept;

    // copy constructor
    vector(const vector<T>& rhs) : basic_vector<T>(rhs), size_(rhs.size_) {}

    // moving copy constructor
    vector(vector<T>&& rhs) noexcept {
        size_ = rhs.size_;
        basic_vector<T>::operator=(std::move(rhs));
    }

    // the destructor
    ~vector() override = default;

    // return whether the vector is empty
    [[nodiscard]] bool empty() const {
        return size_ == 0;
    }

    // return the size
    [[nodiscard]] size_t size() const {
        return size_;
    }

    void shrink() {
        basic_vector<T>::shrink(size_);
    }

    // delete the array and assign nullptr to data_
    void clear() override {
        basic_vector<T>::clear();
        size_ = 0;
    }

    /* return the reference to the element at position index
     * it don't check the boundary */
    const T& operator[](size_t index) const {
        return basic_vector<T>::data()[index];
    }

    // the const version
    T& operator[](size_t index) {
        return basic_vector<T>::data()[index];
    }

    /* the same with operator[] but check the boundary
     * it throws an out_of_range exception */
    const T& at(size_t index) const {
        if (index < size_) {
            return basic_vector<T>::data[index];
        } else {
            throw std::out_of_range("The index is out of range.");
        }
    }

    // the normal version
    T& at(size_t index) {
        return const_cast<T&>(static_cast<const vector<T>*>(this)->at(index));
    }

    // return a vector contains the elements [begin, stop)
    vector<T> splice(size_t begin, size_t stop);

    const T& front() const {
        check_empty();
        return basic_vector<T>::data()[0];
    }

    const T& back() const {
        check_empty();
        return basic_vector<T>::data[size_ - 1];
    }

    T& front() {
        return const_cast<T&>(static_cast<const vector<T>*>(this)->front());
    }

    T& back() {
        return const_cast<T&>(static_cast<const vector<T>*>(this)->back());
    }

    // push a new element to back, with perfect forwarding
    template <typename V>
    void push_back(V&& elem) {
        check_capacity();
        basic_vector<T>::data()[size_] = std::forward<V>(elem);
        ++size_;
    }

    // remove the last element (simply decrease the size_)
    void pop_back() {
        if (size_ == 0) {
            throw std::out_of_range("There's no element to be popped out.");
        }
        --size_;
    }

    // push a new element to front, with perfect forwarding
    template <typename V>
    void push_front(V&& elem) {
        insert(begin(), std::forward<V>(elem));
    }

    // remove the first element.
    void pop_front() {
        remove(begin());
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
    bool operator==(const vector<T>& vec) const {
        return basic_vector<T>::data() == vec.data();
    }

    // the copy assignment operator
    vector<T>& operator=(const vector<T>& rhs) = default;

    // the moving assignment operator
    vector<T>& operator=(vector<T>&& rhs) noexcept {
        size_ = rhs.size_;
        basic_vector<T>::operator=(std::move(rhs));
        rhs.size_ = 0;
        return *this;
    }

    // return a vector_iterator pointing to the position 0
    const_iterator cbegin() const {
        return const_iterator(basic_vector<T>::data());
    }

    /* return a vector_iterator pointing to the position after the last
     * element */
    const_iterator cend() const {
        return const_iterator(basic_vector<T>::data() + size_);
    }

    // return an iterator pointing to the first element
    iterator begin() {
        return iterator(basic_vector<T>::data());
    }

    // return an iterator pointing to the element behind the last one
    iterator end() {
        return iterator(basic_vector<T>::data() + size_);
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

template <typename T>
vector<T>::vector(std::initializer_list<T>&& il) noexcept
    : basic_vector<T>(il.size()), size_(il.size()) {
    auto itr = il.begin();
    auto data_ = basic_vector<T>::data();
    for (int i = 0; i < size_; ++i) {
        data_[i] = std::move(*(itr++));
    }
}

template <typename T>
vector<T> vector<T>::splice(size_t begin, size_t stop) {
    size_t size = stop - begin;
    vector<T> vec(size);
    for (size_t i = 0; i < size; ++i) {
        vec[i] = basic_vector<T>::data[begin + i];
    }

    return vec;
}

template <typename T>
template <typename V>
typename vector<T>::iterator vector<T>::insert(iterator index,
                                               V&& elem) noexcept {
    if (index > this->end()) {
        return iterator();
    }
    if (size_ >= basic_vector<T>::capacity()) {
        size_t len = index - begin();
        basic_vector<T>::expand(size_ * 2);
        index = this->begin() + len;
    }
    for (iterator i = this->end(), j = i - 1; i != index; --i, --j) {
        *i = std::move(*j);
    }
    *index = std::forward<V>(elem);
    ++size_;
    return ++index;
}

template <typename T>
template <typename InputIterator>
typename vector<T>::iterator
vector<T>::insert(iterator index, InputIterator begin, InputIterator end) {
    // check the validity of index
    if (index > this->end()) {
        return iterator();
    }

    size_t len = end - begin;

    // check whether the capacity is big enough
    size_ += len;
    if (size_ > basic_vector<T>::capacity()) {
        size_t len2 = index - begin;
        basic_vector<T>::expand(size_ * 2);
        index = this->begin() + len2;
    }

    // move elements backward
    for (iterator i = this->end() - 1, j = i - len; i >= index + len;
         --i, --j) {
        *i = *j;
    }

    // place elements in the gap
    for (auto itr = begin; itr != end; ++index, ++itr) {
        *index = *itr;
    }

    return index;
}

template <typename T>
typename vector<T>::iterator vector<T>::remove(iterator index) noexcept {
    // check whether the position is valid
    if (index >= this->end()) {
        return iterator();
    }

    // move the following elements
    for (iterator i = index; i != this->end() - 1; ++i) {
        *i = std::move(*(i + 1));
    }
    --size_;

    return index;
}

template <typename T>
typename vector<T>::iterator vector<T>::remove(iterator begin,
                                               iterator stop) noexcept {
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

    size_ -= wid;
    return begin;
}

template <typename T>
template <typename Ref, typename Ptr>
class vector<T>::vector_iterator {
private:
    T* elem_; // pointer to the element
    using self_t = vector_iterator<Ref, Ptr>;

public:
    vector_iterator() : elem_(nullptr) {}

    virtual ~vector_iterator() = default;

    // construct from pointer
    explicit vector_iterator(const T* elem) : elem_(elem) {}

    template <typename Iter,
              typename = std::_Require<std::is_same<Iter, iterator>,
                                       std::is_same<self_t, const_iterator>>>
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
              typename = std::_Require<std::is_same<Iter, iterator>,
                                       std::is_same<self_t, const_iterator>>>
    self_t& operator=(const Iter& rhs) {
        elem_ = rhs.elem_;
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

    difference_t operator-(const vector_iterator& rhs) const {
        return elem_ - rhs.elem_;
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
} // namespace mtl
#endif // MTL_VECTOR_H
