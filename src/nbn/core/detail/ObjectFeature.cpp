#include <memory>

#include "ObjectFeature.h"
#include "private/ObjectFeature.h"

namespace nbn::core::detail {

ObjectFeature::ObjectFeature(std::string_view name, Object* pOwner)
    : m_spImpl{std::make_unique<ObjectFeature::Impl>(name, pOwner)} {}

ObjectFeature::~ObjectFeature() = default;

[[nodiscard]] auto ObjectFeature::getName() const -> std::string_view {
    return m_spImpl->getName();
}

[[nodiscard]] auto ObjectFeature::getOwner() const -> Object* {
    return m_spImpl->getOwner();
}

}  // namespace nbn::core::detail