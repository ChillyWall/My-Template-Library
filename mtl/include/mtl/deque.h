#ifndef MTL_DEQUE_H
#define MTL_DEQUE_H

#include "mtl/types.h"
#include <cmath>
#include <initializer_list>
#include <mtl/list.h>

namespace mtl {

/* The deque (double-end queue) ADT.
 * It uses a dynamical array to store pointers to some arrays with fixed length.
 * When push or pop a element from front or back end, it's only needed to add a
 * new node, which ensure that the push and pop operations will take only O(1)
 * time.
 */
template <typename T>
class deque {
private:
    /* The default size of map array. In any situations, map_size_ won't be less
     * than it */
    const static size_t DEFAULT_MAP_SIZE = 8;
    // The number of elements a node will contain.
    const static size_t BUF_LEN = 16;

    class const_iterator {
    private:
        T* first_; // the first element of current node
        T* last_;  // the element past the last element of current node
        T** node_; // the current node
        T* cur_;   // the current element

        void set_node_(T** new_node) {
            node_ = new_node;
            first_ = *node_;
            last_ = first_ + BUF_LEN;
        }

    public:
        const_iterator();
        const_iterator(T* cur, T** node);
        const_iterator(const const_iterator& rhs) = default;
        virtual ~const_iterator() = default;

        const_iterator& operator=(const const_iterator& rhs) = default;

        const T& operator*() const {
            return *cur_;
        }

        const_iterator& operator++() {
            ++cur_;
            if (cur_ == last_) {
                ++node_;
                set_node_(node_);
                cur_ = first_;
            }
            return *this;
        }

        const_iterator& operator--() {
            if (cur_ == first_) {
                --node_;
                set_node_(node_);
                cur_ = last_;
            }
            --cur_;
            return *this;
        }

        const_iterator operator++(int) {
            auto old = *this;
            this->operator++();
            return old;
        }
        const_iterator operator--(int) {
            auto old = *this;
            this->operator--();
            return old;
        }

        const_iterator& operator+=(size_t n);
        const_iterator& operator-=(size_t n);

        const_iterator operator+(size_t n) const {
            auto new_itr = *this;
            new_itr += n;
            return new_itr;
        }
        const_iterator operator-(size_t n) const {
            auto new_itr = *this;
            new_itr -= n;
            return new_itr;
        }

        size_t operator-(const const_iterator& rhs) const {
            return (cur_ - first_) + (rhs.last_ - rhs.cur_) +
                (node_ - rhs.node_);
        }

        bool operator==(const const_iterator& rhs) const {
            return cur_ == rhs.cur_;
        }

        bool operator!=(const const_iterator& rhs) const {
            return !(*this == rhs);
        }

        bool operator<(const const_iterator& rhs) const {
            if (node_ < rhs.node_) {
                return true;
            } else if (node_ == rhs.node_) {
                return cur_ < rhs.cur_;
            } else {
                return false;
            }
        }

        bool operator<=(const const_iterator& rhs) const {
            return *this < rhs || *this == rhs;
        }

        bool operator>(const const_iterator& rhs) const {
            return !(*this <= rhs);
        }

        bool operator>=(const const_iterator& rhs) const {
            return !(*this < rhs);
        }

        friend class deque<T>;
    };

    class iterator : public const_iterator {
    public:
        template <typename... Args>
        iterator(Args&&... args);
        virtual ~iterator() = default;

        const T& operator*() const {
            return const_iterator::operator*();
        }

        T& operator*() {
            return const_cast<T&>(const_iterator::operator*());
        }

        iterator& operator=(const iterator& rhs) {
            const_iterator::operator=(rhs);
            return *this;
        }

        iterator& operator++() {
            const_iterator::operator++();
            return *this;
        }
        iterator& operator--() {
            const_iterator::operator--();
            return *this;
        }
        iterator operator++(int) {
            auto old = *this;
            const_iterator::operator++();
            return *this;
        }
        iterator operator--(int) {
            auto old = *this;
            const_iterator::operator--();
            return *this;
        }

