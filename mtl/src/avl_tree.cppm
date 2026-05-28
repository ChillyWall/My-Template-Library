export module mtl.avl_tree;

export import mtl.core;
import std;

namespace mtl {
using std::int64_t;
}

export namespace mtl {

/**
 * @brief Self-balancing binary search tree (AVL) container.
 *
 * @tparam T       Type of elements stored in the tree.
 * @tparam Compare Comparator type (strict weak ordering). Defaults to std::less<T>.
 *                 Must be default-constructible and stateless is recommended.
 * @tparam Alloc   Allocator type used to allocate nodes.
 */
template <typename T, typename Compare = std::less<T>,
          typename Alloc = std::allocator<T>>
class avl_tree {
public:
    using self_t = avl_tree<T, Compare, Alloc>;

private:
    // The iterator class
    /**
     * @brief Iterator implementation for avl_tree.
     *
     * @tparam Ref Reference type returned by operator*.
     * @tparam Ptr Pointer type returned by operator->.
     */
    template <typename Ref, typename Ptr>
    class avl_iterator;

public:
    using const_iterator = avl_iterator<const T&, const T*>;
    using iterator = avl_iterator<T&, T*>;

private:
    /**
     * @brief Internal node type for avl_tree.
     */
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
    Compare compare_;

    /**
     * @brief Return the height of a node or -1 for null.
     *
     * @param node Pointer to node.
     * @return Height or -1 if node is null.
     */
    static int64_t height(NdPtr node) {
        return (node == nullptr) ? -1 : node->height_;
    }

    /**
     * @brief Compute node height from child heights.
     * @param node Pointer to node.
     * @return Computed height value.
     */
    static int64_t calc_height(NdPtr node) {
        return max(height(node->left_), height(node->right_)) + 1;
    }

    /**
     * @brief Allocate and construct a node with forwarded arguments.
     * @tparam Args Constructor argument types.
     * @param args Forwarded constructor arguments.
     * @return Pointer to constructed node.
     */
    template <typename... Args>
    NdPtr allocate_node(Args&&... args) {
        NdPtr ptr = allocator_.allocate(1);
        std::construct_at(ptr, std::forward<Args>(args)...);
        return ptr;
    }

    /**
     * @brief Recursively destroy and deallocate a subtree rooted at node.
     * @param node Root node to deallocate.
     */
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

    /**
     * @brief Update heights and rebalance the tree starting from node.
     *
     * @param node Node to start update from
     */
    void update(NdPtr node);

    /**
     * @brief Remove a node from the tree and adjust links.
     *
     * @param node Node to remove
     */
    void remove_node(NdPtr node);

    /**
     * @brief Find node with element equal to elem or last visited node.
     *
     * @param elem Element to search for
     * @return Found node or last node where search ended
     */
    NdPtr find_node(const T& elem) const;

    /**
     * @brief Perform single left rotation on given node.
     *
     * @param node Node to rotate
     */
    void rotate_left(NdPtr node);

    /**
     * @brief Perform single right rotation on given node.
     *
     * @param node Node to rotate
     */
    void rotate_right(NdPtr node);

    /**
     * @brief Perform double left rotation on given node.
     * @param node Node to rotate.
     */
    void double_rotate_left(NdPtr node) {
        rotate_right(node->left_);
        rotate_left(node);
    }

    /**
     * @brief Perform double right rotation on given node.
     * @param node Node to rotate.
     */
    void double_rotate_right(NdPtr node) {
        rotate_left(node->right_);
        rotate_right(node);
    }

