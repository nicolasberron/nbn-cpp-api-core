#include <nbn/core/UnitTests.h>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace nbn::core;

enum class UnserializedTestValue { First, Second };

enum class SerializedTestValue { First, Second };

template <>
struct nbn::core::enum_serialization<SerializedTestValue> {
    static auto toString(SerializedTestValue value) -> std::string_view {
        if (value == SerializedTestValue::First) {
            return "first";
        }
        return "second";
    }

    static auto fromString(std::string_view value) -> SerializedTestValue {
        if (value == "first") {
            return SerializedTestValue::First;
        }
        if (value == "second") {
            return SerializedTestValue::Second;
        }
        throw std::runtime_error{"Unknown serialized test value"};
    }
};

// Define a test suite for our testing framework
void test_testing_framework() {
    constexpr int one = 1;
    constexpr int two = 2;
    constexpr int three = 3;
    constexpr int four = 4;
    constexpr int six = 6;
    constexpr int seven = 7;
    constexpr int ten = 10;

    // Test the unit_tests::equal macro
    unit_tests::equal("1 + 2 should equal 3", three, one + two);
    unit_tests::equal("2 * 3 should equal 6", six, two * three);

    // Test the unit_tests::notEqual macro
    unit_tests::notEqual("1 + 2 should not equal 4", four, one + two);
    unit_tests::notEqual("2 * 3 should not equal 7", seven, two * three);

    // Test the unit_tests::isTrue macro
    unit_tests::isTrue("1 + 2 should be greater than 2", one + two > two);
    unit_tests::isTrue("2 * 3 should be less than 10", two * three < ten);

    // Test the unit_tests::isFalse macro
    unit_tests::isFalse("1 + 2 should not be greater than 3", one + two > three);
    unit_tests::isFalse("2 * 3 should not be greater than 7", two * three > seven);

    // Test the unit_tests::isThrowing macro
    unit_tests::isThrowing<std::runtime_error>("Throws an exception", []() { throw std::runtime_error("test"); });
}

