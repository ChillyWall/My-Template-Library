//
// Created by metal on 2024/9/18.
//

#ifndef VECTOR_H
#define VECTOR_H

#include <stdexcept>
#include <initializer_list>

namespace mystl {
    template <typename T>
    class vector {
    private:
        T* data_;
        size_t size_;
        size_t capability_;
        const static size_t DEFAULT_CAPABILITY = 100;
        void expand(size_t new_size);

        void allocate(size_t capability) {
            data_ = new T [capability];
        }

        void check_capability() {
            if (size_ == capability_) {
                expand(size_);
            }
        }

    public:
        vector();
        explicit vector(size_t size);
        vector(std::initializer_list<T>&& elems) noexcept;
        vector(const vector<T>& vec);
        vector(vector<T>&& vec) noexcept;
        ~vector();

        [[nodiscard]] bool empty() const {
            return size_ == 0;
        }

        [[nodiscard]] size_t  size() const {
            return size_;
        }

        [[nodiscard]] size_t capability() const {
            return capability_;
        }

        void clear();

        const T& operator[](size_t index) const;
        T& operator[](size_t index);
        const T& at(size_t index) const;
        T& at(size_t index);

        void push_back(const T& elem);
        void push_back(T&& elem) noexcept;
        void pop_back();

        void shrink() noexcept;

        bool insert(size_t index, const T& elem) noexcept;
        bool insert(size_t index, T&& elem) noexcept;
        bool insert(size_t index, std::initializer_list<T>&& elems) noexcept;
        bool remove(size_t index) noexcept;
        bool remove(size_t begin, size_t stop) noexcept;

        bool operator==(const vector<T>& vec) const {
            return data_ == vec.data_;
        }

        vector<T>& operator=(const vector<T>& vec);
        vector<T>& operator=(vector<T>&& vec) noexcept;

    private:
        class const_iterator {
        protected:
            T* elem_;
        public:
            const_iterator() = default;
            virtual ~const_iterator() = default;
            explicit const_iterator(T* elem);
            const_iterator(const const_iterator& ci);
            const_iterator(const_iterator&& ci) noexcept;

            const T& operator*() const;

            bool operator>(const const_iterator& ci) const {
                return elem_ > ci.elem_;
            }

            bool operator<(const const_iterator& ci) const {
                return elem_ < ci.elem_;
            }

            bool operator<=(const const_iterator& ci) const {
                return elem_ <= ci.elem_;
            }

            bool operator>=(const const_iterator& ci) const {
                return elem_ <= ci.elem_;
            }

            bool operator==(const const_iterator& ci) const {
                return elem_ == ci.elem_;
            }

            const_iterator operator+(size_t n);
            const_iterator& operator+=(size_t n);
            const_iterator operator-(size_t n);
            const_iterator& operator-=(size_t n);

            const_iterator& operator++();
            const_iterator operator++(int);
            const_iterator& operator--();
            const_iterator operator--(int);
        };

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
        const_iterator cbegin() const {
            return const_iterator(&(data_[0]));
        }

        const_iterator cend() const {
            return const_iterator(&(data_[size_]));
        }

        const_iterator begin() const {
            return cbegin();
        }

        const_iterator end() const {
            return cend();
        }

        iterator begin() {
            return iterator(&(data_[0]));
        }

        iterator end() {
            return iterator(&(data_[size_]));
        }
    };

    template <typename T>
    vector<T>::vector() : data_{nullptr}, size_{0}, capability_(DEFAULT_CAPABILITY) {}

    template <typename T>
    vector<T>::vector(size_t size) : size_{size} {
        capability_ = size_ * 2;
        capability_ = capability_ > DEFAULT_CAPABILITY ? capability_ : DEFAULT_CAPABILITY;
        allocate(capability_);
    }

