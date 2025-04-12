#ifndef MTL_DEQUE_H
#define MTL_DEQUE_H

#include <mtl/mtldefs.h>
#include <mtl/mtlutils.h>
#include <initializer_list>
#include <memory>
#include <stdexcept>

namespace mtl {

/* The deque (double-end queue) ADT.
 * It uses a dynamical array to store pointers to some arrays with fixed length.
 * When push or pop an element from front or back end, it's only needed to add a
 * new node, which ensure that the push and pop operations will take only O(1)
 * time. */
template <typename T, typename Alloc = std::allocator<T>>
class deque {
public:
    using self_t = deque<T, Alloc>;

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
    MapPtr map_;       // the map of nodes
    size_t map_size_;  // the size of the map array
    size_t size_;      // the number of elements
    iterator front_;   // the front element
    iterator back_;    // the back element

    using MapAlloc =
        typename std::allocator_traits<Alloc>::template rebind_alloc<EltPtr>;

    Alloc node_allocator_;
    MapAlloc map_allocator_;

    MapPtr allocate_map(size_t map_size);

    EltPtr allocate_node() {
        return node_allocator_.allocate(BUF_LEN);
    }

    void deallocate_map(MapPtr map, size_t map_size) {
        map_allocator_.deallocate(map, map_size);
    }

    void deallocate_node(EltPtr node) {
        node_allocator_.deallocate(node, BUF_LEN);
    }

    void destroy_all() {
        for (auto itr = front_; itr != back_; ++itr) {
            std::destroy_at(itr.cur_);
        }
    }

    void init(size_t map_size);

    void expand(bool backward) noexcept;

    void check_empty() const {
        if (empty()) {
            throw EmptyContainer();
        }
    }

public:
    deque() : map_(nullptr), map_size_(0), size_(0) {}

