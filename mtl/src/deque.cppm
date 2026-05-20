export module mtl.deque;

export import mtl.core;
import std;

export namespace mtl {

/**
 * @brief The deque (double-ended queue) ADT.
 *
 * The implementation stores pointers to fixed-length node buffers in a map
 * and grows the map as needed. Push/pop at either end allocate or free node
 * buffers and provide amortized O(1) end operations.
 *
 * @tparam T Type of elements stored in the deque.
 * @tparam Alloc Allocator type used for element allocation.
 */
template <typename T, typename Alloc = std::allocator<T>>
class deque {
public:
    using self_t = deque<T, Alloc>;

private:
    /* The default size of map array. In any situations, map_size_ won't be less
     * than it */
    const static size_t DEFAULT_MAP_SIZE = 8;
    // The number of elements a node will contain.
    const static size_t BUF_LEN = 16;
    using MapPtr = T**;
    using EltPtr = T*;

    template <typename Ref, typename Ptr>
    class deque_iterator;

public:
    using iterator = deque_iterator<T&, T*>;
    using const_iterator = deque_iterator<const T&, const T*>;

private:
    MapPtr map_;           // the map of nodes
    size_t map_size_ {0};  // the size of the map array
    size_t size_ {0};      // the number of elements
    iterator front_;       // the front element
    iterator back_;        // the back element

    using MapAlloc =
        typename std::allocator_traits<Alloc>::template rebind_alloc<EltPtr>;

    Alloc node_allocator_;
    MapAlloc map_allocator_;

    /**
     * @brief Allocate a new map array capable of holding map_size node pointers.
     *
     * @param map_size Number of entries to allocate in the map.
     * @return Pointer to the newly allocated map (uninitialized entries
     * are set to nullptr).
     */
    MapPtr allocate_map(size_t map_size);

    /**
     * @brief Allocate a new element buffer node.
     *
     * @return Pointer to the newly allocated buffer.
     */
    EltPtr allocate_node() {
        return node_allocator_.allocate(BUF_LEN);
    }

    /**
     * @brief Deallocate a map array.
     *
     * @param map Map pointer to deallocate.
     * @param map_size Number of entries in the map.
     */
    void deallocate_map(MapPtr map, size_t map_size) {
        map_allocator_.deallocate(map, map_size);
    }

    /**
     * @brief Deallocate an element buffer node.
     *
     * @param node Node buffer to deallocate.
     */
    void deallocate_node(EltPtr node) {
        node_allocator_.deallocate(node, BUF_LEN);
    }

    /**
     * @brief Destroy all constructed elements in the deque range.
     *
     * Iterates from front_ to back_ and calls destructor for each element.
     */
    void destroy_all() {
        for (auto itr = front_; itr != back_; ++itr) {
            std::destroy_at(itr.cur_);
        }
    }

    /**
     * @brief Initialize internal map and allocate initial nodes.
     *
     * @param map_size Initial map size (number of node pointers).
     */
    void init(size_t map_size);

    /**
     * @brief Expand the internal map to accommodate more nodes.
     *
     * Moves existing node pointers into a newly allocated larger map. If
     * backward is true the content is copied toward the front of the new map,
     * otherwise toward the back.
     *
     * @param backward Direction to expand the map into.
     */
    void expand(bool backward) noexcept;

    /**
     * @brief Throw EmptyContainer if the deque is empty.
     */
    void check_empty() const {
        if (empty()) {
            throw EmptyContainer();
        }
    }

public:
    /**
     * @brief Construct an empty deque with no allocated map.
     */
    deque() : map_(nullptr) {}

    /**
     * @brief Construct a deque with n default-inserted elements.
     *
     * @param n Number of elements to create.
     */
    explicit deque(size_t n);

    /**
     * @brief Construct a deque with n copies of val.
     *
     * @param n Number of elements to create.
     * @param val Value to copy into each element.
     */
    explicit deque(size_t n, const T& val);

    /**
     * @brief Construct a deque from an initializer list by moving elements.
     *
     * @param il Initializer list of values.
     */
    deque(std::initializer_list<T> il) noexcept;
    /**
     * @brief Copy-construct a deque from another deque.
     *
     * @param rhs Deque to copy from.
     */
    deque(const self_t& rhs);