    /**
     * @brief Find the minimum node in the subtree rooted at node.
     * @param node Subtree root.
     * @return Pointer to minimum node or nullptr.
     */
    static NdPtr find_min(NdPtr node);
    /**
     * @brief Find the maximum node in the subtree rooted at node.
     * @param node Subtree root.
     * @return Pointer to maximum node or nullptr.
     */
    static NdPtr find_max(NdPtr node);
    /**
     * @brief Recursively copy a subtree rooted at node.
     * @param node Subtree root.
     * @return Pointer to copied subtree root or nullptr.
     */
    NdPtr copy_node(NdPtr node);

public:
    /**
     * @brief Construct an empty AVL tree.
     */
    avl_tree();
    /**
     * @brief Copy-construct from another tree.
     * @param rhs Source tree.
     */
    avl_tree(const self_t& rhs);
    /**
     * @brief Move-construct from another tree.
     * @param rhs Source tree.
     */
    avl_tree(self_t&& rhs) noexcept;
    /**
     * @brief Destroy the tree and deallocate all nodes.
     */
    ~avl_tree() noexcept;

    /**
     * @brief Copy-assign from another tree.
     * @param rhs Source tree.
     * @return Reference to this tree.
     */
    self_t& operator=(const self_t& rhs);
    /**
     * @brief Move-assign from another tree.
     * @param rhs Source tree.
     * @return Reference to this tree.
     */
    self_t& operator=(self_t&& rhs) noexcept;

    /**
     * @brief Check whether the tree is empty.
     * @return True if empty.
     */
    [[nodiscard]] bool empty() const {
        return size_ == 0;
    }

    /**
     * @brief Remove all elements from the tree.
     */
    void clear() {
        size_ = 0;
        deallocate_node(root_);
        root_ = nullptr;
    }

    /**
     * @brief Get number of elements in the tree.
     * @return Element count.
     */
    [[nodiscard]] size_t size() const {
        return size_;
    }

    /**
     * @brief Get iterator to the minimum element.
     * @return Iterator to minimum element.
     */
    iterator begin() {
        return find_min();
    }

    /**
     * @brief Get iterator to past-the-end.
     * @return Iterator representing end.
     */
    iterator end() {
        return iterator(nullptr);
    }

    /**
     * @brief Get const iterator to the minimum element.
     * @return Const iterator to minimum element.
     */
    const_iterator begin() const {
        return find_min();
    }

    /**
     * @brief Get const iterator to past-the-end.
     * @return Const iterator representing end.
     */
    const_iterator end() const {
        return const_iterator(nullptr);
    }

    /**
     * @brief Get const iterator to the minimum element.
     * @return Const iterator to minimum element.
     */
    const_iterator cbegin() const {
        return begin();
    }

    /**
     * @brief Get const iterator to past-the-end.
     * @return Const iterator representing end.
     */
    const_iterator cend() const {
        return end();
    }

    /**
     * @brief Insert an element into the tree.
     * @tparam V Forwarded element type.
     * @param elem Element to insert.
     * @return Iterator to inserted or existing element.
     */
    template <typename V>
    iterator insert(V&& elem) noexcept;

    /**
     * @brief Remove element equal to elem if present.
     * @param elem Element to remove.
     * @return 1 if removed, 0 otherwise.
     */
    int remove(const T& elem) noexcept;

    /**
     * @brief Remove element at iterator and return iterator to next.
     * @param itr Iterator to element to remove.
     * @return Iterator to next element.
     */
    iterator remove(iterator itr) noexcept;

    /**
     * @brief Check whether elem exists in the tree.
     * @param elem Element to search for.
     * @return True if contained.
     */
    bool contain(const T& elem) const;

    /**
     * @brief Find const iterator to element equal to elem.
     * @param elem Element to find.
     * @return Const iterator to element or end().
     */
    const_iterator find(const T& elem) const;

    /**
     * @brief Find iterator to element equal to elem.
     * @param elem Element to find.
     * @return Iterator to element or end().
     */
    iterator find(const T& elem);

    /**
     * @brief Get const iterator to the minimum element.
     * @return Const iterator to minimum element.
     */
    const_iterator find_min() const {
        return const_iterator(find_min(root_));
    }

