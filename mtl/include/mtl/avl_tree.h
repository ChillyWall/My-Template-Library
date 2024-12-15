#ifndef MTL_AVL_TREE_H
#define MTL_AVL_TREE_H

#include <mtl/algorithms.h>
#include <mtl/types.h>

namespace mtl {
template <typename T>
class avl_tree {
private:
    class Node;
    using NdPtr = Node*;

    // The node class
    class Node {
    private:
        T element_;
        // its parent node
        NdPtr parent_;
        // its left child
        NdPtr left_;
        // its right child
        NdPtr right_;
        // its height
        long long height_;

    public:
        Node(const T& elem, NdPtr par, NdPtr lt, NdPtr rt);
        Node(T&& elem, NdPtr par, NdPtr lt, NdPtr rt) noexcept;
        /* an instance is destructed, it will automatically destruct its
         * children. */
        ~Node() noexcept;

        long long height() const {
            return height_;
        }

        T& element() {
            return element_;
        }

        const T& element() const {
            return element_;
        }

        NdPtr left() const {
            return left_;
        }

        NdPtr right() const {
            return right_;
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
        NdPtr node_;
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
            return node_->element();
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

        self_t& operator+=(difference_t n);
        self_t& operator-=(difference_t n) {
            return this->operator+=(-n);
        }

        self_t operator+(difference_t n) {
            self_t res(*this);
            res += n;
            return res;
        }

        self_t operator-(difference_t n) {
            self_t res(*this);
            res -= n;
            return res;
        }

        friend bool operator==(const avl_iterator& lhs,
                               const avl_iterator& rhs) {
            return lhs.node_ == rhs.node_;
        }

        friend bool operator!=(const avl_iterator& lhs,
                               const avl_iterator& rhs) {
            return !(lhs == rhs);
        }

        friend bool operator<(const avl_iterator& lhs,
                              const avl_iterator& rhs) {
            if (lhs && rhs) {
                return *lhs < *rhs;
            } else {
                return !lhs ? false : true;
            }
        }

        friend bool operator>(const avl_iterator& lhs,
                              const avl_iterator& rhs) {
            if (lhs && rhs) {
                return *lhs > *rhs;
            } else {
                return !rhs ? false : true;
            }
        }

        friend bool operator>=(const avl_iterator& lhs,
                               const avl_iterator& rhs) {
            return !(lhs < rhs);
        }

        friend bool operator<=(const avl_iterator& lhs,
                               const avl_iterator& rhs) {
            return !(lhs > rhs);
        }
    };

    // the root node of the tree
    Node* root_;
    // the number of nodes in this tree
    size_t size_;
    // the maximum difference between the heights of left and right children
    static const int ALLOWED_IMBALANCE = 1;
    // if the node is invalid, it's height would be -1
    static long long height_(NdPtr node) {
        return !node ? -1 : node->height_;
    }

    static long long calc_height_(NdPtr node) {
        return max(height_(node->left_), height_(node->right_)) + 1;
    }

    // to update and balance the tree
    void update_(NdPtr node);

    // perform single rotation
    void rotate_left_(NdPtr node);

    // perform single rotation
    void rotate_right_(NdPtr node);

    // perform double rotation
    void double_rotate_left_(NdPtr node) {
        rotate_right(node->left_);
        rotate_left(node);
    }

    // perform double rotation
    void double_rotate_right_(NdPtr node) {
        rotate_left(node->right_);
        rotate_right(node);
    }

    // find the node containing the minimum in the tree with node as root
    static NdPtr find_min_(NdPtr node);
    // find the node containing the maximum int the tree with node as root
    static NdPtr find_max_(NdPtr node);

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
     * so it doesn't support operator++ and operator-- */
    iterator end() {
        return iterator(nullptr);
    }

    // return an iterator containing a the minimum
    const_iterator begin() const {
        return find_min();
    }

    /* return an iterator containing a nullptr,
     * so it doesn't support operator++ and operator-- */
    const_iterator end() const {
        return iterator(nullptr);
    }

    // return an const_iterator containing a the minimum
    const_iterator cbegin() const {
        return const_iterator(nullptr);
    }

    /* return an const_iterator containing a nullptr,
     * so it doesn't support operator++ and operator-- */
    const_iterator cend() const {
        return find_max();
    }

    // return iterator to the inserted node
    template <typename V>
    iterator insert(V&& elem) noexcept;

    // return iterator to the last node of erased node
    size_t remove(const T& elem) noexcept;

    iterator remove(iterator itr) noexcept;

    // return whether the elem is in the tree
    bool contain(const T& elem) const;

    // return iterator to the node containing element
    const_iterator find(const T& elem) const;

    // return iterator to the node containing element
    iterator find(const T& elem) {
        return const_cast<const avl_tree<T>*>(this)->find(elem);
    }

    // return iterator to the minimum element
    const_iterator find_min() const {
        return const_iterator(find_min_(root_));
    }

    // return iterator to the minimum element
    const_iterator find_max() const {
        return const_iterator(find_max_(root_));
    }

    // find the minimum element
    iterator find_min() {
        return iterator(find_min_(root_));
    }

