#pragma once

#include <stdexcept>
#include <unordered_map>

/**
 * @file XRefUnorderedMap.h
 * @brief Bi-directional map helper built on unordered_map.
 */

/**
 * @class XRefUnorderedMap
 * @brief Maintains key-to-value and value-to-key lookups.
 *
 * @par Usage example
 * @code{.cpp}
 * XRefUnorderedMap<int, std::string> map;
 * map.insert(7, "worker");
 *
 * if (map.containsKey(7)) {
 *     auto name = map.getValue(7);      // "worker"
 *     auto id = map.getKey("worker");  // 7
 * }
 * @endcode
 */
template <typename TKey, typename TValue>
class XRefUnorderedMap {
   public:
    XRefUnorderedMap() = default;

    void insert(const TKey& key, const TValue& value) { m_keysToValues[key] = value; }

    [[nodiscard]] auto empty() const -> bool { return m_keysToValues.empty(); }

    [[nodiscard]] auto containsKey(const TKey& key) const -> bool { return m_keysToValues.find(key) != m_keysToValues.end(); }

    [[nodiscard]] auto containsValue(const TValue& value) const -> bool {
        for (const auto& [key, mappedValue] : m_keysToValues) {
            if (mappedValue == value) {
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] auto getValue(const TKey& key) const -> const TValue& { return m_keysToValues.at(key); }

    [[nodiscard]] auto getKey(const TValue& value) const -> const TKey& {
        for (const auto& [key, mappedValue] : m_keysToValues) {
            if (mappedValue == value) {
                return key;
            }
        }
        throw std::out_of_range("XRefUnorderedMap value is not mapped");
    }

   private:
    std::unordered_map<TKey, TValue> m_keysToValues{};
};
