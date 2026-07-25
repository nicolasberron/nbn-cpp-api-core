#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <variant>

#include "../detail/serialization.h"
#include "Interface.h"

namespace nbn::core {

class Object;

namespace interfaces {

/**
 * @file interfaces/object.h
 * @brief Object-model interfaces for identity and object features.
 */

/**
 * @struct IObject
 * @brief Public identity interface implemented by runtime objects.
 */

struct nbn_export_core IObject : public Interface {
    /** @brief Returns the runtime class name. */
    [[nodiscard]] virtual auto getClassName() const -> std::string_view = 0;
    /** @brief Returns the object's unique identifier. */
    [[nodiscard]] virtual auto getUuid() -> std::string_view = 0;
};

/**
 * @struct IObjectFeature
 * @brief Base interface for object-bound features (properties/signals).
 */
struct nbn_export_core IObjectFeature : public Interface {
    /** @brief Returns the feature's registered name. */
    [[nodiscard]] virtual auto getName() const -> std::string_view = 0;
    /** @brief Returns the object that owns this feature. */
    [[nodiscard]] virtual auto getOwner() const -> Object* = 0;
};

/**
 * @struct IProperty
 * @brief Marker interface for serializable object properties.
 */
struct nbn_export_core IProperty : public IObjectFeature, public ISerializable {
    /**
     * @brief Connects a handler receiving the serialized new property value.
     * @param handler Callback invoked after the property changes.
     */
    virtual auto connectAnyChanged(std::function<void(std::string)> handler) -> void = 0;
};

/**
 * @struct ISlot
 * @brief Marker interface for serializable signal slots (handlers identified by name).
 */
struct nbn_export_core ISlot : public IObjectFeature, public ISerializable {
    /** @brief Returns the fully-qualified types accepted by the slot. */
    [[nodiscard]] virtual auto getArgTypeNames() const -> std::vector<std::string> = 0;
};

}  // namespace interfaces

}  // namespace nbn::core
