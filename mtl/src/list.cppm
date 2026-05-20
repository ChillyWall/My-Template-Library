export module mtl.list;

export import mtl.core;
import std;

export namespace mtl {

/**
 * @brief The list ADT: a doubly-linked list container.
 *
 * @tparam T     Element type stored in the list.
 * @tparam Alloc Allocator type used for element allocation.
 */
template <typename T, typename Alloc = std::allocator<T>>
class list {
public:
    using self_t = list<T, Alloc>;

private:
    /**
     * @brief Forward-declaration of internal Node type representing list nodes.
     *
     * The full definition appears later. This forward declaration is used for
     * pointers and iterator implementations.
     */
    class Node;
    using NdPtr = Node*;

    /**
     * @brief Forward-declaration of the nested iterator template.
     *
     * @tparam Ref Reference type returned by dereference (e.g. T& or const T&).
     * @tparam Ptr  Pointer type returned by operator-> (e.g. Node* or const
     * Node*).
     */
    template <typename Ref, typename Ptr>
    class list_iterator;

public:
    /**
     * @brief Const iterator type for the list.
     */
    using const_iterator = list_iterator<const T&, const Node*>;

    /**
     * @brief Mutable iterator type for the list.
     */
    using iterator = list_iterator<T&, Node*>;

private:
    using NodeAlloc =
        typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
    NodeAlloc allocator_;

    NdPtr head_ {nullptr};
    NdPtr tail_ {nullptr};
    size_t size_ {0};

    /**
     * @brief Initialize the sentinel head and tail nodes and reset size.
     */
    void init();
    void check_empty() const {
        if (empty()) {
            throw EmptyContainer("There's no element to be popped out.");
        }
    }

    /**
     * @brief Allocate and construct a Node with forwarded arguments.
     *
     * @tparam Args Parameter pack for node constructor.
     * @param args  Arguments forwarded to Node constructor.
     * @return Pointer to the newly constructed node.
     */
    template <typename... Args>
    NdPtr allocate_node(Args&&... args) {
        auto ptr = allocator_.allocate(1);
        std::construct_at(ptr, std::forward<Args>(args)...);
        return ptr;
    }

    /**
     * @brief Recursively destroy a node chain and deallocate nodes.
     *
     * @param ptr Pointer to the head of node chain to destroy.
     */
    void destroy_node(NdPtr ptr) {
        if (ptr && ptr->next_) {
            destroy_node(ptr->next_);
        }
        std::destroy_at(ptr);
        allocator_.deallocate(ptr, 1);
    }

public:
    /**
     * @brief Default constructor. Initializes an empty list.
     */
    list() {
        init();
    }

    /**
     * @brief Copy constructor. Performs element-wise copy of rhs.
     *
     * @param rhs Source list to copy from.
     */
    list(const self_t& rhs) {
        init();
        for (auto itr = rhs.begin(); itr != rhs.end(); ++itr) {
            push_back(*itr);
        }
    }

    /**
     * @brief Move constructor. Steals node storage from rhs.
     *
     * @param rhs Rvalue source list.
     */
    list(self_t&& rhs) noexcept
        : head_(rhs.head_), tail_(rhs.tail_), size_(rhs.size_) {
        rhs.init();
    }

    /**
     * @brief Construct from initializer list by pushing elements to back.
     *
     * @param il Initializer list of elements to insert.
     */
    list(std::initializer_list<T> il) noexcept {
        init();
        for (auto itr = il.begin(); itr != il.end(); ++itr) {
            push_back(std::move(*itr));
        }
    }

    /**
     * @brief Destructor. Destroys all nodes and deallocates memory.
     */
    ~list() noexcept {
        destroy_node(head_);
    }

    /**
     * @brief Copy assignment operator.
     *
     * @param rhs Source list to copy from.
     * @return Reference to this list after assignment.
     */
    self_t& operator=(const self_t& rhs);
    /**
     * @brief Move assignment operator.
     *
     * @param rhs Rvalue source list to move from.
     * @return Reference to this list after move.
     */
    self_t& operator=(self_t&& rhs) noexcept;

    /**
     * @brief Remove all elements from the list.
     */
    void clear() {
        destroy_node(head_);
        init();
    }

    /**
     * @brief Check whether the list is empty.
     *
     * @return true if list contains no elements, false otherwise.
     */
    [[nodiscard]] bool empty() const {
        return size_ == 0;
    }

