#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

#include <nbn/config.h>
#include <nbn/core/JsonMigratorBase.h>
#include <nbn/core/UnitTests.h>

namespace {

constexpr auto kCurrentVersion = nbn::config::apiVersion;
constexpr auto kExpectedStepIndex = 7;
constexpr auto kTemporaryJsonName = "nbn-json-migrator-coverage.json";

class JsonMigratorCoverage final : public nbn::core::utils::nbn_api_json_loader::JsonMigratorBase {
   public:
    using JsonMigratorBase::findMatchingBraces;
    using JsonMigratorBase::findMatchingBrackets;
    using JsonMigratorBase::getTopLevelStringField;
    using JsonMigratorBase::normalizeJsonArray;
    using JsonMigratorBase::normalizeJsonValue;
    using JsonMigratorBase::parseJsonStringLiteral;
    using JsonMigratorBase::serializeObject;
    using JsonMigratorBase::skipWhitespaces;
    using JsonMigratorBase::splitArrayItems;
    using JsonMigratorBase::trim;
    using JsonMigratorBase::wasJsonBeautified;

   protected:
    auto normalizeJsonObject(std::string_view objectJson, int stepIndex) -> std::string override {
        return std::string{"step-"} + std::to_string(stepIndex) + ":" + std::string{objectJson};
    }

    auto runMigrationSteps(std::string compactJson, std::string_view, bool) -> std::string override { return compactJson; }
};

}  // namespace

void test_json_migrator_base_parses_json_helpers_and_tree_values() {
    const auto objectJson = std::string{" {\"text\":\"{not an object}\",\"nested\":{\"value\":1}} "};
    const auto arrayJson = std::string{" [1,{\"text\":\"[,]\"},[true,false]] "};
    const auto escapedObjectJson = std::string{"{\"text\":\"escaped \\\" brace \\u0041\"}"};
    const auto escapedArrayJson = std::string{"[\"escaped \\\\ bracket\",[1]]"};

    nbn::core::unit_tests::isTrue("Beautified JSON should be detected",
                                  JsonMigratorCoverage::wasJsonBeautified("{\n  \"value\": 1\n}"));
    nbn::core::unit_tests::isFalse("Compact JSON should not be considered beautified",
                                   JsonMigratorCoverage::wasJsonBeautified("{\"value\":1}"));
    nbn::core::unit_tests::equal("Whitespace should be skipped", static_cast<size_t>(3),
                                 JsonMigratorCoverage::skipWhitespaces("   value", 0));
    nbn::core::unit_tests::equal("Text should be trimmed", std::string_view{"value"}, JsonMigratorCoverage::trim("  value  "));
    nbn::core::unit_tests::equal("Matching object brace should be found", objectJson.size() - 2U,
                                 JsonMigratorCoverage::findMatchingBraces(objectJson, 1U));
    nbn::core::unit_tests::equal("Matching array bracket should be found", arrayJson.size() - 2U,
                                 JsonMigratorCoverage::findMatchingBrackets(arrayJson, 1U));
    nbn::core::unit_tests::equal("Braces in escaped strings should be ignored", escapedObjectJson.size() - 1U,
                                 JsonMigratorCoverage::findMatchingBraces(escapedObjectJson, 0U));
    nbn::core::unit_tests::equal("Brackets in escaped strings should be ignored", escapedArrayJson.size() - 1U,
                                 JsonMigratorCoverage::findMatchingBrackets(escapedArrayJson, 0U));
    nbn::core::unit_tests::equal("Invalid object position should return npos", std::string::npos,
                                 JsonMigratorCoverage::findMatchingBraces("[]", 0U));
    nbn::core::unit_tests::equal("Invalid array position should return npos", std::string::npos,
                                 JsonMigratorCoverage::findMatchingBrackets("{}", 0U));
    nbn::core::unit_tests::equal("Unclosed object should return npos", std::string::npos,
                                 JsonMigratorCoverage::findMatchingBraces("{\"value\":1", 0U));
    nbn::core::unit_tests::equal("Unclosed array should return npos", std::string::npos,
                                 JsonMigratorCoverage::findMatchingBrackets("[1,2", 0U));

    const auto items = JsonMigratorCoverage::splitArrayItems(arrayJson);
    nbn::core::unit_tests::equal("Nested array values should remain grouped", static_cast<size_t>(3), items.size());
    const auto escapedItems = JsonMigratorCoverage::splitArrayItems(R"(["escaped \"]", {"value": 1}])");
    nbn::core::unit_tests::equal("Escaped strings should remain one array item", static_cast<size_t>(2), escapedItems.size());
    nbn::core::unit_tests::isTrue("Malformed array should produce no items", JsonMigratorCoverage::splitArrayItems("[1,2").empty());
    nbn::core::unit_tests::isTrue("Empty array should produce no items", JsonMigratorCoverage::splitArrayItems("[]").empty());
    nbn::core::unit_tests::isTrue(
        "Nested object should be normalized",
        JsonMigratorCoverage{}.normalizeJsonValue("{\"value\":1}", kExpectedStepIndex).find("step-7:") == 0U);
    nbn::core::unit_tests::isTrue(
        "Nested array should be normalized",
        JsonMigratorCoverage{}.normalizeJsonValue("[1,{\"value\":2}]", kExpectedStepIndex).find("step-7:") != std::string::npos);
    nbn::core::unit_tests::equal("Empty JSON value should normalize to empty", std::string{},
                                 JsonMigratorCoverage{}.normalizeJsonValue("   ", kExpectedStepIndex));
    nbn::core::unit_tests::equal("Malformed JSON value should remain unchanged", std::string{"{invalid"},
                                 JsonMigratorCoverage{}.normalizeJsonValue("{invalid", kExpectedStepIndex));
}

