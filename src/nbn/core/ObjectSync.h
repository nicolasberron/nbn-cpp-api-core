#pragma once

#include <atomic>
#include <memory>

#include "Object.h"

namespace nbn::core {

/**
 * @class ObjectSync
 * @brief Bidirectional property synchronizer for two Object instances.
 *
 * Connects all matching properties (same name) of two objects so that a
 * change on either side is automatically propagated to the other via
 * serialize / deserialize.  Unmatched properties on either side are ignored.
 *
 * @par Requirements
 * Both objects must have their properties already registered (i.e. each
 * property accessor must have been called at least once) before ObjectSync::create()
 * is invoked.  In practice, this means the objects should be fully constructed
 * and/or deserialized from JSON before syncing.
 *
 * @par Lifetime
 * The sync is active as long as the returned shared_ptr is alive.
 * Destroying it stops all propagation.
 *
 * @par Loop safety
 * Propagation is inherently loop-safe: Property<T>::set() emits
 * signalValueChanged only when the value actually changes, so once both sides
 * hold the same value no further emission occurs.
 *
 * @par Usage example
 * @code{.cpp}
 * auto firstObject = std::make_shared<MyObject>();
 * auto secondObject = std::make_shared<MyObject>();
 *
 * // ensure properties are initialized first (e.g. via deserialize or direct access)
 * auto sync = nbn::core::ObjectSync::create({firstObject, secondObject});
 *
 * firstObject->value()->set("updated");  // secondObject follows automatically
 * @endcode
 */
class nbn_export_core ObjectSync {
   public:
    struct Objects {
        std::shared_ptr<Object> first;
        std::shared_ptr<Object> second;
    };

    /**
     * @brief Connect matching properties of @p objects for bidirectional sync.
     * @return A shared ObjectSync handle; destroy it to disconnect.
     */
    [[nodiscard]] static auto create(Objects objects) -> std::shared_ptr<ObjectSync>;

    ~ObjectSync();

    ObjectSync(const ObjectSync&) = delete;
    auto operator=(const ObjectSync&) = delete;
    ObjectSync(ObjectSync&&) = delete;
    auto operator=(ObjectSync&&) = delete;

   private:
    ObjectSync() = default;

    std::shared_ptr<std::atomic<bool>> m_active{std::make_shared<std::atomic<bool>>(true)};
};

}  // namespace nbn::core
