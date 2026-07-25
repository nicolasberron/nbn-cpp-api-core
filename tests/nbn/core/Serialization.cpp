#include <nbn/core/UnitTests.h>
#include <nbn/core/detail/serialization.h>
#include <nbn/core/utils.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <limits>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace nbn::core::serialization::value::arithmetic_deserializers::integer {
auto parseUnsignedChar(std::string_view str) -> nbn::core::serializable_variants_t;
auto parseUnsignedShort(std::string_view str) -> nbn::core::serializable_variants_t;
auto parseUnsignedInt(std::string_view str) -> nbn::core::serializable_variants_t;
auto parseUnsignedLong(std::string_view str) -> nbn::core::serializable_variants_t;
auto parseUnsignedLongLong(std::string_view str) -> nbn::core::serializable_variants_t;
auto parseChar(std::string_view str) -> nbn::core::serializable_variants_t;
auto parseShort(std::string_view str) -> nbn::core::serializable_variants_t;
auto parseInt(std::string_view str) -> nbn::core::serializable_variants_t;
auto parseLong(std::string_view str) -> nbn::core::serializable_variants_t;
auto parseLongLong(std::string_view str) -> nbn::core::serializable_variants_t;
}  // namespace nbn::core::serialization::value::arithmetic_deserializers::integer

namespace nbn::core::serialization::json {
auto parseObject(std::string_view json, size_t& pos) -> std::string;
}  // namespace nbn::core::serialization::json

namespace nbn::core::serialization {
auto deBeautifyJson(std::string_view json) -> std::string;
}  // namespace nbn::core::serialization

namespace {

constexpr auto kNanoseconds{17};
constexpr auto kSmallValue{7};
constexpr auto kLargeValue{84};
constexpr auto kLargestValue{168};
constexpr auto kIntegerValue{42};
constexpr auto kUnsignedIntegerValue{84U};
constexpr auto kFloatValue{1.25F};
constexpr auto kDoubleValue{2.5};
constexpr auto kLongDoubleValue{3.75L};
constexpr auto kSecondFloatValue{2.0F};
constexpr auto kSecondDoubleValue{3.0};
constexpr auto kSecondLongDoubleValue{4.0L};

class SerializableTestObject final : public nbn::core::Object {
   public:
    SerializableTestObject() : nbn::core::Object{} {}
};

static auto const registerSerializableTestObject{nbn::core::Object::factoryRegister<SerializableTestObject>()};

template <typename T>
void scalar_round_trip(const T& value) {
    const auto serialized = nbn::core::serialization::serialize(value);
    const auto deserialized = nbn::core::serialization::deserialize<T>(serialized);
    nbn::core::unit_tests::equal("Scalar value should round-trip", value, deserialized);
}

template <typename T>
void vector_round_trip(const std::vector<T>& value) {
    const auto serialized = nbn::core::serialization::serialize(value);
    const auto deserialized = nbn::core::serialization::deserialize<std::vector<T>>(serialized);
    nbn::core::unit_tests::equal("Vector size should round-trip", value.size(), deserialized.size());
    for (size_t index{0}; index < value.size(); ++index) {
        if constexpr (std::is_same_v<T, bool>) {
            nbn::core::unit_tests::equal("Boolean vector item should round-trip", value[index], deserialized[index]);
        } else if constexpr (std::is_same_v<T, nbn::core::serializable_ptr_t> ||
                             std::is_same_v<T, nbn::core::serializable_vector_t>) {
            nbn::core::unit_tests::isTrue(
                "Serializable vector item should round-trip",
                nbn::core::serialization::serialize(value[index]) == nbn::core::serialization::serialize(deserialized[index]));
        } else {
            nbn::core::unit_tests::isTrue("Vector item should round-trip", value[index] == deserialized[index]);
        }
    }
}

template <typename T>
void vector_rejects_invalid_inputs() {
    nbn::core::unit_tests::isThrowing<std::exception>("Array without opening bracket should throw", [] {
        (void)nbn::core::serialization::deserialize<std::vector<T>>("invalid");
    });
    nbn::core::unit_tests::equal("Empty array should deserialize", static_cast<size_t>(0),
                                 nbn::core::serialization::deserialize<std::vector<T>>("[]").size());
    nbn::core::unit_tests::isThrowing<std::exception>("Array with invalid separator should throw", [] {
        if constexpr (std::is_same_v<T, char>) {
            (void)nbn::core::serialization::deserialize<std::vector<T>>(R"(["a";"b"])");
        } else if constexpr (std::is_same_v<T, bool>) {
            (void)nbn::core::serialization::deserialize<std::vector<T>>("[true;false]");
        } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
            (void)nbn::core::serialization::deserialize<std::vector<T>>("[null;null]");
        } else if constexpr (std::is_same_v<T, nbn::core::serializable_ptr_t>) {
            (void)nbn::core::serialization::deserialize<std::vector<T>>("[null;null]");
        } else if constexpr (std::is_same_v<T, nbn::core::serializable_vector_t>) {
            (void)nbn::core::serialization::deserialize<std::vector<T>>("[[1];[2]]");
        } else {
            (void)nbn::core::serialization::deserialize<std::vector<T>>("[1;2]");
        }
    });
}

}  // namespace

