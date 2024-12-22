#ifndef MTL_DEQUE_H
#define MTL_DEQUE_H

#include "mtl/algorithms.h"
#include "mtl/types.h"
#include <initializer_list>
#include <stdexcept>
#include <type_traits>

namespace mtl {
/* The deque (double-end queue) ADT.
 * It uses a dynamical array to store pointers to some arrays with fixed length.
 * When push or pop an element from front or back end, it's only needed to add a
 * new node, which ensure that the push and pop operations will take only O(1)
 * time. */
template <typename T>
class deque {
private:
    /* The default size of map array. In any situations, map_size_ won't be less
     * than it */
    const static size_t DEFAULT_MAP_SIZE = 8;
    // The number of elements a node will contain.
    const static size_t BUF_LEN = 16;
    using MapPtr = T**;
    using EltPtr = T*;

    template <typename Ref, typename Ptr>
    class deque_iterator;

public:
    using iterator = deque_iterator<T&, T*>;
    using const_iterator = deque_iterator<const T&, const T*>;

private:
    MapPtr map_;      // the map of nodes
    size_t map_size_; // the size of the map array
    size_t size_;     // the number of elements
    iterator front_;  // the front element
    iterator back_;   // the back element

    MapPtr allocate_map_(size_t map_size);

    EltPtr allocate_node_() {
        return new T[BUF_LEN];
    }

    void init(size_t map_size);

    void expand(bool backward) noexcept;

    void check_empty() const {
        if (empty()) {
            throw EmptyContainer();
        }
    }

    EltPtr copy_node_(EltPtr node);

public:
    deque() : map_(nullptr), map_size_(0), size_(0) {}

    explicit deque(size_t n) : size_(0) {
        init(n / BUF_LEN + 3);
    }

    deque(std::initializer_list<T>&& il) noexcept;
    deque(const deque<T>& rhs);
    deque(deque<T>&& rhs) noexcept
        : map_(rhs.map_),
          map_size_(rhs.map_size_),
          size_(rhs.size_),
          front_(rhs.front_),
          back_(rhs.back_) {
        rhs.size_ = 0;
        rhs.map_size_ = 0;
        rhs.map_ = nullptr;
        rhs.front_ = rhs.back_ = iterator();
    }

    ~deque() {
        clear();
    }

    [[nodiscard]] size_t size() const {
        return size_;
    }

    [[nodiscard]] bool empty() const {
        return size_ == 0;
    }

    void clear();

    const T& operator[](size_t index) const {
        return *(front_ + index);
    }

    T& operator[](size_t index) {
        return const_cast<T&>(
            static_cast<const deque<T>*>(this)->operator[](index));
    }

    const T& at(size_t index) const {
        if (index >= size_) {
            return std::out_of_range("deque::at: index out of range");
        }
        return operator[](index);
    }

    T& at(size_t index) {
        return const_cast<T&>(static_cast<const deque<T>*>(this)->at(index));
    }

    deque<T>& operator=(const deque<T>& rhs) {
        deque<T> tmp(rhs);
        operator=(std::move(tmp));
        return *this;
    }

    deque<T>& operator=(deque<T>&& rhs) noexcept {
        map_ = rhs.map_;
        map_size_ = rhs.map_size_;
        size_ = rhs.size_;
        front_ = rhs.front_;
        back_ = rhs.back_;
        rhs.clear();
        return *this;
    }

    template <typename V>
    void push_back(V&& elem) {
        if (map_ == nullptr) {
            init(DEFAULT_MAP_SIZE);
        }
        *back_ = std::forward<V>(elem);
        ++back_;
        ++size_;
        if (back_.node_ + 1 >= map_ + map_size_) {
            expand(true);
        }
        if (*(back_.node_ + 1) == nullptr) {
            *(back_.node_ + 1) = allocate_node_();
        }
    }

    template <typename V>
    void push_front(V&& elem) {
        if (map_ == nullptr) {
            init(DEFAULT_MAP_SIZE);
        }
        --front_;
        *front_ = std::forward<V>(elem);
        ++size_;
        if (front_.node_ <= map_) {
            expand(false);
        }
        if (*(front_.node_ - 1) == nullptr) {
            *(front_.node_ - 1) = allocate_node_();
        }
    }

    void pop_back() {
        check_empty();
        if (back_.cur_ == back_.first_) {
            delete[] *(back_.node_ + 1);
            *(back_.node_ + 1) = nullptr;
        }
        --back_;
        --size_;
    }

    void pop_front() {
        check_empty();
        if (front_.cur_ == front_.last_ - 1) {
            delete[] *(front_.node_ - 1);
            *(front_.node_ - 1) = nullptr;
        }
        ++front_;
        --size_;
    }

