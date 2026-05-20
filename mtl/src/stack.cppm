export module mtl.stack;

import mtl.deque;
export import mtl.core;
import std;

export namespace mtl {
/**
 * @brief LIFO stack adapter built on deque.
 * @tparam T Element type.
 * @tparam Alloc Allocator type.
 */
template <typename T, typename Alloc = std::allocator<T>>
class stack {
public:
    /**
     * @brief Alias for this stack type.
     */
    using self_t = stack<T, Alloc>;
    
private:
    /**
     * @brief Underlying container type used to store elements.
     */
    using container_type = deque<T, Alloc>;

    /**
     * @brief Underlying storage for the stack elements.
     */
    container_type data_;

public:
    /**
     * @brief Construct an empty stack.
     */
    stack() = default;
    /**
     * @brief Copy-construct a stack.
     * @param rhs Stack to copy from.
     */
    stack(const self_t& rhs) = default;
    /**
     * @brief Move-construct a stack.
     * @param rhs Stack to move from.
     */
    stack(self_t&& rhs) noexcept = default;
    /**
     * @brief Destroy the stack.
     */
    ~stack() noexcept = default;

    /**
     * @brief Copy-assign from another stack.
     * @param rhs Stack to copy from.
     * @return Reference to this stack.
     */
    self_t& operator=(const self_t& rhs) = default;
    /**
     * @brief Move-assign from another stack.
     * @param rhs Stack to move from.
     * @return Reference to this stack.
     */
    self_t& operator=(self_t&& rhs) noexcept = default;

    /**
     * @brief Check whether the stack is empty.
     * @return True if empty.
     */
    [[nodiscard]] bool empty() const {
        return data_.empty();
    }

    /**
     * @brief Get the number of elements.
     * @return Element count.
     */
    [[nodiscard]] size_t size() const {
        return data_.size();
    }

    /**
     * @brief Push an element onto the stack.
     * @tparam V Value type to insert.
     * @param elem Element to push.
     */
    template <typename V>
    void push(V&& elem) {
        data_.push_back(std::forward<V>(elem));
    }

    /**
     * @brief Remove the top element.
     */
    void pop() {
        data_.pop_back();
    }

    /**
     * @brief Access the top element (const).
     * @return Const reference to the top element.
     */
    const T& top() const {
        return data_.back();
    }

    /**
     * @brief Access the top element.
     * @return Reference to the top element.
     */
    T& top() {
        return data_.back();
    }
};

}  // namespace mtl