        iterator& operator+=(size_t n) {
            const_iterator::operator+=(n);
            return *this;
        }
        iterator& operator-=(size_t n) {
            const_iterator::operator-=(n);
            return *this;
        }

        iterator operator+(size_t n) const {
            auto new_itr = *this;
            new_itr += n;
            return new_itr;
        }
        iterator operator-(size_t n) const {
            auto new_itr = *this;
            new_itr -= n;
            return new_itr;
        }

        friend class deque<T>;
    };

    T** map_;         // the map of nodes
    size_t size_;     // the number of elements
    size_t map_size_; // the size of the map array
    T** first_node_;  // the first node
    T** last_node_;   // the node past the last node
    iterator front_;  // the front element
    iterator back_;   // the back element

    static T** allocate_map_(size_t map_size);

    static T* allocate_node_();

    void init(size_t map_size);

    void expand();

    void check_empty() {
        if (empty()) {
            throw EmptyContainer();
        }
    }

    static T* copy_node_(T* node);

public:
    deque();
    deque(size_t n);
    deque(std::initializer_list<T>&& il);
    deque(const deque<T>& rhs);
    deque(deque<T>&& rhs) noexcept;

    size_t size() const {
        return size_;
    }

    bool empty() const {
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

    deque<T>& operator=(const deque<T>& rhs);
    deque<T>& operator=(deque<T>&& rhs) noexcept;

    template <typename V>
    void push_back(V&& elem);
    template <typename V>
    void push_front(V&& elem);
    void pop_back();
    void pop_front();

    const T& front() const {
        return *front_;
    }
    const T& back() const {
        return *back_;
    }
    T& front() {
        return *front_;
    }
    T& back() {
        return *back_;
    }

    const_iterator begin() const {
        return front_;
    }

    const_iterator end() const {
        return back_ + 1;
    }

    const_iterator cbegin() const {
        return front_;
    }

    const_iterator cend() const {
        return back_ + 1;
    }

    iterator begin() {
        return front_;
    }

    iterator end() {
        return back_ + 1;
    }
};

template <typename T>
deque<T>::deque()
    : map_(nullptr),
      size_(0),
      map_size_(0),
      first_node_(nullptr),
      last_node_(nullptr) {}

template <typename T>
deque<T>::deque(size_t n) : size_(0) {
    init(n / 16 + 1);
}

template <typename T>
deque<T>::deque(const deque<T>& rhs)
    : size_(rhs.size_), map_size_(rhs.map_size_) {
    map_ = allocate_map_(map_size_);
    first_node_ = map_ + (rhs.first_node_ - rhs.map_);
    last_node_ = first_node_ + map_size_;
    auto ptr1 = first_node_;
    *ptr1 = allocate_node_();
    auto ptr2 = rhs.first_node_;
    size_t index = rhs.front_.cur_ - rhs.front_.first_;
    for (size_t i = index; i < BUF_LEN; ++i) {
        (*ptr1)[i] = (*ptr2)[i];
    }
    ++ptr1;
    ++ptr2;
    while (ptr1 != last_node_ - 1) {
        *(ptr1++) = copy_node_(*(ptr2++));
    }
    index = rhs.back_.cur_ - rhs.back_.first_;
    for (size_t i = 0; i <= index; ++i) {
        (*ptr1)[i] = (*ptr2)[i];
    }
}

template <typename T>
deque<T>::deque(deque<T>&& rhs) noexcept
    : map_(rhs.map_),
      size_(rhs.size_),
      map_size_(rhs.map_size_),
      first_node_(rhs.first_node_),
      last_node_(rhs.last_node_),
      front_(rhs.front_),
      back_(rhs.back_) {
    rhs.size_ = 0;
    rhs.map_size_ = 0;
    rhs.map_ = rhs.first_node_ = rhs.last_node_ = nullptr;
    rhs.front_ = rhs.back_ = iterator();
}

template <typename T>
T* deque<T>::copy_node_(T* node) {
    T* copy = allocate_node_();
    for (int i = 0; i < BUF_LEN; ++i) {
        copy[i] = node[i];
    }
}

template <typename T>
T** deque<T>::allocate_map_(size_t map_size) {
    T** map = new T*[map_size];
    for (size_t i = 0; i < map_size; ++i) {
        map[i] = nullptr;
    }
    return map;
}

template <typename T>
T* deque<T>::allocate_node_() {
    return new T[BUF_LEN];
}

template <typename T>
void deque<T>::init(size_t map_size) {
    map_size_ = map_size;
    map_ = allocate_map_(map_size_);
    first_node_ = map_ + map_size_ / 2;
    last_node_ = first_node_ + 1;
    *first_node_ = allocate_node_();
    front_ = back_ = iterator(*first_node_ + BUF_LEN / 2, first_node_);
}

template <typename T>
void deque<T>::clear() {
    for (auto ptr = first_node_; ptr != last_node_; ++ptr) {
        delete ptr;
    }
    delete map_;
    map_ = nullptr;
    size_ = 0;
    map_size_ = 0;
    first_node_ = last_node_ = nullptr;
    front_ = back_ = iterator();
}

template <typename T>
void deque<T>::expand() {
    size_t cur_map_size = last_node_ - first_node_;
    map_size_ = cur_map_size * 2;
    auto old = map_;
    map_ = allocate_map_(map_size_);

    auto ptr1 = map_ + cur_map_size / 2;
    auto ptr2 = first_node_;
    first_node_ = ptr1;
    while (ptr2 != last_node_) {
        *(ptr1++) = *(ptr2++);
    }
    last_node_ = ptr1;

    front_.node_ = first_node_;
    back_.node_ = last_node_ - 1;
}

template <typename T>
template <typename V>
void deque<T>::push_back(V&& elem) {
    if (empty()) {
        init(1);
        *back_ = std::forward<V>(elem);
    } else {
        if (back_.cur_ == back_.last_ - 1) {
            if (last_node_ == map_ + map_size_) {
                expand();
            }
            *last_node_ = allocate_node_();
            ++last_node_;
        }
        ++back_;
        *back_ = std::forward<V>(elem);
    }
    ++size_;
}

template <typename T>
template <typename V>
void deque<T>::push_front(V&& elem) {
    if (empty()) {
        init(1);
        *back_ = std::forward<V>(elem);
    } else {
        if (front_.cur_ == front_.first_) {
            if (first_node_ == map_) {
                expand();
            }
            --first_node_;
            *first_node_ = allocate_node_();
        }
        --front_;
        *front_ = std::forward<V>(elem);
    }
    ++size_;
}

template <typename T>
void deque<T>::pop_back() {
    check_empty();
    --back_;
    --size_;
    if (back_.node_ != last_node_ - 1) {
        --last_node_;
        delete last_node_;
        *last_node_ = nullptr;
    }
}

template <typename T>
void deque<T>::pop_front() {
    check_empty();
    ++front_;
    --size_;
    if (front_.node_ != first_node_) {
        delete first_node_;
        *first_node_ = nullptr;
        ++first_node_;
    }
}

template <typename T>
deque<T>::const_iterator::const_iterator()
    : first_(nullptr), last_(nullptr), node_(nullptr), cur_(nullptr) {}

template <typename T>
deque<T>::const_iterator::const_iterator(T* cur, T** node)
    : node_(node), cur_(cur) {
    set_node_(node);
}

template <typename T>
typename deque<T>::const_iterator&
deque<T>::const_iterator::operator+=(size_t n) {
    n -= (last_ - cur_);
    node_ += n / BUF_LEN + 1;
    set_node_(node_);
    cur_ = first_ + n % BUF_LEN;
    return *this;
}

template <typename T>
typename deque<T>::const_iterator&
deque<T>::const_iterator::operator-=(size_t n) {
    n -= (cur_ - first_);
    node_ -= n / BUF_LEN + 1;
    set_node_(node_);
    cur_ = last_ - n % BUF_LEN;
    return *this;
}

template <typename T>
template <typename... Args>
deque<T>::iterator::iterator(Args&&... args)
    : const_iterator(std::forward<Args>(args)...) {}
} // namespace mtl
#endif
