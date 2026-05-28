export module ts_mtl.deque;

import mtl.list; // for list_node
import mtl.core; // for EmptyContainer
import std;

export namespace mtl {

/**
 * @brief Thread-safe double-ended queue using a two-lock linked-list design.
 *
 * push_front/pop_front primarily use head_mtx_, push_back/pop_back primarily
 * use tail_mtx_. When the deque is nearly empty (size_ <= 2), pop operations
 * escalate to hold both locks, avoiding boundary races where a node freed by
 * one end is still referenced by a pointer read at the other end.
 *
 * @tparam T     Element type stored in the deque.
 * @tparam Alloc Allocator type (reserved for API consistency; node allocation
 *               uses std::allocator<list_node<T>>).
 */
template <typename T, typename Alloc = std::allocator<T>>
class ts_deque {
public:
    using self_t = ts_deque<T, Alloc>;

private:
    using node_t = list_node<T>;
    using node_ptr = node_t*;
    using node_alloc_t = std::allocator<node_t>;

    node_ptr head_;
    node_ptr tail_;
    mutable std::mutex head_mtx_;
    mutable std::mutex tail_mtx_;
    std::atomic<size_t> size_ {0};
    node_alloc_t node_alloc_;

    node_ptr allocate_node() {
        auto ptr = node_alloc_.allocate(1);
        std::construct_at(ptr);
        return ptr;
    }

    template <typename V>
    node_ptr allocate_node(V&& elem, node_ptr prev, node_ptr next) {
        auto ptr = node_alloc_.allocate(1);
        std::construct_at(ptr, std::forward<V>(elem), prev, next);
        return ptr;
    }

    void destroy_node(node_ptr ptr) {
        std::destroy_at(ptr);
        node_alloc_.deallocate(ptr, 1);
    }

    void init_sentinels() {
        head_ = allocate_node();
        tail_ = allocate_node();
        head_->next_ = tail_;
        tail_->prev_ = head_;
    }

    void destroy_all() {
        node_ptr curr = head_;
        while (curr) {
            node_ptr next = curr->next_;
            destroy_node(curr);
            curr = next;
        }
    }

public:
    ts_deque() {
        init_sentinels();
    }

    ts_deque(const self_t& rhs) {
        std::scoped_lock lock(rhs.head_mtx_, rhs.tail_mtx_);
        init_sentinels();
        for (node_ptr src = rhs.head_->next_; src != rhs.tail_;
             src = src->next_) {
            push_back_copy(src->elem());
        }
        size_.store(rhs.size_.load(std::memory_order_acquire),
                    std::memory_order_release);
    }

    ts_deque(self_t&& rhs) noexcept {
        std::scoped_lock lock(rhs.head_mtx_, rhs.tail_mtx_);
        head_ = rhs.head_;
        tail_ = rhs.tail_;
        size_.store(rhs.size_.load(std::memory_order_acquire),
                    std::memory_order_release);
        rhs.init_sentinels();
        rhs.size_.store(0, std::memory_order_release);
    }

    ~ts_deque() noexcept {
        destroy_all();
    }

    self_t& operator=(const self_t& rhs) = delete;

    self_t& operator=(self_t&& rhs) noexcept {
        if (this != &rhs) {
            std::unique_lock lh(head_mtx_, std::defer_lock);
            std::unique_lock lt(tail_mtx_, std::defer_lock);
            std::unique_lock rh(rhs.head_mtx_, std::defer_lock);
            std::unique_lock rt(rhs.tail_mtx_, std::defer_lock);
            std::lock(lh, lt, rh, rt);

            destroy_all();
            head_ = rhs.head_;
            tail_ = rhs.tail_;
            size_.store(rhs.size_.load(std::memory_order_acquire),
                        std::memory_order_release);
            rhs.init_sentinels();
            rhs.size_.store(0, std::memory_order_release);
        }
        return *this;
    }

    [[nodiscard]] bool empty() const {
        return size_.load(std::memory_order_acquire) == 0;
    }

