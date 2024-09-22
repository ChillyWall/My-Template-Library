//
// Created by metal on 2024/9/18.
//

#ifndef VECTOR_H
#define VECTOR_H

#include <mtl/algorithms.h>
#include <stdexcept>
#include <initializer_list>

// The namespace where the ADTs are.
namespace mtl {
    /* The vector ADT, it can expand its data array to double size when space is not enough. */
    template <typename T>
    class vector {
    private:
        // the array contain the data
        T* data_;   

        // the number of the elements
        size_t size_;  

        // the length of the array
        size_t capacity_;   

        // the default capacity, vector ensures that the capacity won't be smaller than it
        const static size_t DEFAULT_CAPACITY = 100;   

        // expand the array, the new capacity is new_size * 2
        void expand(size_t new_size);  

        /* allocate a new array with length capacity
           it don't delete the original array */
        void allocate(size_t capacity) {
            data_ = new T [capacity];
        }

        // check whether the capacity is enough and expand when needed
        void check_capacity() {
            if (size_ == capacity_) {
                expand(size_);
            }
        }

    public:
        // the default constructor
        vector();  

        // construct the vector with particular size
        explicit vector(size_t size);   

        // construct from initializer list, the size will be the same with the il.
        vector(std::initializer_list<T>&& elems) noexcept;   

        // copy constructor
        vector(const vector<T>& vec);  

        // moving copy constructor
        vector(vector<T>&& vec) noexcept;  

        // the destructor
        ~vector();   


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
            return capacity_;
        }

        // delete the array and assign nullptr to data_
        void clear() {
            delete [] data_;
            data_ = nullptr; 
            size_ = 0;
            capacity_ = 0;
        }

        /* return the reference to the element at position index 
            it don't check the boundary */
        const T& operator[](size_t index) const;

        // the const version
        T& operator[](size_t index);  

        /* the same with operator[] but check the boundary 
            it throw an out_of_range exception */
        const T& at(size_t index) const;

        // the const version
        T& at(size_t index);  

        // return a vector contains the elements [begin, stop)
        vector<T> splice(size_t begin, size_t stop);

        // append an element to the end of the vector
        void push_back(const T& elem);   

        // the version using right-value reference
        void push_back(T&& elem) noexcept;   

        // remove the last element (simply decrease the size_)
        void pop_back();     

        // reduce the length of the array to size_ * 2
        void shrink() noexcept;  
        
        // insert an element at position index
        bool insert(size_t index, const T& elem) noexcept;   

        // using the right-value reference
        bool insert(size_t index, T&& elem) noexcept;        

        /* insert another from another container (deep copy) with iterators
           which provide ++, --, ==, and != operators*/
        template <typename InputIterator>
        bool insert(size_t index, InputIterator begin, InputIterator end);

        // remove the elements at position index
        bool remove(size_t index) noexcept;    

        // remove the range [begin, stop)
        bool remove(size_t begin, size_t stop) noexcept;  

        // return whether two vector is equal (whether the data_ is equal)
        bool operator==(const vector<T>& vec) const {
            return data_ == vec.data_;
        }

        // the copy assignment operator
        vector<T>& operator=(const vector<T>& vec);  

        // the moving assignment operator
        vector<T>& operator=(vector<T>&& vec) noexcept;  

    private:

        /* the iterator that cannot modify the element it refers but can change which object if refers */
        class const_iterator {
        protected:
            T* elem_;   // point to the element
        public:
            const_iterator() = default;
            virtual ~const_iterator() = default;

            // construct from pointer
            explicit const_iterator(T* elem);   

            const_iterator(const const_iterator& ci);
            const_iterator(const_iterator&& ci) noexcept;

            const T& operator*() const;   // return a reference to the element

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
                return elem_ <= ci.elem_;
            }

            // compare the pointer
            bool operator==(const const_iterator& ci) const {
                return elem_ == ci.elem_;
            }

            // compare the pointer
            bool operator!=(const const_iterator& ci) const {
                return elem_ != ci.elem_;
            }

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
            iterator(const iterator& itr);
            iterator(iterator&& itr) noexcept;
            using const_iterator::const_iterator;
            ~iterator() override = default;

            T& operator*() {
                return const_cast<T&>(const_iterator::operator*());
            }
            iterator operator+(size_t n) {
                auto new_itr = *this;
                return new_itr.operator+=(n);
            }
            iterator& operator+=(size_t n) {
                const_iterator::operator+=(n);
                return *this;
            }
            iterator operator-(size_t n) {
                auto new_itr = *this;
                return new_itr.operator-=(n);
            } 
            iterator& operator-=(size_t n) {
                const_iterator::operator+=(n);
                return *this;
            } 