    const T& front() const {
        check_empty();
        return *front_;
    }

    const T& back() const {
        check_empty();
        return *(back_ - 1);
    }

    T& front() {
        return const_cast<T&>(static_cast<const deque<T>*>(this)->front());
    }

    T& back() {
        return const_cast<T&>(static_cast<const deque<T>*>(this)->back());
    }

    const_iterator begin() const {
        return front_;
    }

    const_iterator end() const {
        return back_;
    }

    const_iterator cbegin() const {
        return front_;
    }

    const_iterator cend() const {
        return back_;
    }

    iterator begin() {
        return front_;
    }

    iterator end() {
        return back_;
    }
};

template <typename T>
deque<T>::deque(std::initializer_list<T>&& il) noexcept {
    init(il.size() / BUF_LEN + 3);
    auto mid = find_mid(il.begin(), il.end());
    for (auto itr = mid; itr != il.end(); ++itr) {
        push_back(std::move(*itr));
    }
    for (auto itr = mid - 1; itr >= il.begin(); --itr) {
        push_front(std::move(*itr));
    }
}

template <typename T>
deque<T>::deque(const deque<T>& rhs)
    : map_size_(rhs.map_size_), size_(rhs.size_) {
    map_ = allocate_map_(map_size_);
    auto first_node = map_ + (rhs.front_.node_ - rhs.map_);
    auto last_node = map_ + (rhs.back_.node_ - rhs.map_);
    *(first_node - 1) = allocate_node_();
    *(last_node + 1) = allocate_node_();

    // copy nodes
    auto dst = first_node;
    auto src = rhs.front_.node_;
    while (dst <= last_node) {
        *(dst++) = copy_node_(*(src++));
    }

    // construct front_ and back_ iterators
    auto front_cur = *first_node + (rhs.front_.cur_ - rhs.front_.first_);
    front_ = iterator(front_cur, first_node);
    auto last_cur = *last_node + (rhs.back_.cur_ - rhs.back_.first_);
    back_ = iterator(last_cur, last_node);
}

template <typename T>
typename deque<T>::EltPtr deque<T>::copy_node_(EltPtr node) {
    EltPtr copy = allocate_node_();
    for (int i = 0; i < BUF_LEN; ++i) {
        copy[i] = node[i];
    }
    return copy;
}

template <typename T>
typename deque<T>::MapPtr deque<T>::allocate_map_(size_t map_size) {
    auto map = new T*[map_size];
    for (size_t i = 0; i < map_size; ++i) {
        map[i] = nullptr;
    }
    return map;
}

template <typename T>
void deque<T>::init(size_t map_size) {
    map_size_ = map_size;
    map_ = allocate_map_(map_size_);
    MapPtr first_node_ = map_ + map_size_ / 2;
    *first_node_ = allocate_node_();
    *(first_node_ + 1) = allocate_node_();
    *(first_node_ - 1) = allocate_node_();
    back_ = iterator(*first_node_ + BUF_LEN / 2, first_node_);
    front_ = back_;
}

template <typename T>
void deque<T>::expand(bool backward) noexcept {
    auto old_map = map_;
    auto old_map_size = map_size_;
    difference_t scope_size = back_.node_ - front_.node_;
    map_size_ = old_map_size + scope_size;
    map_ = allocate_map_(map_size_);
    if (backward) {
        MapPtr start_node = map_ + (front_.node_ - old_map);

        MapPtr ptr1 = start_node - 1;
        MapPtr ptr2 = front_.node_ - 1;
        MapPtr end_node = back_.node_ + 1;
        while (ptr2 < end_node) {
            swap(*(ptr1++), *(ptr2++));
        }
        end_node = ptr1 - 1;
        front_.node_ = start_node;
        back_.node_ = end_node;
    } else {
        MapPtr start_node =
            map_ + map_size_ - ((old_map + old_map_size) - back_.node_);

        MapPtr ptr1 = start_node + 1;
        MapPtr ptr2 = back_.node_ + 1;
        MapPtr end_node = front_.node_ - 1;
        while (ptr2 > end_node) {
            swap(*(ptr1--), *(ptr2--));
        }

        end_node = ptr1 + 1;
        back_.node_ = start_node;
        front_.node_ = end_node;
    }
    delete[] old_map;
}

template <typename T>
void deque<T>::clear() {
    MapPtr start = front_.node_ - 1;
    MapPtr stop = back_.node_ + 2;
    for (MapPtr ptr = start; ptr < stop; ++ptr) {
        delete[] *ptr;
        *ptr = nullptr;
    }
    delete[] map_;
    map_ = nullptr;
    size_ = 0;
    map_size_ = 0;
    front_ = back_ = iterator();
}

template <typename T>
template <typename Ref, typename Ptr>
class deque<T>::deque_iterator {
private:
    using self_t = deque_iterator<Ref, Ptr>;
    EltPtr first_; // the first element of current node
    EltPtr last_;  // the element past the last element of current node
    MapPtr node_;  // the current node
    EltPtr cur_;   // the current element

