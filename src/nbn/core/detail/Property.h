#pragma once

#include <algorithm>
#include <atomic>
#include <mutex>
#include <optional>
#include <unordered_map>

#include "../utils.h"
#include "ObjectFeature.h"
#include "Signal.h"
#include "serialization.h"

namespace nbn::core {

/**
 * @file Property.h
 * @brief Serializable, signal-emitting property implementations.
 */

class Object;

namespace detail {

/**
 * @class Property
 * @brief Thread-safe typed property that emits a signal when its value changes.
 * @tparam T Serializable value type.
 *
 * Serializable object types are stored through a shared pointer. Assigning a
 * different value emits signalValueChanged() with the old and new values.
 *
 * @par Usage example
 * @code{.cpp}
 * auto value = object->property<int>();
 * value->signalValueChanged()->connect([](int oldValue, int newValue) {
 *     // React to the changed value.
 * });
 * value->set(42);
 * @endcode
 */
template <serializable_variant_c T>
class Property final : public detail::ObjectFeature, public interfaces::IProperty {
   public:
    using type = std::conditional_t<std::is_base_of_v<interfaces::ISerializable, T>, std::shared_ptr<T>, T>;
    // std::conditional_t<std::is_trivially_copyable_v<T>, std::atomic<T>, T>>;

    explicit Property(std::string_view name, Object* pOwner)
        : detail::ObjectFeature{name, pOwner},
          m_signalValueChanged{std::make_shared<Signal<type, type>>("signalValueChanged", pOwner)} {
        // Class-typed properties are stored as shared_ptr and should be usable
        // immediately without requiring callers to manually assign
        // std::make_shared<T>().
        if constexpr (std::is_base_of_v<interfaces::ISerializable, T> && std::is_default_constructible_v<T>) {
            m_value = std::make_shared<T>();
        }
    }

    /** @brief Returns the current property value. */
    [[nodiscard]] auto get() const -> type {
        if constexpr (std::is_trivially_copyable_v<type>) {
            return m_value.load();
        } else {
            return getNonTrivial();
        }
    }

    /**
     * @brief Assigns a value and emits signalValueChanged() when it differs.
     * @param value New property value.
     */
    auto set(const type& value) -> void {
        if constexpr (std::is_trivially_copyable_v<type>) {
            auto currentValue = m_value.load(std::memory_order_relaxed);
            if (currentValue != value) {
                m_value.store(value, std::memory_order_relaxed);
                m_signalValueChanged->emit(currentValue, value);
            }
        } else {
            std::optional<type> oldValue;
            {
                std::lock_guard<std::mutex> lock{m_mutex};
                if (m_value != value) {
                    oldValue = m_value;
                    m_value = value;
                }
            }

            if (oldValue.has_value()) {
                m_signalValueChanged->emit(*oldValue, value);
            }
        }
    }

    // IObjectFeature interface
    [[nodiscard]] auto getName() const -> std::string_view override { return detail::ObjectFeature::getName(); }
    [[nodiscard]] auto getOwner() const -> Object* override { return detail::ObjectFeature::getOwner(); }

    // ISerializable interface
    [[nodiscard]] auto serialize() const -> std::string override { return serialization::serialize<type>(get()); }
    auto deserialize(std::string_view str) -> void override { set(serialization::deserialize<type>(str)); }

    /** @brief Returns the signal emitted after the value changes. */
    auto signalValueChanged() -> signal_ptr<type, type> { return m_signalValueChanged; }

    auto connectAnyChanged(std::function<void(std::string)> handler) -> void override {
        m_signalValueChanged->connect(
            [h = std::move(handler)](const type&, const type& newValue) { h(serialization::serialize<type>(newValue)); });
    }

   private:
    [[nodiscard]] auto getNonTrivial() const -> type {
#if !defined(NBN_LLVM_COVERAGE_ENABLE)
        std::lock_guard<std::mutex> lock{m_mutex};
#endif
        return m_value;
    }

    using stored_type = std::conditional_t<std::is_trivially_copyable_v<type>, std::atomic<type>, type>;
    stored_type m_value{};
    signal_ptr<type, type> m_signalValueChanged{};
    mutable std::mutex m_mutex{};
};

template <enum_serializable_c T>
class EnumProperty final : public detail::ObjectFeature, public interfaces::IProperty {
   public:
    using type = T;

    explicit EnumProperty(std::string_view name, Object* pOwner)
        : detail::ObjectFeature{name, pOwner},
          m_signalValueChanged{std::make_shared<Signal<type, type>>("signalValueChanged", pOwner)},
          m_signalIndexChanged{std::make_shared<Signal<int, int>>("signalIndexChanged", pOwner)} {}

    [[nodiscard]] auto get() const -> type {
        std::lock_guard<std::mutex> lock{m_mutex};
        if (m_values.empty() || m_index < 0 || static_cast<size_t>(m_index) >= m_values.size()) {
            return type{};
        }

        return valueAtLocked(m_index);
    }

    /**
     * @brief Assigns an enum value and emits change signals after unlocking.
     *
     * Signal handlers may safely query or update this property reentrantly.
     */
    auto set(const type& value) -> void {
        int oldIndex{0};
        int newIndex{0};
        type oldValue{};
        {
            std::lock_guard<std::mutex> lock{m_mutex};
            if (m_values.empty()) {
                return;
            }

            const std::string target{enum_serialization<type>::toString(value)};
            const auto it = std::find_if(m_values.begin(), m_values.end(), [&](const auto& candidate) {
                return std::holds_alternative<std::string>(candidate) && std::get<std::string>(candidate) == target;
            });
            if (it == m_values.end()) {
                return;
            }

            newIndex = static_cast<int>(std::distance(m_values.begin(), it));
            if (newIndex == m_index) {
                return;
            }

            oldIndex = m_index;
            oldValue = valueAtLocked(oldIndex);
            m_index = newIndex;
        }

        m_signalIndexChanged->emit(oldIndex, newIndex);
        m_signalValueChanged->emit(oldValue, value);
    }