    /**
     * @brief Get const iterator to the maximum element.
     * @return Const iterator to maximum element.
     */
    const_iterator find_max() const {
        return const_iterator(find_max(root_));
    }

    /**
     * @brief Get iterator to the minimum element.
     * @return Iterator to minimum element.
     */
    iterator find_min() {
        return iterator(find_min(root_));
    }

    /**
     * @brief Get iterator to the maximum element.
     * @return Iterator to maximum element.
     */
    iterator find_max() {
        return iterator(find_max(root_));
    }
};

template <typename T, typename Compare, typename Alloc>
avl_tree<T, Compare, Alloc>::avl_tree() : root_(nullptr) {}

template <typename T, typename Compare, typename Alloc>
avl_tree<T, Compare, Alloc>::avl_tree(const self_t& rhs)
    : root_(copy_node(rhs.root_)), size_(rhs.size_) {}

template <typename T, typename Compare, typename Alloc>
avl_tree<T, Compare, Alloc>::self_t& avl_tree<T, Compare, Alloc>::operator=(const self_t& rhs) {
    if (&rhs == this) {
        return *this;
    }
    deallocate_node(root_);
    root_ = copy_node(rhs.root_);
    size_ = rhs.size_;
    return *this;
}

template <typename T, typename Compare, typename Alloc>
avl_tree<T, Compare, Alloc>::self_t&
avl_tree<T, Compare, Alloc>::operator=(self_t&& rhs) noexcept {
    if (&rhs == this) {
        return *this;
    }
    deallocate_node(root_);
    root_ = rhs.root_;
    rhs.root_ = nullptr;
    size_ = rhs.size_;
    rhs.size_ = 0;
    return *this;
}

template <typename T, typename Compare, typename Alloc>
avl_tree<T, Compare, Alloc>::avl_tree(self_t&& rhs) noexcept
    : root_(rhs.root_), size_(rhs.size_) {
    rhs.root_ = nullptr;
    rhs.size_ = 0;
}

template <typename T, typename Compare, typename Alloc>
avl_tree<T, Compare, Alloc>::~avl_tree() noexcept {
    deallocate_node(root_);
}

template <typename T, typename Compare, typename Alloc>
bool avl_tree<T, Compare, Alloc>::contain(const T& elem) const {
    if (this->empty()) {
        return false;
    }
    NdPtr tmp = root_;
    while (true) {
        if (compare_(tmp->element(), elem)) {
            if (tmp->has_right()) {
                tmp = tmp->right();
            } else {
                return false;
            }
        } else if (compare_(elem, tmp->element())) {
            if (tmp->has_left()) {
                tmp = tmp->left();
            } else {
                return false;
            }
        } else {
            return true;
        }
    }
}

template <typename T, typename Compare, typename Alloc>
typename avl_tree<T, Compare, Alloc>::NdPtr avl_tree<T, Compare, Alloc>::copy_node(NdPtr node) {
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
    res->height_ = calc_height(res);

    return res;
}

template <typename T, typename Compare, typename Alloc>
typename avl_tree<T, Compare, Alloc>::NdPtr avl_tree<T, Compare, Alloc>::find_max(NdPtr node) {
    if (node == nullptr) {
        return node;
    }

    while (node->has_right()) {
        node = node->right_;
    }
    return node;
}

template <typename T, typename Compare, typename Alloc>
typename avl_tree<T, Compare, Alloc>::NdPtr avl_tree<T, Compare, Alloc>::find_min(NdPtr node) {
    if (node == nullptr) {
        return node;
    }

    while (node->has_left()) {
        node = node->left_;
    }
    return node;
}

template <typename T, typename Compare, typename Alloc>
void avl_tree<T, Compare, Alloc>::rotate_left(NdPtr node) {
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

    node->height_ = calc_height(node);
    left->height_ = calc_height(left);
}

template <typename T, typename Compare, typename Alloc>
void avl_tree<T, Compare, Alloc>::rotate_right(NdPtr node) {
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

    node->height_ = calc_height(node);
    right->height_ = calc_height(right);
}

template <typename T, typename Compare, typename Alloc>
void avl_tree<T, Compare, Alloc>::update(NdPtr node) {
    while (true) {
        if (abs(height(node->left_) - height(node->right_)) >
            ALLOWED_IMBALANCE) {
            if (height(node->left_) > height(node->right_)) {
                if (height(node->left_->left_) > height(node->left_->right_)) {
                    rotate_left(node);
                } else {
                    double_rotate_left(node);
                }
            } else {
                if (height(node->right_->right_) >
                    height(node->right_->left_)) {
                    rotate_right(node);
                } else {
                    double_rotate_right(node);
                }
            }
            break;
        }
        node->height_ = calc_height(node);
        if (node->parent_) {
            node = node->parent_;
        } else {
            break;
        }
    }
}

template <typename T, typename Compare, typename Alloc>
template <typename V>
typename avl_tree<T, Compare, Alloc>::iterator
avl_tree<T, Compare, Alloc>::insert(V&& elem) noexcept {
    if (!root_) {
        root_ = allocate_node(std::forward<V>(elem), nullptr, nullptr, nullptr);
        ++size_;
        return iterator(root_);
    }

    NdPtr node = find_node(elem);
    NdPtr res = nullptr;

    if (!compare_(elem, node->element()) && !compare_(node->element(), elem)) {
        return iterator(node);
    }
    res = allocate_node(std::forward<V>(elem), node, nullptr, nullptr);
    if (compare_(node->element(), elem)) {
        node->right_ = res;
    } else {
        node->left_ = res;
    }
    update(node);
    ++size_;
    return iterator(res);
}

template <typename T, typename Compare, typename Alloc>
void avl_tree<T, Compare, Alloc>::remove_node(NdPtr node) {
    if (node == nullptr) {
        return;
    }
    if (node->has_right() && node->has_left()) {
        auto sub = find_min(node->right_);
        node->element() = std::move(sub->element());
        if (sub->has_right()) {
            sub->right_->parent_ = sub->parent_;
            if (sub->is_left()) {
                sub->parent_->left_ = sub->right_;
            } else {
                sub->parent_->right_ = sub->right_;
            }
            sub->right_ = nullptr;
        } else {
            if (sub->is_left()) {
                sub->parent_->left_ = nullptr;
            } else {
                sub->parent_->right_ = nullptr;
            }
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
            } else {
                NdPtr par = node->parent_;
                if (node->is_left()) {
                    node->parent_->left_ = nullptr;
                } else {
                    node->parent_->right_ = nullptr;
                }
                deallocate_node(node);
                update(par);
                return;
            }
        }
        deallocate_node(node);
    }
}

