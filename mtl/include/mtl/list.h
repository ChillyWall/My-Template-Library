//
// Created by metal on 2024/9/19.
//

#ifndef MTL_LIST_H
#define MTL_LIST_H

#include <mtl/algorithms.h>
#include <initializer_list>

namespace mtl {
    typedef unsigned long long size_t;

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
            Node(const T& elem, Node* prev, Node* next);
            Node(T&& elem, Node* prev, Node* next) noexcept;
            Node(const Node& node) = delete;

            ~Node();

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
            const_iterator(const_iterator&& ci) noexcept;
            virtual ~const_iterator() = default;

            const_iterator& operator=(const const_iterator& ci) {
                node_ = ci.node_;
                return *this;
            }
            const_iterator& operator=(const_iterator&& ci) noexcept {
                node_ = ci.node_;
                ci.node_ = nullptr;
                return *this;
            }

            const T& operator*() const {
                if (node_->is_head() || node_->is_tail()) {
                    throw std::runtime_error("This iterator is null.");
                }
                return node_->elem();
            }

            T& operator*() {
                return const_cast<T&>(static_cast<const const_iterator*>(this)->operator*());
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

            const_iterator operator+(size_t n) {
                auto res_itr = *this;
                res_itr += n;
                return res_itr;
            }
            const_iterator operator-(size_t n) {
                auto res_itr = *this;
                res_itr -= n;
                return res_itr;
            }

            explicit operator bool() const {
                return node_;
            }

            friend class list<T>;
        };

        class iterator : public const_iterator {
            public:
            iterator();
            explicit iterator(Node* node);
            iterator(const iterator& itr);
            iterator(iterator&& itr) noexcept;
            ~iterator() = default;

            iterator& operator=(const iterator& itr) {
                const_iterator::operator=(itr);
                return *this;
            }

            iterator& operator=(iterator&& itr) noexcept {
                const_iterator::operator=(std::move(itr));
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

        public:
        list();
        list(const list<T>& l);
        list(list<T>&& l) noexcept;
        list(std::initializer_list<T>&& init) noexcept;
        ~list();

        list<T>& operator=(const list<T>& l);
        list<T>& operator=(list<T>&& l) noexcept;

        void clear();

        bool empty() const {
            return size_ == 0;
        }

        size_t size() const {
            return size_;
        }

        void push_back(const T& elem);
        void push_back(T&& elem) noexcept;
        void push_front(const T& elem);
        void push_front(T&& elem) noexcept;

        void pop_front();
        void pop_back();

        iterator insert(iterator itr, const T& elem);
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
    };

    template <typename T>
    list<T>::Node::Node() : elem_(), prev_(nullptr), next_(nullptr) {}

    template <typename T>
    list<T>::Node::Node(const T& elem, Node* prev, Node* next) : elem_(elem), prev_(prev), next_(next) {}

    template <typename T>
    list<T>::Node::Node(T&& elem, Node* prev, Node* next) noexcept : elem_(std::move(elem)), prev_(prev), next_(next) {}

    template <typename T>
    list<T>::Node::~Node() {
        delete next_;
    }

    template <typename T>
    list<T>::const_iterator::const_iterator(Node* node) : node_(node) {}

    template <typename T>
    list<T>::const_iterator::const_iterator(const const_iterator& ci) : node_(ci.node_) {}

    template <typename T>
    list<T>::const_iterator::const_iterator(const_iterator&& ci) noexcept : node_(ci.node_) {
        ci.node_ = nullptr;
    }

    template <typename T>
    typename list<T>::const_iterator& list<T>::const_iterator::operator+=(size_t n) {
        for (size_t i = 0; i < n; ++i)
            this->operator++();
        return *this;
    }

    template <typename T>
    typename list<T>::const_iterator& list<T>::const_iterator::operator-=(size_t n) {
        for (size_t i = 0; i < n; ++i)
            this->operator--();
        return *this;
    }

    template <typename T>
    list<T>::iterator::iterator(Node* node) : const_iterator(node) {}

    template <typename T>
    list<T>::iterator::iterator(const iterator& itr) : const_iterator(itr) {}

    template <typename T>
    list<T>::iterator::iterator(iterator&& itr) noexcept : const_iterator(std::move(itr)) {}

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
    list<T>::list(list<T>&& l) noexcept : head_(l.head_), tail_(l.tail_), size_(l.size_) {
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
    list<T>::~list() {
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
    void list<T>::push_back(const T& elem) {
        Node* node = new Node(elem, tail_->prev_, tail_);
        tail_->prev_->next_ = node;
        tail_->prev_ = node;
        ++size_;
    }

    template <typename T>
    void list<T>::push_back(T&& elem) noexcept {
        Node* node = new Node(std::move(elem), tail_->prev_, tail_);
        tail_->prev_->next_ = node;
        tail_->prev_ = node;
        ++size_;
    }

    template <typename T>
    void list<T>::push_front(const T& elem) {
        Node* node = new Node(elem, head_, head_->next_);
        head_->next_->prev_ = node;
        head_->next_ = node;
        ++size_;
    }

    template <typename T>
    void list<T>::push_front(T&& elem) noexcept {
        Node* node = new Node(elem, head_, head_->next_);
        head_->next_->prev_ = node;
        head_->next_ = node;
        ++size_;
    }

    template <typename T>
    void list<T>::pop_back() {
        if (empty()) {
            throw std::out_of_range("There's no element to be popped out.");
        }

        Node* node = tail_->prev_;
        node->prev_->next_ = tail_;
        tail_->prev_ = node->prev_;
        node->prev_ = node->next_ = nullptr;
        --size_;
        delete node;
    }

    template <typename T>
    void list<T>::pop_front() {
        if (empty()) {
            throw std::out_of_range("There's no element to e popped out.");
        }

        Node* node = head_->next_;
        node->next_->prev_ = head_;
        head_->next_ = node->next_;
        node->prev_ = node->next_ = nullptr;
        --size_;
        delete node;
    }

    template <typename T>
    typename list<T>::iterator list<T>::insert(iterator itr, const T& elem) {
        Node* new_node = new Node(elem, itr.node_->prev_, itr.node_);
        itr.node_->prev_->next_ = new_node;
        itr.node_->prev_ = new_node;
        ++size_;
        return itr;
    }

    template <typename T>
    typename list<T>::iterator list<T>::insert(iterator itr, T&& elem) {
        Node* new_node = new Node(std::move(elem), itr.node_->prev_, itr.node_);
        itr.node_->prev_->next_ = new_node;
        itr.node_->prev_ = new_node;
        ++size_;
        return itr;       
    }

    template <typename T>
    typename list<T>::iterator list<T>::remove(iterator itr) {
        if (itr.node_->is_head() || itr.node_->is_tail()) {
            throw std::out_of_range("This iterator had tried to remove a nonexisting element.");
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
    typename list<T>::iterator list<T>::insert(iterator itr, InputIterator start, InputIterator stop) {
        for (auto in_itr = start; in_itr != stop; ++in_itr) {
            itr = insert(itr, *in_itr);
        }
        return itr;
    }
}

#endif //LIST_H
