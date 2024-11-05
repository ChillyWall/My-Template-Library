#ifndef MTL_LIST_H
#define MTL_LIST_H

#include <initializer_list>
#include <mtl/algorithms.h>
#include <mtl/types.h>
#include <stdexcept>

namespace mtl {
template <typename T>
class list {
private:
    class Node {
    private:
        T elem_;
        Node* next_;
        Node* prev_;

    public:
        Node();
        template <typename V>
        Node(V&& elem, Node* prev, Node* next) noexcept;
        Node(const Node& node) = delete;

        ~Node() noexcept;

        const T& elem() const {
            return elem_;
        }

        T& elem() {
            return const_cast<T&>(static_cast<const Node*>(this)->elem());
        }

        bool is_tail() const {
            return !next_;
        }

        bool is_head() const {
            return !prev_;
        }

        friend class list<T>;
    };

    class const_iterator {
    protected:
        Node* node_;

    public:
        const_iterator();
        explicit const_iterator(Node* node);
        const_iterator(const const_iterator& ci);
        virtual ~const_iterator() noexcept = default;

        const_iterator& operator=(const const_iterator& ci) {
            node_ = ci.node_;
            return *this;
        }

        const T& operator*() const {
            if (node_->is_head() || node_->is_tail()) {
                throw NullIterator();
            }
            return node_->elem();
        }

        bool operator==(const const_iterator& ci) const {
            return node_ == ci.node_;
        }

        bool operator!=(const const_iterator& ci) const {
            return node_ != ci.node_;
        }

        const_iterator& operator++() {
            if (node_->is_tail()) {
                throw std::out_of_range("This iterator has gone out of range.");
            }
            node_ = node_->next_;
            return *this;
        }
        const_iterator operator++(int) {
            auto old = *this;
            this->operator++();
            return old;
        }
        const_iterator& operator--() {
            if (node_->is_head()) {
                throw std::out_of_range("This iterator has gone out of range.");
            }
            node_ = node_->prev_;
            return *this;
        }
        const_iterator operator--(int) {
            auto old = *this;
            this->operator--();
            return old;
        }

        const_iterator& operator+=(size_t n);
        const_iterator& operator-=(size_t n);

        const_iterator operator+(size_t n) const {
            auto res_itr = *this;
            res_itr += n;
            return res_itr;
        }
        const_iterator operator-(size_t n) const {
            auto res_itr = *this;
            res_itr -= n;
            return res_itr;
        }

        size_t operator-(const const_iterator& rhs) const;

        explicit operator bool() const {
            return node_;
        }

        friend class list<T>;
    };

    class iterator : public const_iterator {
    public:
        // use perfect forwarding, calling const_iterator's constructors
        template <typename... V>
        iterator(V&&... args);
        virtual ~iterator() noexcept = default;

        iterator& operator=(const iterator& itr) {
            const_iterator::operator=(itr);
            return *this;
        }

        T& operator*() {
            return const_cast<T&>(const_iterator::operator*());
        }

        iterator& operator++() {
            const_iterator::operator++();
            return *this;
        }

        iterator operator++(int) {
            auto old = *this;
            const_iterator::operator++();
            return old;
        }

        iterator& operator--() {
            const_iterator::operator--();
            return *this;
        }

        iterator operator--(int) {
            auto old = *this;
            const_iterator::operator--();
            return old;
        }

        iterator& operator+=(size_t n) {
            const_iterator::operator+=(n);
            return *this;
        }
        iterator& operator-=(size_t n) {
            const_iterator::operator-=(n);
            return *this;
        }

        iterator operator+(size_t n) {
            auto res_itr = *this;
            res_itr += n;
            return res_itr;
        }
        iterator operator-(size_t n) {
            auto res_itr = *this;
            res_itr -= n;
            return res_itr;
        }
    };

    Node* head_;
    Node* tail_;
    size_t size_;

    void init();
    void check_empty() {
        if (empty()) {
            throw EmptyContainer("There's no element to be popped out.");
        }
    }

public:
    list();
    list(const list<T>& l);
    list(list<T>&& l) noexcept;
    list(std::initializer_list<T>&& init) noexcept;
    ~list() noexcept;

    list<T>& operator=(const list<T>& l);
    list<T>& operator=(list<T>&& l) noexcept;

    void clear();

    bool empty() const {
        return size_ == 0;
    }

    size_t size() const {
        return size_;
    }

    template <typename V>
    void push_back(V&& elem);
    template <typename V>
    void push_front(V&& elem);

    void pop_front();
    void pop_back();

    iterator insert(iterator itr, T&& elem);
    iterator remove(iterator itr);
    iterator remove(iterator start, iterator stop);

    template <typename InputIterator>
    iterator insert(iterator itr, InputIterator start, InputIterator stop);

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

    const_iterator head() const {
        return iterator(head_);
    }

    const_iterator begin() const {
        return const_iterator(head_->next_);
    }

