#pragma once

#include "../interfaces/object.h"

namespace nbn::core::detail {

class ObjectFeature : public nbn::core::interfaces::IObjectFeature {
    class Impl;
    std::unique_ptr<Impl> m_spImpl{};

   public:
    explicit ObjectFeature(std::string_view name, Object* pOwner = nullptr);
    ~ObjectFeature() override;

    [[nodiscard]] auto getName() const -> std::string_view override;
    [[nodiscard]] auto getOwner() const -> Object* override;
};

}  // namespace nbn::core::detail
