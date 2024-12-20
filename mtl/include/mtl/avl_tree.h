#ifndef MTL_AVL_TREE_H
#define MTL_AVL_TREE_H

#include <mtl/algorithms.h>
#include <mtl/types.h>
#include <type_traits>

namespace mtl {
template <typename T>
class avl_tree {
private:
    // The iterator class
    template <typename Ref, typename Ptr>
    class avl_iterator;

public:
    using const_iterator = avl_iterator<const T&, const T*>;
    using iterator = avl_iterator<T&, T*>;

private:
    // The node class
    class Node;
    // the pointer to Node
    using NdPtr = Node*;

    // The root node of the tree
    Node* root_;
    // The number of nodes in this tree
    size_t size_;
    // The maximum difference between the heights of left and right children
    static const int ALLOWED_IMBALANCE = 1;
    // If the node is invalid, it's height would be -1
    static long long height_(NdPtr node) {
        return !node ? -1 : node->height_;
    }

    static long long calc_height_(NdPtr node) {
        return max(height_(node->left_), height_(node->right_)) + 1;
    }

    // To update and balance the tree
    void update_(NdPtr node);

    // To remove the node
    void remove_(NdPtr node);

    /* To find the node whose element_ is equal to elem.
     * If the node doesn't exist, return the last node in the path. */
    NdPtr find_(const T& elem) const;

    // perform single rotation
    void rotate_left_(NdPtr node);

    // perform single rotation
    void rotate_right_(NdPtr node);

    // perform double rotation
    void double_rotate_left_(NdPtr node) {
        rotate_right_(node->left_);
        rotate_left_(node);
    }

    // perform double rotation
    void double_rotate_right_(NdPtr node) {
        rotate_left_(node->right_);
        rotate_right_(node);
    }

    // find the node containing the minimum in the tree with node as root
    static NdPtr find_min_(NdPtr node);
    // find the node containing the maximum int the tree with node as root
    static NdPtr find_max_(NdPtr node);
    // Copy the nodes recursively.
    static NdPtr copy_node_(NdPtr node);

public:
    avl_tree();
    avl_tree(const avl_tree& rhs);
    avl_tree(avl_tree&& rhs) noexcept;
    ~avl_tree() noexcept;

    bool empty() const {
        return size_ == 0;
    }

    // clear all the items.
    void clear() {
        size_ = 0;
        delete root_;
    }

    // return the number of items.
    size_t size() const {
        return size_;
    }