template <typename T, typename Compare, typename Alloc>
int avl_tree<T, Compare, Alloc>::remove(const T& elem) noexcept {
    int res = 0;
    if (root_ == nullptr) {
        return res;
    }

    NdPtr node = find_node(elem);
    if (!compare_(node->element(), elem) && !compare_(elem, node->element())) {
        remove_node(node);
        --size_;
        res = 1;
    }
    return res;
}

template <typename T, typename Compare, typename Alloc>
typename avl_tree<T, Compare, Alloc>::iterator
avl_tree<T, Compare, Alloc>::remove(iterator itr) noexcept {
    auto res = itr++;
    if (res.node()) {
        bool has_two = res.node()->has_right() && res.node()->has_left();
        remove_node(res.node());
        --size_;
        if (has_two) {
            return res;  // res was replaced with successor, still valid
        }
    }
    return itr;
}

template <typename T, typename Compare, typename Alloc>
typename avl_tree<T, Compare, Alloc>::NdPtr
avl_tree<T, Compare, Alloc>::find_node(const T& elem) const {
    if (root_ == nullptr) {
        return nullptr;
    }

    NdPtr node = root_;
    while (true) {
        if (compare_(node->element(), elem) && node->has_right()) {
            node = node->right_;
        } else if (compare_(elem, node->element()) && node->has_left()) {
            node = node->left_;
        } else {
            break;
        }
    }
    return node;
}