    /**
     * @brief Move-construct a deque from another deque.
     *
     * @param rhs Deque to move from.
     */
    deque(self_t&& rhs) noexcept
        : map_(rhs.map_),
          map_size_(rhs.map_size_),
          size_(rhs.size_),
          front_(rhs.front_),
          back_(rhs.back_) {
        rhs.size_ = 0;
        rhs.map_size_ = 0;
        rhs.map_ = nullptr;
        rhs.front_ = rhs.back_ = iterator();
    }

    /**
     * @brief Destroy the deque and release resources.
     */
    ~deque() {
        clear();
    }

    /**
     * @brief Get the number of elements.
     *
     * @return Number of elements in the deque.
     */
    [[nodiscard]] size_t size() const {
        return size_;
    }

    /**
     * @brief Check whether the deque is empty.
     *
     * @return True if empty, false otherwise.
     */
    [[nodiscard]] bool empty() const {
        return size_ == 0;
    }

    /**
     * @brief Destroy all elements and free all allocated nodes and map.
     */
    void clear();

    /**
     * @brief Access element at the given index without bounds checking.
     *
     * @param index Zero-based element index.
     * @return Reference to the element.
     */
    const T& operator[](size_t index) const {
        return *(front_ + index);
    }

    /**
     * @brief Access element at the given index without bounds checking.
     *
     * @param index Zero-based element index.
     * @return Reference to the element.
     */
    T& operator[](size_t index) {
        return const_cast<T&>(
            static_cast<const self_t*>(this)->operator[](index));
    }

    /**
     * @brief Access element at the given index with bounds checking.
     *
     * @param index Zero-based element index.
     * @return Reference to the element.
     */
    const T& at(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("deque::at: index out of range");
        }
        return operator[](index);
    }

    /**
     * @brief Access element at the given index with bounds checking.
     *
     * @param index Zero-based element index.
     * @return Reference to the element.     */
    T& at(size_t index) {
        return const_cast<T&>(static_cast<const self_t*>(this)->at(index));
    }

    /**
     * @brief Copy-assign from another deque.
     *
     * @param rhs Deque to copy from.
     * @return Reference to this deque.
     */
    self_t& operator=(const self_t& rhs) {
        deque<T, Alloc> tmp(rhs);
        operator=(std::move(tmp));
        return *this;
    }

    /**
     * @brief Move-assign from another deque.
     *
     * @param rhs Deque to move from.
     * @return Reference to this deque.
     */
    self_t& operator=(self_t&& rhs) noexcept {
        map_ = rhs.map_;
        rhs.map_ = nullptr;
        map_size_ = rhs.map_size_;
        size_ = rhs.size_;
        front_ = rhs.front_;
        back_ = rhs.back_;
        rhs.clear();
        return *this;
    }

    /**
     * @brief Insert an element at the back of the deque.
     *
     * @tparam V Type of the element (deduced). Accepts lvalue or rvalue.
     * @param elem Element to insert (forwarded to the constructor).
     */
    template <typename V>
    void push_back(V&& elem) {
        if (map_ == nullptr) {
            init(DEFAULT_MAP_SIZE);
        }
        std::construct_at(back_.cur_, std::forward<V>(elem));
        ++back_;
        ++size_;
        if (back_.node_ + 1 >= map_ + map_size_) {
            expand(true);
        }
        if (*(back_.node_ + 1) == nullptr) {
            *(back_.node_ + 1) = allocate_node();
        }
    }

    /**
     * @brief Insert an element at the front of the deque.
     *
     * @tparam V Type of the element (deduced). Accepts lvalue or rvalue.
     * @param elem Element to insert (forwarded to the constructor).
     */
    template <typename V>
    void push_front(V&& elem) {
        if (map_ == nullptr) {
            init(DEFAULT_MAP_SIZE);
        }
        --front_;
        std::construct_at(front_.cur_, std::forward<V>(elem));
        ++size_;
        if (front_.node_ <= map_) {
            expand(false);
        }
        if (*(front_.node_ - 1) == nullptr) {
            *(front_.node_ - 1) = allocate_node();
        }
    }

