#ifndef MTL_AVL_TREE_H
#define MTL_AVL_TREE_H

#include <mtl/algorithms.h>
#include <mtl/mtldefs.h>
#include <memory>

namespace mtl {
template <typename T, typename Alloc = std::allocator<T>>
class avl_tree {
public:
    using self_t = avl_tree<T, Alloc>;

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

    using NodeAlloc =
        typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;

    NodeAlloc allocator_;

    // The root node of the tree
    NdPtr root_;
    // The number of nodes in this tree
    size_t size_ {};
    // The maximum difference between the heights of left and right children
    static const int ALLOWED_IMBALANCE = 1;
    // If the node is invalid, it's height would be -1
    static long long height_(NdPtr node) {
        return (node == nullptr) ? -1 : node->height_;
    }

    static long long calc_height_(NdPtr node) {
        return max(height_(node->left_), height_(node->right_)) + 1;
    }

    template <typename... Args>
    NdPtr allocate_node(Args&&... args) {
        NdPtr ptr = allocator_.allocate(1);
        std::construct_at(ptr, std::forward<Args>(args)...);
        return ptr;
    }

    void deallocate_node(NdPtr node) {
        if (node == nullptr) {
            return;
        }

        if (node->has_left()) {
            deallocate_node(node->left_);
        }
        if (node->has_right()) {
            deallocate_node(node->right_);
        }
        std::destroy_at(node);
        allocator_.deallocate(node, 1);
    }

    // To update and balance the tree
    void update(NdPtr node);

    // To remove the node
    void remove_(NdPtr node);

    /* To find the node whose element_ is equal to elem.
     * If the node doesn't exist, return the last node in the path. */
    NdPtr find_(const T& elem) const;

    // perform single rotation
    void rotate_left(NdPtr node);

    // perform single rotation
    void rotate_right(NdPtr node);

    // perform double rotation
    void double_rotate_left(NdPtr node) {
        rotate_right(node->left_);
        rotate_left(node);
    }

    // perform double rotation
    void double_rotate_right_(NdPtr node) {
        rotate_left(node->right_);
        rotate_right(node);
    }

    // find the node containing the minimum in the tree with node as root
    static NdPtr find_min(NdPtr node);
    // find the node containing the maximum int the tree with node as root
    static NdPtr find_max(NdPtr node);
    // Copy the nodes recursively.
    NdPtr copy_node(NdPtr node);

public:
    avl_tree();
    avl_tree(const self_t& rhs);
    avl_tree(self_t&& rhs) noexcept;
    ~avl_tree() noexcept;

    self_t& operator=(const self_t& rhs);
    self_t& operator=(self_t&& rhs) noexcept;

    [[nodiscard]] bool empty() const {
        return size_ == 0;
    }

    // clear all the items.
    void clear() {
        size_ = 0;
        deallocate_node(root_);
        root_ = nullptr;
    }

    // return the number of items.
    [[nodiscard]] size_t size() const {
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
        return const_iterator(nullptr);
    }

    // return an const_iterator containing a the minimum
    const_iterator cbegin() const {
        return begin();
    }