void test_serialization_round_trips_all_scalar_types() {
    scalar_round_trip<char>('c');
    scalar_round_trip<unsigned char>(static_cast<unsigned char>(kSmallValue));
    scalar_round_trip<short>(-kSmallValue);
    scalar_round_trip<unsigned short>(static_cast<unsigned short>(kSmallValue));
    scalar_round_trip<int>(kIntegerValue);
    scalar_round_trip<unsigned int>(kUnsignedIntegerValue);
    scalar_round_trip<long>(-kLargeValue);
    scalar_round_trip<unsigned long>(static_cast<unsigned long>(kLargeValue));
    scalar_round_trip<long long>(-kLargestValue);
    scalar_round_trip<unsigned long long>(static_cast<unsigned long long>(kLargestValue));
    scalar_round_trip<float>(kFloatValue);
    scalar_round_trip<double>(kDoubleValue);
    scalar_round_trip<long double>(kLongDoubleValue);
    scalar_round_trip<bool>(true);
    scalar_round_trip<bool>(false);
    scalar_round_trip<std::nullptr_t>(nullptr);
    scalar_round_trip<std::string>("serialized text");
    scalar_round_trip<std::chrono::nanoseconds>(std::chrono::nanoseconds{kNanoseconds});
}

void test_serialization_round_trips_all_vector_types() {
    vector_round_trip<char>({'a', 'b'});
    vector_round_trip<unsigned char>({1U, 2U});
    vector_round_trip<short>({-1, 2});
    vector_round_trip<unsigned short>({1U, 2U});
    vector_round_trip<int>({-1, kIntegerValue});
    vector_round_trip<unsigned int>({1U, kUnsignedIntegerValue});
    vector_round_trip<long>({-1L, 2L});
    vector_round_trip<unsigned long>({1UL, 2UL});
    vector_round_trip<long long>({-1LL, 2LL});
    vector_round_trip<unsigned long long>({1ULL, 2ULL});
    vector_round_trip<float>({kFloatValue, kSecondFloatValue});
    vector_round_trip<double>({kDoubleValue, kSecondDoubleValue});
    vector_round_trip<long double>({kLongDoubleValue, kSecondLongDoubleValue});
    vector_round_trip<bool>({true, false});
    vector_round_trip<std::nullptr_t>({nullptr, nullptr});
    vector_round_trip<std::string>({"one", "two"});
    vector_round_trip<std::chrono::nanoseconds>({std::chrono::nanoseconds{1}, std::chrono::nanoseconds{2}});
    vector_round_trip<nbn::core::serializable_ptr_t>({nullptr, nullptr});
    vector_round_trip<nbn::core::serializable_vector_t>(
        {nbn::core::serializable_vector_t{1, std::string{"nested"}}, nbn::core::serializable_vector_t{2}});

    vector_round_trip<int>({});
    vector_round_trip<std::string>({});

    vector_rejects_invalid_inputs<char>();
    vector_rejects_invalid_inputs<unsigned char>();
    vector_rejects_invalid_inputs<short>();
    vector_rejects_invalid_inputs<unsigned short>();
    vector_rejects_invalid_inputs<int>();
    vector_rejects_invalid_inputs<unsigned int>();
    vector_rejects_invalid_inputs<long>();
    vector_rejects_invalid_inputs<unsigned long>();
    vector_rejects_invalid_inputs<long long>();
    vector_rejects_invalid_inputs<unsigned long long>();
    vector_rejects_invalid_inputs<float>();
    vector_rejects_invalid_inputs<double>();
    vector_rejects_invalid_inputs<long double>();
    vector_rejects_invalid_inputs<bool>();
    vector_rejects_invalid_inputs<std::nullptr_t>();
    vector_rejects_invalid_inputs<std::string>();
    vector_rejects_invalid_inputs<std::chrono::nanoseconds>();
    vector_rejects_invalid_inputs<nbn::core::serializable_ptr_t>();
    vector_rejects_invalid_inputs<nbn::core::serializable_vector_t>();
    nbn::core::unit_tests::isThrowing<std::exception>("Serializable array without opening bracket should throw", [] {
        (void)nbn::core::serialization::deserialize<nbn::core::serializable_vector_t>("invalid");
    });
    nbn::core::unit_tests::equal("Serializable empty array should deserialize", static_cast<size_t>(0),
                                 nbn::core::serialization::deserialize<nbn::core::serializable_vector_t>("[]").size());
    nbn::core::unit_tests::isThrowing<std::exception>("Serializable array with invalid separator should throw", [] {
        (void)nbn::core::serialization::deserialize<nbn::core::serializable_vector_t>("[1;2]");
    });
}

