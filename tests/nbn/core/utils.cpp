#include <atomic>
#include <chrono>
#include <stdexcept>
#include <string>

#include <nbn/core/UnitTests.h>
#include <nbn/core/utils.h>
#include <nbn/core/utils/semver.h>

void test_core_utils_string_conversions_and_abi_helpers() {
    using namespace nbn::core::utils;

    nbn::core::unit_tests::equal("toLower should lowercase text", std::string{"mixed text"}, string::toLower("MiXeD TeXt"));
    nbn::core::unit_tests::equal("toUpper should uppercase text", std::string{"MIXED TEXT"}, string::toUpper("MiXeD TeXt"));
    nbn::core::unit_tests::equal("Boolean true should serialize as JSON true", std::string{"true"}, string::toString(true));
    nbn::core::unit_tests::equal("Boolean false should serialize as JSON false", std::string{"false"}, string::toString(false));
    constexpr auto kOneTwoThreeFour{1234};
    nbn::core::unit_tests::equal("Nanoseconds should serialize as a count", std::string{"1234"},
                                 string::toString(std::chrono::nanoseconds{kOneTwoThreeFour}));
    constexpr std::nullptr_t kNullValue{nullptr};
    nbn::core::unit_tests::equal("Null should serialize as JSON null", std::string{"null"}, string::toString(kNullValue));
    nbn::core::unit_tests::equal("Character conversion should preserve the character", 'x', string::fromString<char>("x"));
    nbn::core::unit_tests::equal("Nanosecond conversion should round-trip", std::chrono::nanoseconds{kOneTwoThreeFour},
                                 string::fromString<std::chrono::nanoseconds>("1234"));
    constexpr auto kDoubleValue{2.5};
    nbn::core::unit_tests::equal("Double conversion should parse decimal values", kDoubleValue,
                                 string::fromString<double>("2.5"));
    constexpr long double kLongDoubleValue{3.5L};
    nbn::core::unit_tests::equal("Long double conversion should parse decimal values", kLongDoubleValue,
                                 string::fromString<long double>("3.5"));
    nbn::core::unit_tests::equal("Null pointer conversion should parse JSON null", nullptr,
                                 string::fromString<std::nullptr_t>("null"));

    constexpr auto kSeven{7};
    std::atomic<int> atomicValue{kSeven};
    nbn::core::unit_tests::equal("Atomic conversion should read the value", std::string{"7"}, string::toString(atomicValue));
    nbn::core::unit_tests::isThrowing<std::runtime_error>("Invalid booleans should report a conversion error",
                                                          []() { string::fromString<bool>("not-a-bool"); });
    nbn::core::unit_tests::isTrue("Function name helper should strip qualification",
                                  abi::getFunctionName("void nbn::core::Widget::run(int)") == "run");
    nbn::core::unit_tests::equal("Function name helper should strip an unqualified return type", std::string{"run"},
                                 abi::getFunctionName("void run(int)"));
    const std::string typeNameValue{"value"};
    nbn::core::unit_tests::isTrue("Object type-name helper should return a type name", !abi::getTypeName(typeNameValue).empty());
    const std::string invalidMangledName{"not-a-mangled-name"};
    nbn::core::unit_tests::equal("Invalid ABI names should be returned unchanged", invalidMangledName,
                                 abi::demangle(invalidMangledName));
    nbn::core::unit_tests::isTrue("Qualified type name should preserve const qualification",
                                  abi::qualifiedTypeName<const int>().find("const ") == 0U);
    nbn::core::unit_tests::isTrue("Qualified type name should preserve lvalue references",
                                  abi::qualifiedTypeName<int&>().ends_with('&'));
    nbn::core::unit_tests::isTrue("Qualified type name should preserve rvalue references",
                                  abi::qualifiedTypeName<int&&>().ends_with("&&"));
    constexpr auto kThirtySixU{36U};
    nbn::core::unit_tests::isTrue("UUID helper should produce a UUID-shaped value", uuid::generate().size() == kThirtySixU);
}

void test_core_utils_semver_valid_versions_and_comparisons() {
    using nbn::core::utils::semver::compare;
    using nbn::core::utils::semver::isEqual;
    using nbn::core::utils::semver::isGreater;
    using nbn::core::utils::semver::isGreaterOrEqual;
    using nbn::core::utils::semver::isLess;
    using nbn::core::utils::semver::isLessOrEqual;
    using nbn::core::utils::semver::isValid;

    constexpr int kLess = -1;
    constexpr int kEqual = 0;
    constexpr int kGreater = 1;
    nbn::core::unit_tests::isTrue("Semantic versions with multiple components should be valid", isValid("1.2.10"));
    nbn::core::unit_tests::isTrue("Lower semantic version should compare below the higher version",
                                  compare("1.2.0", "1.2.1") == kLess);
    nbn::core::unit_tests::isTrue("Equal semantic versions should compare equally", compare("1.2", "1.2.0") == kEqual);
    nbn::core::unit_tests::isTrue("Trailing zero components should compare equally in either operand order",
                                  compare("1.2.0", "1.2") == kEqual);
    nbn::core::unit_tests::isTrue("Higher semantic version should compare above the lower version",
                                  compare("2.0", "1.9.9") == kGreater);
    nbn::core::unit_tests::isTrue("isLess should report lower versions", isLess("1.0", "2.0"));
    nbn::core::unit_tests::isTrue("isLessOrEqual should report equal versions", isLessOrEqual("1.0", "1.0"));
    nbn::core::unit_tests::isTrue("isEqual should report equal versions", isEqual("1.0", "1.0.0"));
    nbn::core::unit_tests::isTrue("isGreater should report higher versions", isGreater("2.0", "1.0"));
    nbn::core::unit_tests::isTrue("isGreaterOrEqual should report equal versions", isGreaterOrEqual("1.0", "1.0"));
}

void test_core_utils_semver_rejects_invalid_versions() {
    using nbn::core::utils::semver::compare;
    using nbn::core::utils::semver::isValid;

    constexpr auto kIntMaximumText = "2147483647";
    constexpr auto kIntOverflowText = "2147483648";

    nbn::core::unit_tests::isTrue("Empty semantic versions should be invalid", !isValid(""));
    nbn::core::unit_tests::isTrue("Missing semantic version components should be invalid", !isValid("1..2"));
    nbn::core::unit_tests::isTrue("Non-numeric semantic version components should be invalid", !isValid("1.beta.0"));
    nbn::core::unit_tests::isTrue("Maximum integer components should be valid", isValid(kIntMaximumText));
    nbn::core::unit_tests::isTrue("Overflowing integer components should be invalid", !isValid(kIntOverflowText));
    nbn::core::unit_tests::isFalse("Invalid left-hand versions should not compare", compare("invalid", "1.0").has_value());
    nbn::core::unit_tests::isFalse("Invalid versions should not compare", compare("1.0", "invalid").has_value());
}