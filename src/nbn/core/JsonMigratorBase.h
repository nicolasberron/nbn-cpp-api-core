#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <nbn/core/Object.h>
#include <nbn/core/detail/serialization.h>

namespace nbn::core::utils::nbn_api_json_loader {

/**
 * @brief Process-wide signal emitted whenever a payload is migrated to a newer API version.
 *
 * The signal carries the originating version and the resulting version, allowing clients to react,
 * for example by logging "json has been migrated from <from> to <to>".
 */
[[nodiscard]] nbn_export_core auto signalMigrated() -> nbn::core::signal_ptr<std::string, std::string>;

/**
 * @brief Base class providing the JSON migration infrastructure.
 *
 * Concrete subclasses override normalizeJsonObject() to supply domain-specific
 * per-object transformations for each version step, and runMigrationSteps() to
 * drive the ordered sequence of version bumps.
 *
 * The stepIndex parameter passed to normalizeJsonObject() is an opaque integer
 * whose interpretation is up to the concrete subclass (typically cast to a
 * domain-specific enum).
 */
class nbn_export_core JsonMigratorBase {
   public:
    virtual ~JsonMigratorBase() = default;

    [[nodiscard]] auto migrate_to_current(std::string_view jsonPayload) -> std::string;
    [[nodiscard]] auto load_and_migrate_from_file(const std::filesystem::path& path) -> std::string;

   protected:
    // JSON utilities available to subclasses

    [[nodiscard]] static auto wasJsonBeautified(std::string_view jsonPayload) -> bool;
    [[nodiscard]] static auto skipWhitespaces(std::string_view text, size_t pos) -> size_t;
    [[nodiscard]] static auto trim(std::string_view text) -> std::string_view;
    [[nodiscard]] static auto findMatchingBraces(std::string_view text, size_t openPos) -> size_t;
    [[nodiscard]] static auto findMatchingBrackets(std::string_view text, size_t openPos) -> size_t;
    [[nodiscard]] static auto serializeObject(const std::unordered_map<std::string, std::string>& properties,
                                              bool isTopLevelMetadataFirst = false) -> std::string;
    [[nodiscard]] static auto splitArrayItems(std::string_view arrayJson) -> std::vector<std::string_view>;
    [[nodiscard]] static auto parseJsonStringLiteral(std::string_view literal) -> std::optional<std::string>;
    [[nodiscard]] static auto getTopLevelStringField(const std::unordered_map<std::string, std::string>& rootProperties,
                                                     std::string_view key) -> std::optional<std::string>;

    // JSON tree walkers — call normalizeJsonObject() for every object node
    [[nodiscard]] auto normalizeJsonArray(std::string_view arrayJson, int stepIndex) -> std::string;
    [[nodiscard]] auto normalizeJsonValue(std::string_view jsonValue, int stepIndex) -> std::string;

    // Override point: apply domain-specific per-object transformations for one step
    [[nodiscard]] virtual auto normalizeJsonObject(std::string_view objectJson, int stepIndex) -> std::string = 0;

    // Override point: run the full ordered sequence of version-bump steps
    [[nodiscard]] virtual auto runMigrationSteps(std::string compactJson, std::string_view resolvedVersion, bool wasBeautified)
        -> std::string = 0;
};

}  // namespace nbn::core::utils::nbn_api_json_loader
