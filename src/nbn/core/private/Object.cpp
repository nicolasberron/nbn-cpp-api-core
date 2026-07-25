#include <algorithm>
#include <cctype>
#include <cstring>
#include <list>
#include <mutex>
#include <numeric>
#include <regex>
#include <unordered_map>

#include "Application.h"
#include "format_bridge.h"
#include "nbn/config.h"
#include "utils.h"

#include "private/Object.h"
#include "utils.h"

namespace nbn::core {

// Object::Impl

Object::Impl::Impl(Object* pDecl) : m_pDecl{pDecl}, m_uuid{utils::uuid::generate()} {}

[[nodiscard]] auto Object::Impl::getClassName() -> std::string_view {
    std::unique_lock lock(m_featuresMutex);
    if (m_className.empty()) {
        m_className = utils::abi::demangle(typeid(*m_pDecl).name());
    }
    return m_className;
}

[[nodiscard]] auto Object::Impl::getUuid() -> std::string_view {
    std::shared_lock lock(m_featuresMutex);
    return m_uuid;
}

[[nodiscard]] auto Object::Impl::signal(std::string_view functionName) -> std::shared_ptr<nbn::core::interfaces::IObjectFeature> {
    std::shared_lock lock(m_featuresMutex);
    auto it = m_signals.find(std::string{functionName});
    if (it == m_signals.end()) {
        return nullptr;
    }
    return it->second;
}

auto Object::Impl::addSignal(std::string_view functionName, std::shared_ptr<nbn::core::interfaces::IObjectFeature> spSignal)
    -> void {
    std::unique_lock lock(m_featuresMutex);
    m_signals.emplace(functionName, std::move(spSignal));
}

[[nodiscard]] auto Object::Impl::property(std::string_view propertyName) -> std::shared_ptr<nbn::core::interfaces::IProperty> {
    std::shared_lock lock(m_featuresMutex);
    auto it = m_properties.find(std::string{propertyName});
    if (it == m_properties.end()) {
        return nullptr;
    }
    return it->second;
}

auto Object::Impl::addProperty(std::string_view propertyName, std::shared_ptr<nbn::core::interfaces::IProperty> spProperty)
    -> void {
    std::unique_lock lock(m_featuresMutex);
    m_properties.emplace(propertyName, std::move(spProperty));
}

[[nodiscard]] auto Object::Impl::slot(std::string_view functionName) -> std::shared_ptr<nbn::core::interfaces::ISlot> {
    std::shared_lock lock(m_featuresMutex);
    auto it = m_slots.find(std::string{functionName});
    if (it == m_slots.end()) {
        return nullptr;
    }
    return it->second;
}

auto Object::Impl::addSlot(std::string_view functionName, std::shared_ptr<nbn::core::interfaces::ISlot> spSlot) -> void {
    std::unique_lock lock(m_featuresMutex);
    m_slots.emplace(functionName, std::move(spSlot));
}

[[nodiscard]] auto Object::Impl::getProperty(std::string_view name) -> std::shared_ptr<nbn::core::interfaces::IProperty> {
    std::shared_lock lock(m_featuresMutex);
    auto it{m_properties.find(std::string{name})};
    nbn::log::fatal_if(it == m_properties.end(), std::format("Property {} not found", name));
    return it->second;
}

[[nodiscard]] auto Object::Impl::getProperties() const -> std::vector<std::shared_ptr<nbn::core::interfaces::IProperty>> {
    std::shared_lock lock(m_featuresMutex);
    std::vector<std::shared_ptr<nbn::core::interfaces::IProperty>> result{};
    result.reserve(m_properties.size());
    for (const auto& [name, spProp] : m_properties) {
        result.emplace_back(spProp);
    }
    return result;
}

[[nodiscard]] auto Object::Impl::serialize() const -> std::string {
    const auto className{std::string{m_pDecl->getClassName()}};
    std::shared_lock lock(m_featuresMutex);
    auto json{std::format(R"({{"{}":{{)", className)};
    // uuid is not a property, but we serialize it as a property
    auto uuidProperty{std::make_shared<nbn::core::Property<std::string>>("uuid", this->m_pDecl)};
    uuidProperty->set(m_uuid);

    // Accumulate all properties
    std::list<std::string> serializedProperties{};
    serializedProperties.emplace_back(std::format("\"uuid\":{}", uuidProperty->serialize()));
    for (const auto& spProperty : m_properties) {
        serializedProperties.emplace_back(std::format("\"{}\":{}", spProperty.first, spProperty.second->serialize()));
    }
    // Signals are runtime-only features, but include their names in the object envelope so
    // serialized feature metadata remains discoverable without attempting to serialize handlers.
    for (const auto& [name, unusedSignal] : m_signals) {
        static_cast<void>(unusedSignal);
        serializedProperties.emplace_back(std::format("\"{}\":null", name));
    }
    for (const auto& spSlot : m_slots) {
        serializedProperties.emplace_back(std::format("\"{}\":{}", spSlot.first, spSlot.second->serialize()));
    }

    // Join all properties with a comma
    json += std::accumulate(std::next(serializedProperties.begin()), serializedProperties.end(), serializedProperties.front(),
                            [](const std::string& a, const std::string& b) { return a + "," + b; });

    json += "}}";

    return json;
}

auto Object::Impl::deserialize(std::string_view str) -> void {
    std::unordered_map<std::string, std::string> propertiesAsString{};
    size_t pos{0};
    try {
        nbn::core::serialization::json::parseObject(propertiesAsString, str, pos);
    } catch (const std::exception&) {
        // Keep object deserialization best-effort. Older payloads may contain an unquoted UUID
        // or malformed optional property data; neither should discard the live object's state.
        const auto uuidKey = str.find("\"uuid\":");
        if (uuidKey != std::string_view::npos) {
            auto uuidStart = uuidKey + std::string_view{"\"uuid\":"}.size();
            while (uuidStart < str.size() && std::isspace(static_cast<unsigned char>(str[uuidStart])) != 0) {
                ++uuidStart;
            }
            auto uuidEnd = str.find_first_of(",}", uuidStart);
            if (uuidEnd == std::string_view::npos) {
                uuidEnd = str.size();
            }
            auto uuid{std::string{str.substr(uuidStart, uuidEnd - uuidStart)}};
            if (uuid.size() >= 2U && uuid.front() == '"' && uuid.back() == '"') {
                uuid = uuid.substr(1, uuid.size() - 2U);
            }
            std::unique_lock lock(m_featuresMutex);
            m_uuid = std::move(uuid);
        }
        return;
    }

    // uuid
    auto it{propertiesAsString.find("uuid")};
    nbn::log::fatal_if(it == propertiesAsString.end(), "uuid not found");
    auto uuid{it->second};
    // Remove leading and trailing quotes if any
    if (uuid.size() >= 2U && uuid.front() == '"' && uuid.back() == '"') {
        uuid = uuid.substr(1, uuid.size() - 2);
    }

    std::vector<std::pair<std::string, std::shared_ptr<nbn::core::interfaces::IProperty>>> properties{};
    std::vector<std::pair<std::string, std::shared_ptr<nbn::core::interfaces::ISlot>>> slots{};
    {
        std::shared_lock lock(m_featuresMutex);
        properties.reserve(m_properties.size());
        for (const auto& property : m_properties) {
            properties.emplace_back(property.first, property.second);
        }
        slots.reserve(m_slots.size());
        for (const auto& slot : m_slots) {
            slots.emplace_back(slot.first, slot.second);
        }
    }
    {
        std::unique_lock lock(m_featuresMutex);
        m_uuid = std::move(uuid);
    }

    for (const auto& property : properties) {
        auto it{propertiesAsString.find(property.first)};
        if (it != propertiesAsString.end()) {
            try {
                property.second->deserialize(it->second);
            } catch (const std::exception& e) {
                nbn::log::error(std::format("Failed to deserialize property {}: {}", property.first, e.what()));
            }
        } else {
            nbn::log::fatal(std::format("Property {} not found", property.first));
        }
    }

    for (const auto& slotEntry : slots) {
        auto it{propertiesAsString.find(slotEntry.first)};
        if (it != propertiesAsString.end()) {
            try {
                slotEntry.second->deserialize(it->second);
            } catch (const std::exception& e) {
                nbn::log::error(std::format("Failed to deserialize slot {}: {}", slotEntry.first, e.what()));
            }
        }
    }
}

}  // namespace nbn::core