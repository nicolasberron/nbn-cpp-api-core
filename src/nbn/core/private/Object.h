#pragma once

#include <shared_mutex>

#include "../Object.h"

namespace nbn::core {

class Object::Impl : public interfaces::ISerializable {
    Object* m_pDecl{nullptr};

   public:
    Impl(Object* pDecl);

    ~Impl() override = default;

    [[nodiscard]] auto getClassName() -> std::string_view;

    [[nodiscard]] auto getUuid() -> std::string_view;

    [[nodiscard]] auto signal(std::string_view functionName) -> std::shared_ptr<nbn::core::interfaces::IObjectFeature>;

    auto addSignal(std::string_view functionName, std::shared_ptr<nbn::core::interfaces::IObjectFeature> spSignal) -> void;

    [[nodiscard]] auto property(std::string_view propertyName) -> std::shared_ptr<nbn::core::interfaces::IProperty>;

    auto addProperty(std::string_view propertyName, std::shared_ptr<nbn::core::interfaces::IProperty> spProperty) -> void;

    [[nodiscard]] auto getProperty(std::string_view name) -> std::shared_ptr<nbn::core::interfaces::IProperty>;
    [[nodiscard]] auto getProperties() const -> std::vector<std::shared_ptr<nbn::core::interfaces::IProperty>>;
    [[nodiscard]] auto slot(std::string_view functionName) -> std::shared_ptr<nbn::core::interfaces::ISlot>;
    auto addSlot(std::string_view functionName, std::shared_ptr<nbn::core::interfaces::ISlot> spSlot) -> void;

    // ISerializable interface
    [[nodiscard]] auto serialize() const -> std::string override;

    auto deserialize(std::string_view str) -> void override;

   private:
    std::string m_className{};
    std::string m_uuid{};
    std::unordered_map<std::string, std::shared_ptr<nbn::core::interfaces::IObjectFeature>> m_signals{};
    std::unordered_map<std::string, std::shared_ptr<nbn::core::interfaces::IProperty>> m_properties{};
    std::unordered_map<std::string, std::shared_ptr<nbn::core::interfaces::ISlot>> m_slots{};
    mutable std::shared_mutex m_featuresMutex{};
};

}  // namespace nbn::core