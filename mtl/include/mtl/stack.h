#ifndef MTL_STACK_H
#define MTL_STACK_H

#include <initializer_list>
#include <mtl/basic_vector.h>

namespace mtl {
    template <typename T>
    class stack : public basic_vector<T> {
        private:
        size_t size_;

        public:
        stack();
        explicit stack(size_t size_);
        stack(std::initializer_list<T>&& il) noexcept;
        stack(const stack<T>& rhs);
        stack(stack<T>&& rhs) noexcept;
        virtual ~stack() = default;

        bool empty() const {
            return size_ == 0;
        }

        size_t size() const {
            return size_;
        }

        void push(const T& elem) {
            if (size_ >= basic_vector<T>::capacity()) {
                basic_vector<T>::expand(size_ * 2);
            }
            basic_vector<T>::operator[](size_) = elem;
            ++size_;
        }

        void push(T&& elem) noexcept {
            if (size_ >= basic_vector<T>::capacity()) {
                basic_vector<T>::expand(size_ * 2);
            }
            basic_vector<T>::operator[](size_) = std::move(elem);
            ++size_;
        }

        void pop() {
            --size_;
        }

        const T& top() const {
            if (size_ == 0)
                throw std::out_of_range("There's no element to be popped out.");

            return data_[size_ - 1];
        }

        T& top() {
            return const_cast<T&>(static_cast<const stack<T>*>(this)->top());
        }
    };

    template <typename T>
    stack<T>::stack() : size_(0) {}
    
    template <typename T>
    stack<T>::stack(size_t s) : size_(0), basic_vector<T>(s) {}

    template <typename T>
    stack<T>::stack(std::initializer_list<T>&& il) noexcept :
        size_(0), basic_vector<T>(std::move(il)) {}

    template <typename T>
    stack<T>::stack(const stack<T>& rhs) : size_(rhs.size_), basic_vector<T>(rhs) {}

    template <typename T> stack<T>::stack(stack<T>&& rhs) noexcept :
        size_(rhs.size()), basic_vector<T>(std::move(rhs)) {}
}
#endif