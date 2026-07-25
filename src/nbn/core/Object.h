#pragma once

#include <source_location>
#include <string>

#include "detail/Property.h"
#include "detail/Signal.h"
#include "interfaces/object.h"

namespace nbn::core {

/**
 * @file Object.h
 * @brief Base object model with runtime identity, serialization, signals, and properties.
 */

template <typename... Args>
using Signal = detail::Signal<Args...>;

template <typename U>
using Property = detail::Property<U>;

template <typename U>
using EnumProperty = detail::EnumProperty<U>;

template <typename... Args>
using Slot = detail::Slot<Args...>;

/**
 * @class Object
 * @brief Base class for serializable, introspectable runtime objects.
 *
 * @par Usage example
 * @code{.cpp}
 * class Worker final : public nbn::core::Object {
 * public:
 *     static bool registered;
 * };
 *
 * bool Worker::registered = nbn::core::Object::factoryRegister<Worker>();
 *
 * auto spObject = nbn::core::Object::factoryCreate("Worker");
 * if (spObject) {
 *     auto uuid = spObject->getUuid();
 * }
 * @endcode
 */
class nbn_export_core Object : public interfaces::IObject, public interfaces::ISerializable {
   public:
    struct FactoryCreateOptions {
        std::string_view serializedPayload{};
    };

    Object();
    ~Object() override;

    // Disable copy and move
    Object(const Object&) noexcept = delete;
    auto operator=(const Object&) noexcept -> Object& = delete;
    Object(Object&&) noexcept = delete;
    auto operator=(Object&&) noexcept -> Object& = delete;

    // IObject interface
    [[nodiscard]] auto getClassName() const -> std::string_view final;
    [[nodiscard]] auto getUuid() -> std::string_view final;

    // Serialization
    [[nodiscard]] auto serialize() const -> std::string override;
    auto deserialize(std::string_view str) -> void override;

    // Signals
    auto signalDestroyed() -> signal_ptr<> { return signal<>(); }

    [[nodiscard]] auto getProperty(std::string_view name) -> std::shared_ptr<nbn::core::interfaces::IProperty>;
    [[nodiscard]] auto getProperties() const -> std::vector<std::shared_ptr<nbn::core::interfaces::IProperty>>;

    // Factory registration / creation
    /**
     * @brief Registers a default-constructible class using its type name.
     * @tparam Class Object-derived class to register.
     * @return true when registration succeeds, or false when the name exists.
     * @par Usage example
     * @code{.cpp}
     * const bool registered = nbn::core::Object::factoryRegister<Worker>();
     * @endcode
     */
    template <typename Class>
    [[nodiscard]] static auto factoryRegister() -> bool {
        return factoryRegister(nbn::core::utils::abi::getTypeName<Class>(),
                               []() -> std::shared_ptr<Object> { return std::make_shared<Class>(); });
    }

    /**
     * @brief Registers a class under an explicit name.
     * @tparam Class Object-derived class to register.
     * @param name Name used by factoryCreate().
     * @return true when registration succeeds, or false when the name exists.
     * @par Usage example
     * @code{.cpp}
     * nbn::core::Object::factoryRegister<Worker>("Worker");
     * @endcode
     */
    template <typename Class>
    [[nodiscard]] static auto factoryRegister(std::string_view name) -> bool {
        return factoryRegister(name, []() -> std::shared_ptr<Object> { return std::make_shared<Class>(); });
    }

    /**
     * @brief Creates an object registered under a name.
     * @param name Registered class name.
     * @return A new object, or nullptr when no class is registered under name.
     */
    [[nodiscard]] static auto factoryCreate(std::string_view name) -> std::shared_ptr<Object>;

    [[nodiscard]] static auto factoryCreate(std::string_view name, FactoryCreateOptions options) -> std::shared_ptr<Object>;

    template <typename Class>
    [[nodiscard]] static auto factoryCreate(std::string_view name) -> std::shared_ptr<Class> {
        return std::dynamic_pointer_cast<Class>(factoryCreate(name));
    }

    template <typename Class>
    [[nodiscard]] static auto factoryCreate(std::string_view name, FactoryCreateOptions options) -> std::shared_ptr<Class> {
        return std::dynamic_pointer_cast<Class>(factoryCreate(name, options));
    }

    [[nodiscard]] static auto factoryRegisteredClasses() -> std::vector<std::string>;

    [[nodiscard]] static auto factoryHas(std::string_view name) -> bool;

    auto addProperty(const std::source_location& location, std::shared_ptr<nbn::core::interfaces::IProperty> spProperty) -> void;

   protected:
    template <typename... Args>
    [[nodiscard]] auto signal(const std::source_location& location = std::source_location::current()) -> signal_ptr<Args...>;

    template <typename U>
    [[nodiscard]] auto property(const std::source_location& location = std::source_location::current()) -> property_ptr<U>;

    template <typename U>
    [[nodiscard]] auto enumProperty(const std::source_location& location = std::source_location::current()) -> enum_property_ptr<U>;

    template <typename... Args>
    [[nodiscard]] auto slot(const std::source_location& location = std::source_location::current()) -> slot_ptr<Args...>;

    auto addSignal(const std::source_location& location, std::shared_ptr<nbn::core::interfaces::IObjectFeature> spSignal) -> void;
    auto addSlot(const std::source_location& location, std::shared_ptr<nbn::core::interfaces::ISlot> spSlot) -> void;

   private:
    class Impl;
    std::unique_ptr<Impl> m_spImpl{};

    [[nodiscard]] auto signal(const std::source_location& location) -> std::shared_ptr<nbn::core::interfaces::IObjectFeature>;

    [[nodiscard]] auto property(const std::source_location& location) -> std::shared_ptr<nbn::core::interfaces::IProperty>;

    [[nodiscard]] auto slot(const std::source_location& location) -> std::shared_ptr<nbn::core::interfaces::ISlot>;

    static auto factoryRegister(std::string_view name, std::function<std::shared_ptr<Object>()> creator) -> bool;
};

}  // namespace nbn::core

#include "detail/Object.h"