    /**
     * @brief Get number of elements in the list.
     *
     * @return Number of elements.
     */
    [[nodiscard]] size_t size() const {
        return size_;
    }

    /**
     * @brief Append an element to the end of the list.
     *
     * @tparam V Type of element (deduced).
     * @param elem Element to append (forwarded).
     */
    template <typename V>
    void push_back(V&& elem);

    /**
     * @brief Prepend an element to the front of the list.
     *
     * @tparam V Type of element (deduced).
     * @param elem Element to prepend (forwarded).
     */
    template <typename V>
    void push_front(V&& elem);

    /**
     * @brief Remove the first element of the list.
     *
     * Throws if the list is empty.
     */
    void pop_front();

    /**
     * @brief Remove the last element of the list.
     *
     * Throws if the list is empty.
     */
    void pop_back();

    /**
     * @brief Insert an element before the position pointed by itr.
     *
     * @tparam V Type of element (deduced).
     * @param itr Iterator pointing to insertion position.
     * @param elem Element to insert (forwarded).
     * @return Iterator pointing to element after the inserted one.
     */
    template <typename V>
    iterator insert(iterator itr, V&& elem);

    /**
     * @brief Remove the element pointed by itr.
     *
     * @param itr Iterator pointing to element to remove.
     * @return Iterator pointing to element after the removed one.
     */
    iterator remove(iterator itr);

    /**
     * @brief Remove the range [start, stop) of elements.
     *
     * @param start Iterator to first element to remove.
     * @param stop  Iterator to element after last to remove.
     * @return Iterator pointing to stop after removal.
     */
    iterator remove(iterator start, iterator stop);

    /**
     * @brief Insert a range [start, stop) before position itr.
     *
     * @tparam InputIterator Input iterator type for the source range.
     * @param itr   Destination iterator where range is inserted.
     * @param start Start of source range (inclusive).
     * @param stop  End of source range (exclusive).
     * @return Iterator pointing to element after the last inserted.
     */
    template <typename InputIterator>
    iterator insert(iterator itr, InputIterator start, InputIterator stop) {
        for (auto in_itr = start; in_itr != stop; ++in_itr) {
            itr = insert(itr, *in_itr);
        }
        return itr;
    }

    /**
     * @brief Access the first element.
     *
     * @return Reference to the first element.
     */
    const T& front() const {
        if (size_ == 0) {
            throw EmptyContainer();
        }
        return head_->next_->elem();
    }

    /**
     * @brief Access the last element.
     *
     * @return Reference to the last element.
     */
    const T& back() const {
        if (size_ == 0) {
            throw EmptyContainer();
        }
        return tail_->prev_->elem();
    }

    /**
     * @brief Mutable access to the first element.
     *
     * @return Reference to the first element.
     */
    T& front() {
        return const_cast<T&>(static_cast<const list*>(this)->front());
    }

    /**
     * @brief Mutable access to the last element.
     *
     * @return Reference to the last element.
     */
    T& back() {
        return const_cast<T&>(static_cast<const list*>(this)->back());
    }

    /**
     * @brief Const iterator to first element.
     *
     * @return Iterator to first element.
     */
    const_iterator cbegin() const {
        return const_iterator(head_->next_);
    }

    /**
     * @brief Const iterator to end sentinel.
     *
     * @return Iterator representing end.
     */
    const_iterator cend() const {
        return const_iterator(tail_);
    }

    /**
     * @brief Iterator to first element.
     *
     * @return Iterator to first element.
     */
    iterator begin() {
        return iterator(head_->next_);
    }

    /**
     * @brief Iterator to end sentinel.
     *
     * @return Iterator representing end.
     */
    iterator end() {
        return iterator(tail_);
    }

    /**
     * @brief Const overload: iterator to first element.
     *
     * @return Iterator to first element.
     */
    const_iterator begin() const {
        return const_iterator(head_->next_);
    }

