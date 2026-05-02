export module mtl.priority_queue;

export import mtl.core;
import mtl.vector;
import std;

export namespace mtl {
/* The priority queue ADT, implemented by basic_vector so that it could
 * dynamicly expand its capacity. */
template <typename T, typename Alloc = std::allocator<T>>
class priority_queue {
public:
    using self_t = priority_queue<T, Alloc>;
    using container_t = vector<T, Alloc>;

private:
    /* the number of elements, note that the first element is at index 1 */
    container_t data_;

    // check whether the queue is empty, if true, throw a out_of_range exception
    void check_empty() const {
        if (empty()) {
            throw EmptyContainer("There's no element.");
        }
    }

    /* to percolate up from the last element, to ensure the heap order after
     * push */
    void percolate_up() noexcept;
    // to percolate down from position 1, to ensure the heap order after pop
    void percolate_down() noexcept;

    T& data_at(size_t pos) {
        return const_cast<T&>(static_cast<const self_t*>(this)->data_at(pos));
    }

    const T& data_at(size_t pos) const {
        return data_[pos - 1];
    }

public:
    priority_queue() = default;
    priority_queue(const self_t& rhs);
    priority_queue(self_t&& rhs) noexcept;
    virtual ~priority_queue() = default;

    // clear the queue
    virtual void clear() {
        data_.clear();
    }

    /* get the number of elements of the priority queue */
    [[nodiscard]] size_t size() const {
        return data_.size();
    }

    /* check if the priority queue is empty */
    [[nodiscard]] bool empty() const {
        return size() == 0;
    }

    self_t& operator=(const self_t& rhs) {
        if (&rhs == this) {
            return *this;
        }
        data_ = rhs.data_;
        return *this;
    }

    self_t& operator=(self_t&& rhs) noexcept {
        data_ = std::move(rhs.data_);
        return *this;
    }

    // push a new element
    template <typename V>
    void push(V&& elem) noexcept {
        data_.push_back(std::forward<V>(elem));
        percolate_up();
    }

    void reserve(size_t new_capacity) {
        data_.reserve(new_capacity);
    }

    // pop the minimum element
    void pop() {
        check_empty();
        percolate_down();
    }

    // return the minimum element
    const T& top() const {
        check_empty();
        return data_at(1);
    }

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