void test_serialization_handles_variants_objects_and_json_formatting() {
    const auto object = std::make_shared<SerializableTestObject>();
    const auto serializedObject = nbn::core::serialization::serialize(object);
    const auto deserializedObject =
        nbn::core::serialization::deserialize<std::shared_ptr<SerializableTestObject>>(serializedObject);
    nbn::core::unit_tests::isTrue("Serialized object should deserialize", deserializedObject != nullptr);
    auto serializedObjectWithMetadata = serializedObject;
    serializedObjectWithMetadata.insert(1, "\"nbnProject\":\"coverage\",");
    nbn::core::unit_tests::isTrue(
        "Serialized object metadata should be ignored",
        nbn::core::serialization::deserialize<nbn::core::serializable_ptr_t>(serializedObjectWithMetadata) != nullptr);
    const auto nullObject = nbn::core::serialization::object::deserialize("null");
    nbn::core::unit_tests::isTrue("Top-level null object should deserialize as null", nullObject == nullptr);
    const auto topLevelString = nbn::core::serialization::value::deserialize<std::string>(R"("top-level")");
    nbn::core::unit_tests::equal("Top-level quoted string should deserialize", std::string{"top-level"}, topLevelString);
    const auto topLevelVariant = nbn::core::serialization::value::deserialize<nbn::core::serializable_variants_t>(R"("variant")");
    nbn::core::unit_tests::isTrue("Quoted variant should deserialize as a string",
                                  std::holds_alternative<std::string>(topLevelVariant));
    const auto registeredClasses = nbn::core::Object::factoryRegisteredClasses();
    nbn::core::unit_tests::isTrue("Registered object should be listed", !registeredClasses.empty());
    nbn::core::unit_tests::equal("Null object should serialize as null", std::string{"null"},
                                 nbn::core::serialization::serialize(nbn::core::serializable_ptr_t{}));

    const std::array<nbn::core::serializable_variants_t, 20> values{'c',
                                                                    static_cast<unsigned char>(1),
                                                                    static_cast<short>(-2),
                                                                    static_cast<unsigned short>(2),
                                                                    kIntegerValue,
                                                                    kUnsignedIntegerValue,
                                                                    static_cast<long>(-3),
                                                                    static_cast<unsigned long>(3),
                                                                    static_cast<long long>(-4),
                                                                    static_cast<unsigned long long>(4),
                                                                    kFloatValue,
                                                                    kDoubleValue,
                                                                    kLongDoubleValue,
                                                                    true,
                                                                    false,
                                                                    nullptr,
                                                                    std::string{"variant"},
                                                                    std::chrono::nanoseconds{kNanoseconds},
                                                                    nbn::core::serializable_ptr_t{},
                                                                    nbn::core::serializable_vector_t{1, std::string{"item"}}};
    for (const auto& value : values) {
        const auto serialized = nbn::core::serialization::value::serialize(value);
        const auto deserialized = nbn::core::serialization::value::deserialize<nbn::core::serializable_variants_t>(serialized);
        nbn::core::unit_tests::equal("Variant should round-trip", serialized,
                                     nbn::core::serialization::value::serialize(deserialized));
    }
    const auto objectVariant = nbn::core::serialization::value::deserialize<nbn::core::serializable_variants_t>(serializedObject);
    nbn::core::unit_tests::isTrue("Object variant should deserialize",
                                  std::holds_alternative<nbn::core::serializable_ptr_t>(objectVariant));

    const auto prettyJson = nbn::core::serialization::json::beautifyJson(std::string{"{\"items\":[1,true],\"text\":\"a,b\"}"});
    nbn::core::unit_tests::isTrue("Beautified JSON should contain indentation",
                                  prettyJson.find("  \"items\"") != std::string::npos);
    nbn::core::unit_tests::equal("Beautified JSON should de-beautify", std::string{"{\"items\":[1,true],\"text\":\"a,b\"}"},
                                 nbn::core::serialization::json::deBeautifyJson(prettyJson));
    const auto escapedFormatted = nbn::core::serialization::json::beautifyJson(std::string{"{\"text\":\"a\\\"b\\\\c\"}"});
    nbn::core::unit_tests::isTrue("Beautifier should preserve escaped string characters",
                                  escapedFormatted.find("\"a\\\"b\\\\c\"") != std::string::npos);
    nbn::core::unit_tests::equal("De-beautifier should preserve escaped string characters",
                                 std::string{"{\"text\":\"a\\\"b\\\\c\"}"},
                                 nbn::core::serialization::json::deBeautifyJson(escapedFormatted));
    nbn::core::unit_tests::equal("Escaped JSON should preserve special characters", std::string{"quote: \\\" slash: \\\\"},
                                 nbn::core::serialization::json::escapeString("quote: \" slash: \\"));
    nbn::core::unit_tests::equal("All JSON control characters should escape", std::string{"\\b\\f\\n\\r\\t\\\\\\\""},
                                 nbn::core::serialization::json::escapeString("\b\f\n\r\t\\\""));
    nbn::core::unit_tests::equal("Vector test strings should include delimiters", std::string{"[1, 2]"},
                                 nbn::core::unit_tests::toTestString(std::vector<int>{1, 2}));
    nbn::core::unit_tests::equal("Scalar test strings should use formatting", std::string{"42"},
                                 nbn::core::unit_tests::toTestString(kIntegerValue));

    const auto escaped = nbn::core::serialization::deserialize<std::string>(R"("\"\\\/\b\f\n\r\t")");
    nbn::core::unit_tests::equal("JSON escape sequences should decode", std::string{"\"\\/\b\f\n\r\t"}, escaped);
    nbn::core::unit_tests::equal("Mixed JSON string segments should decode", std::string{"plain\"escaped"},
                                 nbn::core::serialization::deserialize<std::string>(R"("plain\"escaped")"));
    nbn::core::unit_tests::equal("One-byte unicode should decode", std::string{"A"},
                                 nbn::core::serialization::deserialize<std::string>(R"("\u0041")"));
    nbn::core::unit_tests::equal("Two-byte unicode should decode", std::string{"\xC3\x9F"},
                                 nbn::core::serialization::deserialize<std::string>(R"("\u00DF")"));
    nbn::core::unit_tests::equal("Lowercase unicode hex should decode", std::string{"\xC3\x9F"},
                                 nbn::core::serialization::deserialize<std::string>(R"("\u00df")"));
    nbn::core::unit_tests::equal("Three-byte unicode should decode", std::string{"\xE6\x9D\xB1"},
                                 nbn::core::serialization::deserialize<std::string>(R"("\u6771")"));
    nbn::core::unit_tests::equal("Four-byte unicode should decode", std::string{"\xF0\x9F\x98\x80"},
                                 nbn::core::serialization::deserialize<std::string>(R"("\uD83D\uDE00")"));
    nbn::core::unit_tests::isThrowing<std::exception>(
        "Invalid unicode escape should throw", [] { (void)nbn::core::serialization::deserialize<std::string>(R"("\u00G0")"); });
    nbn::core::unit_tests::isThrowing<std::exception>("Invalid JSON escape should throw",
                                                      [] { (void)nbn::core::serialization::deserialize<std::string>(R"("\q")"); });
    nbn::core::unit_tests::isThrowing<std::exception>(
        "Unexpected low surrogate should throw", [] { (void)nbn::core::serialization::deserialize<std::string>(R"("\uDE00")"); });

    std::unordered_map<std::string, std::string> properties{};
    size_t position{0};
    nbn::core::serialization::json::parseObject(
        properties, R"({"text":"value","number":-1.25e+2,"truth":true,"empty":null,"array":[1,{"nested":false}]})", position);
    nbn::core::unit_tests::equal("Nested JSON object should be retained", std::string{"[1,{\"nested\":false}]"},
                                 properties.at("array"));

    properties.clear();
    position = 0;
    nbn::core::serialization::json::parseObject(
        properties, R"({"escaped":"\"\\\/\b\f\n\r\t","unicode":"\uD83D\uDE00","zero":0,"exponent":1E-2})", position);
    nbn::core::unit_tests::equal("Escaped skipped string should be retained", std::string{"\"\\\"\\\\\\/\\b\\f\\n\\r\\t\""},
                                 properties.at("escaped"));
    properties.clear();
    position = 0;
    nbn::core::serialization::json::parseObject(properties, "{}", position);
    nbn::core::unit_tests::equal("Empty JSON object should have no properties", static_cast<size_t>(0), properties.size());
    position = 0;
    nbn::core::unit_tests::equal("Empty JSON object string should be retained", std::string{"{}"},
                                 nbn::core::serialization::json::parseObject("{}", position));
    position = 0;
    nbn::core::serialization::json::parseObject(properties, R"({"emptyObject":{},"emptyArray":[]})", position);

    position = 0;
    nbn::core::unit_tests::equal("String JSON object parser should retain nested values",
                                 std::string{"{\"array\":[1,{\"nested\":false}]}"},
                                 nbn::core::serialization::json::parseObject(R"({"array":[1,{"nested":false}]})", position));
    position = 0;
    nbn::core::unit_tests::equal("Whitespace JSON should be beautified", std::string{"{\n  \"value\": 1\n}"},
                                 nbn::core::serialization::json::beautifyJson("{\r\n\t\"value\"\t:\t1\n}"));
    nbn::core::unit_tests::equal("Public deBeautifyJson should remove whitespace", std::string{"{\"value\":1}"},
                                 nbn::core::serialization::deBeautifyJson("{ \"value\": 1 }"));
    nbn::core::unit_tests::equal("Beautifier should remove ordinary whitespace", std::string{"{\n  \"value\": 1\n}"},
                                 nbn::core::serialization::json::beautifyJson("{ \"value\" : 1 }"));
    nbn::core::unit_tests::isThrowing<std::exception>("Malformed JSON object should throw", [] {
        std::unordered_map<std::string, std::string> malformed{};
        size_t malformedPosition{0};
        nbn::core::serialization::json::parseObject(malformed, R"({"value":1 "other":2})", malformedPosition);
    });
    nbn::core::unit_tests::isThrowing<std::exception>("Duplicate JSON object keys should throw", [] {
        std::unordered_map<std::string, std::string> malformed{};
        size_t malformedPosition{0};
        nbn::core::serialization::json::parseObject(malformed, R"({"value":1,"value":2})", malformedPosition);
    });
    nbn::core::unit_tests::isThrowing<std::exception>("Object parser should reject a non-string key", [] {
        std::unordered_map<std::string, std::string> malformed{};
        size_t malformedPosition{0};
        nbn::core::serialization::json::parseObject(malformed, "{1:2}", malformedPosition);
    });
    nbn::core::unit_tests::isThrowing<std::exception>("String object parser should reject a non-string key", [] {
        size_t malformedPosition{0};
        (void)nbn::core::serialization::json::parseObject("{1:2}", malformedPosition);
    });
    nbn::core::unit_tests::isThrowing<std::exception>(
        "Malformed JSON array should throw", [] { (void)nbn::core::serialization::deserialize<std::vector<int>>("[1 2]"); });
    nbn::core::unit_tests::isThrowing<std::exception>(
        "Leading-zero JSON number should throw", [] { (void)nbn::core::serialization::deserialize<std::vector<int>>("[01]"); });
    nbn::core::unit_tests::isThrowing<std::exception>("Missing JSON integer digits should throw",
                                                      [] { (void)nbn::core::serialization::deserialize<std::vector<int>>("[-]"); });
    const auto negativeFloatingVariant =
        nbn::core::serialization::value::deserialize<nbn::core::serializable_variants_t>("-1.25e+2");
    nbn::core::unit_tests::isTrue("Negative floating-point variant should deserialize",
                                  std::holds_alternative<float>(negativeFloatingVariant) ||
                                      std::holds_alternative<double>(negativeFloatingVariant) ||
                                      std::holds_alternative<long double>(negativeFloatingVariant));
    nbn::core::unit_tests::isThrowing<std::exception>("Invalid variant JSON should throw", [] {
        (void)nbn::core::serialization::value::deserialize<nbn::core::serializable_variants_t>("@");
    });
    nbn::core::unit_tests::isThrowing<std::exception>("Out-of-range floating-point variants should throw", [] {
        (void)nbn::core::serialization::value::deserialize<nbn::core::serializable_variants_t>(
            "-999999999999999999999999999999999999999999999999999999999999.0");
    });
    nbn::core::unit_tests::isThrowing<std::exception>("Signed integer overflow should throw",
                                                      [] { (void)nbn::core::serialization::deserialize<int>("2147483648"); });
    nbn::core::unit_tests::isThrowing<std::exception>(
        "Unsigned integer overflow should throw", [] { (void)nbn::core::serialization::deserialize<unsigned int>("4294967296"); });
    nbn::core::unit_tests::isThrowing<std::exception>("Trailing scalar integer data should throw",
                                                      [] { (void)nbn::core::serialization::deserialize<int>("1 trailing"); });
    nbn::core::unit_tests::isThrowing<std::exception>("Negative unsigned integer should throw",
                                                      [] { (void)nbn::core::serialization::deserialize<unsigned int>("-1"); });
    nbn::core::unit_tests::isThrowing<std::exception>("Non-finite floating-point data should throw",
                                                      [] { (void)nbn::core::serialization::deserialize<double>("1e309"); });
    nbn::core::unit_tests::isThrowing<std::exception>("Nested duplicate JSON keys should throw", [] {
        (void)nbn::core::serialization::value::deserialize<nbn::core::serializable_variants_t>(R"({"outer":{"x":1,"x":2}})");
    });
    nbn::core::unit_tests::isThrowing<std::exception>("Unescaped JSON control characters should throw", [] {
        (void)nbn::core::serialization::value::deserialize<nbn::core::serializable_variants_t>("\"line\nfeed\"");
    });
    nbn::core::unit_tests::isThrowing<std::exception>("Missing JSON exponent digits should throw", [] {
        (void)nbn::core::serialization::deserialize<std::vector<double>>("[1e]");
    });
    nbn::core::unit_tests::isThrowing<std::exception>("Unexpected JSON number suffix should throw", [] {
        (void)nbn::core::serialization::deserialize<std::vector<int>>("[1x]");
    });
    nbn::core::unit_tests::isThrowing<std::exception>("Truncated JSON constant should throw", [] {
        (void)nbn::core::serialization::value::deserialize<nbn::core::serializable_variants_t>("tru");
    });
    nbn::core::unit_tests::isThrowing<std::exception>("Empty variant should throw", [] {
        (void)nbn::core::serialization::value::deserialize<nbn::core::serializable_variants_t>("");
    });
    nbn::core::unit_tests::isThrowing<std::exception>("Out-of-range numeric variant should throw", [] {
        (void)nbn::core::serialization::value::deserialize<nbn::core::serializable_variants_t>(
            "999999999999999999999999999999999999999999999999999999999999");
    });
    nbn::core::unit_tests::isThrowing<std::exception>("Unterminated skipped key should throw", [] {
        std::unordered_map<std::string, std::string> malformed{};
        size_t malformedPosition{0};
        nbn::core::serialization::json::parseObject(malformed, R"({"unterminated:1})", malformedPosition);
    });
    nbn::core::unit_tests::isThrowing<std::exception>("Unterminated skipped value should throw", [] {
        size_t malformedPosition{0};
        (void)nbn::core::serialization::json::parseObject(R"({"value":"unterminated})", malformedPosition);
    });
    nbn::core::unit_tests::isThrowing<std::exception>("Invalid JSON object key should throw", [] {
        size_t malformedPosition{0};
        (void)nbn::core::serialization::json::parseObject("{1:2}", malformedPosition);
    });
    nbn::core::unit_tests::equal("Multiple JSON object keys should be retained", std::string{"{\"a\":1,\"b\":2}"},
                                 nbn::core::serialization::json::parseObject(R"({"a":1,"b":2})", position));
    nbn::core::unit_tests::isThrowing<std::exception>("Trailing JSON object comma should throw", [] {
        size_t malformedPosition{0};
        (void)nbn::core::serialization::json::parseObject(R"({"a":1,})", malformedPosition);
    });
    nbn::core::unit_tests::isThrowing<std::exception>("Malformed nested object should throw", [] {
        size_t malformedPosition{0};
        (void)nbn::core::serialization::json::parseObject(R"({"a":{"b":1 "c":2}})", malformedPosition);
    });
    nbn::core::unit_tests::isThrowing<std::exception>("Malformed nested array should throw", [] {
        size_t malformedPosition{0};
        (void)nbn::core::serialization::json::parseObject(R"({"a":[1 2]})", malformedPosition);
    });
    nbn::core::unit_tests::isThrowing<std::exception>(
        "Trailing JSON data should throw", [] { (void)nbn::core::serialization::deserialize<std::vector<int>>("[1] trailing"); });
    nbn::core::unit_tests::isThrowing<std::exception>("Trailing scalar JSON data should throw", [] {
        (void)nbn::core::serialization::value::deserialize<nbn::core::serializable_variants_t>("true false");
    });
    nbn::core::unit_tests::isThrowing<std::exception>("Invalid skipped string escape should throw", [] {
        size_t malformedPosition{0};
        (void)nbn::core::serialization::json::parseObject(R"({"a":"\q"})", malformedPosition);
    });
    nbn::core::unit_tests::isThrowing<std::exception>("Unexpected skipped low surrogate should throw", [] {
        size_t malformedPosition{0};
        (void)nbn::core::serialization::json::parseObject(R"({"a":"\uDE00"})", malformedPosition);
    });
    nbn::core::unit_tests::isThrowing<std::exception>("Invalid map JSON key should throw", [] {
        std::unordered_map<std::string, std::string> malformed{};
        size_t malformedPosition{0};
        nbn::core::serialization::json::parseObject(malformed, "{1:2}", malformedPosition);
    });
    nbn::core::unit_tests::isThrowing<std::exception>("Array without opening bracket should throw", [] {
        (void)nbn::core::serialization::array::deserialize<std::vector<int>>("abc");
    });
    nbn::core::unit_tests::equal("Empty numeric array should deserialize", std::vector<int>{},
                                 nbn::core::serialization::array::deserialize<std::vector<int>>("[]"));
    nbn::core::unit_tests::equal("Numeric array should deserialize", std::vector<int>{1, 2},
                                 nbn::core::serialization::array::deserialize<std::vector<int>>("[1,2]"));
    nbn::core::unit_tests::isThrowing<std::exception>("Unexpected numeric array separator should throw", [] {
        (void)nbn::core::serialization::array::deserialize<std::vector<int>>("[1;2]");
    });
    nbn::core::unit_tests::isThrowing<std::exception>("String array without comma should throw", [] {
        (void)nbn::core::serialization::deserialize<std::vector<std::string>>(R"(["a" "b"])");
    });
    nbn::core::unit_tests::isThrowing<std::exception>("String array without closing bracket should throw", [] {
        (void)nbn::core::serialization::deserialize<std::vector<std::string>>(R"(["a",)");
    });
    nbn::core::unit_tests::isThrowing<std::exception>("Unexpected object separator should throw", [] {
        size_t malformedPosition{0};
        (void)nbn::core::serialization::json::parseObject(R"({"a":1 "b":2})", malformedPosition);
    });
    nbn::core::unit_tests::isThrowing<std::exception>("Unexpected serialized object separator should throw", [&] {
        const auto objectName = std::string{SerializableTestObject{}.getClassName()};
        const auto malformed =
            std::string{R"({")"} + objectName + R"(":{"uuid":"coverage-uuid"} ")" + objectName + R"(":{"uuid":"second-uuid"}})";
        (void)nbn::core::serialization::value::deserialize<nbn::core::serializable_ptr_t>(malformed);
    });
    nbn::core::unit_tests::isThrowing<std::exception>("Unterminated JSON object should throw", [] {
        size_t malformedPosition{0};
        (void)nbn::core::serialization::json::parseObject(R"({"a":{"nested":true})", malformedPosition);
    });
}

