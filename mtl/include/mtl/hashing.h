#ifndef MTL_HASH_H
#define MTL_HASH_H

#include <mtl/algorithms.h>
#include <mtl/mtldefs.h>
#include <bitset>
#include <memory>
#include <utility>

namespace mtl {

/**
 * @brief an implementation of Hopscotch Hashing
 *
 * @tparam T the value type
 * @tparam hash_func the functor to hash the value
 * @tparam Alloc the allocator type
 */
template <typename T, typename hash_func = std::hash<T>,
          typename Alloc = std::allocator<T>>
class hashing {
public:
    using self_t = hashing<T, hash_func, Alloc>;

private:
    class Cell;

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

    Cell* data_;

    size_t get_next_size_(size_t old_size) {
        return next_prime(old_size * 2);
    }

    Cell* allocate_memory(size_t size) {
        return allocator_.allocate(size);
    }

    void deallocate_memory(Cell* ptr, size_t size) {
        allocator_.deallocate(ptr, size);
    }

    void init(size_t size) {
        data_ = allocate_memory(size);
        size_ = 0;
        max_size_ = size;
        for (size_t i = 0; i < max_size_; ++i) {
            construct_cell(i);
        }
    }

    void clear_all() {
        for (size_t i = 0; i < max_size_; ++i) {
            destruct_cell(i);
        }
        deallocate_memory(data_, max_size_);
        data_ = nullptr;
        size_ = 0;
        max_size_ = 0;
    }

    void expand();

    void rehash(size_t new_capacity);

    template <typename... Args>
    void construct_cell(size_t index, Args&&... args) {
        std::construct_at(data_ + index, std::forward<Args>(args)...);
    }

    void destruct_cell(size_t index) {
        std::destroy_at(data_ + index);
    }

    size_t find_pos(const T& elem) const;

    size_t move_elem(size_t pos);

protected:
    virtual size_t hash_value(const T& elem) const {
        hash_func hsh_fun;
        return hsh_fun(elem) % max_size_;
    }

public:
    hashing();
    /**
     * @brief ensure the hash table's max_size is bigger than init_max_size and
     * no smaller than DEFAULT_SIZE
     * @param init_max_size the initial max size
     */
    explicit hashing(size_t init_max_size);

    hashing(const self_t& rhs);

    hashing(self_t&& rhs) noexcept
        : data_(rhs.data_), size_(rhs.size_), max_size_(rhs.max_size_) {
        rhs.data_ = nullptr;
        rhs.clear();
    }

    ~hashing() {
        clear_all();
    }

    self_t& operator=(const self_t& rhs);

    self_t& operator=(self_t&& rhs) noexcept {
        data_ = rhs.data_;
        size_ = rhs.size_;
        max_size_ = rhs.max_size_;
        rhs.data_ = nullptr;
        rhs.size_ = rhs.max_size_ = 0;
    }

    /**
     * @brief the number of elements in the hash table
     * @return the value
     */
    size_t size() const {
        return size_;
    }

    /**
     * @brief the max size of the table
     * @return the value
     */
    size_t max_size() const {
        return max_size_;
    }

    /**
     * @brief to insert a element into the hash table, return true if success,
     * too many elements with the same hash value would cause insertion to fail.
     * @param elem the element to be inserted
     * @return if the element is inserted successfully
     */
    bool insert(const T& elem);

    /**
     * @brief remove an element from the hash table, return true if success, if
     * the element doesn't exist, return false
     * @param elem the element to be removed
     * @return if the element is removed successfully
     */
    bool remove(const T& elem);

    /**
     * @brief if elem is contained in the hash table
     * @param elem the element
     * @return if contained
     */
    bool contains(const T& elem) const;

    /**
     * @brief clear all elements
     */
    void clear() {
        clear_all();
        init(DEFAULT_SIZE);
    }

