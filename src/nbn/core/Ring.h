#pragma once

namespace nbn::core {

#include <vector>

#include "common.h"

/**
 * @file Ring.h
 * @brief Circular container traversal helper.
 */

/**
 * @class Ring
 * @brief Fixed-order cyclic accessor over a sequence of elements.
 *
 * @par Usage example
 * @code{.cpp}
 * nbn::core::Ring<int> ring({1, 2, 3});
 * int value = 0;
 *
 * ring.get(value);   // value == 1
 * ring.get(value);   // value == 2
 * ring.get(value);   // value == 3
 * ring.get(value);   // value == 1 (wrap-around)
 * @endcode
 */
template <concurrent_safe_c T>
class Ring {
   private:
    size_t m_currentIndex{0};
    std::vector<T> m_elements;

   public:
    /**
     * @brief Constructs a ring containing @p size default-initialized elements.
     * @param size Number of elements in the ring.
     */
    explicit Ring(size_t size = 0) : m_elements(size) {}

    /**
     * @brief Constructs a ring from an existing sequence.
     * @param elements Elements to copy into the ring.
     */
    explicit Ring(std::vector<T> elements) : m_elements(std::move(elements)) {}

    // TODO: emplace_back (everywhere)
    /** @brief Appends an element to the end of the ring. */
    void push_back(T element) { m_elements.push_back(std::move(element)); }

    /**
     * @brief Returns the current element and advances to the next element.
     * @param[out] dest Destination for the current element.
     * @return false when the ring is empty; otherwise true.
     */
    auto get(T& dest) -> bool {
        if (m_elements.empty()) {
            return false;  // Empty ring
        }

        dest = m_elements[m_currentIndex];
        m_currentIndex = (m_currentIndex + 1) % size();
        return true;
    }

    /**
     * @brief Returns the current element without advancing the ring.
     * @param[out] dest Destination for the current element.
     * @return false when the ring is empty; otherwise true.
     */
    auto front(T& dest) -> bool {
        if (m_elements.empty()) {
            return false;  // Empty ring
        }

        dest = m_elements[m_currentIndex];
        return true;
    }

    /**
     * @brief Returns the element immediately before the current position.
     * @param[out] dest Destination for the previous element.
     * @return false when the ring is empty; otherwise true.
     */
    auto back(T& dest) -> bool {
        if (m_elements.empty()) {
            return false;  // Empty ring
        }

        dest = m_elements[(m_currentIndex + size() - 1) % size()];
        return true;
    }

    /** @brief Returns the number of elements in the ring. */
    auto size() const -> size_t { return m_elements.size(); }

    /** @brief Returns whether the ring contains no elements. */
    auto empty() const -> bool { return m_elements.empty(); }

    /** @brief Removes all elements and resets the ring to an empty state. */
    void clear() { m_elements.clear(); }
};

}  // namespace nbn::core