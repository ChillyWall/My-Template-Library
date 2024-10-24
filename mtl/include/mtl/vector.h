//
// Created by metal on 2024/9/18.
//

#ifndef MTL_VECTOR_H
#define MTL_VECTOR_H

#include "algorithms.h"
#include <stdexcept>
#include <initializer_list>
#include "basic_vector.h"

// The namespace where the ADTs are.
namespace mtl {
    /* The vector ADT, it can expand its data array to double size when space is not enough. */
    template <typename T>
    class vector : public basic_vector<T> {
    private:
        // the number of elements
        size_t size_;

        /* the iterator that cannot modify the element it refers but can change which object if refers */
        class const_iterator {
        private:
            const T* elem_;   // pointer to the element
        public:
            const_iterator();
            virtual ~const_iterator() = default;

            // construct from pointer
            explicit const_iterator(const T* elem);   

            const_iterator(const const_iterator& ci);
            const_iterator(const_iterator&& ci) noexcept;
            // return a reference to the element           
            const T& operator*() const;   

            // compare the pointer
            bool operator>(const const_iterator& ci) const {
                return elem_ > ci.elem_;
            }

            // compare the pointer
            bool operator<(const const_iterator& ci) const {
                return elem_ < ci.elem_;
            }

            // compare the pointer
            bool operator<=(const const_iterator& ci) const {
                return elem_ <= ci.elem_;
            }

            // compare the pointer
            bool operator>=(const const_iterator& ci) const {
                return elem_ >= ci.elem_;
            }

            // compare the pointer
            bool operator==(const const_iterator& ci) const {
                return elem_ == ci.elem_;
            }

            // compare the pointer
            bool operator!=(const const_iterator& ci) const {
                return elem_ != ci.elem_;
            }

            explicit operator bool() const {
                return elem_;
            }

            const_iterator& operator=(const const_iterator& ci);
            const_iterator& operator=(const_iterator&& ci) noexcept;

            // move n items next, it don't check the boundary
            const_iterator operator+(size_t n);
            // move n items next, it don't check the boundary
            const_iterator& operator+=(size_t n);
            // move n items previous, it don't check the boundary
            const_iterator operator-(size_t n);
            // move n items previous, it don't check the boundary
            const_iterator& operator-=(size_t n);

            // prefix increment
            const_iterator& operator++();
            // postfix increment
            const_iterator operator++(int);
            // prefix decrement
            const_iterator& operator--();
            // postfix decrement
            const_iterator operator--(int);
        };

        /* The normal iterator which derived from the const_iterator 
           Both the value of the element and which element it refers are modifiable */
        class iterator : public const_iterator {
        public:
            iterator() = default;
            explicit iterator(T* elem);
            iterator(const iterator& itr);
            iterator(iterator&& itr) noexcept;
            ~iterator() override = default;

            T& operator*() {
                return const_cast<T&>(const_iterator::operator*());
            }

            iterator& operator+=(size_t n) {
                const_iterator::operator+=(n);
                return *this;
            }
            iterator operator+(size_t n) {
                auto new_itr = *this;
                return new_itr.operator+=(n);
            }
            iterator& operator-=(size_t n) {
                const_iterator::operator-=(n);
                return *this;
            }
            iterator operator-(size_t n) {
                auto new_itr = *this;
                return new_itr.operator-=(n);
            } 

            iterator& operator=(const iterator& itr);
            iterator& operator=(iterator&& itr) noexcept;

            iterator& operator++();
            iterator operator++(int);
            iterator& operator--();
            iterator operator--(int);
        };

    public:
        // the default constructor
        vector();  

        // construct the vector with particular size
        explicit vector(size_t s);   

        // construct from initializer list, the size will be the same with the il.
        vector(std::initializer_list<T>&& elems) noexcept;   

        // copy constructor
        vector(const vector<T>& vec);  

        // moving copy constructor
        vector(vector<T>&& vec) noexcept;  

        // the destructor
        virtual ~vector() = default;

        // return whether the vector is empty
        [[nodiscard]] bool empty() const {
            return size_ == 0;
        }

