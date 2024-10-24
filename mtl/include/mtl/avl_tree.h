#ifndef MTL_AVL_TREE_H
#define MTL_AVL_TREE_H

#include <utility>

namespace mtl {
    typedef unsigned long long size_t;

    template <typename T>
    class avl_tree {
    private:
        class Node {
        private:
            T element_;
            Node* parent_;
            Node* left_;
            Node* right_;
            long long height_;

        public:
            Node(const T& elem, Node* par, Node* lt, Node* rt);
            Node(T&& elem, Node* par, Node* lt, Node* rt) noexcept;
            ~Node();

            long long height() const {
                return height_;
            }

            T& element() {
                return element_;
            }

            bool is_left() const {
                if (!parent_) {
                    return false;
                }
                return parent_->left_ == this;
            }

            bool is_right() const {
                if (!parent_) {
                    return false;
                }
                return parent_->right_ == this;
            }

            bool is_root() const {
                return !parent_;
            }

            bool has_left() const {
                return bool(left_);
            }

            bool has_right() const {
                return bool(right_);
            }

            friend class avl_tree<T>;
            friend class const_iterator;
        };

        class const_iterator {
        private:
            Node* node_;
            bool visited_;

        public:
            const_iterator();
            explicit const_iterator(Node* node);
            const_iterator(const const_iterator& rhs);
            const_iterator(const_iterator&& rhs) noexcept;

            // it don't check whether the iterator is valid
            const T& operator*() const {
                return node_->element_;
            }

            explicit operator bool() {
                return node_;
            }

            const_iterator& operator++();
            const_iterator& operator--();

            const_iterator operator++(int) {
                const_iterator old(*this);
                this->operator++();
                return old;
            }

            const_iterator operator--(int) {
                const_iterator old(*this);
                this->operator--();
                return old;
            }

            const_iterator& operator=(const const_iterator& rhs) {
                node_ = rhs.node_;
                return *this;
            }

            const_iterator& operator=(const_iterator&& rhs) noexcept {
                node_ = rhs.node_;
                rhs.node_ = nullptr;
                return *this;
            }

            const_iterator& operator+=(size_t n);
            const_iterator& operator-=(size_t n);

            const_iterator operator+(size_t n) {
                const_iterator res(*this);
                res += n;
                return res;
            }

            const_iterator operator-(size_t n) {
                const_iterator res(*this);
                res -= n;
                return res;
            }
        };

        class iterator : public const_iterator {
        public:
            iterator();
            explicit iterator(Node* node);
            iterator(const iterator& rhs);
            iterator(iterator&& rhs) noexcept;

            T& operator*() {
                return const_cast<T&>(const_iterator::operator*());
            }

            iterator& operator++() {
                const_iterator::operator++();
                return *this;
            }
            iterator& operator--() {
                const_iterator::operator--();
                return *this;
            }

            iterator operator++(int) {
                iterator old(*this);
                const_iterator::operator++();
                return old;
            }
            iterator operator--(int) {
                iterator old(*this);
                const_iterator::operator--();
                return old;
            }

            iterator& operator=(const iterator& rhs) {
                iterator::operator=(rhs);
                return *this;
            }
            iterator& operator=(iterator&& rhs) noexcept {
                iterator::operator=(std::move(rhs));
                return *this;
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
                iterator res(*this);
                res += n;
                return res;
            }

            iterator operator-(size_t n) {
                iterator res(*this);
                res -= n;
                return res;
            }
        };

        Node* root_;
        size_t size_;
        static const int ALLOWED_IMBALANCE = 1;
        static long long height(Node* node) {
            return !node ? -1 : node->height();
        }

        void balance(Node* node);

        void rotate_left(Node* node);

        void rotate_right(Node* node);

        void double_rotate_left(Node* node) {
            rotate_right(node->left_);
            rotate_left(node);
        }

        void double_rotate_right(Node* node) {
            rotate_left(node->right_);
            rotate_right(node);
        }

        static Node* find_min(Node* node);
        static Node* find_max(Node* node);

    public:
        avl_tree();
        avl_tree(const avl_tree<T>& rhs);
        avl_tree(avl_tree<T>&& rhs) noexcept;
        ~avl_tree();

        bool empty() const {
            return size_ == 0;
        }

        void clear() {
            size_ = 0;
            delete root_;
        }

        size_t size() const {
            return size_;
        }

        iterator begin() {
            return find_min();
        }

        /* return an iterator containing a nullptr,
           so it doesn't support operator++ and operator-- */
        iterator end() {
            return iterator(nullptr);
        }

        /* return an iterator containing a nullptr,
           so it doesn't support operator++ and operator-- */
        const_iterator cbegin() const {
            return const_iterator(nullptr);
        }

        const_iterator cend() const {
            return find_max();
        }

        // return iterator to the inserted node
        iterator insert(const T& elem);
        // return iterator to the inserted node
        iterator insert(T&& elem) noexcept;

        // return iterator to the last node of erased node
        iterator erase(const T& elem);

        // return whether the elem is in the tree
        bool contain(const T& elem) const;

        // return iterator to the node containing element
        const_iterator find(const T& elem) const;
        // return iterator to the node containing element
        iterator find(const T& elem);

        // return iterator to the minimum element
        const_iterator find_min() const {
            return const_iterator(find_min(root_));
        }

        // return iterator to the minimum element
        const_iterator find_max() const {
            return const_iterator(find_max(root_));
        }

        iterator find_min() {
            return iterator(find_min(root_));
        }

        iterator find_max() {
            return iterator(find_max(root_));
        }
    };

    template <typename T>
    bool avl_tree<T>::contain(const T& elem) const {
        if (this->empty()) {
            return false;
        }
        auto t = root_;
        while (true) {
            if (elem > t->element) {
                if (t->has_right()) {
                    t = t->right;
                } else {
                    return false;
                }
            } else if (elem < t->element){
                if (t->has_left()) {
                    t = t->left;
                } else {
                    return false;
                }
            } else {
                return true;
            }
        }
    }

    template <typename T>
    typename avl_tree<T>::Node* avl_tree<T>::find_min(Node* node) {
        if (!node) {
            return node;
        }

        while (node->has_right()) {
            node = node->right;
        }
        return node;
    }

    template <typename T>
    typename avl_tree<T>::Node* avl_tree<T>::find_max(Node* node) {
        if (!node) {
            return node;
        }

        while (node->has_left()) {
            node = node->left;
        }
        return node;
    }
}

#endif