    /**
     * @brief Const overload: iterator to end sentinel.
     *
     * @return Iterator representing end.
     */
    const_iterator end() const {
        return const_iterator(tail_);
    }
};

template <typename T, typename Alloc>
void list<T, Alloc>::init() {
    head_ = allocate_node();
    tail_ = allocate_node();
    head_->next_ = tail_;
    tail_->prev_ = head_;
    size_ = 0;
}

template <typename T, typename Alloc>
list<T, Alloc>::self_t& list<T, Alloc>::operator=(const self_t& rhs) {
    if (this == &rhs) {
        return *this;
    }
    clear();
    init();
    for (auto itr = rhs.begin(); itr != rhs.end(); ++itr) {
        push_back(*itr);
    }

    return *this;
}

template <typename T, typename Alloc>
list<T, Alloc>::self_t& list<T, Alloc>::operator=(self_t&& rhs) noexcept {
    clear();
    head_ = rhs.head_;
    tail_ = rhs.tail_;
    size_ = rhs.size_;
    rhs.init();
    return *this;
}

template <typename T, typename Alloc>
template <typename V>
void list<T, Alloc>::push_back(V&& elem) {
    auto node = allocate_node(std::forward<V>(elem), tail_->prev_, tail_);
    tail_->prev_->next_ = node;
    tail_->prev_ = node;
    ++size_;
}

template <typename T, typename Alloc>
template <typename V>
void list<T, Alloc>::push_front(V&& elem) {
    auto node = allocate_node(std::forward<V>(elem), head_, head_->next_);
    head_->next_->prev_ = node;
    head_->next_ = node;
    ++size_;
}

template <typename T, typename Alloc>
void list<T, Alloc>::pop_back() {
    check_empty();

    auto node = tail_->prev_;
    node->prev_->next_ = tail_;
    tail_->prev_ = node->prev_;
    node->prev_ = node->next_ = nullptr;
    --size_;
    destroy_node(node);
}

template <typename T, typename Alloc>
void list<T, Alloc>::pop_front() {
    check_empty();

    auto node = head_->next_;
    node->next_->prev_ = head_;
    head_->next_ = node->next_;
    node->prev_ = node->next_ = nullptr;
    --size_;
    destroy_node(node);
}

template <typename T, typename Alloc>
template <typename V>
typename list<T, Alloc>::iterator list<T, Alloc>::insert(iterator itr,
                                                         V&& elem) {
    auto new_node =
        allocate_node(std::forward<V>(elem), itr.node_->prev_, itr.node_);
    itr.node_->prev_->next_ = new_node;
    itr.node_->prev_ = new_node;
    ++size_;
    return itr;
}

template <typename T, typename Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::remove(iterator itr) {
    if (itr.node_->is_head() || itr.node_->is_tail() || !bool(itr)) {
        throw std::out_of_range(
            "This iterator had tried to remove a non-existing element.");
    }
    auto node = itr.node_;
    itr.node_ = node->next_;

    node->prev_->next_ = node->next_;
    node->next_->prev_ = node->prev_;
    node->prev_ = node->next_ = nullptr;
    destroy_node(node);
    --size_;
    return itr;
}

template <typename T, typename Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::remove(iterator start,
                                                         iterator stop) {
    if (start == stop) {
        return stop;
    }
    size_ -= distance(start, stop);
    start.node_->prev_->next_ = stop.node_;
    stop.node_->prev_->next_ = nullptr;
    stop.node_->prev_ = start.node_->prev_;
    start.node_->prev_ = nullptr;
    destroy_node(start.node_);
    return stop;
}

/**
 * @brief Internal node type for list storing element and links.
 *
 * Represents one node in the doubly-linked list. Holds the element and
 * pointers to previous and next nodes. This type is private to the list
 * implementation.
 */
template <typename T, typename Alloc>
class list<T, Alloc>::Node {
public:
    /**
     * @brief Self type alias for Node.
     */
    using self_t = Node;

private:
    T elem_;
    NdPtr prev_;
    NdPtr next_;

public:
    /**
     * @brief Construct a default node with empty element and null links.
     */
    Node() : elem_(), prev_(nullptr), next_(nullptr) {}

    /**
     * @brief Construct a node with element and link pointers.
     *
     * @tparam V Value type forwarded to element construction.
     * @param elem Element value to store.
     * @param prev Pointer to previous node.
     * @param next Pointer to next node.
     */
    template <typename V>
    Node(V&& elem, self_t* prev, self_t* next) noexcept
        : elem_(std::forward<V>(elem)), prev_(prev), next_(next) {}

    /**
     * @brief Copy construction is disabled.
     */
    Node(const self_t& node) = delete;

    /**
     * @brief Move construction is disabled.
     */
    Node(self_t&& node) = delete;

    /**
     * @brief Copy assignment is disabled.
     *
     * @param node Source node.
     * @return Reference to this node.
     */
    self_t& operator=(const self_t& node) = delete;

    /**
     * @brief Move assignment is disabled.
     *
     * @param node Source node.
     * @return Reference to this node.
     */
    self_t& operator=(self_t&& node) = delete;

