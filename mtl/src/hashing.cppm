export module mtl.hashing;

export import mtl.core;
import mtl.algorithms;
import std;

export namespace mtl {

/**
 * @brief an implementation of Hopscotch Hashing
 *
 * @tparam T         the value type
 * @tparam hash_func the functor to hash the value
 * @tparam KeyEqual  the functor to compare keys for equality
 * @tparam Alloc     the allocator type
 */
template <typename T, typename hash_func = std::hash<T>,
          typename KeyEqual = std::equal_to<T>,
          typename Alloc = std::allocator<T>>
class hashing {
public:
    using self_t = hashing<T, hash_func, KeyEqual, Alloc>;

private:
    /**
     * @brief storage cell used by the hashing table
     */
    class Cell;

    /**
     * @brief iterator for traversing occupied cells
     *
     * @tparam Ref reference type returned by operator*
     * @tparam Ptr pointer type returned by operator->
     */
    template <typename Ref, typename Ptr>
    class hashing_iterator;

public:
    using iterator = hashing_iterator<T&, T*>;
    using const_iterator = hashing_iterator<const T&, const T*>;

private:
    static constexpr double MAX_LOAD_FACTOR = 0.8;  // the max load factor
    static constexpr size_t DEFAULT_SIZE = 101;     // the default initial size
    // the max distance of a cell to its home
    static constexpr size_t MAX_DIST = 32;

    size_t size_ {0};  // the number of elements in the hash table
    size_t max_size_ {DEFAULT_SIZE};  // the max size of the table

    using CellAlloc =
        typename std::allocator_traits<Alloc>::template rebind_alloc<Cell>;
    CellAlloc allocator_;

    KeyEqual key_equal_;

    Cell* data_;

    /**
     * @brief Compute the next capacity size (next prime of double).
     * @param old_size Current size.
     * @return Next size to use.
     */
    size_t get_next_size(size_t old_size) {
        return next_prime(old_size * 2);
    }

    /**
     * @brief Allocate memory for given number of cells.
     * @param size Number of cells.
     * @return Pointer to allocated memory.
     */
    Cell* allocate_memory(size_t size) {
        return allocator_.allocate(size);
    }

    /**
     * @brief Deallocate previously allocated cell memory.
     * @param ptr Pointer returned by allocate_memory.
     * @param size Number of cells.
     */
    void deallocate_memory(Cell* ptr, size_t size) {
        allocator_.deallocate(ptr, size);
    }

    /**
     * @brief Initialize internal storage to given capacity.
     * @param size Number of cells to initialize.
     */
    void init(size_t size) {
        data_ = allocate_memory(size);
        size_ = 0;
        max_size_ = size;
        for (size_t i = 0; i < max_size_; ++i) {
            construct_cell(i);
        }
    }

    /**
     * @brief Destroy all cells and free storage.
     */
    void clear_all() {
        for (size_t i = 0; i < max_size_; ++i) {
            destruct_cell(i);
        }
        deallocate_memory(data_, max_size_);
        data_ = nullptr;
        size_ = 0;
        max_size_ = 0;
    }

    /**
     * @brief Grow the table to the next capacity.
     */
    void expand();

    /**
     * @brief Rehash all elements into new capacity.
     * @param new_capacity New table capacity.
     */
    void rehash(size_t new_capacity);

    /**
     * @brief Construct a cell in place.
     * @tparam Args Constructor argument types.
     * @param index Cell index.
     * @param args Forwarded constructor arguments.
     */
    template <typename... Args>
    void construct_cell(size_t index, Args&&... args) {
        std::construct_at(data_ + index, std::forward<Args>(args)...);
    }

    /**
     * @brief Destroy a cell.
     * @param index Cell index.
     */
    void destruct_cell(size_t index) {
        std::destroy_at(data_ + index);
    }

    /**
     * @brief Find the first free or matching position starting from hash.
     * @param elem Element to locate.
     * @return Index where element is or can be placed.
     */
    size_t find_pos(const T& elem) const;

