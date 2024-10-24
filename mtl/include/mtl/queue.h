#ifndef MTL_QUEUE_H
#define MTL_QUEUE_H

#include <initializer_list>
#include "list.h"
#include "types.h"

namespace mtl {
    template <typename T>
    class queue {
        private:
        list<T>* data_;

        public:
        queue();
        explicit queue(size_t s);
        explicit queue(std::initializer_list<T>&& il) noexcept;
        queue(const queue<T>& rhs);
        queue(queue<T>&& rhs) noexcept;
        ~queue();

        size_t size() const {
            return data_->size();
        }

        bool empty() const {
            return data_->empty();
        }

        void push(const T& elem) {
            data_->push_back(elem);
        }

        void push(T&& elem) noexcept {
            data_->push_back(std::move(elem));
        }

        void pop() {
            data_->pop_front();
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
    queue<T>::queue() : data_(new list<T>()) {}

    template <typename T>
    queue<T>::queue(size_t s) : data_(new list<T>(s)) {}

    template <typename T>
    queue<T>::queue(std::initializer_list<T>&& il) noexcept : data_(new list<T>(std::move(il))) {}

    template <typename T>
    queue<T>::queue(const queue& rhs) : data_(new list<T>(*rhs.data_)) {}

    template <typename T>
    queue<T>::queue(queue<T>&& rhs) noexcept : data_(rhs.data_) {
        rhs.data_ = nullptr;
    }

    template <typename T>
    queue<T>::~queue() {
        delete data_;
    }
}

#endif
