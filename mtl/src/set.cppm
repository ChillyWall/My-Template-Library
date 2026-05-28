export module mtl.set;

import mtl.avl_tree;
import std;

export namespace mtl {

/**
 * @brief Ordered unique-key container backed by an AVL tree.
 *
 * @tparam T       Element type.
 * @tparam Compare Comparator type (strict weak ordering). Defaults to
 * std::less<T>.
 * @tparam Alloc   Allocator type.
 */
template <typename T, typename Compare = std::less<T>,
          typename Alloc = std::allocator<T>>
class set {
public:
    using self_t = set<T, Compare, Alloc>;

private:
    using tree_t = avl_tree<T, Compare, Alloc>;
    tree_t tree_;

public:
    using iterator = typename tree_t::iterator;
    using const_iterator = typename tree_t::const_iterator;

    set() = default;
    set(const self_t& rhs) = default;
    set(self_t&& rhs) noexcept = default;
    ~set() noexcept = default;

    self_t& operator=(const self_t& rhs) = default;
    self_t& operator=(self_t&& rhs) noexcept = default;

    [[nodiscard]] bool empty() const {
        return tree_.empty();
    }
    [[nodiscard]] size_t size() const {
        return tree_.size();
    }
    void clear() {
        tree_.clear();
    }

    iterator begin() {
        return tree_.begin();
    }
    const_iterator begin() const {
        return tree_.begin();
    }
    const_iterator cbegin() const {
        return tree_.cbegin();
    }

    iterator end() {
        return tree_.end();
    }
    const_iterator end() const {
        return tree_.end();
    }
    const_iterator cend() const {
        return tree_.cend();
    }

    template <typename V>
    iterator insert(V&& elem) {
        return tree_.insert(std::forward<V>(elem));
    }

    size_t erase(const T& elem) {
        return static_cast<size_t>(tree_.remove(elem));
    }

    iterator erase(iterator itr) {
        return tree_.remove(itr);
    }

    iterator find(const T& elem) {
        return tree_.find(elem);
    }
    const_iterator find(const T& elem) const {
        return tree_.find(elem);
    }

    [[nodiscard]] bool contains(const T& elem) const {
        return tree_.contain(elem);
    }
};

}  // namespace mtl
