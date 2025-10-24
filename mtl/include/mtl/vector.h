#ifndef MTL_VECTOR_H
#define MTL_VECTOR_H

#include <mtl/mtldefs.h>
#include <mtl/mtlutils.h>
#include <initializer_list>
#include <stdexcept>

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

    /* allocate a new array with length size and return the pointer to the old
     * one (the pointer is nodiscard and must be released) */
    [[nodiscard]] T* allocate(size_t new_capacity) {
        capacity_ = new_capacity;
        T* tmp = data_;
        data_ = allocator_.allocate(new_capacity);
        return tmp;
    }

    template <typename... Args>
    void construct(size_t index, Args&&... args) {
        std::construct_at(data_ + index, std::forward<Args>(args)...);
    }

    void construct_all() {
        for (size_t i = 0; i < capacity_; ++i) {
            construct(i);
        }
    }

    void destroy(size_t index) noexcept {
        std::destroy_at(data_ + index);
    }

    void destroy_all() {
        for (size_t i = 0; i < capacity_; ++i) {
            destroy(i);
        }
    }

    void deallocate() {
        deallocate(data_, capacity_);
        data_ = nullptr;
        capacity_ = 0;
    }

    void deallocate(T* array, size_t length) {
        allocator_.deallocate(array, length);
    }

    void expand(size_t new_size) noexcept;
    void shrink(size_t new_size) noexcept;

    template <typename Ref, typename Ptr>
    class vector_iterator;