template <typename T, typename Compare, typename Alloc>
typename avl_tree<T, Compare, Alloc>::const_iterator
avl_tree<T, Compare, Alloc>::find(const T& elem) const {
    NdPtr res = find_node(elem);
    if (!res || compare_(res->element(), elem) || compare_(elem, res->element())) {
        return const_iterator();
    }
    return const_iterator(res);
}

template <typename T, typename Compare, typename Alloc>
typename avl_tree<T, Compare, Alloc>::iterator avl_tree<T, Compare, Alloc>::find(const T& elem) {
    NdPtr res = find_node(elem);
    if (!res || compare_(res->element(), elem) || compare_(elem, res->element())) {
        return iterator();
    }
    return iterator(res);
}

/**
 * @brief Internal node type storing element and links.
 *
 * @tparam T element type
 * @tparam Alloc allocator type
 */
template <typename T, typename Compare, typename Alloc>
class avl_tree<T, Compare, Alloc>::Node {
private:
    T element_;
    // its parent node
    NdPtr parent_;
    // its left child
    NdPtr left_;
    // its right child
    NdPtr right_;
    // its height
    int64_t height_ {0};

public:
    /**
     * @brief Construct a node with element and child/parent pointers.
     *
     * @tparam V forwarding type for element
     * @param elem element value
     * @param par parent pointer
     * @param lf_child left child pointer
     * @param rg_child right child pointer
     */
    template <typename V>
    Node(V&& elem, NdPtr par, NdPtr lf_child, NdPtr rg_child) noexcept
        : element_(std::forward<V>(elem)),
          parent_(par),
          left_(lf_child),
          right_(rg_child) {}

    /**
     * @brief Deleted copy constructor.
     */
    Node(const Node& rhs) = delete;
    /**
     * @brief Deleted move constructor.
     */
    Node(Node&& rhs) noexcept = delete;
    /**
     * @brief Deleted copy assignment.
     */
    Node& operator=(const Node& rhs) = delete;
    /**
     * @brief Deleted move assignment.
     */
    Node& operator=(Node&& rhs) noexcept = delete;

    /**
     * @brief Default destructor.
     */
    ~Node() noexcept = default;

    /**
     * @brief Return node height.
     */
    [[nodiscard]] int64_t height() const {
        return height_;
    }

    /**
     * @brief Mutable access to stored element.
     */
    T& element() {
        return element_;
    }

    /**
     * @brief Const access to stored element.
     */
    const T& element() const {
        return element_;
    }

    /**
     * @brief Get left child pointer.
     */
    NdPtr left() const {
        return left_;
    }

    /**
     * @brief Get right child pointer.
     */
    NdPtr right() const {
        return right_;
    }

    /**
     * @brief True if this node is a left child of its parent.
     */
    [[nodiscard]] bool is_left() const {
        if (parent_ == nullptr) {
            return false;
        }
        return parent_->left_ == this;
    }

    /**
     * @brief True if this node is a right child of its parent.
     */
    [[nodiscard]] bool is_right() const {
        if (parent_ == nullptr) {
            return false;
        }
        return parent_->right_ == this;
    }

    /**
     * @brief True if this node is root (no parent).
     */
    [[nodiscard]] bool is_root() const {
        return parent_ == nullptr;
    }

    /**
     * @brief True if left child exists.
     */
    [[nodiscard]] bool has_left() const {
        return bool(left_);
    }

    /**
     * @brief True if right child exists.
     */
    [[nodiscard]] bool has_right() const {
        return bool(right_);
    }