    /**
     * @brief Remove the last element of the deque.
     *
     * Throws EmptyContainer if the deque is empty.
     */
    void pop_back() {
        check_empty();
        if (back_.cur_ == back_.first_) {
            deallocate_node(*(back_.node_ + 1));
            *(back_.node_ + 1) = nullptr;
        }
        std::destroy_at(back_.cur_);
        --back_;
        --size_;
    }

    /**
     * @brief Remove the first element of the deque.
     *
     * Throws EmptyContainer if the deque is empty.
     */
    void pop_front() {
        check_empty();
        if (front_.cur_ == front_.last_ - 1) {
            deallocate_node(*(front_.node_ - 1));
            *(front_.node_ - 1) = nullptr;
        }
        std::destroy_at(front_.cur_);
        ++front_;
        --size_;
    }

    /**
     * @brief Access the first element.
     *
     * @return Reference to the first element.     */
    const T& front() const {
        check_empty();
        return *front_;
    }

    /**
     * @brief Access the last element.
     *
     * @return Reference to the last element.     */
    const T& back() const {
        check_empty();
        return *(back_ - 1);
    }

    /**
     * @brief Access the first element.
     *
     * @return Reference to the first element.     */
    T& front() {
        return const_cast<T&>(static_cast<const self_t*>(this)->front());
    }

    /**
     * @brief Access the last element.
     *
     * @return Reference to the last element.     */
    T& back() {
        return const_cast<T&>(static_cast<const self_t*>(this)->back());
    }

    /**
     * @brief Get an iterator to the first element (const).
     *
     * @return Iterator to the first element.     */
    const_iterator begin() const {
        return front_;
    }

    /**
     * @brief Get an iterator past the last element (const).
     *
     * @return Iterator past the last element.     */
    const_iterator end() const {
        return back_;
    }

    /**
     * @brief Get a const iterator to the first element.
     *
     * @return Iterator to the first element.     */
    const_iterator cbegin() const {
        return front_;
    }

    /**
     * @brief Get a const iterator past the last element.
     *
     * @return Iterator past the last element.     */
    const_iterator cend() const {
        return back_;
    }

    /**
     * @brief Get an iterator to the first element.
     *
     * @return Iterator to the first element.     */
    iterator begin() {
        return front_;
    }

    /**
     * @brief Get an iterator past the last element.
     *
     * @return Iterator past the last element.     */
    iterator end() {
        return back_;
    }
};

template <typename T, typename Alloc>
deque<T, Alloc>::deque(size_t n) {
    init((n / BUF_LEN) + 3);
    for (auto ptr = map_; ptr != map_ + map_size_; ++ptr) {
        *ptr = allocate_node();
    }
    size_t pre = n / 2;
    size_t suf = n - pre;
    front_ -= pre;
    back_ += suf;
    for (auto itr = front_; itr != back_; ++itr) {
        std::construct_at(itr.cur_);
    }
    size_ = n;
}

template <typename T, typename Alloc>
deque<T, Alloc>::deque(size_t n, const T& val) {
    init((n / BUF_LEN) + 3);
    for (auto ptr = map_; ptr != map_ + map_size_; ++ptr) {
        *ptr = allocate_node();
    }
    size_t pre = n / 2;
    size_t suf = n - pre;
    front_ -= pre;
    back_ += suf;
    for (auto itr = front_; itr != back_; ++itr) {
        std::construct_at(itr.cur_, val);
    }

    size_ = n;
}

template <typename T, typename Alloc>
deque<T, Alloc>::deque(std::initializer_list<T> il) noexcept {
    init((il.size() / BUF_LEN) + 3);
    for (auto ptr = map_; ptr != map_ + map_size_; ++ptr) {
        *ptr = allocate_node();
    }
    size_t pre = il.size() / 2;
    size_t suf = il.size() - pre;
    front_ -= pre;
    suf += suf;
    auto itr1 = front_;
    for (auto itr2 = il.begin(); itr2 != il.end(); ++itr1, ++itr2) {
        std::construct_at(itr1.cur_, std::move(*itr2));
    }
    size_ = il.size();
}