void test_testing_framework_formatting_and_failures() {
    constexpr int one = 1;
    constexpr int two = 2;
    constexpr char characterValue = 'a';
    constexpr char differentCharacterValue = 'b';
    constexpr bool booleanValue = true;
    constexpr bool differentBooleanValue = false;
    const auto values = std::vector<int>{one, two};
    const auto expectedText = std::string{"expected"};
    const auto actualText = std::string{"actual"};

    const auto checkSerializableVector = []<typename T>(const T& value) {
        const auto formatted = unit_tests::toTestString(std::vector<T>{value, value});
        unit_tests::isTrue("serializable vector values should be formatted", !formatted.empty());
    };

    const std::vector<std::function<void()>> serializableTypeChecks{
        [checkSerializableVector, characterValue]() { checkSerializableVector(characterValue); },
        [checkSerializableVector]() { checkSerializableVector(static_cast<unsigned char>(one)); },
        [checkSerializableVector]() { checkSerializableVector(static_cast<short>(one)); },
        [checkSerializableVector]() { checkSerializableVector(static_cast<unsigned short>(one)); },
        [checkSerializableVector, one]() { checkSerializableVector(one); },
        [checkSerializableVector]() { checkSerializableVector(static_cast<unsigned int>(one)); },
        [checkSerializableVector]() { checkSerializableVector(static_cast<long>(one)); },
        [checkSerializableVector]() { checkSerializableVector(static_cast<unsigned long>(one)); },
        [checkSerializableVector]() { checkSerializableVector(static_cast<long long>(one)); },
        [checkSerializableVector]() { checkSerializableVector(static_cast<unsigned long long>(one)); },
        [checkSerializableVector]() { checkSerializableVector(static_cast<float>(one)); },
        [checkSerializableVector]() { checkSerializableVector(static_cast<double>(one)); },
        [checkSerializableVector]() { checkSerializableVector(static_cast<long double>(one)); },
        [checkSerializableVector, booleanValue]() { checkSerializableVector(booleanValue); },
        [checkSerializableVector]() { checkSerializableVector(std::string{"value"}); },
        [checkSerializableVector, one]() { checkSerializableVector(std::chrono::nanoseconds{one}); },
        [checkSerializableVector]() { checkSerializableVector(SerializedTestValue::First); },
        [checkSerializableVector]() { checkSerializableVector(UnserializedTestValue::First); },
    };

    for (const auto& check : serializableTypeChecks) {
        check();
    }

    const auto checkEqualFailure = []<typename T>(const T& expected, const T& actual) {
        unit_tests::isThrowing<std::runtime_error>("equal reports failures for every supported type",
                                                   [&]() { unit_tests::equal("equal type failure", expected, actual); });
    };

    const std::vector<std::function<void()>> equalFailureChecks{
        [checkEqualFailure, characterValue, differentCharacterValue]() {
            checkEqualFailure(characterValue, differentCharacterValue);
        },
        [checkEqualFailure]() { checkEqualFailure(static_cast<unsigned char>(one), static_cast<unsigned char>(two)); },
        [checkEqualFailure]() { checkEqualFailure(static_cast<short>(one), static_cast<short>(two)); },
        [checkEqualFailure]() { checkEqualFailure(static_cast<unsigned short>(one), static_cast<unsigned short>(two)); },
        [checkEqualFailure, one, two]() { checkEqualFailure(one, two); },
        [checkEqualFailure]() { checkEqualFailure(static_cast<unsigned int>(one), static_cast<unsigned int>(two)); },
        [checkEqualFailure]() { checkEqualFailure(static_cast<long>(one), static_cast<long>(two)); },
        [checkEqualFailure]() { checkEqualFailure(static_cast<unsigned long>(one), static_cast<unsigned long>(two)); },
        [checkEqualFailure]() { checkEqualFailure(static_cast<long long>(one), static_cast<long long>(two)); },
        [checkEqualFailure]() { checkEqualFailure(static_cast<unsigned long long>(one), static_cast<unsigned long long>(two)); },
        [checkEqualFailure]() { checkEqualFailure(static_cast<float>(one), static_cast<float>(two)); },
        [checkEqualFailure]() { checkEqualFailure(static_cast<double>(one), static_cast<double>(two)); },
        [checkEqualFailure]() { checkEqualFailure(static_cast<long double>(one), static_cast<long double>(two)); },
        [checkEqualFailure, booleanValue, differentBooleanValue]() { checkEqualFailure(booleanValue, differentBooleanValue); },
        [checkEqualFailure, expectedText, actualText]() { checkEqualFailure(expectedText, actualText); },
        [checkEqualFailure, one, two]() { checkEqualFailure(std::chrono::nanoseconds{one}, std::chrono::nanoseconds{two}); },
        [checkEqualFailure]() { checkEqualFailure(SerializedTestValue::First, SerializedTestValue::Second); },
        [checkEqualFailure]() { checkEqualFailure(UnserializedTestValue::First, UnserializedTestValue::Second); },
    };

    for (const auto& check : equalFailureChecks) {
        check();
    }

    const auto actualView = std::string_view{"value2"};
    unit_tests::isThrowing<std::runtime_error>("equal reports differing string literal and view",
                                               [&]() { unit_tests::equal("string literal failure", "value1", actualView); });
    const auto actualString = std::string{"value2"};
    unit_tests::isThrowing<std::runtime_error>("equal reports differing string literal and string", [&]() {
        unit_tests::equal("string literal and string failure", "value1", actualString);
    });
    const auto expectedView = std::string_view{"expected"};
    const auto differentView = std::string_view{"actual"};
    unit_tests::isThrowing<std::runtime_error>("equal reports differing string views",
                                               [&]() { unit_tests::equal("string view failure", expectedView, differentView); });
    const auto actualShortView = std::string_view{"value5678"};
    unit_tests::isThrowing<std::runtime_error>("equal reports differing ten-character array and view",
                                               [&]() { unit_tests::equal("short array failure", "value1234", actualShortView); });
    const auto actualLongView = std::string_view{"value567890"};
    unit_tests::isThrowing<std::runtime_error>("equal reports differing twelve-character array and view",
                                               [&]() { unit_tests::equal("long array failure", "value123456", actualLongView); });
    unit_tests::isThrowing<std::runtime_error>("equal reports differing integer types", [&]() {
        unit_tests::equal("mixed integer failure", static_cast<unsigned int>(one), static_cast<unsigned long>(two));
    });
    const auto differentValues = std::vector<int>{two, one};
    unit_tests::isThrowing<std::runtime_error>("equal reports differing vectors",
                                               [&]() { unit_tests::equal("vector failure", values, differentValues); });
    unit_tests::isThrowing<std::runtime_error>("equal reports differing task run modes", []() {
        unit_tests::equal("run mode failure", Task::RunMode::SingleShot, Task::RunMode::Loop);
    });
    unit_tests::equal("nullptr values should compare equal", nullptr, nullptr);

    unit_tests::equal("vectors should be formatted", std::string{"[1, 2]"}, unit_tests::toTestString(values));
    unit_tests::equal("serialized enums should be formatted", std::string{"first"},
                      unit_tests::toTestString(SerializedTestValue::First));
    unit_tests::equal("unserialized enums should be formatted", std::string{"0"},
                      unit_tests::toTestString(UnserializedTestValue::First));

    const auto expectedSize = values.size();
    const auto actualSize = expectedSize + values.size();
    unit_tests::isThrowing<std::runtime_error>("size_t equal reports failures",
                                               [&]() { unit_tests::equal("size_t equal failure", expectedSize, actualSize); });
    unit_tests::isThrowing<std::runtime_error>("equal reports failures", [&]() { unit_tests::equal("equal failure", one, two); });
    unit_tests::isThrowing<std::runtime_error>("equal formats strings on failure", []() {
        unit_tests::equal("string equal failure", std::string{"expected"}, std::string{"actual"});
    });
    const auto equalText = std::string{"same"};
    unit_tests::isThrowing<std::runtime_error>("notEqual reports failures",
                                               [&]() { unit_tests::notEqual("notEqual failure", one, one); });
    unit_tests::isThrowing<std::runtime_error>("notEqual formats strings on failure",
                                               [&]() { unit_tests::notEqual("string notEqual failure", equalText, equalText); });
    unit_tests::isThrowing<std::runtime_error>("isTrue reports failures", []() { unit_tests::isTrue("isTrue failure", false); });
    unit_tests::isThrowing<std::runtime_error>("isFalse reports failures", []() { unit_tests::isFalse("isFalse failure", true); });
    unit_tests::isThrowing<std::runtime_error>("isThrowing reports missing exceptions",
                                               []() { unit_tests::isThrowing<int>("missing exception", []() {}); });
    unit_tests::isThrowing<std::runtime_error>("isThrowing reports different exceptions", []() {
        unit_tests::isThrowing<int>("different exception", []() { throw std::runtime_error("wrong type"); });
    });
}