    /**
     * @brief Attempt to move elements to make room at pos.
     * @param pos Desired index to free.
     * @return Index moved-from on success, max_size_ on failure.
     */
    size_t move_elem(size_t pos);

protected:
    /**
     * @brief Compute hash value modulo table size.
     * @param elem Element to hash.
     * @return Hash index in [0, max_size_).
     */
    virtual size_t hash_value(const T& elem) const {
        hash_func hsh_fun;
        return hsh_fun(elem) % max_size_;
    }

public:
    /**
     * @brief Default-construct hash table.
     */
    hashing();

    /**
     * @brief Construct with initial capacity (rounded to prime).
     * @param init_max_size Initial max size.
     */
    explicit hashing(size_t init_max_size);

    /**
     * @brief Copy-construct hashing from another instance.
     * @param rhs Source instance.
     */
    hashing(const self_t& rhs);

    /**
     * @brief Move-construct hashing from another instance.
     * @param rhs Source instance.
     */
    hashing(self_t&& rhs) noexcept
        : data_(rhs.data_), size_(rhs.size_), max_size_(rhs.max_size_) {
        rhs.data_ = nullptr;
        rhs.size_ = 0;
        rhs.max_size_ = 0;
        rhs.init(hashing::DEFAULT_SIZE);
    }

    /**
     * @brief Destroy the hash table and free resources.
     */
    ~hashing() {
        clear_all();
    }

    /**
     * @brief Copy-assign from another hashing.
     * @param rhs Source instance.
     * @return Reference to this instance.
     */
    self_t& operator=(const self_t& rhs);

    /**
     * @brief Move-assign from another hashing.
     * @param rhs Source rvalue.
     * @return Reference to this instance.
     */
    self_t& operator=(self_t&& rhs) noexcept {
        data_ = rhs.data_;
        size_ = rhs.size_;
        max_size_ = rhs.max_size_;
        rhs.data_ = nullptr;
        rhs.size_ = rhs.max_size_ = 0;
        return *this;
    }

    /**
     * @brief Get number of stored elements.
     * @return Element count.
     */
    [[nodiscard]] size_t size() const {
        return size_;
    }

    /**
     * @brief Get current capacity of the table.
     * @return Capacity.
     */
    [[nodiscard]] size_t max_size() const {
        return max_size_;
    }

    /**
     * @brief Insert an element into the table.
     * @param elem Element to insert.
     * @return True if inserted, false if already present or cannot insert.
     */
    bool insert(const T& elem);

    /**
     * @brief Remove an element from the table.
     * @param elem Element to remove.
     * @return True if removed, false if not found.
     */
    bool remove(const T& elem);

    /**
     * @brief Test membership of an element.
     * @param elem Element to test.
     * @return True if present.
     */
    bool contains(const T& elem) const;

    /**
     * @brief Find an element in the table.
     * @param elem Element to find.
     * @return Iterator to element or end() if not found.
     */
    iterator find(const T& elem);
    const_iterator find(const T& elem) const;

    /**
     * @brief Clear all elements and reset to default capacity.
     */
    void clear() {
        clear_all();
        init(DEFAULT_SIZE);
    }

    /**
     * @brief Get iterator to first occupied cell (const).
     * @return Const iterator to first occupied element.
     */
    const_iterator begin() const {
        const_iterator it(data_, data_ + max_size_, data_);
        if (it != end() && !data_->is_occupied()) {
            ++it;
        }
        return it;
    }

    /**
     * @brief Get iterator to past-the-end (const).
     * @return Const iterator to past-the-end.
     */
    const_iterator end() const {
        return const_iterator(data_, data_ + max_size_, data_ + max_size_);
    }

    /**
     * @brief Get iterator to first occupied cell (const).
     * @return Const iterator to first occupied element.
     */
    const_iterator cbegin() const {
        return begin();
    }

    /**
     * @brief Get iterator to past-the-end (const).
     * @return Const iterator to past-the-end.
     */
    const_iterator cend() const {
        return end();
    }