template <typename T, typename Alloc>
deque<T, Alloc>::deque(const self_t& rhs) {
    init((rhs.size_ / BUF_LEN) + 3);
    for (auto ptr = map_; ptr != map_ + map_size_; ++ptr) {
        *ptr = allocate_node();
    }
    size_t pre = rhs.size() / 2;
    size_t suf = rhs.size() - pre;
    front_ -= pre;
    back_ += suf;
    for (auto itr1 = front_, itr2 = rhs.front_; itr1 != back_; ++itr1, ++itr2) {
        std::construct_at(itr1.cur_, *itr2);
    }
    size_ = rhs.size_;
}

template <typename T, typename Alloc>
typename deque<T, Alloc>::MapPtr
deque<T, Alloc>::allocate_map(size_t map_size) {
    auto map = map_allocator_.allocate(map_size);
    for (size_t i = 0; i < map_size; ++i) {
        map[i] = nullptr;
    }
    return map;
}

template <typename T, typename Alloc>
void deque<T, Alloc>::init(size_t map_size) {
    map_size_ = map_size;
    map_ = allocate_map(map_size_);
    MapPtr first_node_ = map_ + (map_size_ / 2);
    *first_node_ = allocate_node();
    *(first_node_ + 1) = allocate_node();
    *(first_node_ - 1) = allocate_node();
    back_ = iterator(*first_node_ + (BUF_LEN / 2), first_node_);
    front_ = back_;
}

template <typename T, typename Alloc>
void deque<T, Alloc>::expand(bool backward) noexcept {
    auto old_map = map_;
    auto old_map_size = map_size_;
    difference_t scope_size = back_.node_ - front_.node_;
    map_size_ = old_map_size + scope_size;
    map_ = allocate_map(map_size_);
    if (backward) {
        MapPtr start_node = map_ + (front_.node_ - old_map);

        MapPtr ptr1 = start_node - 1;
        MapPtr ptr2 = front_.node_ - 1;
        MapPtr end_node = back_.node_ + 1;
        while (ptr2 < end_node) {
            mtl::iter_swap(ptr1++, ptr2++);
        }
        end_node = ptr1 - 1;
        front_.node_ = start_node;
        back_.node_ = end_node;
    } else {
        MapPtr start_node =
            map_ + map_size_ - ((old_map + old_map_size) - back_.node_);

        MapPtr ptr1 = start_node + 1;
        MapPtr ptr2 = back_.node_ + 1;
        MapPtr end_node = front_.node_ - 1;
        while (ptr2 > end_node) {
            mtl::iter_swap(ptr1--, ptr2--);
        }

        end_node = ptr1 + 1;
        back_.node_ = start_node;
        front_.node_ = end_node;
    }
    deallocate_map(old_map, old_map_size);
}

template <typename T, typename Alloc>
void deque<T, Alloc>::clear() {
    MapPtr start = front_.node_ - 1;
    MapPtr stop = back_.node_ + 2;
    destroy_all();
    for (auto ptr = start; ptr < stop; ++ptr) {
        deallocate_node(*ptr);
        *ptr = nullptr;
    }
    deallocate_map(map_, map_size_);
    map_ = nullptr;
    size_ = 0;
    map_size_ = 0;
    front_ = back_ = iterator();
}

/**
 * @brief Random access iterator for deque.
 *
 * @tparam Ref Reference type returned by dereference.
 * @tparam Ptr Pointer type returned by arrow operator.
 */
template <typename T, typename Alloc>
template <typename Ref, typename Ptr>
class deque<T, Alloc>::deque_iterator {
public:
    using self_t = deque_iterator<Ref, Ptr>;

private:
    EltPtr first_;  // the first element of current node
    EltPtr last_;   // the element past the last element of current node
    MapPtr node_;   // the current node
    EltPtr cur_;    // the current element

    /**
     * @brief Set the iterator to point to a new node.
     *
     * @param new_node Node pointer to set.
     */
    void set_node(MapPtr new_node) {
        node_ = new_node;
        first_ = *node_;
        last_ = first_ + BUF_LEN;
    }

    friend const_iterator;

public:
    /**
     * @brief Construct a default iterator.
     */
    deque_iterator()
        : first_(nullptr), last_(nullptr), node_(nullptr), cur_(nullptr) {}