public:
    using const_iterator = vector_iterator<const T&, const T*>;
    using iterator = vector_iterator<T&, T*>;

    // the default constructor
    vector() : capacity_(0), size_(0), data_(nullptr) {}

    // construct the vector with particular size
    explicit vector(size_t init_size) : capacity_(init_size), size_(init_size) {
        auto old = allocate(capacity_);
        for (int i = 0; i < init_size; ++i) {
            construct(i);
        }
    }

    vector(size_t init_size, const T& init_val)
        : capacity_(init_size), size_(init_size) {
        auto old = allocate(capacity_);
        for (int i = 0; i < init_size; ++i) {
            construct(i, init_val);
        }
    }

    /* construct from initializer list, the size will be the same with the
     * il. */
    vector(std::initializer_list<T> il) noexcept
        : capacity_(il.size()), size_(il.size()) {
        auto old = allocate(capacity_);
        auto itr = il.begin();
        for (int i = 0; i < il.size(); ++i) {
            construct(i, std::move(*(itr++)));
        }
    }

    // copy constructor
    vector(const self_t& rhs) : capacity_(rhs.size()), size_(rhs.size()) {
        auto old = allocate(capacity_);
        for (size_t i = 0; i < rhs.size(); ++i) {
            construct(i, rhs[i]);
        }
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
        destroy_all();
        deallocate();
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
        destroy_all();
        size_ = 0;
        deallocate();
    }

    void shrink_to_fit() noexcept;

    void reserve(size_t capacity) noexcept;

    void resize(size_t new_size) noexcept;

    const T& front() const {
        check_empty();
        return data()[0];
    }

    const T& back() const {
        check_empty();
        return at(size() - 1);
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
        check_empty();
        remove(begin());
    }

    template <typename V>
    void push_back(V&& elem) {
        if (size() + 1 > capacity()) {
            reserve(capacity() == 0 ? 1 : capacity() * 2);
        }
        construct(size(), std::forward<V>(elem));
        ++size_;
    }

    void pop_back() {
        check_empty();
        destroy(size() - 1);
        --size_;
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
        return data() == vec.data();
    }

    // the copy assignment operator
    self_t& operator=(const self_t& rhs) {
        if (&rhs == this) {
            return *this;
        }
        clear();
        auto tmp = allocate(rhs.capacity_);
        size_ = rhs.size_;
        for (size_t i = 0; i < size_; ++i) {
            construct(i, rhs[i]);
        }
    }

    // the moving assignment operator
    self_t& operator=(self_t&& rhs) noexcept {
        if (this == &rhs) {
            return *this;
        }
        destroy_all();
        deallocate();

        // copy the object
        capacity_ = rhs.capacity_;
        data_ = rhs.data_;

        rhs.data_ = nullptr;
        rhs.capacity_ = 0;
        return *this;
    }

    // return a vector_iterator pointing to the position 0
    const_iterator cbegin() const {
        return const_iterator(const_cast<T*>(data()));
    }

    /* return a vector_iterator pointing to the position after the last
     * element */
    const_iterator cend() const {
        return const_iterator(const_cast<T*>(data()) + size());
    }

    // return an iterator pointing to the first element
    iterator begin() {
        return iterator(data());
    }

    // return an iterator pointing to the element behind the last one
    iterator end() {
        return iterator(data() + size());
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
typename vector<T, Alloc>::iterator
vector<T, Alloc>::insert(iterator index, V&& elem) noexcept {
    if (index > this->end()) {
        return iterator();
    }
    if (size() + 1 > capacity()) {
        size_t len = index - begin();
        auto old_capacity = capacity();
        auto old_data = allocate(old_capacity == 0 ? 1 : old_capacity * 2);

        for (int i = 0; i < len; ++i) {
            construct(i, std::move(old_data[i]));
            std::destroy_at(old_data + i);
        }
        construct(len, std::forward<V>(elem));
        for (size_t i = len; i < size(); ++i) {
            construct(i + 1, std::move(old_data[i]));
            std::destroy_at(old_data + i);
        }
        deallocate(old_data, old_capacity);
        index = this->begin() + len + 1;
    } else {
        auto itr1 = this->end() - 1;
        auto itr2 = itr1 - 1;
        construct(size(), std::move(*itr1));
        while (itr1 > index) {
            *(itr1--) = std::move(*(itr2--));
        }
        *(index++) = std::forward<V>(elem);
    }

    ++size_;
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
    if (size() + len > capacity()) {
        size_t len2 = index - this->begin();

        auto old_capacity = capacity();
        size_t new_capacity = old_capacity * 2;
        while (new_capacity < old_capacity + len) {
            new_capacity *= 2;
        }
        auto old_data = allocate(new_capacity);

        for (int i = 0; i < len2; ++i) {
            construct(i, std::move(old_data[i]));
            std::destroy_at(old_data + i);
        }
        for (int i = 0; i < len; ++i) {
            construct(len2 + i, *begin);
            ++begin;
        }
        for (size_t i = len2; i < size(); ++i) {
            construct(i + len, std::move(old_data[i]));
            std::destroy_at(old_data + i);
        }
        deallocate(old_data, old_capacity);
        index = this->begin() + len2 + len;
    } else {
        // move elements backward
        size_t old_size = size();
        auto data = this->data();
        for (size_t i = old_size + len - 1; i >= old_size; --i) {
            construct(i, std::move(data[i - len]));
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

    size_ += len;
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
    destroy(size() - 1);
    --size_;

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
    size_t size = this->size();
    for (size_t i = size - 1; i >= size - wid; --i) {
        destroy(i);
    }

    size_ -= wid;
    return begin;
}

template <typename T, typename Alloc>
void vector<T, Alloc>::expand(size_t new_size) noexcept {
    if (new_size >= capacity()) {
        reserve(new_size);
    } else if (new_size <= size_) {
        return;
    }

    for (size_t i = size_; i < new_size; ++i) {
        construct(i);
    }
    size_ = new_size;
}

template <typename T, typename Alloc>
void vector<T, Alloc>::shrink(size_t new_size) noexcept {
    if (new_size >= size_) {
        return;
    }
    for (size_t i = new_size; i < size_; ++i) {
        destroy(i);
    }
    size_ = new_size;
}

template <typename T, typename Alloc>
void vector<T, Alloc>::reserve(size_t capacity) noexcept {
    if (capacity <= capacity_) {
        return;
    }
    auto old_capacity = this->capacity();
    auto old_data = allocate(capacity);
    for (int i = 0; i < size_; ++i) {
        construct(i, std::move(old_data[i]));
        std::destroy_at(old_data + i);
    }
    deallocate(old_data, old_capacity);
}

template <typename T, typename Alloc>
void vector<T, Alloc>::shrink_to_fit() noexcept {
    if (size_ == capacity_) {
        return;
    }
    auto old_capacity = capacity_;
    auto old_data = allocate(size_);
    for (int i = 0; i < size_; ++i) {
        construct(i, std::move(old_data[i]));
        std::destroy_at(old_data + i);
    }
    deallocate(old_data, old_capacity);
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
