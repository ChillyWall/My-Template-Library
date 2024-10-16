#ifndef MTL_PRIORITY_QUEUE_H
#define MTL_PRIORITY_QUEUE_H

#include <mtl/basic_vector.h>

namespace mtl {
    template <typename T>
    class priority_queue : public basic_vector<T> {
    private:
        size_t size_;

        void check_empty() {
            if (size_ == o) {
                throw std::out_of_range("There's no element.");
            }
        }

        void check_expand() {
            if (size_ + 1 >= basic_vector<T>::capacity()) {
                basic_vector<T>::expand(2 * (size_ + 1));
            }
        }

        void percolate_up(size_t pos);
        void percolate_down(size_t pos);

    public:
        priority_queue();
        priority_queue(size_t n);
        priority_queue(const priority_queue<T>& rhs);
        priority_queue(priority_queue<T>&& rhs) noexcept;
        virtual ~priority_queue() = default;

        virtual void clear() {
            size_ = 0;
            basic_vector<T>::clear();
        }

        size_t size() const {
            return size_;
        }

        bool empty() const {
            return size_ == 0;
        }

        priority_queue<T>& operator=(const priority_queue<T>& rhs) {
            size_ = rhs.size_;
            basic_vector<T>::operator=(rhs);
        }

        priority_queue<T>& operator=(priority_queue<T>&& rhs) noexcept {
            size_ = rhs.size_;
            basic_vector<T>::operator=(std::move(rhs));
        }

        void push(const T& elem) {
            check_expand();
            ++size_;
            basic_vector<T>::data()[size_] = elem;
            percolate_up(size_);
        }

        void push(T&& elem) noexcept {
            check_expand();
            ++size_;
            basic_vector<T>::data()[size_] = std::move(elem);
            percolate_up(size_);
        }

        void pop() {
            check_empty();
            percolate_down(1);
            --size_;
        }

        const T& top() const {
            check_empty();
            return basic_vector<T>::data()[1];
        }

        T& top() {
            return const_cast<T&>(static_cast<const priority_queue<T>*>(this)->top());
        }
    };

    template <typename T>
    priority_queue<T>::priority_queue() : size_(0) {}

    template <typename T>
    priority_queue<T>::priority_queue(size_t size) : size_(0), basic_vector<T>(size) {}

    template <typename T>
    priority_queue<T>::priority_queue(const priority_queue<T>& rhs) :
        size_(rhs.size_), basic_vector<T>::(rhs) {}

    template <typename T>
    priority_queue<T>::priority_queue(priority_queue<T>&& rhs) noexcept :
        size_(rhs.size_), basic_vector<T>::(std::move(rhs)) {}

    template <typename T>
    void priority_queue<T>::percolate_up(size_t pos) noexcept {
        auto data = basic_vector<T>::data();
        T temp = std::move(data[pos]);
        while (temp < data[pos >> 1]) {
            data[pos] = std::move(data[pos >>= 1]);
        }
        data[pos] = std::move(temp);
    }

    template <typename T>
    void priority_queue<T>::percolate_down(size_t pos) noexcept {
        auto data = basic_vector<T>::data();
        while ((pos << 1) <= size_) {
            if ((pos << 1) + 1 <= size_) {
                pos = data[pos << 1] < data[(pos << 1) + 1] ? pos << 1 : (pos << 1) + 1;
            } else {
                pos = pos << 1;
            }
            data[pos >> 1] = std::move(data[pos]);
        }
    }
}
#endif