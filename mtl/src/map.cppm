export module mtl.map;

import mtl.avl_tree;
import mtl.core;
import mtl.pair;
import std;

export namespace mtl {

namespace detail {

/**
 * @brief Comparator that compares pairs by their key (first element).
 */
template <typename Pair, typename Compare>
struct pair_less {
    bool operator()(const Pair& a, const Pair& b) const {
        Compare comp;
        return comp(a.first, b.first);
    }
};

}  // namespace detail

/**
 * @brief Ordered key-value container backed by an AVL tree.
 *
 * @tparam K       Key type.
 * @tparam V       Mapped value type.
 * @tparam Compare Comparator type for keys. Defaults to std::less<K>.
 * @tparam Alloc   Allocator type.
 */
template <typename K, typename V, typename Compare = std::less<K>,
          typename Alloc = std::allocator<pair<K, V>>>
class map {
public:
    using key_type = K;
    using mapped_type = V;
    using value_type = pair<K, V>;
    using self_t = map<K, V, Compare, Alloc>;

private:
    using tree_t =
        avl_tree<value_type, detail::pair_less<value_type, Compare>, Alloc>;
    tree_t tree_;

public:
    using iterator = typename tree_t::iterator;
    using const_iterator = typename tree_t::const_iterator;

    map() = default;
    map(const self_t& rhs) = default;
    map(self_t&& rhs) noexcept = default;
    ~map() noexcept = default;

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

    template <typename Key, typename Val>
    iterator insert(Key&& key, Val&& val) {
        return tree_.insert(
            value_type(std::forward<Key>(key), std::forward<Val>(val)));
    }

    size_t erase(const K& key) {
        value_type probe(key, mapped_type {});
        return static_cast<size_t>(tree_.remove(probe));
    }

    iterator erase(iterator itr) {
        return tree_.remove(itr);
    }

    iterator find(const K& key) {
        value_type probe(key, mapped_type {});
        return tree_.find(probe);
    }

    const_iterator find(const K& key) const {
        value_type probe(key, mapped_type {});
        return tree_.find(probe);
    }

    [[nodiscard]] bool contains(const K& key) const {
        value_type probe(key, mapped_type {});
        return tree_.contain(probe);
    }

    /**
     * @brief Access mapped value by key, inserting a default-constructed
     *        value if the key does not exist.
     */
    V& operator[](const K& key) {
        auto it = find(key);
        if (it == end()) {
            it = insert(key, V {});
        }
        return it->second;
    }

    /**
     * @brief Access mapped value by key, inserting a default-constructed
     *        value if the key does not exist.
     */
    V& operator[](K&& key) {
        auto it = find(key);
        if (it == end()) {
            it = insert(std::move(key), V {});
        }
        return it->second;
    }

    /**
     * @brief Access mapped value by key.
     * @throws EmptyContainer if key not found.
     */
    V& at(const K& key) {
        auto it = find(key);
        if (it == end()) {
            throw EmptyContainer("map::at: key not found");
        }
        return it->second;
    }

    const V& at(const K& key) const {
        auto it = find(key);
        if (it == end()) {
            throw EmptyContainer("map::at: key not found");
        }
        return it->second;
    }
};

}  // namespace mtl
