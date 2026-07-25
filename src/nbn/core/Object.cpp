#include "Object.h"
#include "private/Application.h"
#include "private/Object.h"
#include "utils.h"

namespace nbn::core {

// Factory registration
static auto const registerToFactory{[]() { return Object::factoryRegister<Object>(); }()};

Object::Object() : m_spImpl{std::make_unique<Object::Impl>(this)} {}

Object::~Object() {
    signalDestroyed()->emit();
}

[[nodiscard]] auto Object::getClassName() const -> std::string_view {
    return m_spImpl->getClassName();
}

[[nodiscard]] auto Object::getUuid() -> std::string_view {
    return m_spImpl->getUuid();
}

[[nodiscard]] auto Object::serialize() const -> std::string {
    return m_spImpl->serialize();
}

auto Object::deserialize(std::string_view str) -> void {
    m_spImpl->deserialize(str);
}

auto Object::addSignal(const std::source_location&, std::shared_ptr<nbn::core::interfaces::IObjectFeature> spSignal) -> void {
    const auto name = std::string{spSignal->getName()};
    m_spImpl->addSignal(name, std::move(spSignal));
}

[[nodiscard]] auto Object::signal(const std::source_location& location) -> std::shared_ptr<nbn::core::interfaces::IObjectFeature> {
    return m_spImpl->signal(nbn::core::utils::abi::getFunctionName(location.function_name()));
}

auto Object::addProperty(const std::source_location&, std::shared_ptr<nbn::core::interfaces::IProperty> spProperty) -> void {
    const auto name = std::string{spProperty->getName()};
    m_spImpl->addProperty(name, std::move(spProperty));
}

[[nodiscard]] auto Object::property(const std::source_location& location) -> std::shared_ptr<nbn::core::interfaces::IProperty> {
    return m_spImpl->property(nbn::core::utils::abi::getFunctionName(location.function_name()));
}

auto Object::addSlot(const std::source_location&, std::shared_ptr<nbn::core::interfaces::ISlot> spSlot) -> void {
    const auto name = std::string{spSlot->getName()};
    m_spImpl->addSlot(name, std::move(spSlot));
}

[[nodiscard]] auto Object::slot(const std::source_location& location) -> std::shared_ptr<nbn::core::interfaces::ISlot> {
    return m_spImpl->slot(nbn::core::utils::abi::getFunctionName(location.function_name()));
}

[[nodiscard]] auto Object::getProperty(std::string_view name) -> std::shared_ptr<nbn::core::interfaces::IProperty> {
    return m_spImpl->getProperty(name);
}

[[nodiscard]] auto Object::getProperties() const -> std::vector<std::shared_ptr<nbn::core::interfaces::IProperty>> {
    return m_spImpl->getProperties();
}

auto Object::factoryRegister(std::string_view name, std::function<std::shared_ptr<Object>()> creator) -> bool {
    return private_ns::Application::self().factoryRegister(name, creator);
}

auto Object::factoryCreate(std::string_view name) -> std::shared_ptr<Object> {
    return private_ns::Application::self().factoryCreate(name);
}

auto Object::factoryCreate(std::string_view name, FactoryCreateOptions options) -> std::shared_ptr<Object> {
    auto spObject{factoryCreate(name)};
    spObject->deserialize(options.serializedPayload);
    return spObject;
}

auto Object::factoryRegisteredClasses() -> std::vector<std::string> {
    return private_ns::Application::self().factoryRegisteredClasses();
}

auto Object::factoryHas(std::string_view name) -> bool {
    return private_ns::Application::self().factoryHas(name);
}

}  // namespace nbn::core