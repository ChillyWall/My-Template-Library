#ifndef MTL_DEQUE_H
#define MTL_DEQUE_H

#include "mtl/types.h"
#include <initializer_list>
#include <mtl/list.h>

namespace mtl {

/* The deque (double-end queue) ADT.
 * It uses a dynamical array to store pointers to some arrays with fixed length.
 * When push or pop a element from front or back end, it's only needed to add a
 * new node, which ensure that the push and pop operations will take only O(1)
 * time.
 * The BUF_LEN, which is 16 determines how many elements a node will contain.
 */
template <typename T>
class deque {
private:
    /* The default size of map array. In any situations, map_size_ won't be less
     * than it */
    const static size_t DEFAULT_MAP_SIZE = 8;
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

    T* allocate_node_() {
        return new T[BUF_LEN];
    }
    void expand();
    void check_empty() {
        if (empty()) {
            throw EmptyContainer();
        }
    }

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
};

template <typename T>
template <typename V>
void deque<T>::push_back(V&& elem) {
    ++back_;
    ++size_;
    if (back_->node_ == last_node_) {
        if (last_node_ == map_ + map_size_) {
            expand();
        }
        *last_node_ = allocate_node_();
        ++last_node_;
    }
    *back_ = std::forward<V>(elem);
}

template <typename T>
template <typename V>
void deque<T>::push_front(V&& elem) {
    --front_;
    ++size_;
    if (front_->node_ == first_node_ - 1) {
        if (first_node_ == map_) {
            expand();
        }
        --first_node_;
        *first_node_ = allocate_node_();
    }
    *front_ = std::forward<V>(elem);
}

template <typename T>
void deque<T>::pop_back() {
    check_empty();
    --back_;
    --size_;
    if (back_->node_ != last_node_ - 1) {
        --last_node_;
        delete last_node_;
    }
}

template <typename T>
void deque<T>::pop_front() {
    check_empty();
    ++front_;
    --size_;
    if (front_->node_ != first_node_) {
        delete first_node_;
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