    /* return an const_iterator containing a nullptr,
     * so it doesn't support operator++ and operator-- */
    const_iterator cend() const {
        return end();
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
        return const_cast<const self_t*>(this)->find(elem);
    }

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

template <typename T, typename Alloc>
avl_tree<T, Alloc>::avl_tree() : root_(nullptr) {}

template <typename T, typename Alloc>
avl_tree<T, Alloc>::avl_tree(const self_t& rhs)
    : root_(copy_node(rhs.root_)), size_(rhs.size_) {}

template <typename T, typename Alloc>
avl_tree<T, Alloc>::self_t& avl_tree<T, Alloc>::operator=(const self_t& rhs) {
    if (&rhs == this) {
        return *this;
    }
    deallocate_node(root_);
    root_ = copy_node(rhs.root_);
    size_ = rhs.size_;
}

template <typename T, typename Alloc>
avl_tree<T, Alloc>::self_t&
avl_tree<T, Alloc>::operator=(self_t&& rhs) noexcept {
    if (&rhs == this) {
        return *this;
    }
    deallocate_node(root_);
    root_ = rhs.root_;
    rhs.root_ = nullptr;
    size_ = rhs.size_;
    rhs.size_ = 0;
}

template <typename T, typename Alloc>
avl_tree<T, Alloc>::avl_tree(self_t&& rhs) noexcept
    : root_(rhs.root_), size_(rhs.size_) {
    rhs.root_ = nullptr;
    rhs.size_ = 0;
}

template <typename T, typename Alloc>
avl_tree<T, Alloc>::~avl_tree() noexcept {
    deallocate_node(root_);
}

template <typename T, typename Alloc>
bool avl_tree<T, Alloc>::contain(const T& elem) const {
    if (this->empty()) {
        return false;
    }
    auto tmp = root_;
    while (true) {
        if (elem > tmp->element) {
            if (tmp->has_right()) {
                tmp = tmp->right;
            } else {
                return false;
            }
        } else if (elem < tmp->element) {
            if (tmp->has_left()) {
                tmp = tmp->left;
            } else {
                return false;
            }
        } else {
            return true;
        }
    }
}

template <typename T, typename Alloc>
typename avl_tree<T, Alloc>::NdPtr avl_tree<T, Alloc>::copy_node(NdPtr node) {
    if (node == nullptr) {
        return nullptr;
    }
    NdPtr res = allocate_node(node->element(), nullptr, nullptr, nullptr);
    NdPtr lf_tree = copy_node(node->left());
    NdPtr rg_tree = copy_node(node->right());
    if (lf_tree) {
        lf_tree->parent_ = res;
    }
    if (rg_tree) {
        rg_tree->parent_ = res;
    }
    res->left_ = lf_tree;
    res->right_ = rg_tree;

    return res;
}

template <typename T, typename Alloc>
typename avl_tree<T, Alloc>::NdPtr avl_tree<T, Alloc>::find_max(NdPtr node) {
    if (node == nullptr) {
        return node;
    }

    while (node->has_right()) {
        node = node->right_;
    }
    return node;
}

template <typename T, typename Alloc>
typename avl_tree<T, Alloc>::NdPtr avl_tree<T, Alloc>::find_min(NdPtr node) {
    if (node == nullptr) {
        return node;
    }

    while (node->has_left()) {
        node = node->left_;
    }
    return node;
}

template <typename T, typename Alloc>
void avl_tree<T, Alloc>::rotate_left(NdPtr node) {
    NdPtr left = node->left_;
    NdPtr left_rg = left->right_;
    NdPtr par = node->parent_;

    if (node->is_left()) {
        par->left_ = left;
    } else if (node->is_right()) {
        par->right_ = left;
    } else {
        root_ = left;
    }

    left->parent_ = par;
    left->right_ = node;

    if (left_rg) {
        left_rg->parent_ = node;
    }
    node->parent_ = left;
    node->left_ = left_rg;

    node->height_ = calc_height_(node);
    left->height_ = calc_height_(left);
}

template <typename T, typename Alloc>
void avl_tree<T, Alloc>::rotate_right(NdPtr node) {
    NdPtr right = node->right_;
    NdPtr right_lf = right->left_;
    NdPtr par = node->parent_;

    if (node->is_left()) {
        par->left_ = right;
    } else if (node->is_right()) {
        par->right_ = right;
    } else {
        root_ = right;
    }

    right->parent_ = par;
    right->left_ = node;

    if (right_lf) {
        right_lf->parent_ = node;
    }
    node->parent_ = right;
    node->right_ = right_lf;

    node->height_ = calc_height_(node);
    right->height_ = calc_height_(right);
}

template <typename T, typename Alloc>
void avl_tree<T, Alloc>::update(NdPtr node) {
    while (true) {
        if (abs(height_(node->left_) - height_(node->right_)) >
            ALLOWED_IMBALANCE) {
            if (height_(node->left_) > height_(node->right_)) {
                if (height_(node->left_->left_) >
                    height_(node->left_->right_)) {
                    rotate_left(node);
                } else {
                    double_rotate_left(node);
                }
            } else {
                if (height_(node->right_->right_) >
                    height_(node->right_->left_)) {
                    rotate_right(node);
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

template <typename T, typename Alloc>
template <typename V>
typename avl_tree<T, Alloc>::iterator
avl_tree<T, Alloc>::insert(V&& elem) noexcept {
    if (!root_) {
        root_ = allocate_node(std::forward<V>(elem), nullptr, nullptr, nullptr);
        ++size_;
        return iterator(root_);
    }

    NdPtr node = find_(elem);
    NdPtr res = nullptr;

    if (elem == node->element()) {
        return iterator(node);
    } else {
        res = allocate_node(std::forward<V>(elem), node, nullptr, nullptr);
        if (elem > node->element()) {
            node->right_ = res;
        } else {
            node->left_ = res;
        }
        update(node);
        ++size_;
        return iterator(res);
    }
}

template <typename T, typename Alloc>
void avl_tree<T, Alloc>::remove_(NdPtr node) {
    if (node == nullptr) {
        return;
    }
    if (node->has_right() && node->has_left()) {
        auto sub = find_min(node->right_);
        node->element() = std::move(sub->element());
        if (sub->has_right()) {
            sub->right_->parent_ = sub->parent_;
            sub->parent_->left_ = sub->right_;
            sub->right_ = nullptr;
        } else {
            sub->parent_->left_ = nullptr;
        }
        update(sub->parent_);
        deallocate_node(sub);
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
            update(node->left_);
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
            update(node->right_);
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
        deallocate_node(node);
    }
}

template <typename T, typename Alloc>
size_t avl_tree<T, Alloc>::remove(const T& elem) noexcept {
    if (root_ == nullptr) {
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

template <typename T, typename Alloc>
typename avl_tree<T, Alloc>::iterator
avl_tree<T, Alloc>::remove(iterator itr) noexcept {
    auto res = itr++;
    remove(res->node_);
    --size_;
    return itr;
}

template <typename T, typename Alloc>
typename avl_tree<T, Alloc>::NdPtr
avl_tree<T, Alloc>::find_(const T& elem) const {
    if (root_ == nullptr) {
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

template <typename T, typename Alloc>
typename avl_tree<T, Alloc>::const_iterator
avl_tree<T, Alloc>::find(const T& elem) const {
    NdPtr res = nullptr;
    res = find_(elem);
    if (res->element() == elem) {
        return const_iterator(res);
    } else {
        return const_iterator();
    }
}

template <typename T, typename Alloc>
class avl_tree<T, Alloc>::Node {
private:
    T element_;
    // its parent node
    NdPtr parent_;
    // its left child
    NdPtr left_;
    // its right child
    NdPtr right_;
    // its height
    long long height_ {0};

public:
    template <typename V>
    Node(V&& elem, NdPtr par, NdPtr lf_child, NdPtr rg_child) noexcept
        : element_(std::forward<V>(elem)),
          parent_(par),
          left_(lf_child),
          right_(rg_child) {}

    Node(const Node& rhs) = delete;
    Node(Node&& rhs) noexcept = delete;
    Node& operator=(const Node& rhs) = delete;
    Node& operator=(Node&& rhs) noexcept = delete;

    ~Node() noexcept = default;

    [[nodiscard]] long long height() const {
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
    [[nodiscard]] bool is_left() const {
        if (parent_ == nullptr) {
            return false;
        }
        return parent_->left_ == this;
    }

    // return if this node is a right child
    [[nodiscard]] bool is_right() const {
        if (parent_ == nullptr) {
            return false;
        }
        return parent_->right_ == this;
    }

    // return if this node is a left child
    [[nodiscard]] bool is_root() const {
        return parent_ == nullptr;
    }

    // return if this node has a left child
    [[nodiscard]] bool has_left() const {
        return bool(left_);
    }

    // return is this node has a right child
    [[nodiscard]] bool has_right() const {
        return bool(right_);
    }

    friend class avl_tree<T, Alloc>;
    template <typename Ref, typename Ptr>
    friend class avl_tree<T, Alloc>::avl_iterator;
};

template <typename T, typename Alloc>
template <typename Ref, typename Ptr>
class avl_tree<T, Alloc>::avl_iterator {
private:
    // the pointer to the node
    NdPtr node_;
    friend const_iterator;

public:
    using self_t = avl_iterator<Ref, Ptr>;

    avl_iterator() : node_(nullptr) {}
    explicit avl_iterator(NdPtr node) : node_(node) {}
    avl_iterator(const self_t& rhs) : node_(rhs.node_) {}
    avl_iterator(self_t&& rhs) noexcept : node_(rhs.node_) {}
    ~avl_iterator() noexcept = default;

    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    avl_iterator(const Iter& rhs) : node_(rhs.node_) {}

    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    avl_iterator(Iter&& rhs) noexcept : node_(rhs.node_) {}

    /* it don't check whether the iterator is valid
     * so a segmentation fault is possible to be thrown out */
    Ref operator*() const {
        return node_->element();
    }

    Ptr operator->() const {
        return &node_->element();
    }

    // checkk whether the iterator refers to a valid node
    explicit operator bool() const {
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

    self_t& operator=(const self_t& rhs) = default;
    self_t& operator=(self_t&& rhs) = default;

    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    self_t& operator=(const Iter& rhs) {
        node_ = rhs.node_;
        return *this;
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

    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator==(const self_t& lhs, const Iter& rhs) {
        if (lhs && rhs) {
            return *lhs == *rhs;
        } else if (lhs || rhs) {
            return false;
        } else {
            return true;
        }
    }

    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator!=(const self_t& lhs, const Iter& rhs) {
        return !(lhs == rhs);
    }

    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator<(const self_t& lhs, const Iter& rhs) {
        if (lhs && rhs) {
            return *lhs < *rhs;
        } else {
            return static_cast<bool>(lhs);
        }
    }

    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator>(const self_t& lhs, const Iter& rhs) {
        if (lhs && rhs) {
            return *lhs > *rhs;
        } else {
            return static_cast<bool>(rhs);
        }
    }

    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator>=(const self_t& lhs, const Iter& rhs) {
        return !(lhs < rhs);
    }

    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator<=(const self_t& lhs, const Iter& rhs) {
        return !(lhs > rhs);
    }
};

template <typename T, typename Alloc>
template <typename Ref, typename Ptr>
typename avl_tree<T, Alloc>::template avl_iterator<Ref, Ptr>::self_t&
avl_tree<T, Alloc>::avl_iterator<Ref, Ptr>::operator++() {
    if (node_ == nullptr) {
        throw NullIterator();
    }
    if (node_->has_right()) {  // node_ has right child
        node_ = find_min(node_->right_);
    } else {  // node_ doesn't have right child
        NdPtr tmp = node_->parent_;
        while (tmp && node_->is_right()) {
            node_ = tmp;
            tmp = node_->parent_;
        }
        node_ = tmp;
    }
    return *this;
}

template <typename T, typename Alloc>
template <typename Ref, typename Ptr>
typename avl_tree<T, Alloc>::template avl_iterator<Ref, Ptr>::self_t&
avl_tree<T, Alloc>::avl_iterator<Ref, Ptr>::operator--() {
    if (node_ == nullptr) {
        throw NullIterator();
    }
    if (node_->has_left()) {
        node_ = find_max(node_->left_);
    } else {
        NdPtr tmp = node_->parent_;
        while (tmp && node_->is_left()) {
            node_ = tmp;
            tmp = node_->parent_;
        }
        node_ = tmp;
    }
    return *this;
}
}  // namespace mtl

#endif
