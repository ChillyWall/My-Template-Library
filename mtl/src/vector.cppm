export module mtl.vector;

export import mtl.core;

import std;
// The namespace where the ADTs are.
export namespace mtl {

/**
 * @brief Dynamic array container that grows as elements are added.
 *
 * The vector ADT expands its internal storage (typically doubling) when
 * additional capacity is required.
 *
 * @tparam T Type of elements stored.
 * @tparam Alloc Allocator type used for memory management.
 */
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

    /**
     * @brief Throw if container is empty.
     *
     * Helper used to validate that the container is not empty before
     * operations that require at least one element.
     */
    void check_empty() const {
        if (empty()) {
            throw EmptyContainer();
        }
    }

    /**
     * @brief Allocate raw storage for elements.
     *
     * @param new_capacity Number of elements to allocate storage for.
     * @return Pointer to uninitialized storage for T objects.
     */
    [[nodiscard]] T* allocate(size_t new_capacity) {
        return allocator_.allocate(new_capacity);
    }

    /**
     * @brief Deallocate previously allocated storage.
     *
     * @param array Pointer returned from allocate.
     * @param length Number of elements the storage was allocated for.
     */
    void deallocate(T* array, size_t length) {
        allocator_.deallocate(array, length);
    }

    /**
     * @brief Forward declaration for iterator implementation.
     *
     * @tparam Ref Reference type returned by operator*().
     * @tparam Ptr Pointer type returned by operator->().
     */
    template <typename Ref, typename Ptr>
    class vector_iterator;

public:
    /**
     * @brief Iterator type that yields const references.
     *
     * @tparam See vector_iterator documentation for template params.
     */
    using const_iterator = vector_iterator<const T&, const T*>;

    /**
     * @brief Iterator type that yields mutable references.
     *
     * @tparam See vector_iterator documentation for template params.
     */
    using iterator = vector_iterator<T&, T*>;

    /**
     * @brief Default-construct an empty vector.
     */
    vector() : capacity_(0), size_(0), data_(nullptr) {}

    /**
     * @brief Construct a vector with given size value-initialized.
     *
     * @param init_size Number of elements to create.
     */
    explicit vector(size_t init_size)
        : capacity_(init_size), size_(init_size), data_(allocate(init_size)) {
        construct_all(data_, init_size);
    }

    /**
     * @brief Construct a vector with given size and initialize elements.
     *
     * @param init_size Number of elements to create.
     * @param init_val Value to copy-construct each element from.
     */
    vector(size_t init_size, const T& init_val)
        : capacity_(init_size), size_(init_size), data_(allocate(capacity_)) {
        for (int i = 0; i < init_size; ++i) {
            construct(data_, i, init_val);
        }
    }

    /**
     * @brief Construct from initializer list.
     *
     * @param il Initializer list whose elements are copied/moved into the
     *           vector.
     */
    vector(std::initializer_list<T> il) noexcept
        : capacity_(il.size()), size_(il.size()), data_(allocate(il.size())) {
        auto itr = il.begin();
        for (int i = 0; i < il.size(); ++i, ++itr) {
            construct(data_, i, std::move_if_noexcept(*itr));
        }
    }

    /**
     * @brief Copy-construct from another vector (deep copy of elements).
     *
     * @param rhs Source vector to copy from.
     */
    vector(const self_t& rhs)
        : capacity_(rhs.size()),
          size_(rhs.size()),
          data_(allocate(rhs.size())) {
        std::uninitialized_copy_n(rhs.data_, rhs.size_, data_);
    }

    /**
     * @brief Move-construct by taking ownership of another vector's storage.
     *
     * @param rhs Rvalue source to move from.
     */
    vector(self_t&& rhs) noexcept
        : data_(rhs.data_), capacity_(rhs.capacity_), size_(rhs.size_) {
        rhs.data_ = nullptr;
        rhs.capacity_ = 0;
        rhs.size_ = 0;
    }

    /**
     * @brief Destructor destroys elements and frees storage.
     */
    ~vector() noexcept {
        destroy_all(data_, size_);
        deallocate(data_, capacity_);
    }

    /**
     * @brief Access element without bounds checking (const).
     *
     * @param index Index of element to access.
     * @return const reference to element at index.
     */
    const T& operator[](size_t index) const {
        return data()[index];
    }

