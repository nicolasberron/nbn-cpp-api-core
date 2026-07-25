#include <string>
#include <string_view>

#include <nbn/core/UnitTests.h>
#include <nbn/ui/JsonMigrator.h>

using namespace nbn::core;

namespace {

auto countOccurrences(std::string_view text, std::string_view needle) -> size_t {
    size_t count = 0;
    size_t pos = 0;
    while (true) {
        pos = text.find(needle, pos);
        if (pos == std::string::npos) {
            return count;
        }
        ++count;
        pos += needle.size();
    }
}

}  // namespace

void test_api_json_loader_from_0_0_1_to_0_1_0_adds_missing_icon_fields() {
    const auto legacyJson = std::string{
        R"({"nbnProject":"https://github.com/nicolasberron/nbn-cpp-api","nbnApiVersion":"0.0.1","nbn::ui::html::containers::Frame":{"uuid":"root","children":[{"nbn::ui::html::widgets::Button":{"uuid":"btn","name":"b1","text":{"nbn::ui::detail::Text":{"uuid":"t1","content":"Save"}}}},{"nbn::ui::html::widgets::Label":{"uuid":"lbl","name":"l1","text":{"nbn::ui::detail::Text":{"uuid":"t2","content":"Status"}}}}]}})"};

    const auto migrated = nbn::core::utils::nbn_api_json_loader::from_version_0_0_1_TO_0_1_0(legacyJson);

    unit_tests::equal("Button and Label should receive iconSource fields", countOccurrences(migrated, "\"iconSource\":\"\""),
                      static_cast<size_t>(2));
    unit_tests::isTrue("Migrated payload should contain 0.1.0 API version",
                       migrated.find("\"nbnApiVersion\":\"0.1.0\"") != std::string::npos);
}

void test_api_json_loader_from_0_0_1_to_0_1_0_keeps_existing_icon_source() {
    const auto legacyJson = std::string{
        R"({"nbnApiVersion":"0.0.1","nbn::ui::html::widgets::Button":{"uuid":"btn","iconSource":":/icons/save.png","text":{"nbn::ui::detail::Text":{"uuid":"t","content":"Save"}}}})"};

    const auto migrated = nbn::core::utils::nbn_api_json_loader::from_version_0_0_1_TO_0_1_0(legacyJson);

    unit_tests::equal("Existing iconSource should remain unchanged",
                      countOccurrences(migrated, "\"iconSource\":\":/icons/save.png\""), static_cast<size_t>(1));
}

void test_api_json_loader_migrate_to_current_applies_0_0_1_chain() {
    const auto legacyJson = std::string{
        R"({"nbnApiVersion":"0.0.1","nbn::ui::html::widgets::Label":{"uuid":"lbl","text":{"nbn::ui::detail::Text":{"uuid":"t","content":"Hello"}}}})"};

    const auto migrated = nbn::core::utils::nbn_api_json_loader::migrate_to_current(legacyJson);

    unit_tests::isTrue("Version chain should add missing iconSource", migrated.find("\"iconSource\":\"\"") != std::string::npos);
    unit_tests::isTrue("Version chain should add missing iconSize", migrated.find("\"iconSize\":{") != std::string::npos);
    unit_tests::isTrue("Version chain should bump API version to 0.12.0",
                       migrated.find("\"nbnApiVersion\":\"0.12.0\"") != std::string::npos);
}

void test_api_json_loader_from_0_2_0_to_0_3_0_rewrites_html_sub_namespaces() {
    const auto legacyJson = std::string{
        R"({"nbnApiVersion":"0.2.0","nbn::ui::html::Frame":{"uuid":"root","children":[{"nbn::ui::html::Button":{"uuid":"btn","text":{"nbn::ui::detail::Text":{"uuid":"t1","content":"Save"}}}},{"nbn::ui::html::TextBox":{"uuid":"txt","placeholder":"Name","multiline":false}},{"nbn::ui::html::PropertiesTree":{"uuid":"tree"}}]}})"};

    const auto migrated = nbn::core::utils::nbn_api_json_loader::from_version_0_2_0_TO_0_3_0(legacyJson);

    unit_tests::isTrue("Frame should move under containers namespace",
                       migrated.find("\"nbn::ui::html::containers::Frame\"") != std::string::npos);
    unit_tests::isTrue("Button should move under widgets namespace",
                       migrated.find("\"nbn::ui::html::widgets::Button\"") != std::string::npos);
    unit_tests::isTrue("TextBox should move under viewers namespace",
                       migrated.find("\"nbn::ui::html::viewers::TextBox\"") != std::string::npos);
    unit_tests::isTrue("PropertiesTree should move under viewers namespace",
                       migrated.find("\"nbn::ui::html::viewers::PropertiesTree\"") != std::string::npos);
    unit_tests::isTrue("Migrated payload should contain 0.3.0 API version",
                       migrated.find("\"nbnApiVersion\":\"0.3.0\"") != std::string::npos);
}

void test_api_json_loader_migrate_to_current_applies_0_2_0_chain() {
    const auto legacyJson = std::string{
        R"({"nbnApiVersion":"0.2.0","nbn::ui::html::Button":{"uuid":"btn","text":{"nbn::ui::detail::Text":{"uuid":"t","content":"Save"}}}})"};

    const auto migrated = nbn::core::utils::nbn_api_json_loader::migrate_to_current(legacyJson);

    unit_tests::isTrue("Current migration should rewrite Button into widgets namespace",
                       migrated.find("\"nbn::ui::html::widgets::Button\"") != std::string::npos);
    unit_tests::isTrue("Current migration should bump API version to 0.12.0",
                       migrated.find("\"nbnApiVersion\":\"0.12.0\"") != std::string::npos);
}

void test_api_json_loader_from_0_3_0_to_0_4_0_nests_appearance_colors() {
    const auto legacyJson = std::string{
        R"({"nbnApiVersion":"0.3.0","nbn::ui::html::containers::Frame":{"uuid":"root","appearance":{"nbn::ui::detail::Appearance":{"uuid":"a1","visible":true,"borderWidth":1,"borderRadius":5,"backgroundColor":{"nbn::core::Color":{"uuid":"bg","hex":"#1E1E1EFF"}},"foregroundColor":{"nbn::core::Color":{"uuid":"fg","hex":"#CCCCCCFF"}},"borderColor":{"nbn::core::Color":{"uuid":"bc","hex":"#3C3C3CFF"}}}}}})"};

    const auto migrated = nbn::core::utils::nbn_api_json_loader::from_version_0_3_0_TO_0_4_0(legacyJson);

    unit_tests::isTrue("Colors should be nested under a Colors object",
                       migrated.find("\"colors\":{\"nbn::ui::detail::Appearance::Colors\":") != std::string::npos);
    unit_tests::equal("backgroundColor should appear exactly once (moved, not duplicated)",
                      countOccurrences(migrated, "\"backgroundColor\":"), static_cast<size_t>(1));
    unit_tests::equal("foregroundColor should appear exactly once", countOccurrences(migrated, "\"foregroundColor\":"),
                      static_cast<size_t>(1));
    unit_tests::equal("borderColor should appear exactly once", countOccurrences(migrated, "\"borderColor\":"),
                      static_cast<size_t>(1));
    unit_tests::isTrue(
        "Non-color appearance fields should remain at the Appearance level",
        migrated.find("\"borderRadius\":5") != std::string::npos && migrated.find("\"borderWidth\":1") != std::string::npos);
    unit_tests::isTrue("Migrated payload should contain 0.4.0 API version",
                       migrated.find("\"nbnApiVersion\":\"0.4.0\"") != std::string::npos);
}

void test_api_json_loader_migrate_to_current_applies_0_3_0_chain() {
    const auto legacyJson = std::string{
        R"({"nbnApiVersion":"0.3.0","nbn::ui::html::containers::Frame":{"uuid":"root","appearance":{"nbn::ui::detail::Appearance":{"uuid":"a1","backgroundColor":{"nbn::core::Color":{"uuid":"bg","hex":"#1E1E1EFF"}}}}}})"};

    const auto migrated = nbn::core::utils::nbn_api_json_loader::migrate_to_current(legacyJson);

    unit_tests::isTrue("Current migration should nest appearance colors",
                       migrated.find("\"nbn::ui::detail::Appearance::Colors\":") != std::string::npos);
    unit_tests::isTrue("Current migration should bump API version to 0.12.0",
                       migrated.find("\"nbnApiVersion\":\"0.12.0\"") != std::string::npos);
}

void test_api_json_loader_from_0_4_0_to_0_5_0_nests_log_viewer_colors() {
    const auto legacyJson = std::string{
        R"({"nbnApiVersion":"0.4.0","nbn::ui::html::viewers::LogViewer":{"uuid":"log","debugColor":{"nbn::core::Color":{"uuid":"d","hex":"#808080FF"}},"infoColor":{"nbn::core::Color":{"uuid":"i","hex":"#00FF00FF"}},"warningColor":{"nbn::core::Color":{"uuid":"w","hex":"#FFA500FF"}},"errorColor":{"nbn::core::Color":{"uuid":"e","hex":"#FF0000FF"}},"fatalColor":{"nbn::core::Color":{"uuid":"f","hex":"#FF0000FF"}}}})"};

    const auto migrated = nbn::core::utils::nbn_api_json_loader::from_version_0_4_0_TO_0_5_0(legacyJson);

    unit_tests::isTrue("Log colors should be nested under a LogColors object",
                       migrated.find("\"logColors\":{\"nbn::ui::html::viewers::LogViewer::LogColors\":") != std::string::npos);
    unit_tests::equal("debugColor should appear exactly once (moved, not duplicated)",
                      countOccurrences(migrated, "\"debugColor\":"), static_cast<size_t>(1));
    unit_tests::equal("infoColor should appear exactly once", countOccurrences(migrated, "\"infoColor\":"), static_cast<size_t>(1));
    unit_tests::equal("warningColor should appear exactly once", countOccurrences(migrated, "\"warningColor\":"),
                      static_cast<size_t>(1));
    unit_tests::equal("errorColor should appear exactly once", countOccurrences(migrated, "\"errorColor\":"),
                      static_cast<size_t>(1));
    unit_tests::equal("fatalColor should appear exactly once", countOccurrences(migrated, "\"fatalColor\":"),
                      static_cast<size_t>(1));
    unit_tests::isTrue("Migrated payload should contain 0.5.0 API version",
                       migrated.find("\"nbnApiVersion\":\"0.5.0\"") != std::string::npos);
}

void test_api_json_loader_migrate_to_current_applies_0_4_0_chain() {
    const auto legacyJson = std::string{
        R"({"nbnApiVersion":"0.4.0","nbn::ui::html::viewers::LogViewer":{"uuid":"log","errorColor":{"nbn::core::Color":{"uuid":"e","hex":"#FF0000FF"}}}})"};

    const auto migrated = nbn::core::utils::nbn_api_json_loader::migrate_to_current(legacyJson);

    unit_tests::isTrue("Current migration should nest log viewer colors",
                       migrated.find("\"nbn::ui::html::viewers::LogViewer::LogColors\":") != std::string::npos);
    unit_tests::isTrue("Current migration should bump API version to 0.12.0",
                       migrated.find("\"nbnApiVersion\":\"0.12.0\"") != std::string::npos);
}

void test_api_json_loader_covers_remaining_version_steps() {
    const auto payload = std::string{R"({"nbnApiVersion":"0.5.0","value":"legacy"})"};
    const auto migratedTo06 = nbn::core::utils::nbn_api_json_loader::from_version_0_5_0_TO_0_6_0(payload);
    const auto migratedTo07 = nbn::core::utils::nbn_api_json_loader::from_version_0_6_0_TO_0_7_0(payload);
    const auto migratedTo08 = nbn::core::utils::nbn_api_json_loader::from_version_0_7_0_TO_0_8_0(payload);

    unit_tests::isTrue("0.5 to 0.6 migration should update the API version",
                       migratedTo06.find("\"nbnApiVersion\":\"0.6.0\"") != std::string::npos);
    unit_tests::isTrue("0.6 to 0.7 migration should update the API version",
                       migratedTo07.find("\"nbnApiVersion\":\"0.7.0\"") != std::string::npos);
    unit_tests::isTrue("0.7 to 0.8 migration should update the API version",
                       migratedTo08.find("\"nbnApiVersion\":\"0.8.0\"") != std::string::npos);
}

void test_api_json_loader_migrates_legacy_widget_properties_to_current_schema() {
    const auto legacyJson = std::string{
        R"({"nbnApiVersion":"0.7.0","nbn::ui::html::widgets::Button":{"uuid":"button","dragEnabled":false,"dropEnabled":false}})"};

    const auto migrated = nbn::core::utils::nbn_api_json_loader::migrate_to_current(legacyJson);

    unit_tests::isTrue("Legacy widget migration should add drag group", migrated.find("\"dragGroup\":\"\"") != std::string::npos);
    unit_tests::isTrue("Legacy widget migration should add drop accepts",
                       migrated.find("\"dropAccepts\":\"\"") != std::string::npos);
    unit_tests::isTrue("Legacy widget migration should reach the current API version",
                       migrated.find("\"nbnApiVersion\":\"0.12.0\"") != std::string::npos);
}