    /**
     * @brief Get iterator to first occupied cell.
     * @return Iterator to first occupied element.
     */
    iterator begin() {
        iterator it(data_, data_ + max_size_, data_);
        if (it != end() && !data_->is_occupied()) {
            ++it;
        }
        return it;
    }

    /**
     * @brief Get iterator to past-the-end.
     * @return Iterator to past-the-end.
     */
    iterator end() {
        return iterator(data_, data_ + max_size_, data_ + max_size_);
    }
};

template <typename T, typename hash_func, typename KeyEqual, typename Alloc>
hashing<T, hash_func, KeyEqual, Alloc>::hashing() {
    init(DEFAULT_SIZE);
}

template <typename T, typename hash_func, typename KeyEqual, typename Alloc>
hashing<T, hash_func, KeyEqual, Alloc>::hashing(size_t init_max_size) {
    if (!is_prime(init_max_size)) {
        init_max_size = next_prime(init_max_size);
    }
    init(init_max_size > DEFAULT_SIZE ? init_max_size : DEFAULT_SIZE);
}

template <typename T, typename hash_func, typename KeyEqual, typename Alloc>
size_t hashing<T, hash_func, KeyEqual, Alloc>::find_pos(const T& elem) const {
    size_t pos = hash_value(elem);
    while (data_[pos].is_occupied() &&
           !key_equal_(data_[pos].element(), elem)) {
        ++pos;
        if (pos == max_size_) {
            pos = 0;
        }
    }
    return pos;
}

template <typename T, typename hash_func, typename KeyEqual, typename Alloc>
hashing<T, hash_func, KeyEqual, Alloc>::hashing(const self_t& rhs) {
    init(rhs.max_size_);
    for (size_t i = 0; i < rhs.max_size_; ++i) {
        if (rhs.data_[i].is_occupied()) {
            insert(rhs.data_[i].element());
        }
    }
}

template <typename T, typename hash_func, typename KeyEqual, typename Alloc>
hashing<T, hash_func, KeyEqual, Alloc>&
hashing<T, hash_func, KeyEqual, Alloc>::operator=(const self_t& rhs) {
    if (this == &rhs) {
        return *this;
    }
    clear_all();
    init(rhs.max_size_);
    for (size_t i = 0; i < rhs.max_size_; ++i) {
        if (rhs.data_[i].is_occupied()) {
            insert(rhs.data_[i].element());
        }
    }
    return *this;
}

template <typename T, typename hash_func, typename KeyEqual, typename Alloc>
size_t hashing<T, hash_func, KeyEqual, Alloc>::move_elem(size_t pos) {
    size_t start = 0;
    if (pos < MAX_DIST - 1) {
        start = max_size_ + pos - MAX_DIST + 1;
    } else {
        start = pos + 1 - MAX_DIST;
    }
    for (int i = 0; i < MAX_DIST - 1; ++i) {
        size_t home = (start + i) % max_size_;
        auto& cell = data_[home];
        for (int j = 0; j < MAX_DIST - i - 1; ++j) {
            if (cell.get_hop(j)) {
                size_t new_pos = (start + i + j) % max_size_;
                data_[pos].set_element(std::move(data_[new_pos].element()));
                data_[new_pos].set_unoccupied();
                cell.clear_hop(j);
                cell.set_hop(MAX_DIST - i - 1);
                return new_pos;
            }
        }
    }
    return max_size_;
}

template <typename T, typename hash_func, typename KeyEqual, typename Alloc>
bool hashing<T, hash_func, KeyEqual, Alloc>::insert(const T& elem) {
    if (contains(elem)) {
        return false;
    }
    if (size_ >= static_cast<size_t>(max_size_ * MAX_LOAD_FACTOR)) {
        expand();
    }

    size_t hash_val = hash_value(elem);
    size_t pos = find_pos(elem);

    while ((pos >= hash_val ? pos - hash_val : max_size_ - hash_val + pos) >=
           MAX_DIST) {
        pos = move_elem(pos);
        if (pos == max_size_) {
            return false;
        }
    }

    data_[pos].set_element(elem);
    size_t offset =
        pos >= hash_val ? pos - hash_val : max_size_ - hash_val + pos;
    data_[hash_val].set_hop(offset);
    ++size_;
    return true;
}

template <typename T, typename hash_func, typename KeyEqual, typename Alloc>
bool hashing<T, hash_func, KeyEqual, Alloc>::remove(const T& elem) {
    size_t hash_val = hash_value(elem);
    auto& cell = data_[hash_val];
    for (int i = 0; i < MAX_DIST; ++i) {
        size_t idx = (hash_val + i) % max_size_;
        if (cell.get_hop(i) && key_equal_(data_[idx].element(), elem)) {
            cell.clear_hop(i);
            data_[idx].set_unoccupied();
            --size_;
            return true;
        }
    }
    return false;
}

template <typename T, typename hash_func, typename KeyEqual, typename Alloc>
bool hashing<T, hash_func, KeyEqual, Alloc>::contains(const T& elem) const {
    size_t hash_val = hash_value(elem);
    const auto& cell = data_[hash_val];
    for (int i = 0; i < MAX_DIST; ++i) {
        size_t idx = (hash_val + i) % max_size_;
        if (cell.get_hop(i) && key_equal_(data_[idx].element(), elem)) {
            return true;
        }
    }
    return false;
}

template <typename T, typename hash_func, typename KeyEqual, typename Alloc>
typename hashing<T, hash_func, KeyEqual, Alloc>::iterator
hashing<T, hash_func, KeyEqual, Alloc>::find(const T& elem) {
    size_t hash_val = hash_value(elem);
    auto& cell = data_[hash_val];
    for (int i = 0; i < MAX_DIST; ++i) {
        size_t idx = (hash_val + i) % max_size_;
        if (cell.get_hop(i) && key_equal_(data_[idx].element(), elem)) {
            return iterator(data_, data_ + max_size_, data_ + idx);
        }
    }
    return end();
}

template <typename T, typename hash_func, typename KeyEqual, typename Alloc>
typename hashing<T, hash_func, KeyEqual, Alloc>::const_iterator
hashing<T, hash_func, KeyEqual, Alloc>::find(const T& elem) const {
    size_t hash_val = hash_value(elem);
    const auto& cell = data_[hash_val];
    for (int i = 0; i < MAX_DIST; ++i) {
        size_t idx = (hash_val + i) % max_size_;
        if (cell.get_hop(i) && key_equal_(data_[idx].element(), elem)) {
            return const_iterator(data_, data_ + max_size_, data_ + idx);
        }
    }
    return end();
}

template <typename T, typename hash_func, typename KeyEqual, typename Alloc>
void hashing<T, hash_func, KeyEqual, Alloc>::expand() {
    size_t new_size = get_next_size(max_size_);
    rehash(new_size);
}

template <typename T, typename hash_func, typename KeyEqual, typename Alloc>
void hashing<T, hash_func, KeyEqual, Alloc>::rehash(size_t new_capacity) {
    auto old_data = data_;
    size_t old_max_size = max_size_;
    data_ = allocate_memory(new_capacity);
    max_size_ = new_capacity;
    size_ = 0;

    for (size_t i = 0; i < new_capacity; ++i) {
        construct_cell(i);
    }
    for (size_t i = 0; i < old_max_size; ++i) {
        if (old_data[i].is_occupied()) {
            insert(old_data[i].element());
        }
        std::destroy_at(old_data + i);
    }
    deallocate_memory(old_data, old_max_size);
}

/**
 * @brief Storage cell used by the hashing table
 */
template <typename T, typename hash_func, typename KeyEqual, typename Alloc>
class hashing<T, hash_func, KeyEqual, Alloc>::Cell {
public:
    using self_t = Cell;

private:
    bool occupied_ {false};
    std::bitset<MAX_DIST> hop_info_;
    T elem_;

public:
    /**
     * @brief Default-construct a Cell
     */
    Cell() : elem_() {}
    Cell(const self_t& rhs) = default;
    Cell(self_t&& rhs) noexcept = default;
    ~Cell() = default;