    // find the maximum element
    iterator find_max() {
        return iterator(find_max_(root_));
    }
};

template <typename T>
avl_tree<T>::Node::~Node() noexcept {
    delete left_;
    delete right_;
}

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
typename avl_tree<T>::NdPtr avl_tree<T>::find_max_(NdPtr node) {
    if (!node) {
        return node;
    }

    while (node->has_right()) {
        node = node->right_;
    }
    return node;
}

template <typename T>
typename avl_tree<T>::NdPtr avl_tree<T>::find_min_(NdPtr node) {
    if (!node) {
        return node;
    }

    while (node->has_left()) {
        node = node->left_;
    }
    return node;
}

template <typename T>
void avl_tree<T>::rotate_left_(NdPtr node) {
    NdPtr lf, lf_rg, par;
    lf = node->left_;
    lf_rg = lf->right_;
    par = node->parent_;

    if (node->is_left()) {
        par->left_ = lf;
    } else if (node->is_right()) {
        par->right_ = lf;
    } else {
        root_ = lf;
    }

    lf->parent_ = par;
    lf->right_ = node;

    if (lf_rg) {
        lf_rg->parent_ = node;
    }
    node->parent_ = lf;
    node->left_ = lf_rg;

    node->height_ = calc_height_(node);
    lf->height_ = calc_height_(lf);
}

template <typename T>
void avl_tree<T>::rotate_right_(NdPtr node) {
    NdPtr rg, rg_lf, par;
    rg = node->right_;
    rg_lf = rg->left_;
    par = node->parent_;

    if (node->is_left()) {
        par->left_ = rg;
    } else if (node->is_left()) {
        par->right_ = rg;
    } else {
        root_ = rg;
    }

    rg->parent_ = par;
    rg->left_ = node;

    if (rg_lf) {
        rg_lf->parent_ = node;
    }
    node->parent_ = rg;
    node->right_ = rg_lf;

    node->height_ = calc_height_(node);
    rg->height_ = calc_height_(rg);
}

template <typename T>
void avl_tree<T>::update_(NdPtr node) {
    while (true) {
        if (abs(height_(node->left_) - height_(node->right_)) >
            ALLOWED_IMBALANCE) {
            if (height_(node->left_) > height_(node->right_)) {
                if (height_(node->left_->left_) >
                    height_(node->left_->right_)) {
                    rotate_left_(node);
                } else {
                    double_rotate_left_(node);
                }
            } else {
                if (height_(node->right_->right_) >
                    height_(node->right_->left_)) {
                    rotate_right_(node);
                } else {
                    double_rotate_right_(node);
                }
            }
            break;
        } else {
            node->height_ = calc_height_(node);
            if (node->parent_) {
                node = node->parent_;
            } else {
                break;
            }
        }
    }
}

template <typename T>
template <typename V>
typename avl_tree<T>::iterator avl_tree<T>::insert(V&& elem) noexcept {
    if (!root_) {
        root_ = NdPtr(new Node(std::forward<V>(elem), nullptr, nullptr));
        return iterator(root_);
    }

    NdPtr node = root_;
    NdPtr res;
    while (true) {
        if (elem > node->element()) {
            if (node->has_right()) {
                node = node->right_;
            } else {
                res = NdPtr(new Node(std::forward<V>(elem), nullptr, nullptr));
                node->right_ = res;
                break;
            }
        } else if (elem < node->element()) {
            if (node->has_left()) {
                node = node->left_;
            } else {
                res = NdPtr(new Node(std::forward<V>(elem), nullptr, nullptr));
                node->left_ = res;
                break;
            }
        } else {
            return iterator(node);
        }
    }
    update_(res);
    return iterator(res);
}

template <typename T>
size_t avl_tree<T>::remove(const T& elem) noexcept {
    if (!root_) {
        return 0;
    }

    auto node = root_;
    while (true) {
        if (elem > node->element()) {
            if (node->has_right()) {
                node = node->right_;
            } else {
                return 0;
            }
        } else if (elem < node->element()) {
            if (node->has_left()) {
                node = node->left_;
            } else {
                return 0;
            }
        } else if (node->has_right() && node->has_left()) {
            auto sub = find_min_(node->right_);
            node->element() = std::move(sub->element());
            if (sub->has_right()) {
                sub->right_->parent_ = sub->parent_;
                sub->parent_->left_ = sub->right_;
                sub->right_ = nullptr;
            } else {
                sub->parent_->left_ = nullptr;
            }
            update_(sub->parent_);
            delete sub;
        } else {
            if (node->has_left()) {
                node->left_->parent_ = node->parent_;
                node->parent_->left_ = node->left_;
                node->left_ = nullptr;
            } else if (node->has_right()) {
                node->right_->parent_ = node->parent_;
                node->parent_->right_ = node->right_;
                node->right_ = nullptr;
            }
            update_(node->parent_);
            delete node;
        }
    }
}

template <typename T>
typename avl_tree<T>::iterator avl_tree<T>::remove(iterator itr) noexcept {
    auto res = itr++;
    remove(res->element());
    return itr;
}

template <typename T>
typename avl_tree<T>::const_iterator avl_tree<T>::find(const T& elem) const {
    if (!root_) {
        return const_iterator();
    }
}

template <typename T>
template <typename Ref, typename Ptr>
avl_tree<T>::avl_iterator<Ref, Ptr>::self_t&
avl_tree<T>::avl_iterator<Ref, Ptr>::operator+=(difference_t n) {
    if (n > 0) {
        for (difference_t i = 0; i < n; i++) {
            this->operator++();
        }
    } else {
        n = -n;
        for (difference_t i = 0; i < n; i++) {
            this->operator--();
        }
    }
    return *this;
}

template <typename T>
template <typename Ref, typename Ptr>
avl_tree<T>::avl_iterator<Ref, Ptr>::self_t&
avl_tree<T>::avl_iterator<Ref, Ptr>::operator++() {}
} // namespace mtl

#endif
