#ifndef MTL_AVL_TREE_H
#define MTL_AVL_TREE_H

#include <mtl/types.h>

namespace mtl {
template <typename T>
class avl_tree {
private:
    // The node class
    class Node {
    private:
        T element_;
        // its parent node
        Node* parent_;
        // its left child
        Node* left_;
        // its right child
        Node* right_;
        // its height
        long long height_;

    public:
        Node(const T& elem, Node* par, Node* lt, Node* rt);
        Node(T&& elem, Node* par, Node* lt, Node* rt) noexcept;
        /* an instance is destructed, it will automatically destruct its
           children. */
        ~Node();

        long long height() const {
            return height_;
        }

        T& element() {
            return element_;
        }

        const T& element() const {
            return element_;
        }

        // return if this node is a left child
        bool is_left() const {
            if (!parent_) {
                return false;
            }
            return parent_->left_ == this;
        }

        // return if this node is a right child
        bool is_right() const {
            if (!parent_) {
                return false;
            }
            return parent_->right_ == this;
        }

        // return if this node is a left child
        bool is_root() const {
            return !parent_;
        }

        // return if this node has a left child
        bool has_left() const {
            return bool(left_);
        }

        // return is this node has a right child
        bool has_right() const {
            return bool(right_);
        }

        friend class avl_tree<T>;
        friend class const_iterator;
    };

    template <typename Ref, typename Ptr>
    class avl_iterator;

public:
    using const_iterator = avl_iterator<const T&, const T*>;
    using iterator = avl_iterator<T&, T*>;

private:
    template <typename Ref, typename Ptr>
    class avl_iterator {
    private:
        // the pointer to the node
        Node* node_;
        // to record whether the left side is visited
        bool visited_;
        using self_t = avl_iterator<Ref, Ptr>;

    public:
        avl_iterator();
        explicit avl_iterator(Node* node);
        avl_iterator(const avl_iterator& rhs);

        /* it don't check whether the iterator is valid
         * so a segmentation fault is possible to be thrown out */
        const T& operator*() const {
            return node_->element_;
        }

        // checkk whether the iterator refers to a valid node
        explicit operator bool() {
            return bool(node_);
        }

        self_t& operator++();
        self_t& operator--();

        self_t operator++(int) {
            self_t old(*this);
            this->operator++();
            return old;
        }

        self_t operator--(int) {
            self_t old(*this);
            this->operator--();
            return old;
        }

        self_t& operator=(const avl_iterator& rhs) {
            node_ = rhs.node_;
            return *this;
        }

        self_t& operator+=(size_t n);
        self_t& operator-=(size_t n);

        self_t operator+(size_t n) {
            self_t res(*this);
            res += n;
            return res;
        }

        self_t operator-(size_t n) {
            self_t res(*this);
            res -= n;
            return res;
        }
    };

    // the root node of the tree
    Node* root_;
    // the number of nodes in this tree
    size_t size_;
    // the maximum difference between the heights of left and right children
    static const int ALLOWED_IMBALANCE = 1;
    // if the node is invalid, it's height would be -1
    static long long height(Node* node) {
        return !node ? -1 : node->height();
    }

    // to balance the tree
    void balance(Node* node);

    // perform single rotation
    void rotate_left(Node* node);

    // perform single rotation
    void rotate_right(Node* node);

    // perform double rotation
    void double_rotate_left(Node* node) {
        rotate_right(node->left_);
        rotate_left(node);
    }

    // perform double rotation
    void double_rotate_right(Node* node) {
        rotate_left(node->right_);
        rotate_right(node);
    }

    // find the node containing the minimum in the tree with node as root
    static Node* find_min(Node* node);
    // find the node containing the maximum int the tree with node as root
    static Node* find_max(Node* node);

public:
    avl_tree();
    avl_tree(const avl_tree<T>& rhs);
    avl_tree(avl_tree<T>&& rhs) noexcept;
    ~avl_tree();

    bool empty() const {
        return size_ == 0;
    }

    // set size_ to 0 and delete the root
    void clear() {
        size_ = 0;
        delete root_;
    }

    size_t size() const {
        return size_;
    }

    // return an iterator containing a the minimum
    iterator begin() {
        return find_min();
    }

    /* return an iterator containing a nullptr,
       so it doesn't support operator++ and operator-- */
    iterator end() {
        return iterator(nullptr);
    }

    // return an iterator containing a the minimum
    const_iterator begin() const {
        return find_min();
    }

    /* return an iterator containing a nullptr,
       so it doesn't support operator++ and operator-- */
    const_iterator end() const {
        return iterator(nullptr);
    }

    // return an const_iterator containing a the minimum
    const_iterator cbegin() const {
        return const_iterator(nullptr);
    }
    /* return an const_iterator containing a nullptr,
       so it doesn't support operator++ and operator-- */
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

    // find the minimum element
    iterator find_min() {
        return iterator(find_min(root_));
    }

    // find the maximum element
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
        } else if (elem < t->element) {
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
typename avl_tree<T>::Node* avl_tree<T>::find_max(Node* node) {
    if (!node) {
        return node;
    }

    while (node->has_right()) {
        node = node->right_;
    }
    return node;
}

template <typename T>
typename avl_tree<T>::Node* avl_tree<T>::find_min(Node* node) {
    if (!node) {
        return node;
    }

    while (node->has_left()) {
        node = node->left_;
    }
    return node;
}

template <typename T>
template <typename Ref, typename Ptr>
avl_tree<T>::avl_iterator<Ref, Ptr>::self_t
avl_tree<T>::avl_iterator<Ref, Ptr>::operator++() {}
} // namespace mtl

#endif