    friend class avl_tree<T, Compare, Alloc>;
    template <typename Ref, typename Ptr>
    friend class avl_tree<T, Compare, Alloc>::avl_iterator;
};

template <typename T, typename Compare, typename Alloc>
template <typename Ref, typename Ptr>
class avl_tree<T, Compare, Alloc>::avl_iterator {
private:
    // the pointer to the node
    NdPtr node_;
    friend const_iterator;

public:
    using self_t = avl_iterator<Ref, Ptr>;

    /**
     * @brief Get the underlying node pointer.
     *
     * @return Pointer to the current node or nullptr.
     */
    NdPtr node() const {
        return node_;
    }

    /**
     * @brief Construct a null iterator.
     */
    avl_iterator() : node_(nullptr) {}

    /**
     * @brief Construct from a node pointer.
     *
     * @param node Node pointer to wrap.
     */
    explicit avl_iterator(NdPtr node) : node_(node) {}

    /**
     * @brief Copy-construct from another iterator.
     *
     * @param rhs Source iterator.
     */
    avl_iterator(const self_t& rhs) : node_(rhs.node_) {}

    /**
     * @brief Move-construct from another iterator.
     *
     * @param rhs Source iterator.
     */
    avl_iterator(self_t&& rhs) noexcept : node_(rhs.node_) {}

    /**
     * @brief Destroy the iterator.
     */
    ~avl_iterator() noexcept = default;

    /**
     * @brief Construct from a compatible iterator.
     *
     * @tparam Iter Compatible iterator type.
     * @param rhs Source iterator.
     */
    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    avl_iterator(const Iter& rhs) : node_(rhs.node_) {}

    /**
     * @brief Move-construct from a compatible iterator.
     *
     * @tparam Iter Compatible iterator type.
     * @param rhs Source iterator.
     */
    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    avl_iterator(Iter&& rhs) noexcept : node_(rhs.node_) {}

    /**
     * @brief Dereference the iterator.
     *
     * @return Reference to the element.
     */
    Ref operator*() const {
        return node_->element();
    }

    /**
     * @brief Access the element through a pointer-like interface.
     *
     * @return Pointer to the element.
     */
    Ptr operator->() const {
        return &node_->element();
    }

    /**
     * @brief Check whether the iterator refers to a valid node.
     *
     * @return True if the iterator is not null.
     */
    explicit operator bool() const {
        return bool(node_);
    }

    /**
     * @brief Advance to the next node in order.
     *
     * @return Reference to this iterator.
     */
    self_t& operator++();

    /**
     * @brief Move to the previous node in order.
     *
     * @return Reference to this iterator.
     */
    self_t& operator--();

    /**
     * @brief Post-increment to the next node in order.
     *
     * @param unused Placeholder parameter for postfix form.
     * @return Iterator value before increment.
     */
    self_t operator++(int) {
        self_t old(*this);
        this->operator++();
        return old;
    }

    /**
     * @brief Post-decrement to the previous node in order.
     *
     * @param unused Placeholder parameter for postfix form.
     * @return Iterator value before decrement.
     */
    self_t operator--(int) {
        self_t old(*this);
        this->operator--();
        return old;
    }

    /**
     * @brief Copy-assign from another iterator.
     *
     * @param rhs Source iterator.
     * @return Reference to this iterator.
     */
    self_t& operator=(const self_t& rhs) = default;

    /**
     * @brief Move-assign from another iterator.
     *
     * @param rhs Source iterator.
     * @return Reference to this iterator.
     */
    self_t& operator=(self_t&& rhs) = default;

    /**
     * @brief Assign from a compatible iterator.
     *
     * @tparam Iter Compatible iterator type.
     * @param rhs Source iterator.
     * @return Reference to this iterator.
     */
    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    self_t& operator=(const Iter& rhs) {
        node_ = rhs.node_;
        return *this;
    }