    /**
     * @brief Construct an iterator from a current element and node.
     *
     * @param cur Pointer to the current element.
     * @param node Pointer to the current node in the map.
     */
    deque_iterator(EltPtr cur, MapPtr node) : node_(node), cur_(cur) {
        set_node(node);
    }

    /**
     * @brief Copy-construct an iterator.
     *
     * @param rhs Iterator to copy from.
     */
    deque_iterator(const self_t& rhs) = default;

    /**
     * @brief Construct from compatible iterator type.
     *
     * @tparam Iter Compatible iterator type.
     * @param rhs Iterator to copy from.
     */
    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    deque_iterator(const Iter& rhs)
        : first_(rhs.first_),
          last_(rhs.last_),
          node_(rhs.node_),
          cur_(rhs.cur_) {}

    /**
     * @brief Destroy the iterator.
     */
    ~deque_iterator() noexcept = default;

    /**
     * @brief Move-construct an iterator.
     *
     * @param rhs Iterator to move from.
     */
    deque_iterator(self_t&& rhs) noexcept = default;

    /**
     * @brief Move-construct from compatible iterator type.
     *
     * @tparam Iter Compatible iterator type.
     * @param rhs Iterator to move from.
     */
    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    deque_iterator(Iter&& rhs) noexcept
        : first_(rhs.first_),
          last_(rhs.last_),
          node_(rhs.node_),
          cur_(rhs.cur_) {}

    /**
     * @brief Copy-assign from another iterator.
     *
     * @param rhs Iterator to copy from.
     * @return Reference to this iterator.     */
    self_t& operator=(const self_t& rhs) = default;

    /**
     * @brief Move-assign from another iterator.
     *
     * @param rhs Iterator to move from.
     * @return Reference to this iterator.     */
    self_t& operator=(self_t&& rhs) noexcept = default;

    /**
     * @brief Assign from compatible iterator type.
     *
     * @tparam Iter Compatible iterator type.
     * @param rhs Iterator to copy from.
     * @return Reference to this iterator.     */
    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    self_t& operator=(const Iter& rhs) {
        first_ = rhs.first_;
        last_ = rhs.last_;
        node_ = rhs.node_;
        cur_ = rhs.cur_;
        return *this;
    }

    /**
     * @brief Move-assign from compatible iterator type.
     *
     * @tparam Iter Compatible iterator type.
     * @param rhs Iterator to move from.
     * @return Reference to this iterator.     */
    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    self_t& operator=(Iter&& rhs) noexcept {
        first_ = rhs.first_;
        last_ = rhs.last_;
        node_ = rhs.node_;
        cur_ = rhs.cur_;
        return *this;
    }

    /**
     * @brief Dereference the iterator.
     *
     * @return Reference to the current element.     */
    Ref operator*() const {
        return *cur_;
    }

    /**
     * @brief Access the current element.
     *
     * @return Pointer to the current element.     */
    Ptr operator->() const {
        return cur_;
    }

    /**
     * @brief Pre-increment the iterator.
     *
     * @return Reference to this iterator.     */
    self_t& operator++() {
        ++cur_;
        if (cur_ == last_) {
            set_node(node_ + 1);
            cur_ = first_;
        }
        return *this;
    }

    /**
     * @brief Pre-decrement the iterator.
     *
     * @return Reference to this iterator.     */
    self_t& operator--() {
        if (cur_ == first_) {
            set_node(node_ - 1);
            cur_ = last_;
        }
        --cur_;
        return *this;
    }

    /**
     * @brief Post-increment the iterator.
     *
     * @return Copy of the iterator before increment.     */
    self_t operator++(int) {
        auto old = *this;
        this->operator++();
        return old;
    }

    /**
     * @brief Post-decrement the iterator.
     *
     * @return Copy of the iterator before decrement.     */
    self_t operator--(int) {
        auto old = *this;
        this->operator--();
        return old;
    }