    explicit deque(size_t n);
    explicit deque(size_t n, const T& val);
    deque(std::initializer_list<T>&& il) noexcept;
    deque(const self_t& rhs);
    deque(self_t&& rhs) noexcept
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
            static_cast<const self_t*>(this)->operator[](index));
    }

    const T& at(size_t index) const {
        if (index >= size_) {
            return std::out_of_range("deque::at: index out of range");
        }
        return operator[](index);
    }

    T& at(size_t index) {
        return const_cast<T&>(static_cast<const self_t*>(this)->at(index));
    }

    self_t& operator=(const self_t& rhs) {
        deque<T, Alloc> tmp(rhs);
        operator=(std::move(tmp));
        return *this;
    }

    self_t& operator=(self_t&& rhs) noexcept {
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
        std::construct_at(back_.cur_, std::forward<V>(elem));
        ++back_;
        ++size_;
        if (back_.node_ + 1 >= map_ + map_size_) {
            expand(true);
        }
        if (*(back_.node_ + 1) == nullptr) {
            *(back_.node_ + 1) = allocate_node();
        }
    }

    template <typename V>
    void push_front(V&& elem) {
        if (map_ == nullptr) {
            init(DEFAULT_MAP_SIZE);
        }
        --front_;
        std::construct_at(front_.cur_, std::forward<V>(elem));
        ++size_;
        if (front_.node_ <= map_) {
            expand(false);
        }
        if (*(front_.node_ - 1) == nullptr) {
            *(front_.node_ - 1) = allocate_node();
        }
    }

    void pop_back() {
        check_empty();
        if (back_.cur_ == back_.first_) {
            deallocate_node(*(back_.node_ + 1));
            *(back_.node_ + 1) = nullptr;
        }
        std::destroy_at(back_.cur_);
        --back_;
        --size_;
    }

    void pop_front() {
        check_empty();
        if (front_.cur_ == front_.last_ - 1) {
            deallocate_node(*(front_.node_ - 1));
            *(front_.node_ - 1) = nullptr;
        }
        std::destroy_at(front_.cur_);
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
        return const_cast<T&>(static_cast<const self_t*>(this)->front());
    }

    T& back() {
        return const_cast<T&>(static_cast<const self_t*>(this)->back());
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

template <typename T, typename Alloc>
deque<T, Alloc>::deque(size_t n) {
    init(n / BUF_LEN + 3);
    for (auto ptr = map_; ptr != map_ + map_size_; ++ptr) {
        *ptr = allocate_node();
    }
    size_t pre = n / 2;
    size_t suf = n - pre;
    front_ -= pre;
    back_ += suf;
    for (auto itr = front_; itr != back_; ++itr) {
        construct_at(itr.cur_);
    }
}

template <typename T, typename Alloc>
deque<T, Alloc>::deque(size_t n, const T& val) {
    init(n / BUF_LEN + 3);
    for (auto ptr = map_; ptr != map_ + map_size_; ++ptr) {
        *ptr = allocate_node();
    }
    size_t pre = n / 2;
    size_t suf = n - pre;
    front_ -= pre;
    back_ += suf;
    for (auto itr = front_; itr != back_; ++itr) {
        construct_at(itr.cur_, val);
    }
}

template <typename T, typename Alloc>
deque<T, Alloc>::deque(std::initializer_list<T>&& il) noexcept {
    init(il.size() / BUF_LEN + 3);
    for (auto ptr = map_; ptr != map_ + map_size_; ++ptr) {
        *ptr = allocate_node();
    }
    size_t pre = il.size() / 2;
    size_t suf = il.size() - pre;
    front_ -= pre;
    suf += suf;
    auto itr1 = front_;
    for (auto itr2 = il.begin(); itr2 != il.end(); ++itr1, ++itr2) {
        construct_at(itr1.cur_, std::move(*itr2));
    }
}

template <typename T, typename Alloc>
deque<T, Alloc>::deque(const self_t& rhs)
    : map_size_(rhs.map_size_), size_(rhs.size_) {
    map_ = allocate_map(map_size_);
    auto first_node = map_ + (rhs.front_.node_ - rhs.map_);
    auto last_node = map_ + (rhs.back_.node_ - rhs.map_);
    *(first_node - 1) = allocate_node();
    *(last_node + 1) = allocate_node();

    // allcoate nodes
    for (auto pn = first_node; pn <= last_node; ++pn) {
        *pn = allocate_node();
    }

    // construct front_ and back_ iterators
    auto front_cur = *first_node + (rhs.front_.cur_ - rhs.front_.first_);
    front_ = iterator(front_cur, first_node);
    auto last_cur = *last_node + (rhs.back_.cur_ - rhs.back_.first_);
    back_ = iterator(last_cur, last_node);

    // copy elements
    for (auto itr1 = front_, itr2 = rhs.front_; itr1 != back_; ++itr1, ++itr2) {
        construct_at(itr1.cur_, *itr2);
    }
}

template <typename T, typename Alloc>
typename deque<T, Alloc>::MapPtr
deque<T, Alloc>::allocate_map(size_t map_size) {
    auto map = map_allocator_.allocate(map_size);
    for (size_t i = 0; i < map_size; ++i) {
        map[i] = nullptr;
    }
    return map;
}

template <typename T, typename Alloc>
void deque<T, Alloc>::init(size_t map_size) {
    map_size_ = map_size;
    map_ = allocate_map(map_size_);
    MapPtr first_node_ = map_ + map_size_ / 2;
    *first_node_ = allocate_node();
    *(first_node_ + 1) = allocate_node();
    *(first_node_ - 1) = allocate_node();
    back_ = iterator(*first_node_ + BUF_LEN / 2, first_node_);
    front_ = back_;
}

template <typename T, typename Alloc>
void deque<T, Alloc>::expand(bool backward) noexcept {
    auto old_map = map_;
    auto old_map_size = map_size_;
    difference_t scope_size = back_.node_ - front_.node_;
    map_size_ = old_map_size + scope_size;
    map_ = allocate_map(map_size_);
    if (backward) {
        MapPtr start_node = map_ + (front_.node_ - old_map);

        MapPtr ptr1 = start_node - 1;
        MapPtr ptr2 = front_.node_ - 1;
        MapPtr end_node = back_.node_ + 1;
        while (ptr2 < end_node) {
            mtl::iter_swap(ptr1++, ptr2++);
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
            mtl::iter_swap(ptr1--, ptr2--);
        }

        end_node = ptr1 + 1;
        back_.node_ = start_node;
        front_.node_ = end_node;
    }
    deallocate_map(old_map, old_map_size);
}

template <typename T, typename Alloc>
void deque<T, Alloc>::clear() {
    MapPtr start = front_.node_ - 1;
    MapPtr stop = back_.node_ + 2;
    destroy_all();
    for (auto ptr = start; ptr < stop; ++ptr) {
        deallocate_node(*ptr);
        *ptr = nullptr;
    }
    deallocate_map(map_, map_size_);
    map_ = nullptr;
    size_ = 0;
    map_size_ = 0;
    front_ = back_ = iterator();
}

template <typename T, typename Alloc>
template <typename Ref, typename Ptr>
class deque<T, Alloc>::deque_iterator {
public:
    using self_t = deque_iterator<Ref, Ptr>;

private:
    EltPtr first_;  // the first element of current node
    EltPtr last_;   // the element past the last element of current node
    MapPtr node_;   // the current node
    EltPtr cur_;    // the current element

    // set the iterator's node as new_node
    void set_node(MapPtr new_node) {
        node_ = new_node;
        first_ = *node_;
        last_ = first_ + BUF_LEN;
    }

    friend const_iterator;

public:
    deque_iterator()
        : first_(nullptr), last_(nullptr), node_(nullptr), cur_(nullptr) {}

    deque_iterator(EltPtr cur, MapPtr node) : node_(node), cur_(cur) {
        set_node(node);
    }

    deque_iterator(const self_t& rhs)
        : first_(rhs.first_),
          last_(rhs.last_),
          node_(rhs.node_),
          cur_(rhs.cur_) {}

    template <normal_to_const<self_t, iterator, const_iterator> Iter>
    deque_iterator(const Iter& rhs)
        : first_(rhs.first_),
          last_(rhs.last_),
          node_(rhs.node_),
          cur_(rhs.cur_) {}

    ~deque_iterator() noexcept = default;

    self_t& operator=(const self_t& rhs) {
        first_ = rhs.first_;
        last_ = rhs.last_;
        node_ = rhs.node_;
        cur_ = rhs.cur_;
        return *this;
    }

    template <normal_to_const<self_t, iterator, const_iterator> Iter>
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
            set_node(node_ + 1);
            cur_ = first_;
        }
        return *this;
    }

    self_t& operator--() {
        if (cur_ == first_) {
            set_node(node_ - 1);
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
            set_node(node_ + node_offset);
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

    template <is_one_of<iterator, const_iterator> Iter>
    friend difference_t operator-(const self_t& lhs, const Iter& rhs) {
        return (lhs.cur_ - lhs.first_) + (rhs.last_ - rhs.cur_) +
            (lhs.node_ - rhs.node_ - static_cast<bool>(lhs.node_)) * BUF_LEN;
    }

    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator==(const self_t& lhs, const Iter& rhs) {
        return lhs.cur_ == rhs.cur_;
    }

    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator!=(const self_t& lhs, const Iter& rhs) {
        return lhs.cur_ != rhs.cur_;
    }

    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator<(const self_t& lhs, const Iter& rhs) {
        if (lhs.node_ < rhs.node_) {
            return true;
        } else if (lhs.node_ == rhs.node_) {
            return lhs.cur_ < rhs.cur_;
        } else {
            return false;
        }
    }

    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator<=(const self_t& lhs, const Iter& rhs) {
        return lhs < rhs || lhs == rhs;
    }

    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator>(const self_t& lhs, const Iter& rhs) {
        return !(lhs <= rhs);
    }

    template <is_one_of<iterator, const_iterator> Iter>
    friend bool operator>=(const self_t& lhs, const Iter& rhs) {
        return !(lhs < rhs);
    }

    friend class deque<T, Alloc>;
};

template <typename T>
deque<T>::const_iterator advance(typename deque<T>::const_iterator iter,
                                 difference_t n) {
    return iter + n;
}

template <typename T>
deque<T>::const_iterator distance(typename deque<T>::const_iterator first,
                                  typename deque<T>::const_iterator last) {
    return last - first;
}

template <typename T>
deque<T>::iterator advance(typename deque<T>::iterator iter, difference_t n) {
    return iter + n;
}

template <typename T>
deque<T>::iterator distance(typename deque<T>::iterator first,
                            typename deque<T>::iterator last) {
    return last - first;
}

}  // namespace mtl
#endif