    // return an iterator containing the minimum
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
avl_tree<T>::avl_tree() : root_(nullptr), size_(0) {}

template <typename T>
avl_tree<T>::avl_tree(const avl_tree& rhs) : size_(rhs.size_) {
    root_ = copy_node_(rhs.root_);
}

template <typename T>
avl_tree<T>::avl_tree(avl_tree&& rhs) noexcept
    : root_(rhs.root_), size_(rhs.size_) {
    rhs.root_ = nullptr;
    rhs.size_ = 0;
}

template <typename T>
avl_tree<T>::~avl_tree() noexcept {
    delete root_;
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
typename avl_tree<T>::NdPtr avl_tree<T>::copy_node_(NdPtr node) {
    if (!node) {
        return nullptr;
    }
    NdPtr res = new Node(node->element(), nullptr, nullptr, nullptr);
    NdPtr lt = copy_node_(node->left());
    NdPtr rt = copy_node(node->right());
    if (lt) {
        lt->parent_ = res;
    }
    if (rt) {
        rt->parent_ = res;
    }
    res->left_ = lt;
    res->right_ = rt;
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
    } else if (node->is_right()) {
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
        root_ = new Node(std::forward<V>(elem), nullptr, nullptr, nullptr);
        ++size_;
        return iterator(root_);
    }

    NdPtr node = find_(elem);
    NdPtr res;

    if (elem == node->element()) {
        return iterator(node);
    } else {
        res = new Node(std::forward<V>(elem), node, nullptr, nullptr);
        if (elem > node->element()) {
            node->right_ = res;
        } else {
            node->left_ = res;
        }
        update_(node);
        ++size_;
        return iterator(res);
    }
}

template <typename T>
void avl_tree<T>::remove_(NdPtr node) {
    if (!node) {
        return;
    }
    if (node->has_right() && node->has_left()) {
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
            if (node->is_root()) {
                root_ = node->left_;
            } else if (node->is_left()) {
                node->parent_->left_ = node->left_;
            } else {
                node->parent_->right_ = node->left_;
            }
            update_(node->left_);
            node->left_ = nullptr;
        } else if (node->has_right()) {
            node->right_->parent_ = node->parent_;
            if (node->is_root()) {
                root_ = node->right_;
            } else if (node->is_left()) {
                node->parent_->left_ = node->right_;
            } else {
                node->parent_->right_ = node->right_;
            }
            update_(node->right_);
            node->right_ = nullptr;
        } else {
            if (node->is_root()) {
                root_ = nullptr;
            } else if (node->is_left()) {
                node->parent_->left_ = nullptr;
            } else {
                node->parent_->right_ = nullptr;
            }
        }
        delete node;
    }
}

template <typename T>
size_t avl_tree<T>::remove(const T& elem) noexcept {
    if (!root_) {
        return 0;
    }

    NdPtr node = find_(elem);
    if (node->element() == elem) {
        remove_(node);
        --size_;
        return 1;
    } else {
        return 0;
    }
}

template <typename T>
typename avl_tree<T>::iterator avl_tree<T>::remove(iterator itr) noexcept {
    auto res = itr++;
    remove(res->node_);
    --size_;
    return itr;
}

template <typename T>
typename avl_tree<T>::NdPtr avl_tree<T>::find_(const T& elem) const {
    if (!root_) {
        return nullptr;
    }

    NdPtr node = root_;
    while (true) {
        if (elem > node->element() && node->has_right()) {
            node = node->right_;
        } else if (elem < node->element() && node->has_left()) {
            node = node->left_;
        } else {
            break;
        }
    }
    return node;
}

template <typename T>
typename avl_tree<T>::const_iterator avl_tree<T>::find(const T& elem) const {
    NdPtr res;
    res = find_(elem);
    if (res->element() == elem) {
        return const_iterator(res);
    } else {
        return const_iterator();
    }
}

template <typename T>
class avl_tree<T>::Node {
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
    template <typename V>
    Node(V&& elem, NdPtr par, NdPtr lt, NdPtr rt) noexcept;
    /* when an instance is destructed, it will automatically destruct its
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
    template <typename Ref, typename Ptr>
    friend class avl_tree<T>::avl_iterator;
};

template <typename T>
template <typename V>
avl_tree<T>::Node::Node(V&& elem, NdPtr par, NdPtr lt, NdPtr rt) noexcept
    : element_(std::forward<V>(elem)),
      parent_(par),
      left_(lt),
      right_(rt),
      height_(0) {}

template <typename T>
avl_tree<T>::Node::~Node() noexcept {
    delete left_;
    delete right_;
}

template <typename T>
template <typename Ref, typename Ptr>
class avl_tree<T>::avl_iterator {
private:
    // the pointer to the node
    NdPtr node_;
    using self_t = avl_iterator<Ref, Ptr>;
    template <typename Ref2, typename Ptr2>
    friend class avl_iterator;

public:
    avl_iterator();
    explicit avl_iterator(NdPtr node);
    avl_iterator(const self_t& rhs);

    template <typename Iter,
              typename = std::_Require<std::is_same<self_t, const_iterator>,
                                       std::is_same<Iter, iterator>>>
    avl_iterator(const Iter& rhs) : node_(rhs.node_) {}

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

    friend bool operator==(const avl_iterator& lhs, const avl_iterator& rhs) {
        return lhs.node_ == rhs.node_;
    }

    friend bool operator!=(const avl_iterator& lhs, const avl_iterator& rhs) {
        return !(lhs == rhs);
    }

    friend bool operator<(const avl_iterator& lhs, const avl_iterator& rhs) {
        if (lhs && rhs) {
            return *lhs < *rhs;
        } else {
            return !lhs ? false : true;
        }
    }

    friend bool operator>(const avl_iterator& lhs, const avl_iterator& rhs) {
        if (lhs && rhs) {
            return *lhs > *rhs;
        } else {
            return !rhs ? false : true;
        }
    }

    friend bool operator>=(const avl_iterator& lhs, const avl_iterator& rhs) {
        return !(lhs < rhs);
    }

    friend bool operator<=(const avl_iterator& lhs, const avl_iterator& rhs) {
        return !(lhs > rhs);
    }
};

template <typename T>
template <typename Ref, typename Ptr>
avl_tree<T>::avl_iterator<Ref, Ptr>::avl_iterator(NdPtr node) : node_(node) {}

template <typename T>
template <typename Ref, typename Ptr>
avl_tree<T>::avl_iterator<Ref, Ptr>::avl_iterator(const avl_iterator& rhs)
    : node_(rhs.node_) {}

template <typename T>
template <typename Ref, typename Ptr>
avl_tree<T>::avl_iterator<Ref, Ptr>::self_t&
avl_tree<T>::avl_iterator<Ref, Ptr>::operator++() {
    if (!node_) {
        throw NullIterator();
    }
    if (node_->has_right()) { // node_ has right child
        node_ = find_min_(node_->right_);
    } else { // node_ doesn't have right child
        NdPtr p = node_->parent_;
        while (p && node_->is_right()) {
            node_ = p;
            p = node_->parent_;
        }
        node_ = p;
    }
    return *this;
}

template <typename T>
template <typename Ref, typename Ptr>
avl_tree<T>::avl_iterator<Ref, Ptr>::self_t&
avl_tree<T>::avl_iterator<Ref, Ptr>::operator--() {
    if (!node_) {
        throw NullIterator();
    }
    if (node_->has_left()) {
        node_ = find_max_(node_->left_);
    } else {
        NdPtr p = node_->parent_;
        while (p && node_->is_left()) {
            node_ = p;
            p = node_->parent_;
        }
        node_ = p;
    }
    return *this;
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

} // namespace mtl

#endif