    self_t& operator=(const self_t& rhs) = default;
    self_t& operator=(self_t&& rhs) noexcept = default;

    /**
     * @brief Test whether the cell currently holds an element
     * @return True if occupied, false otherwise
     */
    [[nodiscard]] bool is_occupied() const {
        return occupied_;
    }

    /**
     * @brief Mark the cell as occupied
     */
    void set_occupied() {
        occupied_ = true;
    }

    /**
     * @brief Mark the cell as unoccupied
     */
    void set_unoccupied() {
        occupied_ = false;
    }

    /**
     * @brief Access the stored element (const)
     * @return Reference to the stored element
     */
    const T& element() const {
        return elem_;
    }

    /**
     * @brief Access the stored element (mutable)
     * @return Mutable reference to the stored element
     */
    T& element() {
        return const_cast<T&>(static_cast<const self_t*>(this)->element());
    }

    /**
     * @brief Set the stored element and mark cell occupied
     * @tparam V Type of value to store
     * @param elem Value to store (forwarded)
     */
    template <typename V>
    void set_element(V&& elem) {
        elem_ = std::forward<V>(elem);
        set_occupied();
    }

    /**
     * @brief Query hop information at given distance
     * @param dist Distance from home
     * @return True if hop bit is set
     */
    [[nodiscard]] bool get_hop(size_t dist) const {
        return hop_info_[dist];
    }