    [[nodiscard]] auto values() const -> serializable_vector_t {
        std::lock_guard<std::mutex> lock{m_mutex};
        return m_values;
    }

    auto setValues(const serializable_vector_t& values, const std::vector<type>* typedValues = nullptr) -> void {
        std::unique_lock<std::mutex> lock{m_mutex};
        serializable_vector_t normalizedValues{};
        normalizedValues.reserve(values.size());
        for (const auto& value : values) {
            if (std::holds_alternative<std::string>(value)) {
                normalizedValues.push_back(value);
            }
        }

        const auto oldIndex = m_index;
        const auto hadOldValue = !m_values.empty() && oldIndex >= 0 && static_cast<size_t>(oldIndex) < m_values.size();
        const auto oldValue = hadOldValue ? valueAtLocked(oldIndex) : type{};

        m_values = std::move(normalizedValues);
        m_enumValues = typedValues != nullptr ? *typedValues : std::vector<type>{};

        if (m_values.empty()) {
            m_index = 0;
            return;
        }

        if (m_index < 0 || static_cast<size_t>(m_index) >= m_values.size()) {
            m_index = 0;
        }

        const auto newIndex = m_index;
        const auto newValue = valueAtLocked(newIndex);
        lock.unlock();
        if (oldIndex != newIndex) {
            m_signalIndexChanged->emit(oldIndex, newIndex);
        }
        if (hadOldValue && oldValue != newValue) {
            m_signalValueChanged->emit(oldValue, newValue);
        }
    }

    auto setEnumValues(const std::vector<type>& values) -> void {
        serializable_vector_t serializedValues{};
        serializedValues.reserve(values.size());
        for (const auto value : values) {
            serializedValues.emplace_back(std::string(enum_serialization<type>::toString(value)));
        }
        setValues(serializedValues, &values);
    }

    [[nodiscard]] auto index() const -> int {
        std::lock_guard<std::mutex> lock{m_mutex};
        return m_index;
    }

    auto setIndex(int index) -> void {
        std::unique_lock<std::mutex> lock{m_mutex};
        if (m_values.empty()) {
            m_index = 0;
            return;
        }
        if (index < 0 || static_cast<size_t>(index) >= m_values.size()) {
            return;
        }

        if (m_index == index) {
            return;
        }

        const auto oldIndex = m_index;
        const auto oldValue = valueAtLocked(oldIndex);
        m_index = index;
        const auto newIndex = m_index;
        const auto newValue = valueAtLocked(newIndex);

        lock.unlock();
        m_signalIndexChanged->emit(oldIndex, newIndex);
        m_signalValueChanged->emit(oldValue, newValue);
    }

    [[nodiscard]] auto getName() const -> std::string_view override { return detail::ObjectFeature::getName(); }
    [[nodiscard]] auto getOwner() const -> Object* override { return detail::ObjectFeature::getOwner(); }

    [[nodiscard]] auto serialize() const -> std::string override {
        std::lock_guard<std::mutex> lock{m_mutex};
        return std::format(R"({{"values":{},"index":{}}})", serialization::serialize(m_values), serialization::serialize(m_index));
    }

    auto deserialize(std::string_view str) -> void override {
        if (!str.empty() && str.front() != '{') {
            set(serialization::deserialize<type>(str));
            return;
        }

        std::unordered_map<std::string, std::string> properties{};
        size_t pos{0};
        serialization::json::parseObject(properties, str, pos);

        const auto itValues = properties.find("values");
        const auto itIndex = properties.find("index");

        setValues(itValues != properties.end() ? serialization::deserialize<serializable_vector_t>(itValues->second)
                                               : serializable_vector_t{});

        if (itIndex != properties.end()) {
            setIndex(serialization::deserialize<int>(itIndex->second));
        } else {
            setIndex(0);
        }
    }

    auto signalValueChanged() -> signal_ptr<type, type> { return m_signalValueChanged; }
    auto signalIndexChanged() -> signal_ptr<int, int> { return m_signalIndexChanged; }

    auto connectAnyChanged(std::function<void(std::string)> handler) -> void override {
        m_signalValueChanged->connect([h = std::move(handler)](const type&, const type& newValue) {
            h(std::string(enum_serialization<type>::toString(newValue)));
        });
    }

   private:
    [[nodiscard]] auto valueAtLocked(int index) const -> type {
        if (m_values.empty() || index < 0 || static_cast<size_t>(index) >= m_values.size()) {
            return type{};
        }
        if (static_cast<size_t>(index) < m_enumValues.size()) {
            return m_enumValues[static_cast<size_t>(index)];
        }
        const auto& rawValue = m_values[static_cast<size_t>(index)];
        if (!std::holds_alternative<std::string>(rawValue)) {
            return type{};
            // removes non-string entries.
        }
        return enum_serialization<type>::fromString(std::get<std::string>(rawValue));
    }

    serializable_vector_t m_values{};
    std::vector<type> m_enumValues{};
    int m_index{0};
    signal_ptr<type, type> m_signalValueChanged{};
    signal_ptr<int, int> m_signalIndexChanged{};
    mutable std::mutex m_mutex{};
};

}  // namespace detail

template <serializable_variant_c T>
using property_ptr = std::shared_ptr<detail::Property<T>>;

template <enum_serializable_c T>
using enum_property_ptr = std::shared_ptr<detail::EnumProperty<T>>;

}  // namespace nbn::core