            iterator& operator++();
            iterator operator++(int);
            iterator& operator--();
            iterator operator--(int);
        };

    public:
        // return a const_iterator pointing to the position 0
        const_iterator cbegin() const {
            return const_iterator(&(data_[0]));
        }

        // return a const_iterator pointing to the position after the last element
        const_iterator cend() const {
            return const_iterator(&(data_[size_]));
        }

        // return a const_iterator when the object is const
        const_iterator begin() const {
            return cbegin();
        }

        // return a const_iteartor when the object is const
        const_iterator end() const {
            return cend();
        }

        // return an iterator pointing to the first element
        iterator begin() {
            return iterator(&(data_[0]));
        }

        // return an iterator pointing to the element behind the last one
        iterator end() {
            return iterator(&(data_[size_]));
        }
    };

    template <typename T>
    vector<T>::vector() : size_{0}, capacity_(DEFAULT_CAPACITY) {
        allocate(capacity_); 
    }

    template <typename T>
    vector<T>::vector(size_t size) : size_{0} {
        capacity_ = size * 2;
        capacity_ = capacity_ > DEFAULT_CAPACITY ? capacity_ : DEFAULT_CAPACITY;
        allocate(capacity_);
    }

    template <typename T>
    vector<T>::vector(std::initializer_list<T>&& elems) noexcept :
        size_{elems.size()} {
        capacity_ = size_ * 2;
        capacity_ = capacity_ > DEFAULT_CAPACITY ? capacity_ : DEFAULT_CAPACITY;
        allocate(capacity_);
        auto itr = elems.begin();
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = *itr;
            ++itr;
        }
    }

    template <typename T>
    vector<T>::vector(const vector<T>& vec) :
        size_{vec.size_}, capacity_{vec.capacity_} {
        allocate(capacity_);
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = vec.data_[i];
        }
    }

    template <typename T>
    vector<T>::vector(vector<T>&& vec) noexcept :
        data_{vec.data_}, size_{vec.size_}, capacity_{vec.capacity_} {
        vec.data_ = nullptr;
        vec.clear();
    }

    template <typename T>
    vector<T>::~vector() {
        delete[] data_;
    }

    template <typename T>
    const T& vector<T>::operator[](size_t index) const {
        return data_[index];
    }

    template <typename T>
    T& vector<T>::operator[](size_t index) {
        return const_cast<T&>(static_cast<const vector<T>>(*this)[index]);
    }

    template <typename T>
    const T& vector<T>::at(size_t index) const {
        if (index < size_) {
            return data_[index];
        } else {
            throw std::out_of_range("The index is out of range.");
        }
    }

    template <typename T>
    T& vector<T>::at(size_t index) {
        return const_cast<T&>(static_cast<const vector<T>>(*this).at(index));
    }

    template <typename T>
    vector<T> vector<T>::splice(size_t begin, size_t stop) {
        size_t size = stop - begin;
        vector<T> vec(size);
        for (size_t i = 0; i < size; ++i) {
            vec.data_[i] = data_[begin + i];
        }

        return vec;
    }
    
    template <typename T>
    void vector<T>::shrink() noexcept {
        capacity_ = size_ * 2;
        capacity_ = capacity_ > DEFAULT_CAPACITY ? capacity_ : DEFAULT_CAPACITY;
        auto old = data_;
        allocate(capacity_);
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = std::move(old[i]);
        }

        delete [] old;
    }

    template <typename T>
    void vector<T>::push_back(const T& elem) {
        if (size_ >= capacity_) {
            expand(size_);
        }
        data_[size_] = elem;
        ++size_;
    }

    template <typename T>
    void vector<T>::push_back(T&& elem) noexcept {
        if (size_ >= capacity_) {
            expand(size_);
        }
        data_[size_] = std::move(elem);
        ++size_;
    }

    template <typename T>
    void vector<T>::pop_back() {
        if (size_ == 0) {
            throw std::runtime_error("There's no element to be popped out.");
        }
        --size_;
    }

    template <typename T>
    bool vector<T>::insert(size_t index, const T& elem) noexcept {
        // check the validity of index
        if (index > size_) {
            return false;
        }

        // check the capacity
        if (size_ >= capacity_) {
            expand(size_);
        }

        for (size_t i = size_; i > index; --i) {
            data_[i] = std::move(data_[i - 1]);
        }
        data_[index] = elem;
        ++size_;
        return true;
    }

    template <typename T>
    bool vector<T>::insert(size_t index, T&& elem) noexcept {
        if (index > size_) {
            return false;
        }
        if (size_ > capacity_) {
            expand(size_);
        }
        for (size_t i = size_; i > index; --i) {
            data_[i] = std::move(data_[i - 1]);
        }
        data_[index] = std::move(elem);
        ++size_;
        return true;
    }

    template <typename T> template <typename InputIterator>
    bool vector<T>::insert(size_t index, InputIterator begin, InputIterator end) {
        // check the validity of index
        if (index > size_) {
            return false;
        }

        size_t len = count_length(begin, end);

        // check whether the capacity is big enough
        size_ += len;
        if (size_ > capacity_) {
            expand(size_);
        }

        // move elements backward
        for (size_t i = size_ - 1; i >= index + len; --i) {
            data_[i] = data_[i - len]; 
        }

        // place elements in the gap
        auto itr = begin;
        for (size_t i = index; itr != end ; ++i, ++itr) {
            data_[i] = *itr;
        }

        return true;
    }

    template <typename T>
    bool vector<T>::remove(size_t index) noexcept {
        // check whether the position is valid
        if (index >= size_) {
            return false;
        }

        // move the following elements
        for (size_t i = index; i < size_ - 1; ++i) {
            data_[i] = std::move(data_[i + 1]);
        }
        --size_;

        return true;
    }

    template <typename T>
    bool vector<T>::remove(size_t begin, size_t stop) noexcept {
        // check whether the range is valid
        if (begin >= stop || begin >= size_ || stop > size_) {
            return false;
        }

        // move the elements
        size_t wid = stop - begin;
        for (size_t i = 0; i < wid; ++i) {
            data_[begin + i] = std::move(data_[stop + i]);
        }

        size_ -= wid;
        return true;
    }

    template <typename T>
    vector<T>& vector<T>::operator=(const vector<T>& vec) {
        // process the self-assignment
        if (this == &vec) {
            return *this;
        }

        // delete original array
        clear();
        size_ = vec.size_;
        capacity_ = vec.capacity_;
        allocate(capacity_);

        // copy every element
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = vec.data_[i];
        }

        return *this;
    }

    template <typename T>
    vector<T>& vector<T>::operator=(vector<T>&& vec) noexcept {
        if (this == &vec) {
            return *this;
        }
        // delete original array
        clear();

        // copy the object
        size_ = vec.size_;
        capacity_ = vec.capacity_;
        data_ = vec.data_;

        vec.data_ = nullptr;
        vec.clear();

        return *this;
    }

    template<typename T>
    void vector<T>::expand(size_t new_size) {
        if (new_size * 2 < capacity_) {
            return;
        }
        // backup the original array
        auto old = data_;

        // create a new array
        capacity_ = new_size * 2;
        allocate(capacity_);

        // move the elements
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = old[i];
        }

        delete [] old;
    }

    template <typename T>
    vector<T>::const_iterator::const_iterator(T* elem) {
        elem_ = elem;
    }

    template <typename T>
    vector<T>::const_iterator::const_iterator(const const_iterator& ci) : elem_{ci.elem_} {}

    template <typename T>
    vector<T>::const_iterator::const_iterator(const_iterator&& ci) noexcept {
        elem_ = ci.elem_;
        ci.elem_ = nullptr;
    }

    template <typename T>
    const T& vector<T>::const_iterator::operator*() const {
        return *elem_;
    }

    template <typename T>
    typename vector<T>::const_iterator& vector<T>::const_iterator::operator+=(size_t n) {
        data_ += n;
        return *this;
    }

    template <typename T>
    typename vector<T>::const_iterator& vector<T>::const_iterator::operator-=(size_t n) {
        data_ -= n;
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
    vector<T>::iterator::iterator(const iterator& itr) : const_iterator(itr) {}

    template <typename T>
    vector<T>::iterator::iterator(iterator&& itr) noexcept : const_iterator(std::move(itr)) {}

    template <typename T>
    typename vector<T>::iterator& vector<T>::iterator::operator++() {
        ++const_iterator::elem_;
        return *this;
    }

    template <typename T>
    typename vector<T>::iterator vector<T>::iterator::operator++(int) {
        auto old = *this;
        ++const_iterator::elem_;
        return old;
    }

    template <typename T>
    typename vector<T>::iterator& vector<T>::iterator::operator--() {
        --const_iterator::elem_;
        return *this;
    }

    template <typename T>
    typename vector<T>::iterator vector<T>::iterator::operator--(int) {
        auto old = *this;
        --const_iterator::elem_;
        return old;
    }
}
#endif //VECTOR_H
