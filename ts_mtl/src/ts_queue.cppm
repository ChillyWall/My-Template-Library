export module ts_mtl.queue;

import mtl.list; // for mtl::list_node
import mtl.core; // for EmptyContainer
import std;

export namespace mtl {

/**
 * @brief Thread-safe FIFO queue using a two-lock linked-list design.
 *
 * @tparam T     Element type stored in the queue.
 * @tparam Alloc Allocator type (reserved for API consistency; node allocation
 *               uses std::allocator<list_node<T>>).
 */
template <typename T, typename Alloc = std::allocator<T>>
class ts_queue {
public:
    using self_t = ts_queue<T, Alloc>;

private:
    using node_t = list_node<T>;
    using node_ptr = node_t*;
    using node_alloc_t = std::allocator<node_t>;

    node_ptr head_;  // head sentinel
    node_ptr tail_;  // tail sentinel
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
    ts_queue() {
        init_sentinels();
    }

    ts_queue(const self_t& rhs) {
        std::scoped_lock lock(rhs.head_mtx_, rhs.tail_mtx_);
        init_sentinels();
        node_ptr src = rhs.head_->next_;
        while (src != rhs.tail_) {
            push_back_copy(src->elem());
            src = src->next_;
        }
        size_.store(rhs.size_.load(std::memory_order_acquire),
                    std::memory_order_release);
    }

    ts_queue(self_t&& rhs) noexcept {
        std::scoped_lock lock(rhs.head_mtx_, rhs.tail_mtx_);
        head_ = rhs.head_;
        tail_ = rhs.tail_;
        size_.store(rhs.size_.load(std::memory_order_acquire),
                    std::memory_order_release);
        rhs.init_sentinels();
        rhs.size_.store(0, std::memory_order_release);
    }

    ~ts_queue() noexcept {
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
     * @brief Push an element onto the back of the queue.
     *
     * @tparam V Element type (deduced).
     * @param elem Element to push (forwarded).
     */
    template <typename V>
    void push(V&& elem) {
        // Allocate outside the critical section.
        auto* n = allocate_node(std::forward<V>(elem), nullptr, nullptr);

        std::unique_lock t_lock(tail_mtx_);
        std::unique_lock h_lock(head_mtx_, std::defer_lock);

        // When the queue is empty, the new node will sit between head
        // and tail — both sentinel regions are touched, so lock both
        // to prevent a concurrent pop from seeing an inconsistent state.
        if (tail_->prev_ == head_) {
            h_lock.lock();
        }

        n->prev_ = tail_->prev_;
        n->next_ = tail_;
        tail_->prev_->next_ = n;
        tail_->prev_ = n;
        size_.fetch_add(1, std::memory_order_release);
    }

    /**
     * @brief Pop an element from the front of the queue into @p value.
     *
     * @param value Reference to store the popped element.
     * @throws EmptyContainer If the queue is empty.
     */
    void pop(T& value) {
        // Fast-path: avoid lock contention when the queue is empty.
        if (size_.load(std::memory_order_acquire) == 0) {
            throw EmptyContainer("This ts_queue is empty.");
        }

        std::unique_lock h_lock(head_mtx_);

        // Re-check under lock: another thread may have drained the
        // queue between the atomic load and lock acquisition.
        if (head_->next_ == tail_) {
            throw EmptyContainer("This ts_queue is empty.");
        }

        std::unique_lock t_lock(tail_mtx_, std::defer_lock);

        // When only one element remains, popping it touches the tail
        // sentinel (head_->next_->next_ is the tail). A concurrent
        // push would also touch tail_->prev_ — lock both to serialize.
        if (head_->next_->next_ == tail_) {
            t_lock.lock();
        }

        node_ptr node = head_->next_;
        value = std::move(node->elem_);

        head_->next_ = node->next_;
        node->next_->prev_ = head_;
        destroy_node(node);
        size_.fetch_sub(1, std::memory_order_release);
    }

private:
    // Copy-helper used by copy-constructor (caller holds both locks).
    void push_back_copy(const T& elem) {
        auto* n = allocate_node(elem, tail_->prev_, tail_);
        tail_->prev_->next_ = n;
        tail_->prev_ = n;
    }
};

}  // namespace mtl
