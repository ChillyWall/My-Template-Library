//
// Created by metal on 2024/9/19.
//

#ifndef LIST_H
#define LIST_H

#include <initializer_list>

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
            Node(const T& elem, Node* next, Node* prev);
            Node(T&& elem, Node* next, Node* prev) noexcept;

            const T& elem() const;
            T& elem();
        };

        class const_iterator {
            private:
            Node* node_;

            public:
            const_iterator();
            const_iterator(Node* node);
            const_iterator(const const_iterator& ci);
            const_iterator(const_iterator&& ci) noexcept;

            const T& operator*() const {
                return node_->elem();
            }

            bool operator==(const const_iterator& ci) const {
                return node_ == ci.node_;
            }

            const_iterator& operator++() {
                node_ = node_->next_;
                return *this;
            }
            const_iterator operator++(int) {
                auto old = *this;
                this->operator++();
                return old;
            }
            const_iterator& operator--() {
                node_ = node_->prev_;
                return *this;
            }
            const_iterator operator--(int) {
                auto old = *this;
                this->operator--();
                return old;
            }
        };

        class iterator : public const_iterator {
            iterator();
            using const_iterator::const_iterator;
            iterator(const iterator& itr);
            iterator(iterator&& itr) noexcept;

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
                const_iterator::operator--(int);
                return old;
            }
        };

        Node head_;
        Node tail_;
        size_t size_;

        public:
        list();
        list(const list<T>& l);
        list(list<T>&& l);
        list(std::initializer_list&& init) noexcept;
        ~list()

        void clear();

        list<T>& operator=(const list<T>& l);
        list<T>& operator=(list<T>&& l) noexcept;

        void push_back(const T& elem);
        void push_back(T&& elem) noexcept;
        void push_front(const T& elem);
        void puss_front(T&& elem) noexcept;

        void pop_front();
        void pop_back();

        const_iterator cbegin() const {
            return const_iterator(head_->next_);
        }
        const_iterator cend() const {
            return const_iterator(tail_);
        }

        const_iterator begin() const {
            return cbegin();
        }
        const_iterator end() const {
            return cend();
        }

        iterator begin() {
            return iterator(head_->next_);
        }
        iterator end() {
            return iterator(tail_); 
        }
    };
}

#endif //LIST_H