        // return the size
        [[nodiscard]] size_t  size() const {
            return size_;
        }

        // return the capacity
        [[nodiscard]] size_t capacity() const {
            return basic_vector<T>::capacity();
        }

        virtual void shrink() {
            basic_vector<T>::shrink(size_);
        }

        // delete the array and assign nullptr to data_
        virtual void clear() {
            basic_vector<T>::clear();
            size_ = 0;
        }

        /* return the reference to the element at position index 
           it don't check the boundary */
        virtual const T& operator[](size_t index) const {
            return basic_vector<T>::data()[index];
        }

        // the const version
        virtual T& operator[](size_t index) {
            return basic_vector<T>::data()[index];
        }

        /* the same with operator[] but check the boundary 
            it throw an out_of_range exception */
        const T& at(size_t index) const;

        // the const version
        T& at(size_t index);  

        // return a vector contains the elements [begin, stop)
        vector<T> splice(size_t begin, size_t stop);

        const T& front() const {
            if (size_ == 0) {
                throw std::out_of_range("There's no element in this vector.");
            }
            return basic_vector<T>::data()[0];
        }

        const T& back() const {
            if (size_ == 0) {
                throw std::out_of_range("There's no element in this vector.");
            }
            return basic_vector<T>::data[size_ - 1];
        }

        T& front() {
            return const_cast<T&>(static_cast<const vector<T>*>(this)->front());
        }

        T& back() {
            return const_cast<T&>(static_cast<const vector<T>*>(this)->back());
        }

        // append an element to the end of the vector
        void push_back(const T& elem);   

        // the version using right-value reference
        void push_back(T&& elem) noexcept;   

        // remove the last element (simply decrease the size_)
        void pop_back();     

        /* insert an element at position index, r
        return an iterator pointing to the next cell */
        iterator insert(iterator index, const T& elem) noexcept;   

        // using the right-value reference
        iterator insert(iterator index, T&& elem) noexcept;        

        /* insert another from another container (deep copy) with iterators
           which provide ++, --, ==, and != operators*/
        template <typename InputIterator>
        iterator insert(iterator index, InputIterator begin, InputIterator end);

        // remove the elements at position index
        iterator remove(iterator index) noexcept;    

        // remove the range [begin, stop)
        iterator remove(iterator begin, iterator stop) noexcept;  

        // return whether two vector is equal (whether the data_ is equal)
        bool operator==(const vector<T>& vec) const {
            return basic_vector<T>::data() == vec.data();
        }

        // the copy assignment operator
        vector<T>& operator=(const vector<T>& vec);  

        // the moving assignment operator
        vector<T>& operator=(vector<T>&& vec) noexcept;  

        // return a const_iterator pointing to the position 0
        const_iterator cbegin() const {
            return const_iterator(basic_vector<T>::data());
        }

        // return a const_iterator pointing to the position after the last element
        const_iterator cend() const {
            return const_iterator(basic_vector<T>::data() + size_);
        }

        // return an iterator pointing to the first element
        iterator begin() {
            return iterator(basic_vector<T>::data());
        }

        // return an iterator pointing to the element behind the last one
        iterator end() {
            return iterator(basic_vector<T>::data() + size_);
        }

        // return a const_iterator pointing to the position 0
        const_iterator begin() const {
            return const_iterator(basic_vector<T>::data());
        }

        // return a const_iterator pointing to the position after the last element
        const_iterator end() const {
            return const_iterator(basic_vector<T>::data() + size_);
        }
    };

    template <typename T>
    vector<T>::vector() : size_(0) {}

    template <typename T>
    vector<T>::vector(size_t n) : size_(0), basic_vector<T>(n) {}

    template <typename T>
    vector<T>::vector(std::initializer_list<T>&& il) noexcept : size_(il.size()), basic_vector<T>(il.size()) {
        auto itr = il.begin();
        auto data_ = basic_vector<T>::data();
        for (int i = 0; i < size_; ++i) {
            data_[i] = std::move(*(itr++));
        }
    }

    template <typename T>
    vector<T>::vector(const vector<T>& rhs) : size_(rhs.size_), basic_vector<T>(rhs) {}

