#pragma once

#include <atomic>
#include <memory>

#include "common.h"

namespace nbn::core {

#ifdef NBN_CONCURRENT_QUEUE_TEST_HOOK
namespace detail::concurrent_queue_test {

using hook_t = void (*)();

auto setHook(hook_t callback) noexcept -> void;
auto invokeHook() noexcept -> void;

}  // namespace detail::concurrent_queue_test
#endif

/**
 * @file ConcurrentQueue.h
 * @brief Lock-free multi-producer multi-consumer queue.
 */

/**
 * @brief A concurrent multi-producer, multi-consumer queue implementation.
 * Based on https://www.cs.rochester.edu/~scott/papers/1996_PODC_queues.pdf.
 *
 * A successful link of a new node is the push linearization point. A successful
 * head CAS is the pop linearization point. FIFO order is guaranteed for a
 * single producer; concurrent producers are ordered by their successful link
 * operations. Values remain alive through shared ownership until the winning
 * operation has completed. The implementation does not promise a formal
 * lock-free progress classification. The queue object must outlive all
 * concurrent operations; destruction is not synchronized with push or pop.
 *
 * @tparam T The type of elements stored in the queue.
 *
 * @par Usage example
 * @code{.cpp}
 * nbn::core::ConcurrentQueue<int> queue;
 * queue.push(10);
 *
 * int value = 0;
 * if (queue.pop(value)) {
 *     // value == 10
 * }
 * @endcode
 */
template <concurrent_safe_c T>
class ConcurrentQueue {
   private:
    /**
     * @brief Node structure for the linked list.
     */
    struct Node {
        T m_value{};                                          /**< The value stored in the node. */
        std::atomic<std::shared_ptr<Node>> m_spNext{nullptr}; /**< Pointer to the next node in the list. */

        /**
         * @brief Default constructor for Node.
         */
        Node() = default;

        /**
         * @brief Constructor for Node that sets the value.
         *
         * @param value The value to be set.
         */
        explicit Node(const T& value)
            requires(!is_smart_ptr<T>::value)
            : m_value{value} {}

        explicit Node(T&& value)
            requires(is_smart_ptr<T>::value)
            : m_value{std::move(value)} {}
    };

    std::atomic<std::shared_ptr<Node>> m_head{std::make_shared<Node>()}; /**< Pointer to the head of the queue. */
    std::atomic<std::shared_ptr<Node>> m_tail{m_head.load()};            /**< Pointer to the tail of the queue. */
   public:
    /**
     * @brief Default constructor for ConcurrentQueue.
     */
    ConcurrentQueue() = default;

    /**
     * @brief Destructor for ConcurrentQueue.
     *
     * Clears the queue by repeatedly popping elements until it is empty.
     */
    ~ConcurrentQueue() {
        auto noop = [](T&) {};
        T value;
        while (pop(value)) {
            noop(value);
        }
    }

    // Delete copy and move constructors and assignment operators
    ConcurrentQueue(const ConcurrentQueue&) = delete;
    auto operator=(const ConcurrentQueue&) -> ConcurrentQueue& = delete;
    ConcurrentQueue(ConcurrentQueue&&) = delete;
    auto operator=(ConcurrentQueue&&) -> ConcurrentQueue& = delete;

    /**
     * @brief Pushes a value into the queue.
     *
     * @param value The value to be pushed.
     */
    void push(T value) noexcept {
        std::shared_ptr<Node> spNewNode;
        if constexpr (is_smart_ptr<T>::value) {
            spNewNode = std::make_shared<Node>(std::move(value));
        } else {
            spNewNode = std::make_shared<Node>(value);
        }
        std::shared_ptr<Node> spTail;

        while (true) {
            spTail = m_tail.load(std::memory_order_relaxed);
            auto spNext = spTail->m_spNext.load(std::memory_order_acquire);

            if (spTail == m_tail.load(std::memory_order_acquire)) {
                if (!spNext) {
                    if (spTail->m_spNext.compare_exchange_weak(spNext, spNewNode, std::memory_order_release,
                                                               std::memory_order_relaxed)) {
#ifdef NBN_CONCURRENT_QUEUE_TEST_HOOK
                        detail::concurrent_queue_test::invokeHook();
#endif
                        break;
                    }
                } else {
                    m_tail.compare_exchange_weak(spTail, spNext, std::memory_order_release, std::memory_order_relaxed);
                }
            }
        }

        m_tail.compare_exchange_strong(spTail, spNewNode, std::memory_order_acq_rel, std::memory_order_relaxed);
    }

    /**
     * @brief Pops a value from the queue.
     *
     * @param dest The destination variable to store the popped value.
     * @return true if the pop operation is successful, false otherwise.
     */
    [[nodiscard]] auto pop(T& dest) noexcept -> bool {
        std::shared_ptr<Node> spHead;
        while (true) {
            spHead = m_head.load(std::memory_order_relaxed);
            auto spTail = m_tail.load(std::memory_order_acquire);
            auto spNext = spHead->m_spNext.load(std::memory_order_acquire);

            if (spHead == m_head.load(std::memory_order_acquire)) {
                if (spHead == spTail) {
                    if (!spNext) {
                        return false;
                    }
                    m_tail.compare_exchange_weak(spTail, spNext, std::memory_order_release, std::memory_order_relaxed);
                } else {
                    if (m_head.compare_exchange_weak(spHead, spNext, std::memory_order_release, std::memory_order_relaxed)) {
                        if constexpr (is_smart_ptr<T>::value) {
                            dest = std::move(spNext->m_value);
                        } else {
                            dest = spNext->m_value;
                        }
                        break;
                    }
                }
            }
        }
        return true;
    }
};

}  // namespace nbn::core
