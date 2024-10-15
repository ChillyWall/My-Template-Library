#ifndef STACK_H
#define STACK_H

#include <mtl/vector.h>
#include <initializer_list>

namespace mtl {
    template <typename T>
    class stack {
        private:
        vector<T>* data_;

        public:
        stack();
        stack(std::initializer_list<T>&& il) noexcept;
        explicit stack(size_t size_);
        stack(const stack<T>& rhs);
        stack(stack<T>&& rhs) noexcept;
        ~stack();

        bool empty() const {
            return data_->empty();
        }

        size_t size() const {
            return data_->size();
        }

        void push(const T& elem) {
            data_.push_back(elem);
        }

        void push(T&& elem) noexcept {
            data_.push_back(std::move(elem));
        }

        void pop() {
            data_.pop_back();
        }

        const T& top() const {
            return data_.back();
        }

        T& top() {
            return data_.back();
        }
    };

    template <typename T>
    stack<T>::stack() : data_(new vector<T>()) {}
    
    template <typename T>
    stack<T>::stack(std::initializer_list<T>&& il) noexcept : data_(new vector<T>(std::move(il))) {}

    template <typename T>
    stack<T>::stack(size_t s) : data_(new vector<T>(s)) {}

    template <typename T>
    stack<T>::stack(const stack<T>& rhs) : data_(new vector<T>(*rhs.data_)) {}

    template <typename T>
    stack<T>::stack(stack<T>&& rhs) noexcept : data_(rhs.data_) {
        rhs.data_ = nullptr;
    }

    template <typename T>
    stack<T>::~stack() {
        delete data_;
    }
}
#endif