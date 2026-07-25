#pragma once

#include <functional>
#include <future>
#include <mutex>
#include <vector>

#include "../utils.h"
#include "ObjectFeature.h"

namespace nbn::core {

namespace detail {

/**
 * @file Signal.h
 * @brief Type-safe signal and slot event dispatching.
 */

namespace internal {
// Implementation detail — not part of the public API. Use nbn::core::sender() instead.
[[nodiscard]] inline auto currentSender() -> Object*& {
    thread_local Object* s{nullptr};
    return s;
}
}  // namespace internal

// Forward declaration so Signal can reference Slot in its connect/disconnect overloads.
template <slot_arg_c... Args>
class Slot;

/**
 * @class Signal
 * @brief Emits a typed event to connected callable handlers.
 * @tparam Args Argument types passed to each handler.
 *
 * Connections and emission are synchronized. An asynchronous emission can be
 * started with emitAsync().
 *
 * @par Usage example
 * @code{.cpp}
 * nbn::core::Signal<int> signal{"valueChanged"};
 * signal.connect([](int value) {
 *     // Handle the new value.
 * });
 * signal.emit(42);
 * @endcode
 */
template <slot_arg_c... Args>
class Signal : public nbn::core::detail::ObjectFeature {
   public:
    using slot_t = std::function<void(Args...)>;

    explicit Signal(std::string name, Object* pOwner = nullptr) : nbn::core::detail::ObjectFeature(std::move(name), pOwner) {}

    /** @brief Connects a callable handler to the signal. */
    void connect(const slot_t& slot) {
        std::lock_guard lock(m_mutex);
        m_slots.emplace_back(slot);
    }

    /**
     * @brief Connects a named slot and its handler to the signal.
     * @param slot Slot identity associated with the connection.
     * @param handler Callable invoked when the signal is emitted.
     */
    void connect(std::shared_ptr<Slot<Args...>> slot, slot_t handler) {
        std::lock_guard lock(m_mutex);
        m_slotConnections.emplace_back(std::move(slot), std::move(handler));
    }

    /** @brief Disconnects the first matching callable handler. */
    void disconnect(const slot_t& slot) {
        std::lock_guard lock(m_mutex);
        auto it =
            std::find_if(m_slots.begin(), m_slots.end(), [&](const slot_t& s) { return s.target_type() == slot.target_type(); });
        if (it != m_slots.end()) {
            m_slots.erase(it);
        }
    }

    /** @brief Disconnects a named slot and removes expired slot connections. */
    void disconnect(std::shared_ptr<Slot<Args...>> slot) {
        std::lock_guard lock(m_mutex);
        std::erase_if(m_slotConnections, [&](const auto& entry) {
            auto sp = entry.first.lock();
            return !sp || sp.get() == slot.get();
        });
    }

    /** @brief Disconnects matching handlers also connected to another signal. */
    template <slot_arg_c... OtherArgs>
    void disconnect(Signal<OtherArgs...>& otherSignal) {
        std::lock_guard lock(m_mutex);
        for (const auto& slot : otherSignal.m_slots) {
            auto it = std::find_if(m_slots.begin(), m_slots.end(),
                                   [&](const slot_t& s) { return s.target_type() == slot.target_type(); });
            if (it != m_slots.end()) {
                m_slots.erase(it);
            }
        }
    }

    /**
     * @brief Synchronously invokes all connected handlers.
     * @param args Arguments forwarded to each handler.
     *
     * Handlers are snapshotted under the connection mutex and invoked after
     * the mutex is released. Handlers may therefore connect, disconnect, or
     * emit signals reentrantly. The snapshot preserves the connection order
     * at the start of dispatch.
     */
    void emit(Args... args) {
        std::vector<slot_t> slots;
        std::vector<std::pair<std::weak_ptr<Slot<Args...>>, slot_t>> slotConnections;
        {
            std::lock_guard lock(m_mutex);
            slots = m_slots;
            slotConnections = m_slotConnections;
        }

        auto& senderRef = internal::currentSender();
        auto* const prevSender = senderRef;
        senderRef = getOwner();
        struct SenderGuard final {
            Object* previous;
            ~SenderGuard() { internal::currentSender() = previous; }
        } senderGuard{prevSender};

        for (const auto& slot : slots) {
            slot(args...);
        }
        for (const auto& [weakSlot, callable] : slotConnections) {
            if (weakSlot.lock()) {
                callable(args...);
            }
        }
    }

    /**
     * @brief Emits the signal asynchronously.
     * @param args Arguments forwarded to each handler.
     * @return A future that completes after dispatch finishes.
     */
    auto emitAsync(Args... args) -> std::future<void> {
        return std::async(std::launch::async, [this, args...]() { emit(args...); });
    }

    /** @brief Returns the fully-qualified names of the signal argument types. */
    [[nodiscard]] auto getArgTypeNames() const -> std::vector<std::string> {
        return {nbn::core::utils::abi::qualifiedTypeName<Args>()...};
    }

   private:
    std::vector<slot_t> m_slots{};
    std::vector<std::pair<std::weak_ptr<Slot<Args...>>, slot_t>> m_slotConnections{};
    std::mutex m_mutex{};
};

/**
 * @class Slot
 * @brief A pure-identity, serializable named event receiver (e.g. onClicked).
 *
 * Carries no callable state. It is an identity tag used for named, serializable connections:
 * the Signal stores the (slot → handler) pair and sets nbn::core::sender() before each
 * invocation. Wire a signal to this slot with signal->connect(slot_ptr, handler).
 */
template <slot_arg_c... Args>
class Slot final : public nbn::core::detail::ObjectFeature, public interfaces::ISlot {
   public:
    explicit Slot(std::string_view name, Object* pOwner = nullptr) : nbn::core::detail::ObjectFeature(std::string{name}, pOwner) {}

    // IObjectFeature – resolve non-virtual diamond with ObjectFeature
    [[nodiscard]] auto getName() const -> std::string_view override { return ObjectFeature::getName(); }
    [[nodiscard]] auto getOwner() const -> Object* override { return ObjectFeature::getOwner(); }

    [[nodiscard]] auto getArgTypeNames() const -> std::vector<std::string> override {
        return {nbn::core::utils::abi::qualifiedTypeName<Args>()...};
    }

    // ISerializable – serialized as {"name":"slotName","args":["type1",...]}; fixed at construction
    [[nodiscard]] auto serialize() const -> std::string override {
        const auto argTypes = getArgTypeNames();
        std::string argsJson{"["};
        for (size_t i = 0; i < argTypes.size(); ++i) {
            if (i > 0) {
                argsJson += ",";
            }
            argsJson += serialization::value::serialize<std::string>(argTypes[i]);
        }
        argsJson += "]";
        return std::format(R"({{"name":{},"args":{}}})", serialization::value::serialize<std::string>(std::string{getName()}),
                           argsJson);
    }
    auto deserialize(std::string_view) -> void override { /* name and args are fixed at construction */ }
};

}  // namespace detail

/**
 * @brief Returns the Object that owns the signal currently being emitted on this thread.
 *
 * Valid only during the synchronous execution of a slot handler triggered by Signal::emit().
 * Returns nullptr outside of a signal dispatch or when emitted by an ownerless signal.
 */
[[nodiscard]] inline auto sender() -> Object* {
    return detail::internal::currentSender();
}

template <slot_arg_c... Args>
using signal_ptr = std::shared_ptr<detail::Signal<Args...>>;

template <slot_arg_c... Args>
using slot_ptr = std::shared_ptr<detail::Slot<Args...>>;

}  // namespace nbn::core