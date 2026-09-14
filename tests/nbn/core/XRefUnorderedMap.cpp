#include <stdexcept>
#include <string>
#include <string_view>

#include <nbn/core/UnitTests.h>
#include <nbn/core/XRefUnorderedMap.h>
#include <nbn/core/detail/serialization.h>

namespace test_types {

enum class AccessLevel { Worker, Manager, Administrator };

}  // namespace test_types

namespace nbn::core {

template <>
struct enum_serialization<test_types::AccessLevel>
    : enum_serialization_xref<enum_serialization<test_types::AccessLevel>, test_types::AccessLevel> {
    using enum_type = test_types::AccessLevel;
    using self = enum_serialization<enum_type>;
    using base = enum_serialization_xref<self, enum_type>;

    enum_serialization()
        : base({{enum_type::Worker, "worker"}, {enum_type::Manager, "manager"}, {enum_type::Administrator, "administrator"}}) {}
};

}  // namespace nbn::core

namespace {

constexpr int kFirstKey = 7;
constexpr int kUpdatedKey = 8;
constexpr int kMissingKey = 99;
constexpr auto kFirstValue = "worker";
constexpr auto kSecondValue = "manager";
constexpr auto kMissingValue = "missing";

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
    using test_types::AccessLevel;

    nbn::core::unit_tests::equal("Enum should deserialize through the reverse map", AccessLevel::Manager,
                                 nbn::core::enum_serialization<AccessLevel>::fromString(kSecondValue));
    nbn::core::unit_tests::equal("Enum should serialize through the forward map", std::string_view{kFirstValue},
                                 nbn::core::enum_serialization<AccessLevel>::toString(AccessLevel::Worker));
}