    /**
     * @brief Access element without bounds checking (mutable).
     *
     * @param index Index of element to access.
     * @return reference to element at index.
     */
    T& operator[](size_t index) {
        return const_cast<T&>(
            static_cast<const self_t*>(this)->operator[](index));
    }

    /**
     * @brief Access element with bounds checking (const).
     *
     * @param index Index of element to access.
     * @return const reference to element at index.
     * @throws std::out_of_range if index >= capacity().
     */
    const T& at(size_t index) const {
        if (index >= capacity_) {
            throw std::out_of_range("The index is out of range.");
        }
        return data()[index];
    }

    /**
     * @brief Access element with bounds checking (mutable).
     *
     * @param index Index of element to access.
     * @return reference to element at index.
     */
    T& at(size_t index) {
        return const_cast<T&>(static_cast<const self_t*>(this)->at(index));
    }

    /**
     * @brief Return current storage capacity.
     *
     * @return Number of elements the vector can hold without reallocating.
     */
    [[nodiscard]] size_t capacity() const {
        return capacity_;
    }

    /**
     * @brief Return current number of elements.
     *
     * @return Number of constructed elements in the vector.
     */
    [[nodiscard]] size_t size() const {
        return size_;
    }

    /**
     * @brief Get pointer to underlying data (const).
     *
     * @return Pointer to first element or nullptr if empty.
     */
    const T* data() const {
        return data_;
    }

    /**
     * @brief Get pointer to underlying data (mutable).
     *
     * @return Pointer to first element or nullptr if empty.
     */
    T* data() {
        return const_cast<T*>(static_cast<const self_t*>(this)->data());
    }

    /**
     * @brief Check whether vector contains no elements.
     *
     * @return true if size() == 0.
     */
    [[nodiscard]] bool empty() const {
        return size() == 0;
    }

    /**
     * @brief Destroy all elements and release storage.
     */
    void clear() noexcept {
        destroy_all(data_, size_);
        deallocate(data_, capacity_);
        capacity_ = 0;
        size_ = 0;
        data_ = nullptr;
    }

    /**
     * @brief Reduce capacity to match size, freeing unused storage.
     */
    void shrink_to_fit() noexcept;

    /**
     * @brief Ensure at least the given capacity is available.
     *
     * @param capacity Minimum capacity to reserve.
     */
    void reserve(size_t capacity);

    /**
     * @brief Resize container to contain new_size elements.
     *
     * If new_size > size(), default-construct additional elements. If
     * new_size < size(), destroy trailing elements.
     *
     * @param new_size Desired number of elements.
     */
    void resize(size_t new_size) noexcept;

    /**
     * @brief Access first element (const).
     *
     * @return const reference to first element.
     */
    const T& front() const {
        return at(0);
    }

    /**
     * @brief Access last element (const).
     *
     * @return const reference to last element.
     */
    const T& back() const {
        return at(size() - 1);
    }

    /**
     * @brief Access first element (mutable).
     *
     * @return reference to first element.
     */
    T& front() {
        return const_cast<T&>(static_cast<const self_t*>(this)->front());
    }

    /**
     * @brief Access last element (mutable).
     *
     * @return reference to last element.
     */
    T& back() {
        return const_cast<T&>(static_cast<const self_t*>(this)->back());
    }

    /**
     * @brief Append element to the end (perfect-forwarding overload).
     *
     * @tparam V Value type deduced for forwarding.
     * @param elem Element value to append.
     */
    template <typename V>
    void push_back(V&& elem) {
        // emplace_back(std::forward<V>(elem));
        if (size() + 1 > capacity()) {
            reserve(capacity() == 0 ? 1 : capacity() * 2);
        }
        construct(data_, size(), std::forward<V>(elem));
        ++size_;
    }

    /**
     * @brief Remove the last element.
     *
     * @throws EmptyContainer if the container is empty.
     */
    void pop_back() {
        check_empty();
        destroy(data_, size() - 1);
        --size_;
    }

    /**
     * @brief Construct element in-place at the end.
     *
     * @tparam Args Parameter pack forwarded to element constructor.
     * @param args Arguments used to construct the new element.
     */
    template <typename... Args>
    void emplace_back(Args&&... args) {
        if (size() + 1 > capacity()) {
            reserve(capacity() == 0 ? 1 : capacity() * 2);
        }
        construct(data_, size(), std::forward<Args>(args)...);
        ++size_;
    }

