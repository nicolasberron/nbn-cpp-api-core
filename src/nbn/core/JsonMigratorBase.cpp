#include "JsonMigratorBase.h"
#include "nbn/config.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

#include <nbn/core/Application.h>
#include <nbn/core/Object.h>
#include <nbn/core/detail/serialization.h>
#include <nbn/core/utils.h>
#include <nbn/core/utils/semver.h>

namespace nbn::core::utils::nbn_api_json_loader {

auto signalMigrated() -> nbn::core::signal_ptr<std::string, std::string> {
    static const auto spSignal = std::make_shared<nbn::core::Signal<std::string, std::string>>("signalMigrated");
    return spSignal;
}

// ---------------------------------------------------------------------------
// Static JSON helpers
// ---------------------------------------------------------------------------

auto JsonMigratorBase::wasJsonBeautified(std::string_view jsonPayload) -> bool {
    for (const auto ch : jsonPayload) {
        if (ch == '\n') {
            return true;
        }
    }
    return false;
}

auto JsonMigratorBase::skipWhitespaces(std::string_view text, size_t pos) -> size_t {
    while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos])) != 0) {
        ++pos;
    }
    return pos;
}

auto JsonMigratorBase::trim(std::string_view text) -> std::string_view {
    auto start = skipWhitespaces(text, 0);
    auto end = text.size();
    while (end > start && std::isspace(static_cast<unsigned char>(text[end - 1])) != 0) {
        --end;
    }
    return text.substr(start, end - start);
}

auto JsonMigratorBase::findMatchingBraces(std::string_view text, size_t openPos) -> size_t {
    if (openPos >= text.size() || text[openPos] != '{') {
        return std::string::npos;
    }

    size_t depth = 0;
    bool isInString = false;
    bool isEscaped = false;

    for (size_t i = openPos; i < text.size(); ++i) {
        const auto ch = text[i];

        if (isInString) {
            if (isEscaped) {
                isEscaped = false;
                continue;
            }
            if (ch == '\\') {
                isEscaped = true;
                continue;
            }
            if (ch == '"') {
                isInString = false;
            }
            continue;
        }

        if (ch == '"') {
            isInString = true;
            continue;
        }
        if (ch == '{') {
            ++depth;
            continue;
        }
        if (ch == '}') {
            --depth;
            if (depth == 0) {
                return i;
            }
        }
    }

    return std::string::npos;
}

auto JsonMigratorBase::findMatchingBrackets(std::string_view text, size_t openPos) -> size_t {
    if (openPos >= text.size() || text[openPos] != '[') {
        return std::string::npos;
    }

    size_t depth = 0;
    bool isInString = false;
    bool isEscaped = false;

    for (size_t i = openPos; i < text.size(); ++i) {
        const auto ch = text[i];

        if (isInString) {
            if (isEscaped) {
                isEscaped = false;
                continue;
            }
            if (ch == '\\') {
                isEscaped = true;
                continue;
            }
            if (ch == '"') {
                isInString = false;
            }
            continue;
        }

        if (ch == '"') {
            isInString = true;
            continue;
        }
        if (ch == '[') {
            ++depth;
            continue;
        }
        if (ch == ']') {
            --depth;
            if (depth == 0) {
                return i;
            }
        }
    }

    return std::string::npos;
}

auto JsonMigratorBase::serializeObject(const std::unordered_map<std::string, std::string>& properties, bool isTopLevelMetadataFirst)
    -> std::string {
    std::vector<std::string> keys{};
    keys.reserve(properties.size());
    for (const auto& [key, _] : properties) {
        keys.push_back(key);
    }
    std::sort(keys.begin(), keys.end());

    if (!isTopLevelMetadataFirst) {
        std::string json{"{"};
        bool isFirst = true;
        for (const auto& key : keys) {
            if (!isFirst) {
                json += ',';
            }
            isFirst = false;
            json += std::format("\"{}\":{}", nbn::core::serialization::json::escapeString(key), properties.at(key));
        }
        json += '}';
        return json;
    }

    std::vector<std::string> orderedKeys{};
    orderedKeys.reserve(keys.size());

    constexpr std::array<std::string_view, 3> preferredKeys{"nbnProject", "nbnApiVersion", "version"};
    for (const auto preferredKey : preferredKeys) {
        if (properties.contains(std::string(preferredKey))) {
            orderedKeys.push_back(std::string(preferredKey));
        }
    }

    for (const auto& key : keys) {
        if (key != "nbnProject" && key != "nbnApiVersion" && key != "version") {
            orderedKeys.push_back(key);
        }
    }

    std::string json{"{"};
    bool isFirst = true;
    for (const auto& key : orderedKeys) {
        if (!isFirst) {
            json += ',';
        }
        isFirst = false;
        json += std::format("\"{}\":{}", nbn::core::serialization::json::escapeString(key), properties.at(key));
    }
    json += '}';
    return json;
}