    /**
     * @brief Set hop bit at given distance
     * @param dist Distance from home
     */
    void set_hop(size_t dist) {
        hop_info_.set(dist);
    }

    /**
     * @brief Clear hop bit at given distance
     * @param dist Distance from home
     */
    void clear_hop(size_t dist) {
        hop_info_.reset(dist);
    }

    /**
     * @brief Get const reference to hop info bitset
     * @return Const reference to internal hop bitset
     */
    [[nodiscard]] const std::bitset<MAX_DIST>& hop_info() const {
        return hop_info_;
    }

    /**
     * @brief Get mutable reference to hop info bitset
     * @return Mutable reference to internal hop bitset
     */
    std::bitset<MAX_DIST>& hop_info() {
        return const_cast<std::bitset<MAX_DIST>&>(
            static_cast<const self_t*>(this)->hop_info());
    }
};

/**
 * @brief Iterator for traversing occupied cells
 * @tparam Ref reference type returned by operator*
 * @tparam Ptr pointer type returned by operator->
 */
template <typename T, typename hash_func, typename KeyEqual, typename Alloc>
template <typename Ref, typename Ptr>
class hashing<T, hash_func, KeyEqual, Alloc>::hashing_iterator {
private:
    Cell* begin_;
    Cell* end_;
    Cell* cur_;
    using self_t = hashing_iterator<Ref, Ptr>;

public:
    /**
     * @brief Construct a null iterator.
     */
    hashing_iterator() : begin_(nullptr), end_(nullptr), cur_(nullptr) {}

    /**
     * @brief Construct from a range and current cell.
     *
     * @param begin Pointer to the first cell in the table.
     * @param end Pointer one past the last cell in the table.
     * @param cur Pointer to the current cell.
     */
    explicit hashing_iterator(Cell* begin, Cell* end, Cell* cur)
        : begin_(begin), end_(end), cur_(cur) {}

    /**
     * @brief Copy-construct from another iterator.
     *
     * @param rhs Source iterator.
     */
    hashing_iterator(const self_t& rhs) = default;

    /**
     * @brief Construct from a compatible iterator.
     *
     * @tparam Iter Compatible iterator type.
     * @param rhs Source iterator.
     */
    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    hashing_iterator(const Iter& rhs)
        : begin_(rhs.begin_), end_(rhs.end_), cur_(rhs.cell_) {}

    /**
     * @brief Move-construct from another iterator.
     *
     * @param rhs Source iterator.
     */
    hashing_iterator(self_t&& rhs) noexcept = default;

