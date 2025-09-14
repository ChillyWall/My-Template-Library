#ifndef MTL_LIST_H
#define MTL_LIST_H

#include <mtl/algorithms.h>
#include <mtl/mtldefs.h>
#include <initializer_list>
#include <memory>
#include <stdexcept>

namespace mtl {

/* the list ADT, it's a double linked list. */
template <typename T, typename Alloc = std::allocator<T>>
class list {
public:
    using self_t = list<T, Alloc>;

private:
    // the node class
    class Node;
    using NdPtr = Node*;

    // the list iterator class
    template <typename Ref, typename Ptr>
    class list_iterator;

public:
    using const_iterator = list_iterator<const T&, const Node*>;
    using iterator = list_iterator<T&, Node*>;

private:
    using NodeAlloc =
        typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
    NodeAlloc allocator_;

    NdPtr head_ {nullptr};
    NdPtr tail_ {nullptr};
    size_t size_ {0};

    void init();
    void check_empty() const {
        if (empty()) {
            throw EmptyContainer("There's no element to be popped out.");
        }
    }

    template <typename... Args>
    NdPtr allocate_node(Args&&... args) {
        auto ptr = allocator_.allocate(1);
        std::construct_at(ptr, std::forward<Args>(args)...);
        return ptr;
    }

    void destroy_node(NdPtr ptr) {
        if (ptr && ptr->next_) {
            destroy_node(ptr->next_);
        }
        std::destroy_at(ptr);
        allocator_.deallocate(ptr, 1);
    }

public:
    list() {
        init();
    }

    list(const self_t& rhs) {
        init();
        for (auto itr = rhs.begin(); itr != rhs.end(); ++itr) {
            push_back(*itr);
        }
    }

    list(self_t&& rhs) noexcept
        : head_(rhs.head_), tail_(rhs.tail_), size_(rhs.size_) {
        rhs.init();
    }

    list(std::initializer_list<T> il) noexcept {
        init();
        for (auto itr = il.begin(); itr != il.end(); ++itr) {
            push_back(std::move(*itr));
        }
    }

    ~list() noexcept {
        destroy_node(head_);
    }

    self_t& operator=(const self_t& rhs);
    self_t& operator=(self_t&& rhs) noexcept;

    void clear() {
        destroy_node(head_);
        init();
    }

    [[nodiscard]] bool empty() const {
        return size_ == 0;
    }

    [[nodiscard]] size_t size() const {
        return size_;
    }

    template <typename V>
    void push_back(V&& elem);
    template <typename V>
    void push_front(V&& elem);

    void pop_front();
    void pop_back();

    /* insert a element at the position itr points to.
     * the return value points to the element after the inserted element.
     */
    template <typename V>
    iterator insert(iterator itr, V&& elem);

    /* remove the element the itr points to.
     * the return value points to the element after the removed element.
     */
    iterator remove(iterator itr);

    /* remove the elements in the range [start, stop).
     * the start points to the first element to be removed.
     * the stop points to the element after the last element to be removed.
     * the return value points to the element after the last removed element.
     */
    iterator remove(iterator start, iterator stop);

    /* insert the elements in the range [start, stop) at the position itr points
     * to. the start points to the first element to be inserted. the stop points
     * to the element after the last element to be inserted. the return value
     * points to the element after the last inserted element.
     */
    template <typename InputIterator>
    iterator insert(iterator itr, InputIterator start, InputIterator stop) {
        for (auto in_itr = start; in_itr != stop; ++in_itr) {
            itr = insert(itr, *in_itr);
        }
        return itr;
    }

    const T& front() const {
        if (size_ == 0) {
            throw EmptyContainer();
        }
        return head_->next_->elem();
    }

    const T& back() const {
        if (size_ == 0) {
            throw EmptyContainer();
        }
        return tail_->prev_->elem();
    }

    T& front() {
        return const_cast<T&>(static_cast<const list*>(this)->front());
    }

    T& back() {
        return const_cast<T&>(static_cast<const list*>(this)->back());
    }

    const_iterator cbegin() const {
        return const_iterator(head_->next_);
    }

    const_iterator cend() const {
        return const_iterator(tail_);
    }

    iterator begin() {
        return iterator(head_->next_);
    }

    iterator end() {
        return iterator(tail_);
    }

    const_iterator begin() const {
        return const_iterator(head_->next_);
    }

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

template <typename T, typename Alloc>
class list<T, Alloc>::Node {
public:
    using self_t = Node;

private:
    T elem_;
    NdPtr prev_;
    NdPtr next_;

public:
    Node() : elem_(), prev_(nullptr), next_(nullptr) {}
    template <typename V>
    Node(V&& elem, self_t* prev, self_t* next) noexcept
        : elem_(std::forward<V>(elem)), prev_(prev), next_(next) {}

    Node(const self_t& node) = delete;
    Node(self_t&& node) = delete;
    self_t& operator=(const self_t& node) = delete;
    self_t& operator=(self_t&& node) = delete;

    ~Node() noexcept = default;

    const T& elem() const {
        return elem_;
    }

    T& elem() {
        return const_cast<T&>(static_cast<const self_t*>(this)->elem());
    }

    [[nodiscard]] bool is_tail() const {
        return next_ == nullptr;
    }

    [[nodiscard]] bool is_head() const {
        return prev_ == nullptr;
    }

    friend class list<T, Alloc>;
};

template <typename T, typename Alloc>
template <typename Ref, typename Ptr>
class list<T, Alloc>::list_iterator {
public:
    using self_t = list_iterator<Ref, Ptr>;

private:
    NdPtr node_;
    friend class list<T, Alloc>;
    friend const_iterator;

public:
    list_iterator() : node_(nullptr) {}
    explicit list_iterator(NdPtr node) : node_(node) {}

    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    list_iterator(const Iter& rhs) : node_(rhs.node_) {}

    list_iterator(const self_t& rhs) = default;
    list_iterator(self_t&& rhs) noexcept = default;

    ~list_iterator() noexcept = default;

    self_t& operator=(const self_t& rhs) = default;
    self_t& operator=(self_t&& rhs) noexcept = default;

    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    self_t& operator=(const Iter& rhs) {
        node_ = rhs.node_;
        return *this;
    }

    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    self_t& operator=(Iter&& rhs) noexcept {
        node_ = rhs.node_;
        return *this;
    }

    Ref operator*() const {
        if (node_ == nullptr) {
            throw NullIterator();
        }
        return node_->elem();
    }

    Ptr operator->() const {
        return &node_->elem();
    }

    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator==(const self_t& lhs, const Iter& rhs) {
        return lhs.node_ == rhs.node_;
    }

    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator!=(const list_iterator& lhs, const Iter& rhs) {
        return lhs.node_ != rhs.node_;
    }

    self_t& operator++() {
        if (node_->is_tail()) {
            throw std::out_of_range(
                "This iterator has gone out of range. No previous "
                "element.");
        }
        node_ = node_->next_;
        return *this;
    }

    self_t operator++(int) {
        auto old = *this;
        this->operator++();
        return old;
    }

    self_t& operator--() {
        if (node_->is_head()) {
            throw std::out_of_range(
                "This iterator has gone out of range. No next element.");
        }
        node_ = node_->prev_;
        return *this;
    }

    self_t operator--(int) {
        auto old = *this;
        this->operator--();
        return old;
    }

    explicit operator bool() const {
        return node_;
    }
};
}  // namespace mtl
#endif  // LIST_H
//
