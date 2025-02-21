#ifndef MTL_LIST_H
#define MTL_LIST_H

#include <mtl/algorithms.h>
#include <mtl/types.h>
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace mtl {

/* the list ADT, it's a double linked list. */
template <typename T, template <typename> typename Alloc = std::allocator>
class list {
private:
    // the node class
    class Node;
    using NdPtr = Node*;

    // the list iterator class
    template <typename Ref, typename Ptr>
    class list_iterator;

    using const_iterator = list_iterator<const T&, const Node*>;
    using iterator = list_iterator<T&, Node*>;

    Alloc<Node> allocator_;

    NdPtr head_;
    NdPtr tail_;
    size_t size_;

    void init();
    void check_empty() const {
        if (empty()) {
            throw EmptyContainer("There's no element to be popped out.");
        }
    }

    template <typename... Args>
    NdPtr allocate_node_(Args&&... args) {
        auto ptr = allocator_.allocate(1);
        std::construct_at(ptr, std::forward<Args>(args)...);
        return ptr;
    }

    void destroy_node_(NdPtr ptr) {
        if (ptr && ptr->next_) {
            destroy_node_(ptr->next_);
        }
        std::destroy_at(ptr);
        allocator_.deallocate(ptr, 1);
    }

public:
    list() {
        init();
    }

    list(const list<T, Alloc>& l) {
        init();
        for (auto itr = l.begin(); itr != l.end(); ++itr) {
            push_back(*itr);
        }
    }

    list(list<T, Alloc>&& l) noexcept
        : head_(l.head_), tail_(l.tail_), size_(l.size_) {
        l.init();
    }

    list(std::initializer_list<T>&& il) noexcept {
        init();
        for (auto itr = il.begin(); itr != il.end(); ++itr) {
            push_back(std::move(*itr));
        }
    }

    ~list() noexcept {
        destroy_node_(head_);
    }

    list<T, Alloc>& operator=(const list<T, Alloc>& l);
    list<T, Alloc>& operator=(list<T, Alloc>&& l) noexcept;

    void clear() {
        destroy_node_(head_);
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

template <typename T, template <typename> typename Alloc>
void list<T, Alloc>::init() {
    head_ = allocate_node_();
    tail_ = allocate_node_();
    head_->next_ = tail_;
    tail_->prev_ = head_;
    size_ = 0;
}

template <typename T, template <typename> typename Alloc>
list<T, Alloc>& list<T, Alloc>::operator=(const list<T, Alloc>& l) {
    if (this == &l) {
        return *this;
    }
    clear();
    init();
    for (auto itr = l.begin(); itr != l.end(); ++itr) {
        push_back(*itr);
    }

    return *this;
}

template <typename T, template <typename> typename Alloc>
list<T, Alloc>& list<T, Alloc>::operator=(list<T, Alloc>&& l) noexcept {
    clear();
    head_ = l.head_;
    tail_ = l.tail_;
    size_ = l.size_;
    l.init();
    return *this;
}

template <typename T, template <typename> typename Alloc>
template <typename V>
void list<T, Alloc>::push_back(V&& elem) {
    auto node = allocate_node_(std::forward<V>(elem), tail_->prev_, tail_);
    tail_->prev_->next_ = node;
    tail_->prev_ = node;
    ++size_;
}

template <typename T, template <typename> typename Alloc>
template <typename V>
void list<T, Alloc>::push_front(V&& elem) {
    auto node = allocate_node_(std::forward<V>(elem), head_, head_->next_);
    head_->next_->prev_ = node;
    head_->next_ = node;
    ++size_;
}

template <typename T, template <typename> typename Alloc>
void list<T, Alloc>::pop_back() {
    check_empty();

    auto node = tail_->prev_;
    node->prev_->next_ = tail_;
    tail_->prev_ = node->prev_;
    node->prev_ = node->next_ = nullptr;
    --size_;
    destroy_node_(node);
}

template <typename T, template <typename> typename Alloc>
void list<T, Alloc>::pop_front() {
    check_empty();

    auto node = head_->next_;
    node->next_->prev_ = head_;
    head_->next_ = node->next_;
    node->prev_ = node->next_ = nullptr;
    --size_;
    destroy_node_(node);
}

template <typename T, template <typename> typename Alloc>
template <typename V>
typename list<T, Alloc>::iterator list<T, Alloc>::insert(iterator itr,
                                                         V&& elem) {
    auto new_node =
        allocate_node_(std::forward<V>(elem), itr.node_->prev_, itr.node_);
    itr.node_->prev_->next_ = new_node;
    itr.node_->prev_ = new_node;
    ++size_;
    return itr;
}

template <typename T, template <typename> typename Alloc>
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
    destroy_node_(node);
    --size_;
    return itr;
}

template <typename T, template <typename> typename Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::remove(iterator start,
                                                         iterator stop) {
    if (start == stop) {
        return stop;
    }
    size_ -= stop - start;
    start.node_->prev_->next_ = stop.node_;
    stop.node_->prev_->next_ = nullptr;
    stop.node_->prev_ = start.node_->prev_;
    start.node_->prev_ = nullptr;
    destroy_node_(start.node_);
    return stop;
}

template <typename T, template <typename> typename Alloc>
class list<T, Alloc>::Node {
private:
    T elem_;
    NdPtr prev_;
    NdPtr next_;

public:
    Node() : elem_(), prev_(nullptr), next_(nullptr) {}
    template <typename V>
    Node(V&& elem, Node* prev, Node* next) noexcept
        : elem_(std::forward<V>(elem)), prev_(prev), next_(next) {}

    Node(const Node& node) = delete;

    ~Node() noexcept {}

    const T& elem() const {
        return elem_;
    }

    T& elem() {
        return const_cast<T&>(static_cast<const Node*>(this)->elem());
    }

    [[nodiscard]] bool is_tail() const {
        return !next_;
    }

    [[nodiscard]] bool is_head() const {
        return !prev_;
    }

    friend class list<T, Alloc>;
};

template <typename T, template <typename> typename Alloc>
template <typename Ref, typename Ptr>
class list<T, Alloc>::list_iterator {
protected:
    NdPtr node_;
    using self_t = list_iterator<Ref, Ptr>;

public:
    list_iterator() : node_(nullptr) {}
    explicit list_iterator(NdPtr node) : node_(node) {}

    template <typename Iter,
              typename = std::enable_if_t<
                  std::is_same<self_t, const_iterator>::value &&
                  std::is_same<Iter, iterator>::value>>
    list_iterator(const Iter& rhs) : node_(rhs.node_) {}

    list_iterator(const list_iterator& rhs) : node_(rhs.node_) {}
    ~list_iterator() noexcept = default;

    self_t& operator=(const list_iterator& rhs) = default;

    template <typename Iter,
              typename = std::enable_if_t<
                  std::is_same<self_t, const_iterator>::value &&
                  std::is_same<Iter, iterator>::value>>
    self_t& operator=(const Iter& rhs) {
        node_ = const_cast<Ptr>(rhs.node_);
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

    friend bool operator==(const list_iterator& lhs, const list_iterator& rhs) {
        return lhs.node_ == rhs.node_;
    }

    friend bool operator!=(const list_iterator& lhs, const list_iterator& rhs) {
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

    self_t& operator+=(size_t n) {
        for (size_t i = 0; i < n; ++i)
            this->operator++();
        return *this;
    }

    self_t& operator-=(size_t n) {
        for (size_t i = 0; i < n; ++i)
            this->operator--();
        return *this;
    }

    self_t operator+(size_t n) const {
        auto res_itr = *this;
        res_itr += n;
        return res_itr;
    }

    self_t operator-(size_t n) const {
        auto res_itr = *this;
        res_itr -= n;
        return res_itr;
    }

    difference_t operator-(list_iterator rhs) const {
        difference_t res = 0;
        while (*this != rhs) {
            ++rhs;
            ++res;
        }
        return res;
    }

    explicit operator bool() const {
        return node_;
    }

    friend class list<T, Alloc>;
};
}  // namespace mtl
#endif  // LIST_H
//