auto JsonMigratorBase::splitArrayItems(std::string_view arrayJson) -> std::vector<std::string_view> {
    std::vector<std::string_view> items{};
    auto array = trim(arrayJson);
    if (array.size() < 2 || array.front() != '[' || array.back() != ']') {
        return items;
    }

    const auto end = array.size() - 1;
    auto start = skipWhitespaces(array, 1);
    if (start >= end) {
        return items;
    }

    int depth = 0;
    bool isInString = false;
    bool isEscaped = false;

    for (size_t i = start; i < end; ++i) {
        const auto ch = array[i];
        if (isInString) {
            if (isEscaped) {
                isEscaped = false;
                continue;
            }
            if (ch == '\\') {
                isEscaped = true;
                continue;
            }
            if (ch == '"') {
                isInString = false;
            }
            continue;
        }

        if (ch == '"') {
            isInString = true;
            continue;
        }
        if (ch == '{' || ch == '[') {
            ++depth;
            continue;
        }
        if (ch == '}' || ch == ']') {
            --depth;
            continue;
        }
        if (ch == ',' && depth == 0) {
            items.push_back(trim(array.substr(start, i - start)));
            start = skipWhitespaces(array, i + 1);
        }
    }

    if (start < end) {
        items.push_back(trim(array.substr(start, end - start)));
    }

    return items;
}

auto JsonMigratorBase::parseJsonStringLiteral(std::string_view literal) -> std::optional<std::string> {
    auto view = trim(literal);
    if (view.size() < 2 || view.front() != '"' || view.back() != '"') {
        return std::nullopt;
    }

    size_t pos = 0;
    return nbn::core::serialization::json::parseString(view, pos);
}

auto JsonMigratorBase::getTopLevelStringField(const std::unordered_map<std::string, std::string>& rootProperties,
                                              std::string_view key) -> std::optional<std::string> {
    const auto it = rootProperties.find(std::string(key));
    if (it == rootProperties.end()) {
        return std::nullopt;
    }
    return parseJsonStringLiteral(it->second);
}

// ---------------------------------------------------------------------------
// JSON tree walkers
// ---------------------------------------------------------------------------

auto JsonMigratorBase::normalizeJsonArray(std::string_view arrayJson, int stepIndex) -> std::string {
    const auto items = splitArrayItems(arrayJson);
    std::string json{"["};
    bool isFirst = true;
    for (const auto item : items) {
        if (!isFirst) {
            json += ',';
        }
        isFirst = false;
        json += normalizeJsonValue(item, stepIndex);
    }
    json += ']';
    return json;
}

auto JsonMigratorBase::normalizeJsonValue(std::string_view jsonValue, int stepIndex) -> std::string {
    const auto value = trim(jsonValue);
    if (value.empty()) {
        return {};
    }

    if (value.front() == '{') {
        const auto closePos = findMatchingBraces(value, 0);
        if (closePos == value.size() - 1) {
            return normalizeJsonObject(value, stepIndex);
        }
    }

    if (value.front() == '[') {
        const auto closePos = findMatchingBrackets(value, 0);
        if (closePos == value.size() - 1) {
            return normalizeJsonArray(value, stepIndex);
        }
    }

    return std::string(value);
}

// ---------------------------------------------------------------------------
// Public migration entry points
// ---------------------------------------------------------------------------

auto JsonMigratorBase::migrate_to_current(std::string_view jsonPayload) -> std::string {
    const auto wasBeautified = wasJsonBeautified(jsonPayload);
    auto migrated = nbn::core::serialization::json::deBeautifyJson(jsonPayload);

    std::unordered_map<std::string, std::string> root{};
    size_t pos = 0;
    nbn::core::serialization::json::parseObject(root, migrated, pos);

    const auto apiVersion = getTopLevelStringField(root, "nbnApiVersion");
    const auto version = apiVersion.has_value() ? apiVersion : getTopLevelStringField(root, "version");

    if (version.has_value() && nbn::core::utils::semver::isGreaterOrEqual(*version, nbn::config::apiVersion)) {
        nbn::log::info(
            std::format("No migration needed. Current version: {}, file version: {}", nbn::config::apiVersion, *version));
        if (wasBeautified) {
            // Preserve the caller's whitespace style when no migration is needed.
            migrated = std::string{jsonPayload};
        }
    } else {
        const auto resolvedVersion = version.value_or("0.0.1");
        std::cout << std::format("WARNING: Migrating json from {} to {}. This may take a while...", resolvedVersion,
                                 nbn::config::apiVersion)
                  << std::endl;
        migrated = runMigrationSteps(migrated, resolvedVersion, wasBeautified);
        signalMigrated()->emit(resolvedVersion, std::string{nbn::config::apiVersion});
    }

    return migrated;
}

auto JsonMigratorBase::load_and_migrate_from_file(const std::filesystem::path& path) -> std::string {
    std::ifstream stream(path);
    if (!stream.is_open()) {
        throw std::runtime_error(std::format("failed to open {} for reading", path.string()));
    }

    const std::string payload{std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
    return migrate_to_current(payload);
}

}  // namespace nbn::core::utils::nbn_api_json_loader
