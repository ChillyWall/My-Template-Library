export module mtl.hash_set;

import mtl.hashing;
import std;

export namespace mtl {

/**
 * @brief Unordered unique-key container backed by a hopscotch hash table.
 *
 * @tparam T        Element type.
 * @tparam Hash     Hash function type. Defaults to std::hash<T>.
 * @tparam KeyEqual Equality comparison type. Defaults to std::equal_to<T>.
 * @tparam Alloc    Allocator type.
 */
template <typename T, typename Hash = std::hash<T>,
          typename KeyEqual = std::equal_to<T>,
          typename Alloc = std::allocator<T>>
class hash_set {
public:
    using self_t = hash_set<T, Hash, KeyEqual, Alloc>;

private:
    using table_t = hashing<T, Hash, KeyEqual, Alloc>;
    table_t table_;

public:
    using iterator = typename table_t::iterator;
    using const_iterator = typename table_t::const_iterator;

    hash_set() = default;
    hash_set(const self_t& rhs) = default;
    hash_set(self_t&& rhs) noexcept = default;
    ~hash_set() noexcept = default;

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

    bool insert(const T& elem) { return table_.insert(elem); }

    size_t erase(const T& elem) { return table_.remove(elem) ? 1 : 0; }

    bool erase(iterator itr) {
        if (itr != end()) {
            return table_.remove(*itr);
        }
        return false;
    }

    iterator find(const T& elem) { return table_.find(elem); }
    const_iterator find(const T& elem) const { return table_.find(elem); }

    [[nodiscard]] bool contains(const T& elem) const { return table_.contains(elem); }
};

}  // namespace mtl