    /**
     * @brief Destroy the node.
     */
    ~Node() noexcept = default;

    /**
     * @brief Access the stored element (const).
     *
     * @return Const reference to the element.
     */
    const T& elem() const {
        return elem_;
    }

    /**
     * @brief Access the stored element (mutable).
     *
     * @return Reference to the element.
     */
    T& elem() {
        return const_cast<T&>(static_cast<const self_t*>(this)->elem());
    }

    /**
     * @brief Check whether this node is the tail sentinel.
     *
     * @return True if this node is the tail.
     */
    [[nodiscard]] bool is_tail() const {
        return next_ == nullptr;
    }

    /**
     * @brief Check whether this node is the head sentinel.
     *
     * @return True if this node is the head.
     */
    [[nodiscard]] bool is_head() const {
        return prev_ == nullptr;
    }

    friend class list<T, Alloc>;
};

/**
 * @brief Iterator for the list container.
 *
 * @tparam Ref Reference type returned by operator* (e.g. T& or const T&).
 * @tparam Ptr Pointer type returned by operator-> (e.g. Node* or const Node*).
 */
template <typename T, typename Alloc>
template <typename Ref, typename Ptr>
class list<T, Alloc>::list_iterator {
public:
    /**
     * @brief Self type alias for list_iterator.
     */
    using self_t = list_iterator<Ref, Ptr>;

private:
    NdPtr node_;
    friend class list<T, Alloc>;
    friend const_iterator;

public:
    /**
     * @brief Construct a null iterator.
     */
    list_iterator() : node_(nullptr) {}

    /**
     * @brief Construct from a node pointer.
     *
     * @param node Node pointer to wrap.
     */
    explicit list_iterator(NdPtr node) : node_(node) {}

    /**
     * @brief Construct from a compatible iterator.
     *
     * @tparam Iter Compatible iterator type.
     * @param rhs Source iterator.
     */
    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    list_iterator(const Iter& rhs) : node_(rhs.node_) {}

    /**
     * @brief Copy-construct from another iterator.
     *
     * @param rhs Source iterator.
     */
    list_iterator(const self_t& rhs) = default;

    /**
     * @brief Move-construct from another iterator.
     *
     * @param rhs Source iterator.
     */
    list_iterator(self_t&& rhs) noexcept = default;

    /**
     * @brief Destroy the iterator.
     */
    ~list_iterator() noexcept = default;

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
        node_ = rhs.node_;
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
        node_ = rhs.node_;
        return *this;
    }

    /**
     * @brief Dereference the iterator.
     *
     * @return Reference to the element.
     */
    Ref operator*() const {
        if (node_ == nullptr) {
            throw NullIterator();
        }
        return node_->elem();
    }

    /**
     * @brief Access the element through a pointer-like interface.
     *
     * @return Pointer to the element.
     */
    Ptr operator->() const {
        return &node_->elem();
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
        return lhs.node_ == rhs.node_;
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
    friend bool operator!=(const list_iterator& lhs, const Iter& rhs) {
        return lhs.node_ != rhs.node_;
    }

    /**
     * @brief Pre-increment to the next node.
     *
     * @return Reference to this iterator.
     */
    self_t& operator++() {
        if (node_->is_tail()) {
            throw std::out_of_range(
                "This iterator has gone out of range. No previous "
                "element.");
        }
        node_ = node_->next_;
        return *this;
    }

    /**
     * @brief Post-increment to the next node.
     *
     * @param unused Placeholder parameter for postfix form.
     * @return Iterator value before increment.
     */
    self_t operator++(int) {
        auto old = *this;
        this->operator++();
        return old;
    }

    /**
     * @brief Pre-decrement to the previous node.
     *
     * @return Reference to this iterator.
     */
    self_t& operator--() {
        if (node_->is_head()) {
            throw std::out_of_range(
                "This iterator has gone out of range. No next element.");
        }
        node_ = node_->prev_;
        return *this;
    }

    /**
     * @brief Post-decrement to the previous node.
     *
     * @param unused Placeholder parameter for postfix form.
     * @return Iterator value before decrement.
     */
    self_t operator--(int) {
        auto old = *this;
        this->operator--();
        return old;
    }

    /**
     * @brief Check whether the iterator refers to a valid node.
     *
     * @return True if the iterator is not null.
     */
    explicit operator bool() const {
        return node_;
    }
};
}  // namespace mtl