    template <typename T>
    vector<T>::vector(vector<T>&& rhs) noexcept : 
        size_(rhs.size_), basic_vector<T>(std::move(rhs)) {
        rhs.size_ = 0;
    }

    template <typename T>
    const T& vector<T>::at(size_t index) const {
        if (index < size_) {
            return basic_vector<T>::data[index];
        } else {
            throw std::out_of_range("The index is out of range.");
        }
    }

    template <typename T>
    T& vector<T>::at(size_t index) {
        return const_cast<T&>(static_cast<const vector<T>*>(this)->at(index));
    }

    template <typename T>
    vector<T> vector<T>::splice(size_t begin, size_t stop) {
        size_t size = stop - begin;
        vector<T> vec(size);
        for (size_t i = 0; i < size; ++i) {
            vec[i] = basic_vector<T>::data[begin + i];
        }

        return vec;
    }

    template <typename T>
    void vector<T>::push_back(const T& elem) {
        if (size_ >= basic_vector<T>::capacity()) {
            basic_vector<T>::expand(size_ * 2);
        }
        basic_vector<T>::data()[size_] = elem;
        ++size_;
    }

    template <typename T>
    void vector<T>::push_back(T&& elem) noexcept {
        if (size_ >= basic_vector<T>::capacity()) {
            basic_vector<T>::expand(size_ * 2);
        }
        basic_vector<T>::data()[size_] = std::move(elem);
        ++size_;
    }

    template <typename T>
    void vector<T>::pop_back() {
        if (size_ == 0) {
            throw std::out_of_range("There's no element to be popped out.");
        }
        --size_;
    }

    template <typename T>
    typename vector<T>::iterator vector<T>::insert(iterator index, const T& elem) noexcept {
        // check the validity of index
        if (index > this->end()) {
            return iterator();
        }
        // check the capacity
        if (size_ >= basic_vector<T>::capacity()) {
            size_t len = count_length(this->begin(), index);
            basic_vector<T>::expand(size_ * 2);
            index = this->begin() + len;
        }

        for (iterator i = this->end(), j = i - 1; i > index; --i, --j) {
            *i = std::move(*j);
        }
        *index = elem;
        ++size_;
        return ++index;
    }

    template <typename T>
    typename vector<T>::iterator vector<T>::insert(iterator index, T&& elem) noexcept {
        if (index > this->end()) {
            return iterator();
        }
        if (size_ >= basic_vector<T>::capacity()) {
            size_t len = count_length(this->begin(), index);
            basic_vector<T>::expand(size_ * 2);
            index = this->begin() + len;
        }
        for (iterator i = this->end(), j = i - 1; i != index; --i, --j) {
            *i = std::move(*j);
        }
        *index = std::move(elem);
        ++size_;
        return ++index; 
    }

    template <typename T> template <typename InputIterator>
    typename vector<T>::iterator vector<T>::insert(iterator index, InputIterator begin, InputIterator end) {
        // check the validity of index
        if (index > this->end()) {
            return iterator();
        }

        size_t len = count_length(begin, end);

        // check whether the capacity is big enough
        size_ += len;
        if (size_ > basic_vector<T>::capacity()) {
            size_t len2 = count_length(this->begin(), index);
            basic_vector<T>::expand(size_ * 2);
            index = this->begin() + len2;
        }

        // move elements backward
        for (iterator i = this->end() - 1, j = i - len; i >= index + len; --i, --j) {
            *i = *j;
        }

        // place elements in the gap
        for (auto itr = begin; itr != end ; ++index, ++itr) {
            *index = *itr;
        }

        return index;
    }

    template <typename T>
    typename vector<T>::iterator vector<T>::remove(iterator index) noexcept {
        // check whether the position is valid
        if (index >= this->end()) {
            return iterator();
        }

        // move the following elements
        for (iterator i = index; i != this->end() - 1; ++i) {
            *i = std::move(*(i + 1));
        }
        --size_;

        return index;
    }

