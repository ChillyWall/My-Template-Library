export module mtl.priority_queue;

export import mtl.core;
import mtl.vector;
import std;

export namespace mtl {
/**
 * @brief The priority queue ADT, implemented by basic_vector so that it
 *        can dynamically expand its capacity.
 *
 * @tparam T The element type stored in the priority queue.
 * @tparam Alloc The allocator type used for element storage.
 */
template <typename T, typename Alloc = std::allocator<T>>
class priority_queue {
public:
    /** @brief Alias for this priority_queue instantiation. */
    using self_t = priority_queue<T, Alloc>;
    /** @brief Underlying container type used to store elements. */
    using container_t = vector<T, Alloc>;

private:
    /**
     * @brief Underlying storage for the heap elements.
     *
     * Note: The implementation treats the first logical element as index 1
     * when performing heap arithmetic; storage index is offset by -1.
     */
    container_t data_;

    /**
     * @brief Check whether the queue is empty and throw if it is.
     *
     * Throws EmptyContainer when the queue contains no elements.
     */
    void check_empty() const {
        if (empty()) {
            throw EmptyContainer("There's no element.");
        }
    }

    /**
     * @brief Restore heap order by percolating the last element up.
     */
    void percolate_up() noexcept;
    /** @brief Restore heap order by percolating the root element down. */
    void percolate_down() noexcept;

    /**
     * @brief Mutable access to element at logical position pos (1-based).
     * @param pos Logical 1-based position in the heap.
     * @return Reference to element at position pos.
     */
    T& data_at(size_t pos) {
        return const_cast<T&>(static_cast<const self_t*>(this)->data_at(pos));
    }

    /**
     * @brief Const access to element at logical position pos (1-based).
     * @param pos Logical 1-based position in the heap.
     * @return Const reference to element at position pos.
     */
    const T& data_at(size_t pos) const {
        return data_[pos - 1];
    }

public:
    /** @brief Default construct a priority_queue. */
    priority_queue() = default;
    /**
     * @brief Copy construct a priority_queue.
     * @param rhs The priority_queue to copy from.
     */
    priority_queue(const self_t& rhs);
    /**
     * @brief Move construct a priority_queue.
     * @param rhs The priority_queue to move from.
     */
    priority_queue(self_t&& rhs) noexcept;
    /** @brief Virtual destructor. */
    virtual ~priority_queue() = default;

    /** @brief Clear all elements from the queue. */
    virtual void clear() {
        data_.clear();
    }

    /**
     * @brief Get the number of elements in the priority queue.
     * @return The number of elements stored.
     */
    [[nodiscard]] size_t size() const {
        return data_.size();
    }

    /**
     * @brief Check whether the priority queue is empty.
     * @return true if the queue contains no elements, false otherwise.
     */
    [[nodiscard]] bool empty() const {
        return size() == 0;
    }

    /**
     * @brief Copy-assign from another priority_queue.
     * @param rhs The source priority_queue to copy from.
     * @return Reference to this priority_queue.
     */
    self_t& operator=(const self_t& rhs) {
        if (&rhs == this) {
            return *this;
        }
        data_ = rhs.data_;
        return *this;
    }

    /**
     * @brief Move-assign from another priority_queue.
     * @param rhs The source priority_queue to move from.
     * @return Reference to this priority_queue.
     */
    self_t& operator=(self_t&& rhs) noexcept {
        data_ = std::move(rhs.data_);
        return *this;
    }

    /**
     * @brief Push a new element into the priority queue.
     * @tparam V The value type or forwarding reference type.
     * @param elem The element to push (forwarded).
     */
    template <typename V>
    void push(V&& elem) noexcept {
        data_.push_back(std::forward<V>(elem));
        percolate_up();
    }

    /**
     * @brief Reserve capacity for the underlying container.
     * @param new_capacity New capacity to reserve.
     */
    void reserve(size_t new_capacity) {
        data_.reserve(new_capacity);
    }

    /**
     * @brief Remove the minimum element from the queue.
     *
     * Throws EmptyContainer when the queue is empty.
     */
    void pop() {
        check_empty();
        percolate_down();
    }

    /**
     * @brief Access the minimum element.
     * @return Const reference to the minimum element.
     *
     * Throws EmptyContainer when the queue is empty.
     */
    const T& top() const {
        check_empty();
        return data_at(1);
    }

    /**
     * @brief Mutable access to the minimum element.
     * @return Reference to the minimum element.
     */
    T& top() {
        return const_cast<T&>(static_cast<const self_t*>(this)->top());
    }
};

template <typename T, typename Alloc>
priority_queue<T, Alloc>::priority_queue(const self_t& rhs)
    : data_(rhs.data_) {}

template <typename T, typename Alloc>
priority_queue<T, Alloc>::priority_queue(self_t&& rhs) noexcept
    : data_(std::move(rhs.data_)) {}

template <typename T, typename Alloc>
void priority_queue<T, Alloc>::percolate_up() noexcept {
    size_t pos = size();
    T temp = std::move(data_at(pos));

    while (temp < data_at(pos / 2)) {
        // move the parent down
        data_at(pos) = std::move(data_at(pos / 2));
        pos /= 2;
    }
    data_at(pos) = std::move(temp);
}

template <typename T, typename Alloc>
void priority_queue<T, Alloc>::percolate_down() noexcept {
    T temp = std::move(data_at(size()));
    data_.pop_back();
    size_t pos = 1;
    while ((pos * 2) <= size()) {
        size_t child = pos * 2;
        // choose the smaller child
        if (child + 1 <= size()) {
            child = data_at(child) > data_at(child + 1) ? child + 1 : child;
        }
        // move the child up
        if (data_at(child) < temp) {
            data_at(pos) = std::move(data_at(child));
            pos = child;
        } else {
            break;
        }
    }
    data_at(pos) = std::move(temp);
}
}  // namespace mtl