    /**
     * @brief Insert a single element before position index.
     *
     * @tparam V Value type deduced for forwarding.
     * @param index Iterator pointing to insertion position.
     * @param elem Element to insert (forwarded).
     * @return Iterator pointing to the inserted element.
     */
    template <typename V>
    iterator insert(iterator index, V&& elem);

    /**
     * @brief Insert a range of elements before position index.
     *
     * @tparam InputIterator Iterator type for the input range.
     * @param index Insertion position.
     * @param begin Begin of input range.
     * @param end End of input range (one-past-last).
     * @return Iterator pointing to the first of the newly inserted elements.
     */
    template <typename InputIterator>
    iterator insert(iterator index, InputIterator begin, InputIterator end);

    /**
     * @brief Remove element at iterator index.
     *
     * @param index Iterator pointing to element to remove.
     * @return Iterator pointing to the element that followed the erased one.
     */
    iterator remove(iterator index) noexcept;

    /**
     * @brief Remove a range of elements [begin, stop).
     *
     * @param begin Iterator to first element to remove.
     * @param stop Iterator one past the last element to remove.
     * @return Iterator pointing to the position of first removed element.
     */
    iterator remove(iterator begin, iterator stop) noexcept;

    /**
     * @brief Compare whether two vectors share the same underlying storage.
     *
     * @param vec Other vector to compare with.
     * @return true if both vectors refer to the same data pointer.
     */
    bool operator==(const self_t& vec) const {
        return data() == vec.data();
    }

    /**
     * @brief Copy-assign from another vector (deep copy).
     *
     * @param rhs Source vector to copy from.
     * @return Reference to *this.
     */
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

    /**
     * @brief Move-assign by taking ownership of rhs storage.
     *
     * @param rhs Rvalue source to move from.
     * @return Reference to *this.
     */
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

    /**
     * @brief Return const iterator to first element.
     *
     * @return const_iterator pointing to element 0.
     */
    const_iterator cbegin() const {
        return const_iterator(const_cast<T*>(data()));
    }

    /**
     * @brief Return const iterator one past the last element.
     *
     * @return const_iterator pointing to end.
     */
    const_iterator cend() const {
        return const_iterator(
            data() == nullptr ? nullptr : const_cast<T*>(data()) + size());
    }

    /**
     * @brief Return mutable iterator to first element.
     *
     * @return iterator pointing to element 0.
     */
    iterator begin() {
        return iterator(data());
    }

    /**
     * @brief Return mutable iterator one past the last element.
     *
     * @return iterator pointing to end.
     */
    iterator end() {
        return iterator(data() == nullptr ? nullptr : data() + size());
    }

    /**
     * @brief Const overload of begin().
     * @return const_iterator to first element.
     */
    const_iterator begin() const {
        return cbegin();
    }

    /**
     * @brief Const overload of end().
     * @return const_iterator to one past last element.
     */
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

/**
 * @brief Iterator implementation for vector exposing pointer-like semantics.
 *
 * Instances behave like contiguous iterators over the vector's storage.
 *
 * @tparam Ref Reference type returned by operator*().
 * @tparam Ptr Pointer type returned by operator->().
 */
template <typename T, typename Alloc>
template <typename Ref, typename Ptr>
class vector<T, Alloc>::vector_iterator {
private:
    T* elem_;  // pointer to the element
    using self_t = vector_iterator<Ref, Ptr>;
    friend const_iterator;

public:
    /**
     * @brief Default-construct an invalid iterator.
     */
    vector_iterator() : elem_(nullptr) {}

    /**
     * @brief Default destructor.
     */
    ~vector_iterator() = default;

    /**
     * @brief Construct iterator from raw element pointer.
     *
     * @param elem Pointer to element within the vector storage.
     */
    explicit vector_iterator(T* elem) : elem_(elem) {}

    /**
     * @brief Construct from a compatible iterator.
     *
     * @tparam Iter Compatible iterator type.
     * @param rhs Source iterator.
     */
    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    vector_iterator(const Iter& rhs) : elem_(rhs.elem_) {}