    const_iterator begin() const {
        return const_iterator(data_, data_ + max_size_, data_);
    }
    const_iterator end() const {
        return const_iterator(data_, data_ + max_size_, data_ + max_size_);
    }
    const_iterator cbegin() const {
        return begin();
    }
    const_iterator cend() const {
        return end();
    }
    iterator begin() {
        return iterator(data_, data_ + max_size_, data_);
    }
    iterator end() {
        return iterator(data_, data_ + max_size_, data_ + max_size_);
    }
};

template <typename T, typename hash_func, typename Alloc>
hashing<T, hash_func, Alloc>::hashing() {
    init(DEFAULT_SIZE);
}

template <typename T, typename hash_func, typename Alloc>
hashing<T, hash_func, Alloc>::hashing(size_t init_max_size) {
    if (!is_prime(init_max_size)) {
        init_max_size = next_prime(init_max_size);
    }
    init(init_max_size > DEFAULT_SIZE ? init_max_size : DEFAULT_SIZE);
}

template <typename T, typename hash_func, typename Alloc>
size_t hashing<T, hash_func, Alloc>::find_pos(const T& elem) const {
    size_t pos = hash_value(elem);
    while (data_[pos].is_occupied() && data_[pos].element() != elem) {
        ++pos;
        if (pos == max_size_) {
            pos = 0;
        }
    }
    return pos;
}

template <typename T, typename hash_func, typename Alloc>
hashing<T, hash_func, Alloc>::hashing(const self_t& rhs) {
    init(rhs.max_size_);
    for (size_t i = 0; i < rhs.max_size_; ++i) {
        if (rhs.data_[i].is_occupied()) {
            insert(rhs.data_[i].element());
        }
    }
}

template <typename T, typename hash_func, typename Alloc>
hashing<T, hash_func, Alloc>&
hashing<T, hash_func, Alloc>::operator=(const self_t& rhs) {
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

template <typename T, typename hash_func, typename Alloc>
size_t hashing<T, hash_func, Alloc>::move_elem(size_t pos) {
    size_t start = 0;
    if (pos < MAX_DIST - 1) {
        start = max_size_ + pos - MAX_DIST + 1;
    } else {
        start = pos + 1 - MAX_DIST;
    }
    for (int i = 0; i < MAX_DIST - 1; ++i) {
        auto cell = data_[start + i];
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

template <typename T, typename hash_func, typename Alloc>
bool hashing<T, hash_func, Alloc>::insert(const T& elem) {
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

    data_[pos].element() = elem;
    data_[pos].set_hop(pos - hash_val);
    ++size_;
    return true;
}

template <typename T, typename hash_func, typename Alloc>
bool hashing<T, hash_func, Alloc>::remove(const T& elem) {
    size_t hash_val = hash_value(elem);
    auto cell = data_[hash_val];
    for (int i = 0; i < MAX_DIST; ++i) {
        if (cell.get_hop(i) && cell.element() == elem) {
            cell.clear_hop(i);
            data_[hash_val + i].set_unoccupied();
            --size_;
            return true;
        }
    }
    return false;
}

template <typename T, typename hash_func, typename Alloc>
bool hashing<T, hash_func, Alloc>::contains(const T& elem) const {
    size_t hash_val = hash_value(elem);
    auto cell = data_[hash_val];
    for (int i = 0; i < MAX_DIST; ++i) {
        if (cell.get_hop(i) && cell.element() == elem) {
            return true;
        }
    }
    return false;
}

template <typename T, typename hash_func, typename Alloc>
void hashing<T, hash_func, Alloc>::expand() {
    size_t new_size = get_next_size_(max_size_);
    rehash(new_size);
}

template <typename T, typename hash_func, typename Alloc>
void hashing<T, hash_func, Alloc>::rehash(size_t new_capacity) {
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

template <typename T, typename hash_func, typename Alloc>
class hashing<T, hash_func, Alloc>::Cell {
public:
    using self_t = Cell;

private:
    bool occupied_ {false};
    std::bitset<MAX_DIST> hop_info_;
    T elem_;

public:
    Cell() : elem_() {}
    Cell(const self_t& rhs) = default;
    Cell(self_t&& rhs) noexcept = default;
    ~Cell() = default;

    self_t& operator=(const self_t& rhs) = default;
    self_t& operator=(self_t&& rhs) noexcept = default;

    [[nodiscard]] bool is_occupied() const {
        return occupied_;
    }

    void set_occupied() {
        occupied_ = true;
    }

    void set_unoccupied() {
        occupied_ = false;
    }

    const T& element() const {
        return elem_;
    }

    T& element() {
        return const_cast<T&>(static_cast<const self_t*>(this)->element());
    }

    template <typename V>
    void set_element(V&& elem) {
        elem_ = std::forward<V>(elem);
        set_occupied();
    }

    bool get_hop(size_t dist) const {
        return hop_info_[dist];
    }

    void set_hop(size_t dist) {
        hop_info_.set(dist);
        occupied_ = true;
    }

    void clear_hop(size_t dist) {
        hop_info_.reset(dist);
        occupied_ = false;
    }

    const std::bitset<MAX_DIST>& hop_info() const {
        return hop_info_;
    }

    std::bitset<MAX_DIST>& hop_info() {
        return const_cast<std::bitset<MAX_DIST>&>(
            static_cast<const self_t*>(this)->hop_info());
    }
};

template <typename T, typename hash_func, typename Alloc>
template <typename Ref, typename Ptr>
class hashing<T, hash_func, Alloc>::hashing_iterator {
private:
    Cell* begin_;
    Cell* end_;
    Cell* cur_;
    using self_t = hashing_iterator<Ref, Ptr>;

public:
    hashing_iterator() : begin_(nullptr), end_(nullptr), cur_(nullptr) {}
    explicit hashing_iterator(Cell* begin, Cell* end, Cell* cur)
        : begin_(begin), end_(end), cur_(cur) {}
    hashing_iterator(const self_t& rhs) = default;

    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    hashing_iterator(const Iter& rhs)
        : begin_(rhs.begin_), end_(rhs.end_), cur_(rhs.cell_) {}

    hashing_iterator(self_t&& rhs) noexcept = default;

    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    hashing_iterator(Iter&& rhs) noexcept
        : begin_(rhs.begin_), end_(rhs.end_), cur_(rhs.cell_) {}

    ~hashing_iterator() = default;

    self_t& operator=(const self_t& rhs) = default;

    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    self_t& operator=(const Iter& rhs) {
        cur_ = rhs.cell_;
    }

    Ref operator*() const {
        return cur_->element();
    }

    Ptr operator->() const {
        return &cur_->element();
    }

    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator==(const self_t& lhs, const Iter& rhs) {
        return lhs.cur_ == rhs.cur_;
    }

    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator!=(const self_t& lhs, const Iter& rhs) {
        return lhs.cur_ != rhs.cur_;
    }

    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator>(const self_t& lhs, const Iter& rhs) {
        return lhs.cur_ > rhs.cur_;
    }

    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator<(const self_t& lhs, const Iter& rhs) {
        return lhs.cur_ < rhs.cur_;
    }

    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator>=(const self_t& lhs, const Iter& rhs) {
        return lhs.cur_ >= rhs.cur_;
    }

    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator<=(const self_t& lhs, const Iter& rhs) {
        return lhs.cur_ <= rhs.cur_;
    }

    self_t& operator++() {
        do {
            ++cur_;
        } while (cur_ < end_ && !cur_->is_occupied());
        return *this;
    }

    self_t operator++(int) {
        self_t tmp(*this);
        ++(*this);
        return tmp;
    }

    self_t& operator--() {
        do {
            --cur_;
        } while (cur_ >= begin_ && !cur_->is_occupied());
        return *this;
    }

    self_t operator--(int) {
        self_t tmp(*this);
        --(*this);
        return tmp;
    }
};
}  // namespace mtl

#endif