void test_serialization_exercises_numeric_variant_boundaries() {
    const auto signedValues = std::array<std::string, 4>{nbn::core::utils::string::toString(std::numeric_limits<short>::min()),
                                                         nbn::core::utils::string::toString(std::numeric_limits<int>::min()),
                                                         nbn::core::utils::string::toString(std::numeric_limits<long>::min()),
                                                         nbn::core::utils::string::toString(std::numeric_limits<long long>::min())};
    const auto unsignedValues =
        std::array<std::string, 5>{nbn::core::utils::string::toString(std::numeric_limits<unsigned char>::max()),
                                   nbn::core::utils::string::toString(std::numeric_limits<unsigned short>::max()),
                                   nbn::core::utils::string::toString(std::numeric_limits<unsigned int>::max()),
                                   nbn::core::utils::string::toString(std::numeric_limits<unsigned long>::max()),
                                   nbn::core::utils::string::toString(std::numeric_limits<unsigned long long>::max())};
    // no executable behavior. here
    for (const auto& input : signedValues) {
        const auto value = nbn::core::serialization::value::deserialize<nbn::core::serializable_variants_t>(input);
        nbn::core::unit_tests::equal("Signed numeric boundary should round-trip", std::string{input},
                                     nbn::core::serialization::value::serialize(value));
    }
    for (const auto& input : unsignedValues) {
        const auto value = nbn::core::serialization::value::deserialize<nbn::core::serializable_variants_t>(input);
        nbn::core::unit_tests::equal("Unsigned numeric boundary should round-trip", std::string{input},
                                     nbn::core::serialization::value::serialize(value));
    }
    for (const auto input : {std::string_view{"1.25"}, std::string_view{"2.5e1"}}) {
        const auto value = nbn::core::serialization::value::deserialize<nbn::core::serializable_variants_t>(input);
        nbn::core::unit_tests::isTrue("Floating-point variant should serialize",
                                      !nbn::core::serialization::value::serialize(value).empty());
    }

    constexpr auto kInvalidNumber = "1x";
    (void)nbn::core::serialization::value::arithmetic_deserializers::integer::parseUnsignedChar(kInvalidNumber);
    (void)nbn::core::serialization::value::arithmetic_deserializers::integer::parseUnsignedShort(kInvalidNumber);
    (void)nbn::core::serialization::value::arithmetic_deserializers::integer::parseUnsignedInt(kInvalidNumber);
    (void)nbn::core::serialization::value::arithmetic_deserializers::integer::parseUnsignedLong(kInvalidNumber);
    (void)nbn::core::serialization::value::arithmetic_deserializers::integer::parseUnsignedLongLong(kInvalidNumber);
    (void)nbn::core::serialization::value::arithmetic_deserializers::integer::parseChar("a");
    (void)nbn::core::serialization::value::arithmetic_deserializers::integer::parseShort(kInvalidNumber);
    (void)nbn::core::serialization::value::arithmetic_deserializers::integer::parseInt(kInvalidNumber);
    (void)nbn::core::serialization::value::arithmetic_deserializers::integer::parseLong(kInvalidNumber);
    (void)nbn::core::serialization::value::arithmetic_deserializers::integer::parseLongLong(kInvalidNumber);
    (void)nbn::core::serialization::value::arithmetic_deserializers::integer::parseUnsignedLongLong("1");
    (void)nbn::core::serialization::value::arithmetic_deserializers::integer::parseChar("1");
    (void)nbn::core::serialization::value::arithmetic_deserializers::integer::parseLongLong("1");
}

