export module ts_mtl.hash_set;

import mtl.core;
import mtl.hash_set;
import std;

export namespace mtl {

/**
 * @brief Thread-safe unordered unique-key set backed by a hopscotch hash
 *        table, protected by a shared mutex for read/write locking.
 *
 * Read operations (empty, size, contains, find) acquire a shared lock.
 * Write operations (insert, erase, clear) acquire an exclusive lock.
 *
 * @tparam T        Element type.
 * @tparam Hash     Hash function type. Defaults to std::hash<T>.
 * @tparam KeyEqual Equality comparison type. Defaults to std::equal_to<T>.
 * @tparam Alloc    Allocator type.
 */
template <typename T, typename Hash = std::hash<T>,
          typename KeyEqual = std::equal_to<T>,
          typename Alloc = std::allocator<T>>
class ts_hash_set {
public:
    using self_t = ts_hash_set<T, Hash, KeyEqual, Alloc>;

private:
    using inner_t = hash_set<T, Hash, KeyEqual, Alloc>;
    inner_t data_;
    mutable std::shared_mutex mtx_;

public:
    using iterator = typename inner_t::iterator;
    using const_iterator = typename inner_t::const_iterator;

    ts_hash_set() = default;

    ts_hash_set(const self_t& rhs) {
        std::shared_lock lock(rhs.mtx_);
        data_ = rhs.data_;
    }

    ts_hash_set(self_t&& rhs) noexcept {
        std::lock_guard lock(rhs.mtx_);
        data_ = std::move(rhs.data_);
    }

    ~ts_hash_set() noexcept = default;

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

    bool insert(const T& elem) {
        std::lock_guard lock(mtx_);
        return data_.insert(elem);
    }

    size_t erase(const T& elem) {
        std::lock_guard lock(mtx_);
        return data_.erase(elem);
    }

    bool erase(iterator itr) {
        std::lock_guard lock(mtx_);
        return data_.erase(itr);
    }

    iterator find(const T& elem) {
        std::shared_lock lock(mtx_);
        return data_.find(elem);
    }

    const_iterator find(const T& elem) const {
        std::shared_lock lock(mtx_);
        return data_.find(elem);
    }

    [[nodiscard]] bool contains(const T& elem) const {
        std::shared_lock lock(mtx_);
        return data_.contains(elem);
    }
};

}  // namespace mtl
