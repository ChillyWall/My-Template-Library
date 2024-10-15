#ifndef MTL_DEQUE_H
#define MTL_DEQUE_H

#include <mtl/list>
#include <initializer_list>

namespace mtl {
    template <typename T>
    class deque {
        private:
        list<T>* data_;

        public:
        deque();
        deque(std::initializer_list<T>&& il) noexcept;
        deque(const deque<T>& rhs);
        deque(deque<T>&& rhs) noexcept;
        explicit deque(size_t n);
        ~deque();

        size_t size() const {
            return data_->size();
        }

        bool empty() const {
            return data_->empty();
        }

        void push_front(const T& elem) {
            return data_->push_front(elem);
        }
        void push_front(T&& elem) noexcept {
            return data_->push_front(std::move(elem));
        }

        void push_back(const T& elem) {
            return data_->push_back(elem);
        }
        void push_back(T&& elem) noexcept {
            return data_->push_back(std::move(elem));
        }

        void pop_front() {
            return data_->pop_front();
        }
        void pop_back() {
            return data_->pop_back();
        }

        const T& front() const {
            return data_->front();
        }
        const T& back() const {
            return data_->back();
        }

        T& front() {
            return data_->front();
        }
        T& back() {
            return data_->back();
        }
    };

    template <typename T>
    deque<T>::deque() : data_(new list<T>()) {}

    template <typename T>
    deque<T>::deque(std::initializer_list<T>&& il) noexcept : data_(new list<T>(std::move(il))) {}

    template <typename T>
    deque<T>::deque(size_t n) : data_(new list<T>(n)) {}

    template <typename T>
    deque<T>::deque(const deque<T>& rhs) : data_(new list<T>(*rhs.data_)) {}

    template <typename T>
    deque<T>::deque(deque<T>&& rhs) noexcept : data_(rhs.data_) {
        rhs.data_ = nullptr;
    }

    template <typename T>
    deque<T>::~deque() {
        delete data_;
    }
}

#endif