    const_iterator end() const {
        return const_iterator(tail_);
    }
};

template <typename T>
list<T>::Node::Node() : elem_(), prev_(nullptr), next_(nullptr) {}

template <typename T>
template <typename V>
list<T>::Node::Node(V&& elem, Node* prev, Node* next) noexcept
    : elem_(std::forward<V>(elem)), prev_(prev), next_(next) {}

template <typename T>
list<T>::Node::~Node() noexcept {
    delete next_;
}

template <typename T>
list<T>::const_iterator::const_iterator(Node* node) : node_(node) {}

template <typename T>
list<T>::const_iterator::const_iterator(const const_iterator& ci)
    : node_(ci.node_) {}

template <typename T>
typename list<T>::const_iterator&
list<T>::const_iterator::operator+=(size_t n) {
    for (size_t i = 0; i < n; ++i)
        this->operator++();
    return *this;
}

template <typename T>
typename list<T>::const_iterator&
list<T>::const_iterator::operator-=(size_t n) {
    for (size_t i = 0; i < n; ++i)
        this->operator--();
    return *this;
}

template <typename T>
size_t list<T>::const_iterator::operator-(const const_iterator& rhs) const {
    auto itr = *this;
    size_t res = 0;
    while (itr != rhs) {
        --itr;
        ++res;
    }
    return res;
}

template <typename T>
template <typename... V>
list<T>::iterator::iterator(V&&... args)
    : const_iterator(std::forward<V>(args)...) {}

template <typename T>
void list<T>::init() {
    head_ = new Node();
    tail_ = new Node();
    head_->next_ = tail_;
    tail_->prev_ = head_;
    size_ = 0;
}

template <typename T>
list<T>::list() {
    init();
}

template <typename T>
list<T>::list(const list<T>& l) {
    init();
    for (auto itr = l.begin(); itr != l.end(); ++itr) {
        push_back(*itr);
    }
}

template <typename T>
list<T>::list(list<T>&& l) noexcept
    : head_(l.head_), tail_(l.tail_), size_(l.size_) {
    l.init();
}

template <typename T>
list<T>::list(std::initializer_list<T>&& il) noexcept {
    init();
    for (auto itr = il.begin(); itr != il.end(); ++itr) {
        push_back(std::move(*itr));
    }
}

template <typename T>
list<T>::~list() noexcept {
    delete head_;
}

template <typename T>
void list<T>::clear() {
    delete head_;
    init();
}

template <typename T>
list<T>& list<T>::operator=(const list<T>& l) {
    clear();
    init();
    for (auto itr = l.begin(); itr != l.end(); ++itr) {
        push_back(*itr);
    }

    return *this;
}

template <typename T>
list<T>& list<T>::operator=(list<T>&& l) noexcept {
    clear();
    head_ = l.head_;
    tail_ = l.tail_;
    size_ = l.size_;
    l.init();
}

template <typename T>
template <typename V>
void list<T>::push_back(V&& elem) {
    Node* node = new Node(std::forward<V>(elem), tail_->prev_, tail_);
    tail_->prev_->next_ = node;
    tail_->prev_ = node;
    ++size_;
}

template <typename T>
template <typename V>
void list<T>::push_front(V&& elem) {
    Node* node = new Node(std::forward<V>(elem), head_, head_->next_);
    head_->next_->prev_ = node;
    head_->next_ = node;
    ++size_;
}

template <typename T>
void list<T>::pop_back() {
    check_empty();

    Node* node = tail_->prev_;
    node->prev_->next_ = tail_;
    tail_->prev_ = node->prev_;
    node->prev_ = node->next_ = nullptr;
    --size_;
    delete node;
}

template <typename T>
void list<T>::pop_front() {
    check_empty();

    Node* node = head_->next_;
    node->next_->prev_ = head_;
    head_->next_ = node->next_;
    node->prev_ = node->next_ = nullptr;
    --size_;
    delete node;
}

template <typename T>
typename list<T>::iterator list<T>::insert(iterator itr, T&& elem) {
    Node* new_node =
        new Node(std::forward<T>(elem), itr.node_->prev_, itr.node_);
    itr.node_->prev_->next_ = new_node;
    itr.node_->prev_ = new_node;
    ++size_;
    return itr;
}

template <typename T>
typename list<T>::iterator list<T>::remove(iterator itr) {
    if (itr.node_->is_head() || itr.node_->is_tail() || bool(itr)) {
        throw std::out_of_range(
            "This iterator had tried to remove a nonexisting element.");
    }
    Node* node = itr.node_;
    itr.node_ = node->next_;

    node->prev_->next_ = node->next_;
    node->next_->prev_ = node->prev_;
    node->prev_ = node->next_ = nullptr;
    delete node;
    --size_;
    return itr;
}

template <typename T>
typename list<T>::iterator list<T>::remove(iterator start, iterator stop) {
    if (start == stop) {
        return stop;
    }
    size_ -= count_length(start, stop);
    start.node_->prev_->next_ = stop.node_;
    stop.node_->prev_->next_ = nullptr;
    stop.node_->prev_ = start.node_->prev_;
    start.node_->prev_ = nullptr;
    delete start.node_;
    return stop;
}

template <typename T>
template <typename InputIterator>
typename list<T>::iterator list<T>::insert(iterator itr, InputIterator start,
                                           InputIterator stop) {
    for (auto in_itr = start; in_itr != stop; ++in_itr) {
        itr = insert(itr, *in_itr);
    }
    return itr;
}
} // namespace mtl
#endif // LIST_H
//