    /**
     * @brief Move-construct from a compatible iterator.
     *
     * @tparam Iter Compatible iterator type.
     * @param rhs Source iterator.
     */
    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    hashing_iterator(Iter&& rhs) noexcept
        : begin_(rhs.begin_), end_(rhs.end_), cur_(rhs.cell_) {}

    /**
     * @brief Destroy the iterator.
     */
    ~hashing_iterator() = default;

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
        cur_ = rhs.cell_;
        return *this;
    }

    /**
     * @brief Dereference the iterator.
     *
     * @return Reference to the element.
     */
    Ref operator*() const {
        return cur_->element();
    }

    /**
     * @brief Access the element through a pointer-like interface.
     *
     * @return Pointer to the element.
     */
    Ptr operator->() const {
        return &cur_->element();
    }

    /**
     * @brief Compare two iterators for equality.
     *
     * @tparam Iter Compatible iterator type.
     * @param lhs Left iterator.
     * @param rhs Right iterator.
     * @return True if the iterators are equal.
     */
    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator==(const self_t& lhs, const Iter& rhs) {
        return lhs.cur_ == rhs.cur_;
    }

    /**
     * @brief Compare two iterators for inequality.
     *
     * @tparam Iter Compatible iterator type.
     * @param lhs Left iterator.
     * @param rhs Right iterator.
     * @return True if the iterators are not equal.
     */
    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator!=(const self_t& lhs, const Iter& rhs) {
        return lhs.cur_ != rhs.cur_;
    }

    /**
     * @brief Compare two iterators for ordering.
     *
     * @tparam Iter Compatible iterator type.
     * @param lhs Left iterator.
     * @param rhs Right iterator.
     * @return True if lhs is ordered after rhs.
     */
    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator>(const self_t& lhs, const Iter& rhs) {
        return lhs.cur_ > rhs.cur_;
    }

    /**
     * @brief Compare two iterators for ordering.
     *
     * @tparam Iter Compatible iterator type.
     * @param lhs Left iterator.
     * @param rhs Right iterator.
     * @return True if lhs is ordered before rhs.
     */
    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator<(const self_t& lhs, const Iter& rhs) {
        return lhs.cur_ < rhs.cur_;
    }

    /**
     * @brief Compare two iterators for ordering.
     *
     * @tparam Iter Compatible iterator type.
     * @param lhs Left iterator.
     * @param rhs Right iterator.
     * @return True if lhs is not ordered before rhs.
     */
    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator>=(const self_t& lhs, const Iter& rhs) {
        return lhs.cur_ >= rhs.cur_;
    }

    /**
     * @brief Compare two iterators for ordering.
     *
     * @tparam Iter Compatible iterator type.
     * @param lhs Left iterator.
     * @param rhs Right iterator.
     * @return True if lhs is not ordered after rhs.
     */
    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator<=(const self_t& lhs, const Iter& rhs) {
        return lhs.cur_ <= rhs.cur_;
    }

    /**
     * @brief Advance to the next occupied cell.
     *
     * @return Reference to this iterator.
     */
    self_t& operator++() {
        ++cur_;
        while (cur_ < end_ && !cur_->is_occupied()) {
            ++cur_;
        };
        return *this;
    }

    /**
     * @brief Post-increment to the next occupied cell.
     *
     * @param unused Placeholder parameter for postfix form.
     * @return Iterator value before increment.
     */
    self_t operator++(int) {
        self_t tmp(*this);
        ++(*this);
        return tmp;
    }

    /**
     * @brief Move to the previous occupied cell.
     *
     * @return Reference to this iterator.
     */
    self_t& operator--() {
        --cur_;
        while (cur_ >= begin_ && !cur_->is_occupied()) {
            --cur_;
        };
        return *this;
    }

    /**
     * @brief Post-decrement to the previous occupied cell.
     *
     * @param unused Placeholder parameter for postfix form.
     * @return Iterator value before decrement.
     */
    self_t operator--(int) {
        self_t tmp(*this);
        --(*this);
        return tmp;
    }
};
}  // namespace mtl
