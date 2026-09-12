#include "ObjectSync.h"

#include <algorithm>

namespace nbn::core {

auto ObjectSync::create(Objects objects) -> std::shared_ptr<ObjectSync> {
    auto spSync = std::shared_ptr<ObjectSync>(new ObjectSync{});
    auto active = spSync->m_active;
    const auto& a = objects.first;
    const auto& b = objects.second;

    const auto propsA = a->getProperties();
    const auto propsB = b->getProperties();

    for (const auto& spPropA : propsA) {
        const auto name = std::string{spPropA->getName()};

        const auto itB = std::find_if(propsB.begin(), propsB.end(), [&](const auto& p) { return p->getName() == name; });
        if (itB == propsB.end()) {
            continue;
        }
        const auto spPropB = *itB;
        const std::weak_ptr<nbn::core::interfaces::IProperty> wpPropB{spPropB};
        const std::weak_ptr<nbn::core::interfaces::IProperty> wpPropA{spPropA};

        // A → B: when A's property changes, propagate to B
        auto wpB = std::weak_ptr<Object>(b);
        spPropA->connectAnyChanged([active, wpB, wpPropB](std::string serialized) {
            if (!active->load()) {
                return;
            }
            auto spB = wpB.lock();
            if (!spB) {
                return;
            }
            if (const auto spBProperty = wpPropB.lock()) {
                spBProperty->deserialize(serialized);
            }
        });

        // B → A: when B's property changes, propagate to A
        auto wpA = std::weak_ptr<Object>(a);
        spPropB->connectAnyChanged([active, wpA, wpPropA](std::string serialized) {
            if (!active->load()) {
                return;
            }
            auto spA = wpA.lock();
            if (!spA) {
                return;
            }
            if (const auto spAProperty = wpPropA.lock()) {
                spAProperty->deserialize(serialized);
            }
        });
    }

    return spSync;
}

ObjectSync::~ObjectSync() {
    m_active->store(false);
}

}  // namespace nbn::core