    /**
     * @brief Copy-construct from another iterator.
     *
     * @param rhs Source iterator.
     */
    vector_iterator(const self_t& rhs) = default;

    /**
     * @brief Move-construct from another iterator.
     *
     * @param rhs Source iterator.
     */
    vector_iterator(self_t&& rhs) noexcept = default;

    /**
     * @brief Dereference to access referenced element.
     *
     * @return Reference to the element the iterator points to.
     */
    Ref operator*() const {
        return *elem_;
    }

    /**
     * @brief Pointer-like access to element members.
     *
     * @return Pointer to the element the iterator points to.
     */
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

    /**
     * @brief Test whether iterator is pointing to a valid element.
     *
     * @return true if iterator contains a non-null pointer.
     */
    explicit operator bool() const {
        return elem_;
    }

    /**
     * @brief Copy-assign from another iterator.
     *
     * @param rhs Source iterator.
     * @return Reference to this iterator.
     */
    self_t& operator=(const self_t& rhs) = default;

    /**
     * @brief Move-assign from another iterator.
     *
     * @param rhs Source iterator.
     * @return Reference to this iterator.
     */
    self_t& operator=(self_t&& rhs) noexcept = default;

    /**
     * @brief Assign from a compatible iterator.
     *
     * @tparam Iter Compatible iterator type.
     * @param rhs Source iterator.
     * @return Reference to this iterator.
     */
    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    self_t& operator=(const Iter& rhs) {
        elem_ = rhs.elem_;
        return *this;
    }

    /**
     * @brief Move-assign from a compatible iterator.
     *
     * @tparam Iter Compatible iterator type.
     * @param rhs Source iterator.
     * @return Reference to this iterator.
     */
    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    self_t& operator=(Iter&& rhs) noexcept {
        elem_ = rhs.elem_;
        return *this;
    }

    /**
     * @brief Return an iterator advanced by n positions (no bounds check).
     *
     * @param n Number of positions to advance.
     * @return New iterator advanced by n.
     */
    self_t operator+(difference_t n) const {
        auto new_itr = *this;
        new_itr += n;
        return new_itr;
    }

    /**
     * @brief Advance iterator by n positions (no bounds check).
     *
     * @param n Number of positions to advance.
     * @return Reference to *this after advancement.
     */
    self_t& operator+=(difference_t n) {
        elem_ += n;
        return *this;
    }

    /**
     * @brief Return an iterator moved backward by n positions (no bounds
     * check).
     *
     * @param n Number of positions to move backward.
     * @return New iterator moved backward by n.
     */
    self_t operator-(difference_t n) const {
        auto new_itr = *this;
        new_itr -= n;
        return new_itr;
    }

    template <is_one_of<iterator, const_iterator> Iter>
    /**
     * @brief Compute distance between two iterators.
     *
     * @tparam Iter Compatible iterator type.
     * @param lhs Left iterator.
     * @param rhs Right iterator.
     * @return Difference between iterators.
     */
    friend difference_t operator-(const self_t& lhs, const Iter& rhs) {
        return lhs.elem_ - rhs.elem_;
    }

    /**
     * @brief Move iterator backward by n positions (no bounds check).
     *
     * @param n Number of positions to move backward.
     * @return Reference to *this after movement.
     */
    self_t& operator-=(difference_t n) {
        elem_ -= n;
        return *this;
    }

    /**
     * @brief Prefix increment: advance to next element.
     *
     * @return Reference to advanced iterator.
     */
    self_t& operator++() {
        ++elem_;
        return *this;
    }

    /**
     * @brief Postfix increment: advance to next element, returning previous.
     *
     * @return Iterator prior to increment.
     */
    self_t operator++(int) {
        auto new_itr = *this;
        ++elem_;
        return new_itr;
    }

    /**
     * @brief Prefix decrement: move to previous element.
     *
     * @return Reference to decremented iterator.
     */
    self_t& operator--() {
        --elem_;
        return *this;
    }

    /**
     * @brief Postfix decrement: move to previous element, returning previous.
     *
     * @return Iterator prior to decrement.
     */
    self_t operator--(int) {
        auto new_itr = *this;
        --elem_;
        return new_itr;
    }
};

}  // namespace mtl