    /**
     * @brief Move-assign from a compatible iterator.
     *
     * @tparam Iter Compatible iterator type.
     * @param rhs Source iterator.
     * @return Reference to this iterator.
     */
    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    self_t& operator=(Iter&& rhs) noexcept {
        node_ = rhs.node_;
        return *this;
    }

    /**
     * @brief Return an iterator advanced by n steps.
     *
     * @param n Number of steps to advance.
     * @return Advanced iterator.
     */
    self_t operator+(difference_t n) {
        self_t res(*this);
        res += n;
        return res;
    }

    /**
     * @brief Return an iterator moved backward by n steps.
     *
     * @param n Number of steps to move backward.
     * @return Moved iterator.
     */
    self_t operator-(difference_t n) {
        self_t res(*this);
        res -= n;
        return res;
    }

    /**
     * @brief Compare two iterators for equality.
     *
     * @tparam Iter Compatible iterator type.
     * @param lhs Left iterator.
     * @param rhs Right iterator.
     * @return True if both iterators compare equal.
     */
    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator==(const self_t& lhs, const Iter& rhs) {
        bool res = false;
        if (lhs && rhs) {
            res = *lhs == *rhs;
        } else if (lhs || rhs) {
            res = false;
        } else {
            res = true;
        }
        return res;
    }

    /**
     * @brief Compare two iterators for inequality.
     *
     * @tparam Iter Compatible iterator type.
     * @param lhs Left iterator.
     * @param rhs Right iterator.
     * @return True if the iterators are not equal.
     */
    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator!=(const self_t& lhs, const Iter& rhs) {
        return !(lhs == rhs);
    }

    /**
     * @brief Compare two iterators for ordering.
     *
     * @tparam Iter Compatible iterator type.
     * @param lhs Left iterator.
     * @param rhs Right iterator.
     * @return True if lhs is ordered before rhs.
     */
    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator<(const self_t& lhs, const Iter& rhs) {
        bool res = false;
        if (lhs && rhs) {
            res = *lhs < *rhs;
        } else {
            res = static_cast<bool>(lhs);
        }
        return res;
    }

    /**
     * @brief Compare two iterators for ordering.
     *
     * @tparam Iter Compatible iterator type.
     * @param lhs Left iterator.
     * @param rhs Right iterator.
     * @return True if lhs is ordered after rhs.
     */
    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator>(const self_t& lhs, const Iter& rhs) {
        bool res = false;
        if (lhs && rhs) {
            res = *lhs > *rhs;
        } else {
            res = static_cast<bool>(rhs);
        }
        return res;
    }

    /**
     * @brief Compare two iterators for ordering.
     *
     * @tparam Iter Compatible iterator type.
     * @param lhs Left iterator.
     * @param rhs Right iterator.
     * @return True if lhs is not ordered before rhs.
     */
    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator>=(const self_t& lhs, const Iter& rhs) {
        return !(lhs < rhs);
    }

    /**
     * @brief Compare two iterators for ordering.
     *
     * @tparam Iter Compatible iterator type.
     * @param lhs Left iterator.
     * @param rhs Right iterator.
     * @return True if lhs is not ordered after rhs.
     */
    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator<=(const self_t& lhs, const Iter& rhs) {
        return !(lhs > rhs);
    }
};

template <typename T, typename Compare, typename Alloc>
template <typename Ref, typename Ptr>
typename avl_tree<T, Compare, Alloc>::template avl_iterator<Ref, Ptr>::self_t&
avl_tree<T, Compare, Alloc>::avl_iterator<Ref, Ptr>::operator++() {
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

template <typename T, typename Compare, typename Alloc>
template <typename Ref, typename Ptr>
typename avl_tree<T, Compare, Alloc>::template avl_iterator<Ref, Ptr>::self_t&
avl_tree<T, Compare, Alloc>::avl_iterator<Ref, Ptr>::operator--() {
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
