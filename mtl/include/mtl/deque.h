#ifndef MTL_DEQUE_H
#define MTL_DEQUE_H

#include <initializer_list>
#include <mtl/list.h>

namespace mtl {
    template <typename T, size_t buf_len = 8>
    class deque {
    private:
        T** map_;         // the map of nodes
        size_t size_;     // the number of elements
        size_t map_size_; // the size of the map
        T** first_node_;  // the first node
        T** last_node_;   // the node past the last node
        T* front_;        // the front element
        T* back_;         // the back element

        class const_iterator {
        private:
            T* first_; // the first element of current node
            T* last_;  // the element past the last element of current node
            T** node_; // the current node
            T* cur_;   // the current element

            void set_node_(T* new_node) {
                node_ = new_node;
                first_ = *node_;
                last_ = first_ + buf_len;
            }

        public:
            const_iterator();
            const_iterator(T* cur, T** node);
            const_iterator(const const_iterator& rhs) = default;

            const_iterator& operator=(const const_iterator& rhs);

            const_iterator& operator++() {
                ++cur_;
                if (cur_ == last_) {
                    set_node_(++node_);
                    cur_ = first_;
                }
                return *this;
            }

            const_iterator& operator--() {
                if (cur_ == first_) {
                    set_node_(--node_);
                    cur_ = last_;
                }
                --cur_;
                return *this;
            }

            const_iterator operator++(int);
            const_iterator operator--(int);

            const_iterator operator+=(size_t n);
            const_iterator operator-=(size_t n);
            const_iterator operator+(size_t n) const;
            const_iterator operator-(size_t n) const;
        };

    public:
    };
} // namespace mtl
//
#endif
