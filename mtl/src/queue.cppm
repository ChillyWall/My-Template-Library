export module mtl.queue;

export import mtl.core;
import mtl.deque;

import std;

namespace mtl {
/**
 * @brief FIFO queue adapter built on deque.
 * @tparam T Element type.
 * @tparam Alloc Allocator type.
 */
template <typename T, typename Alloc = std::allocator<T>>
class queue {
public:
    /**
     * @brief Alias for this queue instantiation.
     */
    using self_t = queue<T, Alloc>;

private:
    /**
     * @brief Container type used to store elements.
     *
     * This alias names the deque instantiation used as the underlying
     * storage for the queue.
     */
    using container_t = deque<T, Alloc>;
    /**
     * @brief Storage for the queue elements.
     */
    container_t data_;

public:
    /**
     * @brief Construct an empty queue.
     */
    queue() = default;
    /**
     * @brief Copy-construct a queue.
     * @param rhs Queue to copy from.
     */
    queue(const self_t& rhs) = default;
    /**
     * @brief Move-construct a queue.
     * @param rhs Queue to move from.
     */
    queue(self_t&& rhs) = default;
    /**
     * @brief Copy-assign a queue.
     * @param rhs Queue to copy from.
     * @return Reference to this queue.
     */
    queue& operator=(const self_t& rhs) = default;
    /**
     * @brief Move-assign a queue.
     * @param rhs Queue to move from.
     * @return Reference to this queue.
     */
    queue& operator=(self_t&& rhs) = default;
    /**
     * @brief Destroy the queue.
     */
    ~queue() noexcept = default;

    /**
     * @brief Get the number of elements.
     * @return Element count.
     */
    [[nodiscard]] size_t size() const {
        return data_.size();
    }

    /**
     * @brief Check whether the queue is empty.
     * @return True if empty.
     */
    [[nodiscard]] bool empty() const {
        return data_.empty();
    }

    /**
     * @brief Clear all elements from the queue.
     */
    void clear() {
        data_.clear();
    }

    /**
     * @brief Enqueue an element at the back.
     * @tparam V Value type to insert.
     * @param elem Element to push.
     */
    template <typename V>
    void push(V&& elem) noexcept {
        data_.push_back(std::forward<V>(elem));
    }

    /**
     * @brief Dequeue the front element.
     */
    void pop() {
        data_.pop_front();
    }

    /**
     * @brief Access the front element (const).
     * @return Const reference to the front element.
     */
    const T& front() const {
        return data_.front();
    }

    /**
     * @brief Access the back element (const).
     * @return Const reference to the back element.
     */
    const T& back() const {
        return data_.back();
    }

    /**
     * @brief Access the front element.
     * @return Reference to the front element.
     */
    T& front() {
        return data_.front();
    }

    /**
     * @brief Access the back element.
     * @return Reference to the back element.
     */
    T& back() {
        return data_.back();
    }
};

}  // namespace mtl
