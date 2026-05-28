export module mtl.hash_map;

import mtl.core;
import mtl.hashing;
import mtl.pair;
import std;

export namespace mtl {

namespace detail {

/**
 * @brief Hash a pair by its key (first element).
 */
template <typename Pair, typename Hash>
struct pair_hash {
    size_t operator()(const Pair& p) const {
        Hash hash;
        return hash(p.first);
    }
};

/**
 * @brief Compare pairs by their key (first element) for equality.
 */
template <typename Pair, typename KeyEqual>
struct pair_equal {
    bool operator()(const Pair& a, const Pair& b) const {
        KeyEqual eq;
        return eq(a.first, b.first);
    }
};

}  // namespace detail

/**
 * @brief Unordered key-value container backed by a hopscotch hash table.
 *
 * @tparam K        Key type.
 * @tparam V        Mapped value type.
 * @tparam Hash     Hash function type for keys. Defaults to std::hash<K>.
 * @tparam KeyEqual Equality comparison type for keys. Defaults to std::equal_to<K>.
 * @tparam Alloc    Allocator type.
 */
template <typename K, typename V, typename Hash = std::hash<K>,
          typename KeyEqual = std::equal_to<K>,
          typename Alloc = std::allocator<pair<K, V>>>
class hash_map {
public:
    using key_type = K;
    using mapped_type = V;
    using value_type = pair<K, V>;
    using self_t = hash_map<K, V, Hash, KeyEqual, Alloc>;

private:
    using table_t =
        hashing<value_type,
                detail::pair_hash<value_type, Hash>,
                detail::pair_equal<value_type, KeyEqual>,
                Alloc>;
    table_t table_;

public:
    using iterator = typename table_t::iterator;
    using const_iterator = typename table_t::const_iterator;

    hash_map() = default;
    hash_map(const self_t& rhs) = default;
    hash_map(self_t&& rhs) noexcept = default;
    ~hash_map() noexcept = default;

    self_t& operator=(const self_t& rhs) = default;
    self_t& operator=(self_t&& rhs) noexcept = default;

    [[nodiscard]] bool empty() const { return table_.size() == 0; }
    [[nodiscard]] size_t size() const { return table_.size(); }
    void clear() { table_.clear(); }

    iterator begin() { return table_.begin(); }
    const_iterator begin() const { return table_.begin(); }
    const_iterator cbegin() const { return table_.cbegin(); }

    iterator end() { return table_.end(); }
    const_iterator end() const { return table_.end(); }
    const_iterator cend() const { return table_.cend(); }

    template <typename Key, typename Val>
    bool insert(Key&& key, Val&& val) {
        return table_.insert(
            value_type(static_cast<Key&&>(key), static_cast<Val&&>(val)));
    }

    size_t erase(const K& key) {
        value_type probe(key, mapped_type{});
        return table_.remove(probe) ? 1 : 0;
    }

    bool erase(iterator itr) {
        if (itr != end()) {
            return table_.remove(*itr);
        }
        return false;
    }

    iterator find(const K& key) {
        value_type probe(key, mapped_type{});
        return table_.find(probe);
    }

    const_iterator find(const K& key) const {
        value_type probe(key, mapped_type{});
        return table_.find(probe);
    }

    [[nodiscard]] bool contains(const K& key) const {
        value_type probe(key, mapped_type{});
        return table_.contains(probe);
    }

    /**
     * @brief Access mapped value by key, inserting a default-constructed
     *        value if the key does not exist.
     */
    V& operator[](const K& key) {
        auto it = find(key);
        if (it == end()) {
            insert(key, V{});
            it = find(key);
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
            insert(static_cast<K&&>(key), V{});
            it = find(key);
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
            throw EmptyContainer("hash_map::at: key not found");
        }
        return it->second;
    }

    const V& at(const K& key) const {
        auto it = find(key);
        if (it == end()) {
            throw EmptyContainer("hash_map::at: key not found");
        }
        return it->second;
    }
};

}  // namespace mtl