    // set the iterator's node as new_node
    void set_node_(MapPtr new_node) {
        node_ = new_node;
        first_ = *node_;
        last_ = first_ + BUF_LEN;
    }

public:
    deque_iterator()
        : first_(nullptr), last_(nullptr), node_(nullptr), cur_(nullptr) {}

    deque_iterator(EltPtr cur, MapPtr node) : node_(node), cur_(cur) {
        set_node_(node);
    }

    deque_iterator(const iterator& rhs)
        : first_(rhs.first_),
          last_(rhs.last_),
          node_(rhs.node_),
          cur_(rhs.cur_) {}

    template <typename Iter,
              typename = std::_Require<std::is_same<self_t, const_iterator>,
                                       std::is_same<Iter, iterator>>>
    deque_iterator(const Iter& rhs)
        : first_(rhs.first_),
          last_(rhs.last_),
          node_(rhs.node_),
          cur_(rhs.cur_) {}

    ~deque_iterator() noexcept = default;

    self_t& operator=(const deque_iterator& rhs) {
        first_ = rhs.first_;
        last_ = rhs.last_;
        node_ = rhs.node_;
        cur_ = rhs.cur_;
        return *this;
    }

    template <typename Iter,
              typename = std::_Require<std::is_same<self_t, const_iterator>,
                                       std::is_same<Iter, iterator>>>
    self_t& operator=(const Iter& rhs) {
        first_ = rhs.first_;
        last_ = rhs.last_;
        node_ = rhs.node_;
        cur_ = rhs.cur_;
        return *this;
    }

    Ref operator*() const {
        return *cur_;
    }

    Ptr operator->() const {
        return cur_;
    }

    self_t& operator++() {
        ++cur_;
        if (cur_ == last_) {
            set_node_(node_ + 1);
            cur_ = first_;
        }
        return *this;
    }

    self_t& operator--() {
        if (cur_ == first_) {
            set_node_(node_ - 1);
            cur_ = last_;
        }
        --cur_;
        return *this;
    }

    self_t operator++(int) {
        auto old = *this;
        this->operator++();
        return old;
    }

    self_t operator--(int) {
        auto old = *this;
        this->operator--();
        return old;
    }

    /* to implement the support of random access
     * n could be positive or negative */
    self_t& operator+=(difference_t n) {
        const difference_t offset = n + (cur_ - first_);
        if (offset >= 0 && offset < BUF_LEN) {
            cur_ += n;
        } else {
            difference_t node_offset = offset > 0
                ? offset / BUF_LEN
                : -static_cast<difference_t>((-offset - 1) / BUF_LEN) - 1;
            set_node_(node_ + node_offset);
            cur_ = first_ +
                (offset - static_cast<difference_t>(BUF_LEN) * node_offset);
        }
        return *this;
    }

    self_t& operator-=(difference_t n) {
        return *this += -n;
    }

    self_t operator+(difference_t n) const {
        auto new_itr = *this;
        new_itr += n;
        return new_itr;
    }

    self_t operator-(difference_t n) const {
        auto new_itr = *this;
        new_itr -= n;
        return new_itr;
    }

    friend difference_t operator-(const self_t& lhs, const self_t& rhs) {
        return (lhs.cur_ - lhs.first_) + (rhs.last_ - rhs.cur_) +
            (lhs.node_ - rhs.node_ - static_cast<bool>(lhs.node_)) * BUF_LEN;
    }

    friend bool operator==(const self_t& lhs, const self_t& rhs) {
        return lhs.cur_ == rhs.cur_;
    }

    friend bool operator!=(const self_t& lhs, const self_t& rhs) {
        return lhs.cur_ != rhs.cur_;
    }

    friend bool operator<(const self_t& lhs, const self_t& rhs) {
        if (lhs.node_ < rhs.node_) {
            return true;
        } else if (lhs.node_ == rhs.node_) {
            return lhs.cur_ < rhs.cur_;
        } else {
            return false;
        }
    }

    friend bool operator<=(const self_t& lhs, const self_t& rhs) {
        return lhs < rhs || lhs == rhs;
    }

    friend bool operator>(const self_t& lhs, const self_t& rhs) {
        return !(lhs <= rhs);
    }

    friend bool operator>=(const self_t& lhs, const self_t& rhs) {
        return !(lhs < rhs);
    }

    friend class deque<T>;
};
} // namespace mtl
#endif
