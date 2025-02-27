#ifndef MTL_HASH_H
#define MTL_HASH_H

#include <mtl/algorithms.h>
#include <mtl/types.h>
#include <bitset>
#include <memory>
#include <utility>

namespace mtl {

/**
 * @brief an implementation of Hopscotch Hashing
 *
 * @tparam T the value type
 * @tparam hash_func the functor to hash the value
 * @tparam Alloc the allocator type
 */
template <typename T, typename hash_func = std::hash<T>,
          typename Alloc = std::allocator<T>>
class hash {
public:
    using self_t = hash<T, hash_func, Alloc>;

private:
    class Cell;
    size_t size_;      // the number of elements in the hash table
    size_t max_size_;  // the max size of the table
    static constexpr double MAX_LOAD_FACTOR = 0.8;  // the max load factor
    static constexpr size_t DEFAULT_SIZE = 101;     // the default initial size
    // the max distance of a cell to its home
    static constexpr size_t MAX_DIST = 32;

    using CellAlloc =
        typename std::allocator_traits<Alloc>::template rebind_alloc<Cell>;
    CellAlloc allocator_;

    Cell* data_;

    // TODO: implement a function to find the new size expanded to in rehashing
    size_t get_next_size_(size_t old_sie);

    // TODO: implement a function to rehash
    void rehash();

    template <typename... Args>
    void construct_cell(size_t index, Args&&... args) {
        std::construct_at(data_ + index, std::forward<Args>(args)...);
    }

    void destruct_cell(size_t index) {
        std::destroy_at(data_ + index);
    }

    // TODO: implement find pos
    size_t find_pos(const T& elem) const;

public:
    // TODO: implement default constructor
    hash();
    /**
     * @brief ensure the hash table's max_size is bigger than init_max_size
     * @param init_max_size the initial max size
     */
    // TODO: implement constructor with initial max size
    explicit hash(size_t init_max_size);

    // TODO: implement copy and move constructor and assignment
    hash(const self_t& rhs);
    hash(self_t&& rhs) noexcept;

    self_t& operator=(const self_t& rhs);
    self_t& operator=(self_t&& rhs) noexcept;

    /**
     * @brief the number of elements in the hash table
     * @return the value
     */
    size_t size() const {
        return size_;
    }

    /**
     * @brief the max size of the table
     * @return the value
     */
    size_t max_size() const {
        return max_size_;
    }

    // TODO: implement insert
    /**
     * @brief to insert a element into the hash table, return true if success
     * @param elem the element to be inserted
     * @return if the element is inserted successfully
     */
    bool insert(const T& elem);

    // TODO: implement remove
    /**
     * @brief remove an element from the hash table, return true if success, if
     * the element doesn't exist, return true either
     * @param elem the element to be removed
     * @return if the element is removed successfully
     */
    bool remove(const T& elem);

    // TODO: implement contains
    /**
     * @brief if elem is contained in the hash table
     * @param elem the element
     * @return if contained
     */
    bool contains(const T& elem) const;

    // TODO: implement clear
    /**
     * @brief clear all elements
     */
    void clear();
};

template <typename T, typename hash_func, typename Alloc>
class hash<T, hash_func, Alloc>::Cell {
public:
    using self_t = Cell;

private:
    bool occupied_;
    std::bitset<MAX_DIST> hop_info_;
    T elem_;

public:
    Cell() : occupied_(false), hop_info_(), elem_() {}
    Cell(const self_t& rhs) = default;
    Cell(self_t&& rhs) noexcept = default;
    ~Cell() = default;

    self_t& operator=(const self_t& rhs) = default;
    self_t& operator=(self_t&& rhs) noexcept = default;

    bool is_occupied() const {
        return occupied_;
    }

    void set_occupied(bool occupied) {
        occupied_ = occupied;
    }

    const Cell& element() const {
        return elem_;
    }

    Cell& element() {
        return const_cast<Cell&>(static_cast<const self_t*>(this)->element());
    }

    bool get_hop(size_t dist) const {
        return hop_info_[dist];
    }

    void set_hop(size_t dist) {
        hop_info_.set(dist);
        occupied_ = true;
    }

    void clear_hip(size_t dist) {
        hop_info_.reset(dist);
        occupied_ = false;
    }
};

}  // namespace mtl

#endif