void test_serialization_enforces_json_resource_limits() {
    constexpr size_t maxInputSize = static_cast<size_t>(16U) * 1024U * 1024U;

    nbn::core::unit_tests::isThrowing<std::exception>("Oversized JSON input should be rejected", [] {
        const std::string oversized(maxInputSize + 1U, ' ');
        size_t position{0};
        nbn::core::serialization::json::parseObject(oversized, position);
    });

    nbn::core::unit_tests::isTrue("Large decoded strings should remain within the configured input limit", [&] {
        const auto payloadSize = maxInputSize / 4U + 1U;
        std::string largeString;
        largeString.reserve(payloadSize + 2U);
        largeString.push_back('"');
        largeString.append(payloadSize, 'a');
        largeString.push_back('"');
        return nbn::core::serialization::deserialize<std::string>(largeString).size() == payloadSize;
    }());

    nbn::core::unit_tests::isThrowing<std::exception>("Excessively nested JSON should be rejected", [] {
        constexpr size_t maxNestingDepth = 256U;
        std::string nested;
        nested.reserve((maxNestingDepth + 1U) * 2U + 1U);
        nested.append(maxNestingDepth + 1U, '[');
        nested.push_back('0');
        nested.append(maxNestingDepth + 1U, ']');
        (void)nbn::core::serialization::deserialize<nbn::core::serializable_variants_t>(nested);
    });
}