    template <typename T>
    typename vector<T>::iterator vector<T>::remove(iterator begin, iterator stop) noexcept {
        // check whether the range is valid
        if (begin >= stop || begin >= this->end() || stop > this->end()) {
            return iterator();
        }

        // move the elements
        size_t wid = count_length(begin, stop);
        auto itr1 = begin;
        auto itr2 = stop;
        while (itr2 != this->end()) {
            *(itr1++) = std::move(*(itr2++));
        }

        size_ -= wid;
        return begin;
    }

    template <typename T>
    vector<T>::const_iterator::const_iterator() : elem_(nullptr) {}

    template <typename T>
    vector<T>::const_iterator::const_iterator(const T* elem) : elem_(elem) {}

    template <typename T>
    vector<T>::const_iterator::const_iterator(const const_iterator& ci) : elem_{ci.elem_} {}

    template <typename T>
    vector<T>::const_iterator::const_iterator(const_iterator&& ci) noexcept : elem_(ci.elem_) {
        ci.elem_ = nullptr;
    }

    template <typename T>
    const T& vector<T>::const_iterator::operator*() const {
        return *elem_;
    }

    template <typename T>
    typename vector<T>::const_iterator& vector<T>::const_iterator::operator=(const const_iterator& ci) {
        elem_ = ci.elem_;
        return *this;
    }

    template <typename T>
    typename vector<T>::const_iterator& vector<T>::const_iterator::operator=(const_iterator&& ci) noexcept{
        elem_ = ci.elem_;
        return *this;
    }

    template <typename T>
    typename vector<T>::const_iterator& vector<T>::const_iterator::operator+=(size_t n) {
        elem_ += n;
        return *this;
    }

    template <typename T>
    typename vector<T>::const_iterator& vector<T>::const_iterator::operator-=(size_t n) {
        elem_ -= n;
        return *this;
    }

    template <typename T>
    typename vector<T>::const_iterator vector<T>::const_iterator::operator+(size_t n) {
        auto new_itr = *this;
        new_itr += n;
        return new_itr;
    }

    template <typename T>
    typename vector<T>::const_iterator vector<T>::const_iterator::operator-(size_t n) {
        auto new_itr = *this;
        new_itr -= n;
        return new_itr;
    }

    template <typename T>
    typename vector<T>::const_iterator& vector<T>::const_iterator::operator++() {
        ++elem_;
        return *this;
    }

    template <typename T>
    typename vector<T>::const_iterator vector<T>::const_iterator::operator++(int) {
        auto old = *this;
        ++elem_;
        return old;
    }

    template <typename T>
    typename vector<T>::const_iterator& vector<T>::const_iterator::operator--() {
        --elem_;
        return *this;
    }

    template <typename T>
    typename vector<T>::const_iterator vector<T>::const_iterator::operator--(int) {
        auto old = *this;
        --elem_;
        return old;
    }

    template <typename T>
    vector<T>::iterator::iterator(T* elem) : const_iterator(elem) {}

    template <typename T>
    vector<T>::iterator::iterator(const iterator& itr) : const_iterator(itr) {}

    template <typename T>
    vector<T>::iterator::iterator(iterator&& itr) noexcept : const_iterator(std::move(itr)) {}

    template <typename T>
    typename vector<T>::iterator& vector<T>::iterator::operator=(const iterator& itr) {
        const_iterator::operator=(itr);
        return *this;
    }

    template <typename T>
    typename vector<T>::iterator& vector<T>::iterator::operator=(iterator&& itr) noexcept {
        const_iterator::operator=(std::move(itr));
        return *this;
    }

    template <typename T>
    typename vector<T>::iterator& vector<T>::iterator::operator++() {
        const_iterator::operator++();
        return *this;
    }

    template <typename T>
    typename vector<T>::iterator vector<T>::iterator::operator++(int) {
        auto old = *this;
        const_iterator::operator++();
        return old;
    }

    template <typename T>
    typename vector<T>::iterator& vector<T>::iterator::operator--() {
        const_iterator::operator--();
        return *this;
    }

    template <typename T>
    typename vector<T>::iterator vector<T>::iterator::operator--(int) {
        auto old = *this;
        const_iterator::operator--();
        return old;
    }
}
#endif //VECTOR_H