void test_json_migrator_base_serializes_metadata_and_reads_fields() {
    const std::unordered_map<std::string, std::string> properties{
        {"version", "\"legacy\""}, {"nbnApiVersion", "\"0.1.0\""}, {"nbnProject", "\"project\""}, {"name", "\"value\""}};
    const auto serialized = JsonMigratorCoverage::serializeObject(properties, true);
    const auto compactSerialized = JsonMigratorCoverage::serializeObject(properties);
    nbn::core::unit_tests::isTrue("Metadata should be serialized first", serialized.find("{\"nbnProject\"") == 0U);
    nbn::core::unit_tests::isTrue("Regular serialization should be deterministic", compactSerialized.find("{\"name\"") == 0U);
    nbn::core::unit_tests::isTrue("Serialized object should escape keys and preserve values",
                                  serialized.find("\"name\":\"value\"") != std::string::npos);
    const auto apiVersion = JsonMigratorCoverage::getTopLevelStringField(properties, "nbnApiVersion");
    nbn::core::unit_tests::isTrue("Top-level API version should be present", apiVersion.has_value());
    if (apiVersion.has_value()) {
        nbn::core::unit_tests::equal("Top-level string field should be parsed", std::string{"0.1.0"}, *apiVersion);
    }
    nbn::core::unit_tests::isFalse("Missing top-level field should be absent",
                                   JsonMigratorCoverage::getTopLevelStringField(properties, "missing").has_value());
    nbn::core::unit_tests::isFalse("Invalid JSON string literal should be rejected",
                                   JsonMigratorCoverage::parseJsonStringLiteral("legacy").has_value());
}

void test_json_migrator_base_handles_current_payload_and_files() {
    JsonMigratorCoverage migrator{};
    auto migratedFrom = std::string{};
    auto migratedTo = std::string{};
    nbn::core::utils::nbn_api_json_loader::signalMigrated()->connect(
        [&migratedFrom, &migratedTo](std::string from, std::string to) {
            migratedFrom = std::move(from);
            migratedTo = std::move(to);
        });
    const auto currentJson = std::string{"{\"nbnApiVersion\":\""} + std::string{kCurrentVersion} + "\",\"value\":1}";
    nbn::core::unit_tests::equal("Current payload should not be changed", currentJson, migrator.migrate_to_current(currentJson));
    const auto legacyJson = std::string{"{\"version\":\"0.1.0\",\"value\":1}"};
    nbn::core::unit_tests::equal("Legacy payload should pass through migration steps", legacyJson,
                                 migrator.migrate_to_current(legacyJson));
    nbn::core::unit_tests::equal("Migration signal should contain the source version", std::string{"0.1.0"}, migratedFrom);
    nbn::core::unit_tests::equal("Migration signal should contain the current version", std::string{kCurrentVersion}, migratedTo);
    const auto beautifiedJson = std::string{"{\n  \"nbnApiVersion\":\""} + std::string{kCurrentVersion} + "\"\n}";
    nbn::core::unit_tests::equal("Beautified current payload should remain beautified", beautifiedJson,
                                 migrator.migrate_to_current(beautifiedJson));

    const auto path = std::filesystem::temp_directory_path() / kTemporaryJsonName;
    std::ofstream stream(path);
    stream << currentJson;
    stream.close();
    nbn::core::unit_tests::equal("JSON file should be loaded and migrated", currentJson, migrator.load_and_migrate_from_file(path));
    nbn::core::unit_tests::isThrowing<std::runtime_error>("Missing JSON file should throw", [&migrator]() {
        (void)migrator.load_and_migrate_from_file("missing-json-migrator-coverage.json");
    });

    std::error_code error;
    std::filesystem::remove(path, error);
}