    template <typename T>
    vector<T>::vector(std::initializer_list<T>&& elems) noexcept :
        size_{elems.size()} {
        capability_ = size_ * 2;
        capability_ = capability_ > DEFAULT_CAPABILITY ? capability_ : DEFAULT_CAPABILITY;
        allocate(capability_);
        auto itr = elems.begin();
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = *itr;
            ++itr;
        }
    }

    template <typename T>
    vector<T>::vector(const vector<T>& vec) :
        size_{vec.size_}, capability_{vec.capability_} {
        allocate(capability_);
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = vec.data_[i];
        }
    }

    template <typename T>
    vector<T>::vector(vector<T>&& vec) noexcept :
        data_{vec.data_}, size_{vec.size_}, capability_{vec.capability_} {
        vec.data_ = nullptr;
    }

    template <typename T>
    vector<T>::~vector() {
        delete[] data_;
    }

    template <typename T>
    void vector<T>::clear() {
        delete [] data_;
        data_ = nullptr;
        size_ = 0;
        capability_ = 0;
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
    void vector<T>::shrink() noexcept {
        capability_ = size_ * 2;
        capability_ = capability_ > DEFAULT_CAPABILITY ? capability_ : DEFAULT_CAPABILITY;
        auto old = data_;
        allocate(capability_);
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = std::move(old[i]);
        }

        delete [] old;
    }

    template <typename T>
    void vector<T>::push_back(const T& elem) {
        if (size_ >= capability_) {
            expand(size_);
        }
        data_[size_++] = elem;
    }

    template <typename T>
    void vector<T>::push_back(T&& elem) noexcept {
        if (size_ >= capability_) {
            expand(size_);
        }
        data_[size_++] = std::move(elem);
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
        if (index > size_) {
            return false;
        }

        if (size_ >= capability_) {
            expand(size_);
        }

        for (size_t i = size_; i > index; --i) {
            data_[i] = std::move(data_[i - 1]);
        }
        data_[index] = elem;
        return true;
    }

    template <typename T>
    bool vector<T>::insert(size_t index, T&& elem) noexcept {
        if (index > size_) {
            return false;
        }

        if (size_ > capability_) {
            expand(size_);
        }

        for (size_t i = size_; i > index; --i) {
            data_[i] = std::move(data_[i - 1]);
        }
        data_[index] = std::move(elem);
        return true;
    }

    template <typename T>
    bool vector<T>::insert(size_t index, std::initializer_list<T>&& elems) noexcept{
        if (index > size_) {
            return false;
        }

        size_t new_size = size_ + elems.size();
        if (new_size > capability_) {
            expand(new_size);
        }

        for (size_t i = new_size - 1; i >= index + elems.size(); ++i) {
            data_[i] = data_[i - elems.size()];
        }
        auto itr = elems.begin();
        for (size_t i = index; i < index + elems.size(); ++i) {
            data_[i] = std::move(*itr);
            ++itr;
        }

        return true;
    }

    template <typename T>
    bool vector<T>::remove(size_t index) noexcept {
        if (index >= size_) {
            return false;
        }

        for (size_t i = index; i < size_ - 1; ++i) {
            data_[i] = std::move(data_[i + 1]);
        }
        --size_;
        return true;
    }

    template <typename T>
    bool vector<T>::remove(size_t begin, size_t stop) noexcept {
        if (begin >= stop || begin >= size_ || stop > size_) {
            return false;
        }

        size_t wid = stop - begin;
        for (size_t i = 0; i < wid; ++i) {
            data_[begin + i] = std::move(data_[stop + i]);
        }

        size_ -= wid;
        return true;
    }

    template <typename T>
    vector<T>& vector<T>::operator=(const vector<T>& vec) {
        if (this == &vec) {
            return *this;
        }

        clear();
        size_ = vec.size_;
        capability_ = vec.capability_;
        allocate(capability_);

        for (size_t i = 0; i < size_; ++i) {
            data_[i] = vec.data_[i];
        }

        return *this;
    }

    template <typename T>
    vector<T>& vector<T>::operator=(vector<T>&& vec) noexcept {
        clear();
        size_ = vec.size_;
        capability_ = vec.capability_;
        data_ = vec.data_;
        vec.data_ = nullptr;
        return *this;
    }

    template<typename T>
    void vector<T>::expand(size_t new_size) {
        auto old = data_;
        capability_ = new_size * 2;
        capability_ = capability_ > DEFAULT_CAPABILITY ? capability_ : DEFAULT_CAPABILITY;
        allocate(capability_);
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
