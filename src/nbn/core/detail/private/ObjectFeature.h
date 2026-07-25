#pragma once

#include <string>

#include "utils.h"

namespace nbn::core {

class Object;

namespace detail {

class ObjectFeature::Impl {
    nbn::core::Object* m_pOwner{nullptr};
    std::string m_name{};

   public:
    Impl(std::string_view name, Object* pOwner) : m_pOwner{pOwner}, m_name{nbn::core::utils::abi::getFunctionName(name)} {}

    ~Impl() = default;

    [[nodiscard]] auto getName() const -> std::string_view { return m_name; }

    [[nodiscard]] auto getOwner() const -> Object* { return m_pOwner; }
};

}  // namespace detail

}  // namespace nbn::core
