#pragma once

#include <atomic>
#include <memory>

#include "common.h"

namespace nbn::core {

/**
 * @file ConcurrentStack.h
 * @brief Lock-free concurrent stack implementation.
 */

/**
 * @brief A concurrent multi-producer, multi-consumer stack implementation.
 *
 * A successful head CAS is the linearization point for both push and pop.
 * LIFO order is guaranteed for non-overlapping operations; overlapping
 * operations are ordered by their successful CAS operations. A popped node
 * remains alive through the local shared pointer until its value is copied or
 * moved. The implementation does not promise a formal lock-free progress
 * classification. The stack object must outlive all concurrent operations;
 * destruction is not synchronized with push or pop.
 *
 * @tparam T The type of elements stored in the stack.
 *
 * @par Usage example
 * @code{.cpp}
 * nbn::core::ConcurrentStack<int> stack;
 * stack.push(42);
 *
 * int value = 0;
 * if (stack.pop(value)) {
 *     // value == 42
 * }
 * @endcode
 */
template <concurrent_safe_c T>
class ConcurrentStack {
   private:
    struct Node {
        T m_value{};
        std::atomic<std::shared_ptr<Node>> m_spNext{nullptr};

        Node() = default;

        explicit Node(const T& value)
            requires(!is_smart_ptr<T>::value)
            : m_value{value} {}
        explicit Node(T&& value)
            requires(is_smart_ptr<T>::value)
            : m_value{std::move(value)} {}
    };

    std::atomic<std::shared_ptr<Node>> m_head{};

   public:
    /**
     * @brief Constructs an empty ConcurrentStack object.
     */
    ConcurrentStack() = default;

    /**
     * @brief Destroys the ConcurrentStack object and pops all remaining elements.
     */
    ~ConcurrentStack() {
        auto noop = [](T&) {};
        T value;
        while (pop(value)) {
            noop(value);  // Make llvm coverage happy by using the popped value
        }
    }

    // Delete copy and move constructors and assignment operators
    ConcurrentStack(const ConcurrentStack&) = delete;
    auto operator=(const ConcurrentStack&) -> ConcurrentStack& = delete;
    ConcurrentStack(ConcurrentStack&&) = delete;
    auto operator=(ConcurrentStack&&) -> ConcurrentStack& = delete;

    /**
     * @brief Pushes a new element onto the stack.
     * @param value The value to be pushed onto the stack.
     */
    void push(T value) noexcept {
        // Create a new node
        std::shared_ptr<Node> spNewNode;
        if constexpr (is_smart_ptr<T>::value) {
            spNewNode = std::make_shared<Node>(std::move(value));
        } else {
            spNewNode = std::make_shared<Node>(value);
        }
        auto spHead = m_head.load(std::memory_order_relaxed);
        while (true) {
            spNewNode->m_spNext.store(spHead, std::memory_order_relaxed);
            if (m_head.compare_exchange_weak(spHead, spNewNode, std::memory_order_release, std::memory_order_relaxed)) {
                return;
            }
        }
    }

    /**
     * @brief Pops an element from the stack.
     * @param dest A reference to store the popped value.
     * @return true if the stack is not empty and a value was popped, false otherwise.
     */
    [[nodiscard]] auto pop(T& dest) noexcept -> bool {
        std::shared_ptr<Node> spHead = m_head.load(std::memory_order_acquire);
        if (spHead.get() == nullptr) {
            return false;
        }
        std::shared_ptr<Node> spNewHead;

        while (spHead != nullptr) {
            spNewHead = spHead->m_spNext.load(std::memory_order_acquire);
            if (m_head.compare_exchange_weak(spHead, spNewHead, std::memory_order_acq_rel, std::memory_order_relaxed)) {
                break;
            }
        }
        if (spHead == nullptr) {
            return false;
        }

        if constexpr (is_smart_ptr<T>::value) {
            dest = std::move(spHead->m_value);
        } else {
            dest = spHead->m_value;
        }
        return true;
    }
};

}  // namespace nbn::core