    [[nodiscard]] size_t size() const {
        return size_.load(std::memory_order_acquire);
    }

    /**
     * @brief Push an element onto the front of the deque.
     */
    template <typename V>
    void push_front(V&& elem) {
        auto* n = allocate_node(std::forward<V>(elem), nullptr, nullptr);

        std::unique_lock h_lock(head_mtx_);
        std::unique_lock t_lock(tail_mtx_, std::defer_lock);

        // Empty deque: the new node will sit between head and tail, touching
        // both sentinels. Lock both to serialize with concurrent push_back.
        if (head_->next_ == tail_) {
            h_lock.unlock();
            std::lock(h_lock, t_lock);
        }

        n->prev_ = head_;
        n->next_ = head_->next_;
        head_->next_->prev_ = n;
        head_->next_ = n;
        size_.fetch_add(1, std::memory_order_release);
    }

    /**
     * @brief Push an element onto the back of the deque.
     */
    template <typename V>
    void push_back(V&& elem) {
        auto* n = allocate_node(std::forward<V>(elem), nullptr, nullptr);

        std::unique_lock t_lock(tail_mtx_);
        std::unique_lock h_lock(head_mtx_, std::defer_lock);

        if (tail_->prev_ == head_) {
            t_lock.unlock();
            std::lock(t_lock, h_lock);
        }

        n->prev_ = tail_->prev_;
        n->next_ = tail_;
        tail_->prev_->next_ = n;
        tail_->prev_ = n;
        size_.fetch_add(1, std::memory_order_release);
    }

    /**
     * @brief Pop an element from the front of the deque.
     *
     * @throws EmptyContainer If the deque is empty.
     */
    void pop_front(T& value) {
        if (size_.load(std::memory_order_acquire) == 0) {
            throw EmptyContainer("This ts_deque is empty.");
        }

        std::unique_lock h_lock(head_mtx_);
        std::unique_lock t_lock(tail_mtx_, std::defer_lock);

        if (head_->next_ == tail_) {
            throw EmptyContainer("This ts_deque is empty.");
        }

        // When the deque is small (<= 2 elements), the node we are popping
        // may be adjacent to or identical to the tail sentinel's neighbour,
        // and a concurrent pop_back could free a node whose pointer we still
        // hold. Escalate to both locks to avoid the boundary race.
        if (size_.load(std::memory_order_acquire) <= 2) {
            h_lock.unlock();
            std::lock(h_lock, t_lock);
            if (head_->next_ == tail_) {
                throw EmptyContainer("This ts_deque is empty.");
            }
        }

        node_ptr node = head_->next_;
        value = std::move(node->elem_);

        head_->next_ = node->next_;
        node->next_->prev_ = head_;
        destroy_node(node);
        size_.fetch_sub(1, std::memory_order_release);
    }

    /**
     * @brief Pop an element from the back of the deque.
     *
     * @throws EmptyContainer If the deque is empty.
     */
    void pop_back(T& value) {
        if (size_.load(std::memory_order_acquire) == 0) {
            throw EmptyContainer("This ts_deque is empty.");
        }

        std::unique_lock t_lock(tail_mtx_);
        std::unique_lock h_lock(head_mtx_, std::defer_lock);

        if (tail_->prev_ == head_) {
            throw EmptyContainer("This ts_deque is empty.");
        }

        if (size_.load(std::memory_order_acquire) <= 2) {
            t_lock.unlock();
            std::lock(t_lock, h_lock);
            if (tail_->prev_ == head_) {
                throw EmptyContainer("This ts_deque is empty.");
            }
        }

        node_ptr node = tail_->prev_;
        value = std::move(node->elem_);

        tail_->prev_ = node->prev_;
        node->prev_->next_ = tail_;
        destroy_node(node);
        size_.fetch_sub(1, std::memory_order_release);
    }

private:
    void push_back_copy(const T& elem) {
        auto* n = allocate_node(elem, tail_->prev_, tail_);
        tail_->prev_->next_ = n;
        tail_->prev_ = n;
    }
};

}  // namespace mtl