    /**
     * @brief Advance the iterator by n positions.
     *
     * @param n Offset to add (can be negative).
     * @return Reference to this iterator.     */
    self_t& operator+=(difference_t n) {
        const difference_t offset = n + (cur_ - first_);
        if (offset >= 0 && offset < BUF_LEN) {
            cur_ += n;
        } else {
            difference_t node_offset = offset > 0
                ? offset / BUF_LEN
                : -static_cast<difference_t>((-offset - 1) / BUF_LEN) - 1;
            set_node(node_ + node_offset);
            cur_ = first_ +
                (offset - static_cast<difference_t>(BUF_LEN) * node_offset);
        }
        return *this;
    }

    /**
     * @brief Move the iterator backward by n positions.
     *
     * @param n Offset to subtract.
     * @return Reference to this iterator.     */
    self_t& operator-=(difference_t n) {
        return *this += -n;
    }

    /**
     * @brief Return a new iterator advanced by n positions.
     *
     * @param n Offset to add.
     * @return New iterator advanced by n.     */
    self_t operator+(difference_t n) const {
        auto new_itr = *this;
        new_itr += n;
        return new_itr;
    }

    /**
     * @brief Return a new iterator moved backward by n positions.
     *
     * @param n Offset to subtract.
     * @return New iterator moved backward by n.     */
    self_t operator-(difference_t n) const {
        auto new_itr = *this;
        new_itr -= n;
        return new_itr;
    }

    /**
     * @brief Compute distance between iterators.
     *
     * @tparam Iter Compatible iterator type.
     * @param lhs Left-hand iterator.
     * @param rhs Right-hand iterator.
     * @return Distance between iterators.     */
    template <is_one_of<iterator, const_iterator> Iter>
    friend difference_t operator-(const self_t& lhs, const Iter& rhs) {
        return (lhs.cur_ - lhs.first_) + (rhs.last_ - rhs.cur_) +
            ((lhs.node_ - rhs.node_ - static_cast<bool>(lhs.node_)) * BUF_LEN);
    }

    /**
     * @brief Equality comparison.
     *
     * @tparam Iter Compatible iterator type.
     * @param lhs Left-hand iterator.
     * @param rhs Right-hand iterator.
     * @return True if iterators refer to the same element.     */
    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator==(const self_t& lhs, const Iter& rhs) {
        return lhs.cur_ == rhs.cur_;
    }

    /**
     * @brief Inequality comparison.
     *
     * @tparam Iter Compatible iterator type.
     * @param lhs Left-hand iterator.
     * @param rhs Right-hand iterator.
     * @return True if iterators refer to different elements.     */
    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator!=(const self_t& lhs, const Iter& rhs) {
        return lhs.cur_ != rhs.cur_;
    }

    /**
     * @brief Less-than comparison.
     *
     * @tparam Iter Compatible iterator type.
     * @param lhs Left-hand iterator.
     * @param rhs Right-hand iterator.
     * @return True if lhs precedes rhs.     */
    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator<(const self_t& lhs, const Iter& rhs) {
        bool res = false;
        if (lhs.node_ < rhs.node_) {
            res = true;
        } else if (lhs.node_ == rhs.node_) {
            res = lhs.cur_ < rhs.cur_;
        }
        return res;
    }

    /**
     * @brief Less-than-or-equal comparison.
     *
     * @tparam Iter Compatible iterator type.
     * @param lhs Left-hand iterator.
     * @param rhs Right-hand iterator.
     * @return True if lhs does not follow rhs.     */
    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator<=(const self_t& lhs, const Iter& rhs) {
        return lhs < rhs || lhs == rhs;
    }

    /**
     * @brief Greater-than comparison.
     *
     * @tparam Iter Compatible iterator type.
     * @param lhs Left-hand iterator.
     * @param rhs Right-hand iterator.
     * @return True if lhs follows rhs.     */
    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator>(const self_t& lhs, const Iter& rhs) {
        return !(lhs <= rhs);
    }

    /**
     * @brief Greater-than-or-equal comparison.
     *
     * @tparam Iter Compatible iterator type.
     * @param lhs Left-hand iterator.
     * @param rhs Right-hand iterator.
     * @return True if lhs does not precede rhs.     */
    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator>=(const self_t& lhs, const Iter& rhs) {
        return !(lhs < rhs);
    }

    friend class deque<T, Alloc>;
};

}  // namespace mtl
