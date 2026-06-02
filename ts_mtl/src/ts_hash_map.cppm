export module ts_mtl.hash_map;

import mtl.core;
import mtl.hash_map;
import mtl.pair;
import std;

export namespace mtl {

/**
 * @brief Thread-safe unordered key-value container backed by a hopscotch
 *        hash table, protected by a shared mutex for read/write locking.
 *
 * Read operations (empty, size, contains, find, at-const) acquire a shared
 * lock. Write operations (insert, erase, clear, operator[], at-non-const)
 * acquire an exclusive lock.
 *
 * @tparam K        Key type.
 * @tparam V        Mapped value type.
 * @tparam Hash     Hash function type for keys. Defaults to std::hash<K>.
 * @tparam KeyEqual Equality comparison type for keys. Defaults to
 *                  std::equal_to<K>.
 * @tparam Alloc    Allocator type.
 */
template <typename K, typename V, typename Hash = std::hash<K>,
          typename KeyEqual = std::equal_to<K>,
          typename Alloc = std::allocator<pair<K, V>>>
class ts_hash_map {
public:
    using key_type = K;
    using mapped_type = V;
    using self_t = ts_hash_map<K, V, Hash, KeyEqual, Alloc>;

private:
    using inner_t = hash_map<K, V, Hash, KeyEqual, Alloc>;
    inner_t data_;
    mutable std::shared_mutex mtx_;

public:
    using iterator = typename inner_t::iterator;
    using const_iterator = typename inner_t::const_iterator;

    ts_hash_map() = default;

    ts_hash_map(const self_t& rhs) {
        std::shared_lock lock(rhs.mtx_);
        data_ = rhs.data_;
    }

    ts_hash_map(self_t&& rhs) noexcept {
        std::lock_guard lock(rhs.mtx_);
        data_ = std::move(rhs.data_);
    }

    ~ts_hash_map() noexcept = default;

    self_t& operator=(const self_t& rhs) = delete;

    self_t& operator=(self_t&& rhs) noexcept {
        if (this != &rhs) {
            std::unique_lock lock_this(mtx_, std::defer_lock);
            std::unique_lock lock_rhs(rhs.mtx_, std::defer_lock);
            std::lock(lock_this, lock_rhs);

            data_ = std::move(rhs.data_);
        }
        return *this;
    }

    [[nodiscard]] bool empty() const {
        std::shared_lock lock(mtx_);
        return data_.empty();
    }

    [[nodiscard]] size_t size() const {
        std::shared_lock lock(mtx_);
        return data_.size();
    }

    void clear() {
        std::lock_guard lock(mtx_);
        data_.clear();
    }

    template <typename Key, typename Val>
    bool insert(Key&& key, Val&& val) {
        std::lock_guard lock(mtx_);
        return data_.insert(std::forward<Key>(key), std::forward<Val>(val));
    }

    size_t erase(const K& key) {
        std::lock_guard lock(mtx_);
        return data_.erase(key);
    }

    bool erase(iterator itr) {
        std::lock_guard lock(mtx_);
        return data_.erase(itr);
    }

    iterator find(const K& key) {
        std::shared_lock lock(mtx_);
        return data_.find(key);
    }

    const_iterator find(const K& key) const {
        std::shared_lock lock(mtx_);
        return data_.find(key);
    }

    [[nodiscard]] bool contains(const K& key) const {
        std::shared_lock lock(mtx_);
        return data_.contains(key);
    }

    /**
     * @brief Access mapped value by key, inserting a default-constructed
     *        value if the key does not exist. Acquires an exclusive lock.
     */
    V& operator[](const K& key) {
        std::lock_guard lock(mtx_);
        return data_[key];
    }

    /**
     * @brief Access mapped value by key, inserting a default-constructed
     *        value if the key does not exist. Acquires an exclusive lock.
     */
    V& operator[](K&& key) {
        std::lock_guard lock(mtx_);
        return data_[std::move(key)];
    }

    /**
     * @brief Access mapped value by key.
     * @throws EmptyContainer if key not found.
     */
    V& at(const K& key) {
        std::lock_guard lock(mtx_);
        return data_.at(key);
    }

    /**
     * @brief Access mapped value by key (const).
     * @throws EmptyContainer if key not found.
     */
    const V& at(const K& key) const {
        std::shared_lock lock(mtx_);
        return data_.at(key);
    }
};

}  // namespace mtl
