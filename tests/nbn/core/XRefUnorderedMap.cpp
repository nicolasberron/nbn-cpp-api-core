#include <stdexcept>
#include <string>
#include <string_view>

#include <nbn/core/UnitTests.h>
#include <nbn/core/XRefUnorderedMap.h>
#include <nbn/ui/html/dialogs/FileDialog.h>
#include <nbn/ui/html/dialogs/FileOpenDialog.h>
#include <nbn/ui/html/dialogs/MessageBox.h>

namespace {

constexpr int kFirstKey = 7;
constexpr int kUpdatedKey = 8;
constexpr int kMissingKey = 99;
constexpr auto kFirstValue = "worker";
constexpr auto kSecondValue = "manager";
constexpr auto kMissingValue = "missing";
constexpr auto kFileBrowseModeName = "client";
constexpr auto kMessageBoxButtonsName = "ok-cancel";
constexpr auto kFileOpenDialogModeName = "folders";

}  // namespace

void test_xref_unordered_map_supports_bidirectional_lookup() {
    XRefUnorderedMap<int, std::string> map{};

    nbn::core::unit_tests::isTrue("Map should be empty initially", map.empty());
    map.insert(kFirstKey, kFirstValue);
    map.insert(kUpdatedKey, kSecondValue);

    nbn::core::unit_tests::isFalse("Map should not be empty after insertion", map.empty());
    nbn::core::unit_tests::isTrue("Map should contain an inserted key", map.containsKey(kFirstKey));
    nbn::core::unit_tests::isFalse("Map should not contain a missing key", map.containsKey(kMissingKey));
    nbn::core::unit_tests::isTrue("Map should contain an inserted value", map.containsValue(kFirstValue));
    nbn::core::unit_tests::isFalse("Map should not contain a missing value", map.containsValue(kMissingValue));
    nbn::core::unit_tests::equal("Map should find a value by key", std::string{kFirstValue}, map.getValue(kFirstKey));
    nbn::core::unit_tests::equal("Map should find a key by value", kUpdatedKey, map.getKey(kSecondValue));
}

void test_xref_unordered_map_reports_missing_entries() {
    XRefUnorderedMap<int, std::string> map{};
    map.insert(kFirstKey, kFirstValue);

    auto missingKeyThrows = false;
    try {
        static_cast<void>(map.getValue(kMissingKey));
    } catch (const std::out_of_range&) {
        missingKeyThrows = true;
    }

    auto missingValueThrows = false;
    try {
        static_cast<void>(map.getKey(kMissingValue));
    } catch (const std::out_of_range&) {
        missingValueThrows = true;
    }

    nbn::core::unit_tests::isTrue("Map should reject a missing key lookup", missingKeyThrows);
    nbn::core::unit_tests::isTrue("Map should reject a missing value lookup", missingValueThrows);
}

void test_xref_unordered_map_covers_enum_serialization_lookups() {
    using FileBrowseMode = nbn::ui::html::dialogs::FileBrowseMode;
    using MessageBoxButtons = nbn::ui::html::dialogs::MessageBoxButtons;
    using FileOpenDialogMode = nbn::ui::html::dialogs::FileOpenDialogMode;

    nbn::core::unit_tests::equal("File browse mode should deserialize through the reverse map", FileBrowseMode::Client,
                                 nbn::core::enum_serialization<FileBrowseMode>::fromString(kFileBrowseModeName));
    nbn::core::unit_tests::equal("Message box buttons should deserialize through the reverse map", MessageBoxButtons::OkCancel,
                                 nbn::core::enum_serialization<MessageBoxButtons>::fromString(kMessageBoxButtonsName));
    nbn::core::unit_tests::equal("File open mode should deserialize through the reverse map", FileOpenDialogMode::Folders,
                                 nbn::core::enum_serialization<FileOpenDialogMode>::fromString(kFileOpenDialogModeName));
}
