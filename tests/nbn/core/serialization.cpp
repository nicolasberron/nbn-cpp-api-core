#include <nbn/config.h>
#include <nbn/core/UnitTests.h>
#include <nbn/core/utils.h>
#include <nbn/ui/interfaces/Widget.h>

#include <limits>
#include <stdexcept>
#include <unordered_map>

using namespace nbn::core;

namespace nbn::core::serialization {
auto deBeautifyJson(std::string_view json) -> std::string;
namespace json {
auto parseObject(std::string_view json, size_t& pos) -> std::string;
}  // namespace json
namespace value::arithmetic_deserializers::integer {
auto parseUnsignedChar(std::string_view str) -> serializable_variants_t;
auto parseUnsignedShort(std::string_view str) -> serializable_variants_t;
auto parseUnsignedInt(std::string_view str) -> serializable_variants_t;
auto parseUnsignedLong(std::string_view str) -> serializable_variants_t;
auto parseUnsignedLongLong(std::string_view str) -> serializable_variants_t;
auto parseChar(std::string_view str) -> serializable_variants_t;
auto parseShort(std::string_view str) -> serializable_variants_t;
auto parseInt(std::string_view str) -> serializable_variants_t;
auto parseLong(std::string_view str) -> serializable_variants_t;
auto parseLongLong(std::string_view str) -> serializable_variants_t;
}  // namespace value::arithmetic_deserializers::integer
}  // namespace nbn::core::serialization

template <typename T>
auto pi() -> T {
    return std::acos(static_cast<T>(-1.0));
}

template <typename T>
void testSerializeArithmetic(T input) {
    auto serialized{nbn::core::serialization::serialize(input)};
    nbn::log::debug(std::format("Serialized<{}>: {}", utils::abi::getTypeName<T>(), serialized));

    auto deserialized{nbn::core::serialization::deserialize<T>(serialized)};
    unit_tests::equal("Deserialized should be equal to input", input, deserialized);
}

void test_serialize_arithmetics() {
    testSerializeArithmetic<char>(std::numeric_limits<char>::min());
    testSerializeArithmetic<unsigned char>(std::numeric_limits<unsigned char>::max());
    testSerializeArithmetic<short>(std::numeric_limits<short>::min());
    testSerializeArithmetic<unsigned short>(std::numeric_limits<unsigned short>::max());
    testSerializeArithmetic<int>(std::numeric_limits<int>::min());
    testSerializeArithmetic<unsigned int>(std::numeric_limits<unsigned int>::max());
    testSerializeArithmetic<long>(std::numeric_limits<long>::min());
    testSerializeArithmetic<unsigned long>(std::numeric_limits<unsigned long>::max());
    testSerializeArithmetic<long long>(std::numeric_limits<long long>::min());
    testSerializeArithmetic<unsigned long long>(std::numeric_limits<unsigned long long>::max());
    testSerializeArithmetic<float>(pi<float>());
    testSerializeArithmetic<double>(pi<double>());
    testSerializeArithmetic<long double>(pi<long double>());
    testSerializeArithmetic<bool>(true);
    testSerializeArithmetic<std::nullptr_t>(nullptr);
}

void test_serialize_string() {
    std::string input{"Hello, World!"};
    auto serialized{nbn::core::serialization::serialize(input)};
    nbn::log::debug(std::format("Serialized string: {}", serialized));

    auto deserialized{nbn::core::serialization::deserialize<std::string>(serialized)};
    unit_tests::equal("Deserialized should be equal to input", input, deserialized);
}

void test_serialize_escape_characters() {
    // Valid escape characters
    std::string input{R"(\n\t\r\b\f\"\\\/)"};
    auto serialized{nbn::core::serialization::serialize(input)};
    nbn::log::debug(std::format("Serialized string: {}", serialized));

    auto deserialized{nbn::core::serialization::deserialize<std::string>(serialized)};
    unit_tests::equal("Deserialized should be equal to input", input, deserialized);

    // Invalid escape characters
    std::string invalidSerialized{"\"\\a\""};
    nbn::log::debug(std::format("Serialized invalid string: {}", invalidSerialized));

    try {
        deserialized = nbn::core::serialization::deserialize<std::string>(invalidSerialized);
        nbn::log::fatal("Invalid escape characters should throw an exception");
    } catch (const std::exception& e) {
        return;
    }
    nbn::log::fatal("Invalid escape characters should throw an exception");
}

template <typename T>
void testSerializeVector(std::vector<T> input) {
    auto serialized{nbn::core::serialization::serialize(input)};
    nbn::log::debug(std::format("Serialized vector<{}>: {}", utils::abi::getTypeName<T>(), serialized));

    auto deserialized{nbn::core::serialization::deserialize<std::vector<T>>(serialized)};
    for (size_t i{0}; i < input.size(); ++i) {
        if constexpr (std::is_same_v<T, bool>) {
            // std::vector<bool> is a special case: It is actually a bitset (srd::vector<std::_Bit_reference>)
            unit_tests::equal("Deserialized should be equal to input", static_cast<T>(input[i]), static_cast<T>(deserialized[i]));
        } else {
            unit_tests::equal("Deserialized should be equal to input", input[i], deserialized[i]);
        }
    }
}

void test_serialize_vectors() {
    testSerializeVector<char>({std::numeric_limits<char>::min(), std::numeric_limits<char>::max()});
    testSerializeVector<unsigned char>({std::numeric_limits<unsigned char>::min(), std::numeric_limits<unsigned char>::max()});
    testSerializeVector<short>({std::numeric_limits<short>::min(), std::numeric_limits<short>::max()});
    testSerializeVector<unsigned short>({std::numeric_limits<unsigned short>::min(), std::numeric_limits<unsigned short>::max()});
    testSerializeVector<int>({std::numeric_limits<int>::min(), std::numeric_limits<int>::max()});
    testSerializeVector<unsigned int>({std::numeric_limits<unsigned int>::min(), std::numeric_limits<unsigned int>::max()});
    testSerializeVector<long>({std::numeric_limits<long>::min(), std::numeric_limits<long>::max()});
    testSerializeVector<unsigned long>({std::numeric_limits<unsigned long>::min(), std::numeric_limits<unsigned long>::max()});
    testSerializeVector<long long>({std::numeric_limits<long long>::min(), std::numeric_limits<long long>::max()});
    testSerializeVector<unsigned long long>(
        {std::numeric_limits<unsigned long long>::min(), std::numeric_limits<unsigned long long>::max()});
    testSerializeVector<float>({pi<float>(), pi<float>()});
    testSerializeVector<double>({pi<double>(), pi<double>()});
    testSerializeVector<long double>({pi<long double>(), pi<long double>()});
    testSerializeVector<bool>({true, false});
    testSerializeVector<std::nullptr_t>({nullptr, nullptr});
    testSerializeVector<std::string>({"Hello, World!", "Hello, World!"});
    testSerializeVector<std::chrono::nanoseconds>({std::chrono::nanoseconds(1), std::chrono::nanoseconds(2)});
}

void test_serialize_vector_missing_opening_bracket() {
    auto input{"Not a vector"};
    try {
        auto deserialized{nbn::core::serialization::deserialize<std::vector<std::string>>(input)};
        nbn::log::fatal("Missing opening bracket should throw an exception");
    } catch (const std::exception& e) {
        return;
    }
    nbn::log::fatal("Missing opening bracket should throw an exception");
}

void test_serialize_vector_missing_closing_bracket_in_strings_vector() {
    auto input{R"(["Hello",)"};
    try {
        auto deserialized{nbn::core::serialization::deserialize<std::vector<std::string>>(input)};
        nbn::log::fatal("Missing closing bracket should throw an exception");
    } catch (const std::exception& e) {
        return;
    }
    nbn::log::fatal("Missing closing bracket should throw an exception");
}

void test_serialize_vector_missing_comma_in_strings_vector() {
    auto input{R"(["Hello" "World"])"};
    try {
        auto deserialized{nbn::core::serialization::deserialize<std::vector<std::string>>(input)};
        nbn::log::fatal("Missing comma should throw an exception");
    } catch (const std::exception& e) {
        return;
    }
    nbn::log::fatal("Missing comma should throw an exception");
}

void test_serialize_vector_missing_closing_bracket_in_non_strings_vector() {
    auto input{R"([1, 2)"};
    try {
        auto deserialized{nbn::core::serialization::deserialize<std::vector<int>>(input)};
        nbn::log::fatal("Missing closing bracket should throw an exception");
    } catch (const std::exception& e) {
        return;
    }
    nbn::log::fatal("Missing closing bracket should throw an exception");
}

void test_serialize_shared_ptr() {
    auto inputObject{std::make_shared<Object>()};
    auto serializedObject{nbn::core::serialization::serialize(inputObject)};
    nbn::log::debug(std::format("Serialized object: {}", serializedObject));

    auto deserializedObject{nbn::core::serialization::deserialize<std::shared_ptr<Object>>(serializedObject)};
    nbn::log::fatal_if(deserializedObject == nullptr, "Could not deserialize object");
}

void test_nullptr() {
    auto inputObject{nullptr};
    auto serializedObject{nbn::core::serialization::serialize<std::shared_ptr<Object>>(inputObject)};
    nbn::log::debug(std::format("Serialized object: {}", serializedObject));

    auto deserializedObject{nbn::core::serialization::deserialize<std::shared_ptr<Object>>(serializedObject)};
    nbn::log::fatal_if(deserializedObject != nullptr, "Deserialized object should be nullptr");
}

class TestObjectSharedProperties : public Object {
   public:
    TestObjectSharedProperties() : Object() {
        // Initialize properties
        prop_char();
        prop_uchar();
        prop_nullptr();
        prop_true()->set(true);
        prop_false();
        prop_object()->set(std::make_shared<Object>());
        prop_vector_int()->set(std::vector<int>{1, 2, 3});
    }

    auto prop_char() -> property_ptr<char> { return property<char>(); }
    auto prop_uchar() -> property_ptr<unsigned char> { return property<unsigned char>(); }
    auto prop_nullptr() -> property_ptr<std::nullptr_t> { return property<std::nullptr_t>(); }
    auto prop_true() -> property_ptr<bool> { return property<bool>(); }
    auto prop_false() -> property_ptr<bool> { return property<bool>(); }
    auto prop_object() -> property_ptr<std::shared_ptr<Object>> { return property<std::shared_ptr<Object>>(); }
    auto prop_vector_int() -> property_ptr<std::vector<int>> { return property<std::vector<int>>(); }
};

// Factory registration
static auto const registerToFactorySerializeTestObject{[]() { return Object::factoryRegister<TestObjectSharedProperties>(); }()};

void test_object_shared_properties() {
    auto inputObject{std::make_shared<TestObjectSharedProperties>()};
    inputObject->prop_char()->set(std::numeric_limits<char>::min());
    inputObject->prop_uchar()->set(std::numeric_limits<unsigned char>::max());

    auto serializedObject{nbn::core::serialization::serialize(inputObject)};
    nbn::log::debug(std::format("Serialized object: {}", serializedObject));

    auto deserializedObject{nbn::core::serialization::deserialize<std::shared_ptr<TestObjectSharedProperties>>(serializedObject)};
    nbn::log::fatal_if(deserializedObject == nullptr, "Could not deserialize object");

    unit_tests::equal("Deserialized char property should be equal to input", inputObject->prop_char()->get(),
                      deserializedObject->prop_char()->get());
    unit_tests::equal("Deserialized unsigned char property should be equal to input", inputObject->prop_uchar()->get(),
                      deserializedObject->prop_uchar()->get());
}

void test_missing_ending_quote_in_parsed_string() {
    auto input{"\"Hello, World!"};
    try {
        nbn::core::serialization::deserialize<std::string>(input);
        nbn::log::fatal("Missing ending quote should throw an exception");
    } catch (const std::exception& e) {
        return;
    }
    nbn::log::fatal("Missing ending quote should throw an exception");
}

void test_missing_opening_quote_in_object_name() {
    auto input{"{Object\": {\"}"};
    try {
        std::unordered_map<std::string, std::string> propertiesAsString{};
        size_t pos{0};
        nbn::core::serialization::json::parseObject(propertiesAsString, input, pos);
    } catch (const std::exception& e) {
        return;
    }
    nbn::log::fatal("Missing opening quote in object name should throw an exception");
    // returns before this guard.
}

class TestMissingOpeningQuoteInSharedObjectName : public Object {
   public:
    TestMissingOpeningQuoteInSharedObjectName() : Object() {
        // Initialize properties
        prop_object();
    }

    auto prop_object() -> property_ptr<std::shared_ptr<Object>> { return property<std::shared_ptr<Object>>(); }
};

// Factory registration
static auto const registerToFactoryTestMissingOpeningQuoteInSharedObjectName{
    []() { return Object::factoryRegister<TestMissingOpeningQuoteInSharedObjectName>(); }()};

void test_missing_opening_quote_in_shared_object_name() {
    auto fixture = std::make_shared<TestMissingOpeningQuoteInSharedObjectName>();
    (void)fixture->prop_object();
    auto input{
        R"({"TestMissingOpeningQuoteInSharedObjectName":{"uuid":"9bd784fc-904f-47cc-a869-33a9b1ad82e2","prop_object":{nbn::core::Object":{"uuid":"c4369781-d84c-40d1-bba4-7e17bb038f01"}}}})"};
    try {
        nbn::core::serialization::deserialize<std::shared_ptr<TestMissingOpeningQuoteInSharedObjectName>>(input);
    } catch (const std::exception& e) {
        return;
    }
    nbn::log::fatal("Missing opening quote in shared object name should throw an exception");
    // returns before this guard.
}

class TestBadConstantObject : public Object {
   public:
    TestBadConstantObject() : Object() {
        // Initialize properties
        prop_false();
    }

    auto prop_false() -> property_ptr<bool> { return property<bool>(); }
};

// Factory registration
static auto const registerToFactoryTestBadConstantObject{[]() { return Object::factoryRegister<TestBadConstantObject>(); }()};

void test_bad_constant_object() {
    auto fixture = std::make_shared<TestBadConstantObject>();
    (void)fixture->prop_false();
    auto input{R"({"TestBadConstantObject":{"uuid":"9bd784fc-904f-47cc-a869-33a9b1ad82e2","prop_false":fal})"};
    try {
        nbn::core::serialization::deserialize<std::shared_ptr<TestBadConstantObject>>(input);
    } catch (const std::exception& e) {
        return;
    }
    nbn::log::fatal("Bad constant object should throw an exception");
}

class TestBadValueObject : public Object {
   public:
    TestBadValueObject() : Object() {
        // Initialize properties
        prop_false();
    }

    auto prop_false() -> property_ptr<bool> { return property<bool>(); }
};

// Factory registration
static auto const registerToFactoryTestBadValueObject{[]() { return Object::factoryRegister<TestBadValueObject>(); }()};

void test_bad_value_object() {
    auto fixture = std::make_shared<TestBadValueObject>();
    (void)fixture->prop_false();
    auto input{R"({"TestBadValueObject":{"uuid":"9bd784fc-904f-47cc-a869-33a9b1ad82e2","prop_false":bad_value})"};
    try {
        nbn::core::serialization::deserialize<std::shared_ptr<TestBadValueObject>>(input);
    } catch (const std::exception& e) {
        return;
    }
    nbn::log::fatal("Bad value object should throw an exception");
}

template <typename T>
void testArithmeticSerializeVariant() {
    std::string minInputString;
    if constexpr (!std::is_floating_point_v<T>) {
        minInputString = nbn::core::utils::string::toString<T>(std::numeric_limits<T>::min());
    } else {
        minInputString = nbn::core::utils::string::toString<T>(-std::numeric_limits<T>::max());
    }
    auto minResult{nbn::core::serialization::value::deserialize<serializable_variants_t>(minInputString)};
    auto minDeserializeResult{nbn::core::serialization::value::serialize(minResult)};
    nbn::log::fatal_if(minInputString != nbn::core::serialization::value::serialize(minResult),
                       std::format("Deserialized {} should be equal to input {}", minDeserializeResult, minInputString));

    auto maxInputString{nbn::core::utils::string::toString<T>(std::numeric_limits<T>::max())};
    auto maxResult{nbn::core::serialization::value::deserialize<serializable_variants_t>(maxInputString)};
    auto maxDeserializeResult{nbn::core::serialization::value::serialize(maxResult)};
    nbn::log::fatal_if(maxInputString != nbn::core::serialization::value::serialize(maxResult),
                       std::format("Deserialized {} should be equal to input {}", maxDeserializeResult, maxInputString));
}

void test_serializable_variants() {
    // Test string
    std::string inputString{R"("Hello, World!")"};
    auto result{nbn::core::serialization::value::deserialize<serializable_variants_t>(inputString)};
    nbn::log::fatal_if(inputString != nbn::core::serialization::value::serialize(result),
                       "Deserialized string should be equal to input");

    // Test empty string
    inputString = "";
    auto hasThrown{false};
    try {
        result = nbn::core::serialization::value::deserialize<serializable_variants_t>(inputString);
    } catch (const std::exception& e) {
        hasThrown = true;
    }
    nbn::log::fatal_if(!hasThrown, "Empty string should throw an exception");

    // Test invalid character
    inputString = "a";
    hasThrown = false;
    try {
        result = nbn::core::serialization::value::deserialize<serializable_variants_t>(inputString);
    } catch (const std::exception& e) {
        hasThrown = true;
    }
    nbn::log::fatal_if(!hasThrown, "Invalid character should throw an exception");

    // Strict JSON: leading plus is not allowed for numbers
    inputString = "+1";
    hasThrown = false;
    try {
        result = nbn::core::serialization::value::deserialize<serializable_variants_t>(inputString);
    } catch (const std::exception& e) {
        hasThrown = true;
    }
    nbn::log::fatal_if(!hasThrown, "Leading plus in number should throw an exception");

    // Test boolean
    inputString = "true";
    result = nbn::core::serialization::value::deserialize<serializable_variants_t>(inputString);
    nbn::log::fatal_if(inputString != nbn::core::serialization::value::serialize(result),
                       "Deserialized boolean should be equal to input");

    inputString = "false";
    result = nbn::core::serialization::value::deserialize<serializable_variants_t>(inputString);
    nbn::log::fatal_if(inputString != nbn::core::serialization::value::serialize(result),
                       "Deserialized boolean should be equal to input");

    // Test null pointer
    inputString = "null";
    result = nbn::core::serialization::value::deserialize<serializable_variants_t>(inputString);
    nbn::log::fatal_if(inputString != nbn::core::serialization::value::serialize(result),
                       "Deserialized null pointer should be equal to input");

    // Test char
    inputString = R"("a")";
    result = nbn::core::serialization::value::deserialize<serializable_variants_t>(inputString);
    nbn::log::fatal_if(inputString != nbn::core::serialization::value::serialize(result),
                       "Deserialized char should be equal to input");

    // Test unsigned char
    testArithmeticSerializeVariant<unsigned char>();
    testArithmeticSerializeVariant<short>();
    testArithmeticSerializeVariant<unsigned short>();
    testArithmeticSerializeVariant<int>();
    testArithmeticSerializeVariant<unsigned int>();
    testArithmeticSerializeVariant<long>();
    testArithmeticSerializeVariant<unsigned long>();
    testArithmeticSerializeVariant<long long>();
    testArithmeticSerializeVariant<unsigned long long>();
    testArithmeticSerializeVariant<float>();
    testArithmeticSerializeVariant<double>();
    testArithmeticSerializeVariant<long double>();

    // Test floating point value out of range
    inputString = "1.79769313486231579999999999999999999999999999999999999e+308";
    hasThrown = false;
    try {
        result = nbn::core::serialization::value::deserialize<serializable_variants_t>(inputString);
    } catch (const std::exception& e) {
        hasThrown = true;
    }
    nbn::log::fatal_if(!hasThrown, "Floating point value out of range should throw an exception");

    // Test integer out of range
    inputString = "92233720368547758089889898989898989898";
    hasThrown = false;
    try {
        result = nbn::core::serialization::value::deserialize<serializable_variants_t>(inputString);
    } catch (const std::exception& e) {
        hasThrown = true;
    }
    nbn::log::fatal_if(!hasThrown, "Integer out of range should throw an exception");

    // Test vector
    inputString = R"([1,2,3])";
    result = nbn::core::serialization::value::deserialize<serializable_variants_t>(inputString);
    nbn::log::fatal_if(inputString != nbn::core::serialization::value::serialize(result),
                       "Deserialized vector should be equal to input");

    // Strict JSON: trailing commas are not allowed
    inputString = R"([1,2,])";
    hasThrown = false;
    try {
        result = nbn::core::serialization::value::deserialize<serializable_variants_t>(inputString);
    } catch (const std::exception& e) {
        hasThrown = true;
    }
    nbn::log::fatal_if(!hasThrown, "Trailing comma in array should throw an exception");

    inputString = R"({"nbn::core::Object":{"uuid":"9bd784fc-904f-47cc-a869-33a9b1ad82e2",}})";
    hasThrown = false;
    try {
        result = nbn::core::serialization::value::deserialize<serializable_variants_t>(inputString);
    } catch (const std::exception& e) {
        hasThrown = true;
    }
    nbn::log::fatal_if(!hasThrown, "Trailing comma in object should throw an exception");

    // Unicode escape decoding
    inputString = R"("\u0041\u00DF\u6771")";
    result = nbn::core::serialization::value::deserialize<serializable_variants_t>(inputString);
    auto unicodeDecoded{std::get<std::string>(result)};
    unit_tests::equal("Unicode escapes should decode to UTF-8", std::string{"A\xC3\x9F\xE6\x9D\xB1"}, unicodeDecoded);

    // Unicode surrogate pair decoding (U+1F600)
    inputString = R"("\uD83D\uDE00")";
    result = nbn::core::serialization::value::deserialize<serializable_variants_t>(inputString);
    auto surrogateDecoded{std::get<std::string>(result)};
    unit_tests::equal("Unicode surrogate pair should decode to UTF-8", std::string{"\xF0\x9F\x98\x80"}, surrogateDecoded);

    // Test object
    inputString = std::format(
        R"({{"nbnProject":"https://github.com/nicolasberron/nbn-cpp-api","version":"{}","nbn::core::Object":{{"uuid":"9bd784fc-904f-47cc-a869-33a9b1ad82e2"}}}})",
        nbn::config::apiVersion);
    result = nbn::core::serialization::value::deserialize<serializable_variants_t>(inputString);
    const auto serializedObject = nbn::core::serialization::value::serialize(result);
    // Project and API version are envelope metadata consumed by object deserialization and are
    // intentionally not serialized as object properties. The object payload itself must survive.
    unit_tests::isTrue("Deserialized object should preserve object UUID",
                       serializedObject.find("\"uuid\":\"9bd784fc-904f-47cc-a869-33a9b1ad82e2\"") != std::string::npos);
}

void test_serialization_json_control_characters_and_nested_values() {
    const auto controls = std::string{"\b\f\n\r\t\\\"/"};
    unit_tests::equal("JSON control characters should be escaped", std::string{"\\b\\f\\n\\r\\t\\\\\\\"/"},
                      nbn::core::serialization::json::escapeString(controls));
    unit_tests::equal("JSON escape sequences should decode", std::string{"/\\\b\f\n\r\t"},
                      nbn::core::serialization::deserialize<std::string>(R"("\/\\\b\f\n\r\t")"));

    std::unordered_map<std::string, std::string> properties{};
    size_t position{0};
    nbn::core::serialization::json::parseObject(properties, R"({"nested":{"value":1},"items":[true,null]})", position);
    unit_tests::equal("Nested object should be retained by the JSON parser", std::string{"{\"value\":1}"}, properties.at("nested"));
    unit_tests::equal("Nested array should be retained by the JSON parser", std::string{"[true,null]"}, properties.at("items"));

    properties.clear();
    position = 0;
    nbn::core::serialization::json::parseObject(properties, "{}", position);
    unit_tests::isTrue("Empty object should parse", properties.empty());
    unit_tests::equal("deBeautifyJson should remove formatting", std::string{"{\"value\":1}"},
                      nbn::core::serialization::json::deBeautifyJson("{\n  \"value\": 1\n}"));
}

void test_serialization_json_rejects_invalid_unicode_and_numbers() {
    unit_tests::isThrowing<std::exception>("Invalid unicode hex should throw",
                                           [] { (void)nbn::core::serialization::deserialize<std::string>(R"("\u00G0")"); });
    unit_tests::isThrowing<std::exception>("Unpaired low surrogate should throw",
                                           [] { (void)nbn::core::serialization::deserialize<std::string>(R"("\uDE00")"); });
    unit_tests::isThrowing<std::exception>("Unpaired low surrogate in skipped object value should throw", [] {
        constexpr auto kUnpairedLowSurrogateObject = R"({"value":"\uDE00"})";
        std::unordered_map<std::string, std::string> properties{};
        size_t position{0};
        nbn::core::serialization::json::parseObject(properties, kUnpairedLowSurrogateObject, position);
    });
    unit_tests::isThrowing<std::exception>("Missing low surrogate in skipped object value should throw", [] {
        constexpr auto kMissingLowSurrogateObject = R"({"value":"\uD83D"})";
        std::unordered_map<std::string, std::string> properties{};
        size_t position{0};
        nbn::core::serialization::json::parseObject(properties, kMissingLowSurrogateObject, position);
    });
    unit_tests::isThrowing<std::exception>("Invalid escape in skipped object value should throw", [] {
        constexpr auto kInvalidEscapeObject = R"({"value":"\q"})";
        std::unordered_map<std::string, std::string> properties{};
        size_t position{0};
        nbn::core::serialization::json::parseObject(properties, kInvalidEscapeObject, position);
    });
    unit_tests::isThrowing<std::exception>("Missing nested object separator should throw", [] {
        constexpr auto kMissingNestedObjectSeparator = R"({"value":{"first":1 "second":2}})";
        std::unordered_map<std::string, std::string> properties{};
        size_t position{0};
        nbn::core::serialization::json::parseObject(properties, kMissingNestedObjectSeparator, position);
    });
    unit_tests::isThrowing<std::exception>("Missing low surrogate should throw",
                                           [] { (void)nbn::core::serialization::deserialize<std::string>(R"("\uD800")"); });
    unit_tests::isThrowing<std::exception>("Invalid low surrogate should throw",
                                           [] { (void)nbn::core::serialization::deserialize<std::string>(R"("\uD800\u0041")"); });
    unit_tests::isThrowing<std::exception>("Leading zero JSON number should throw",
                                           [] { (void)nbn::core::serialization::deserialize<serializable_variants_t>("01"); });
    unit_tests::isThrowing<std::exception>("Unexpected JSON number suffix should throw",
                                           [] { (void)nbn::core::serialization::deserialize<serializable_variants_t>("1a"); });
}

void test_serialization_enum_errors_and_empty_arrays() {
    constexpr auto kInvalidLayoutValue = static_cast<nbn::ui::interfaces::LayoutType>(-1);
    unit_tests::isThrowing<std::exception>("Unmapped enum values should throw", [&] {
        (void)nbn::core::enum_serialization<nbn::ui::interfaces::LayoutType>::toString(kInvalidLayoutValue);
    });
    unit_tests::isThrowing<std::exception>("Unmapped enum strings should throw", [] {
        (void)nbn::core::serialization::value::deserialize<nbn::ui::interfaces::LayoutType>(R"("missing")");
    });

    const auto emptyStrings = nbn::core::serialization::deserialize<std::vector<std::string>>("[]");
    const auto emptyNumbers = nbn::core::serialization::deserialize<std::vector<int>>("[]");
    unit_tests::isTrue("Empty string arrays should deserialize", emptyStrings.empty());
    unit_tests::isTrue("Empty numeric arrays should deserialize", emptyNumbers.empty());
}

void test_serialization_handles_nested_serializable_vectors_and_structure_errors() {
    constexpr auto kNestedInteger{42};
    nbn::core::serializable_vector_t nestedValues{};
    nestedValues.emplace_back(kNestedInteger);
    nestedValues.emplace_back(std::string{"nested"});

    const auto serialized = nbn::core::serialization::serialize(nestedValues);
    const auto deserialized = nbn::core::serialization::deserialize<nbn::core::serializable_vector_t>(serialized);
    nbn::core::unit_tests::equal("Nested serializable vector should round-trip", serialized,
                                 nbn::core::serialization::serialize(deserialized));

    nbn::core::unit_tests::isThrowing<std::exception>("Missing object comma should throw", [] {
        std::unordered_map<std::string, std::string> properties{};
        size_t position{0};
        nbn::core::serialization::json::parseObject(properties, R"({"first":1 "second":2})", position);
    });
    nbn::core::unit_tests::isThrowing<std::exception>("Missing array comma should throw", [] {
        (void)nbn::core::serialization::value::deserialize<nbn::core::serializable_variants_t>(R"([1 2])");
    });
}

void test_serialization_covers_lowercase_unicode_and_numeric_deserializers() {
    const auto lowercaseUnicode = nbn::core::serialization::deserialize<std::string>(R"("\u00df")");
    nbn::core::unit_tests::equal("Lowercase unicode hex should decode to UTF-8", std::string{"\xC3\x9F"}, lowercaseUnicode);

    const auto minimumSignedLongLong = nbn::core::utils::string::toString(std::numeric_limits<long long>::min());
    const auto signedVariant = nbn::core::serialization::value::deserialize<serializable_variants_t>(minimumSignedLongLong);
    nbn::core::unit_tests::equal("Minimum signed long long should deserialize as the original value", minimumSignedLongLong,
                                 nbn::core::serialization::value::serialize(signedVariant));

    const auto maximumUnsignedLongLong = nbn::core::utils::string::toString(std::numeric_limits<unsigned long long>::max());
    const auto unsignedVariant = nbn::core::serialization::value::deserialize<serializable_variants_t>(maximumUnsignedLongLong);
    nbn::core::unit_tests::equal("Maximum unsigned long long should deserialize as the original value", maximumUnsignedLongLong,
                                 nbn::core::serialization::value::serialize(unsignedVariant));
}

void test_serialization_covers_numeric_deserializer_fallbacks() {
    constexpr auto kInvalidNumeric = "1x";
    constexpr auto kValidUnsignedLongLong = "1";
    constexpr auto kValidLongLong = "-1";

    (void)nbn::core::serialization::value::arithmetic_deserializers::integer::parseUnsignedChar(kInvalidNumeric);
    (void)nbn::core::serialization::value::arithmetic_deserializers::integer::parseUnsignedShort(kInvalidNumeric);
    (void)nbn::core::serialization::value::arithmetic_deserializers::integer::parseUnsignedInt(kInvalidNumeric);
    (void)nbn::core::serialization::value::arithmetic_deserializers::integer::parseUnsignedLong(kInvalidNumeric);
    (void)nbn::core::serialization::value::arithmetic_deserializers::integer::parseChar("a");
    const auto validChar = nbn::core::serialization::value::arithmetic_deserializers::integer::parseChar("97");
    unit_tests::equal("Char parser should return a successfully parsed character", std::string{"\"a\""},
                      nbn::core::serialization::value::serialize(validChar));
    (void)nbn::core::serialization::value::arithmetic_deserializers::integer::parseShort(kInvalidNumeric);
    (void)nbn::core::serialization::value::arithmetic_deserializers::integer::parseInt(kInvalidNumeric);
    const auto fallbackLong = nbn::core::serialization::value::arithmetic_deserializers::integer::parseLong(kInvalidNumeric);
    unit_tests::equal("Long parser fallback should preserve the parsed prefix", std::string{"1"},
                      nbn::core::serialization::value::serialize(fallbackLong));
    (void)nbn::core::serialization::value::arithmetic_deserializers::integer::parseUnsignedLongLong(kInvalidNumeric);
    (void)nbn::core::serialization::value::arithmetic_deserializers::integer::parseLongLong(kInvalidNumeric);
    (void)nbn::core::serialization::value::arithmetic_deserializers::integer::parseUnsignedLongLong(kValidUnsignedLongLong);
    (void)nbn::core::serialization::value::arithmetic_deserializers::integer::parseLongLong(kValidLongLong);
}

void test_serialization_covers_public_debeautify_and_nested_json_values() {
    const auto formatted = std::string{"{\n  \"items\": [1, true, null]\n}"};
    const auto compact = nbn::core::serialization::deBeautifyJson(formatted);
    nbn::core::unit_tests::equal("Public deBeautifyJson should remove external whitespace",
                                 std::string{"{\"items\":[1,true,null]}"}, compact);

    std::unordered_map<std::string, std::string> properties{};
    size_t position{0};
    nbn::core::serialization::json::parseObject(properties, R"({"nested":{"value":[1,2]}})", position);
    nbn::core::unit_tests::equal("Nested JSON values should be retained", std::string{"{\"value\":[1,2]}"},
                                 properties.at("nested"));

    position = 0;
    const auto parsedObject = nbn::core::serialization::json::parseObject(R"({"empty":{},"items":[1,true]})", position);
    nbn::core::unit_tests::equal("String JSON object parser should preserve nested values",
                                 std::string{"{\"empty\":{},\"items\":[1,true]}"}, parsedObject);
    position = 0;
    nbn::core::unit_tests::equal("String JSON object parser should preserve an empty object", std::string{"{}"},
                                 nbn::core::serialization::json::parseObject("{}", position));
}

void test_serialization_covers_variant_object_array_and_string_skip_paths() {
    constexpr auto kNestedArray = R"([1,[2,[3]]])";

    const auto arrayValue = nbn::core::serialization::value::deserialize<serializable_variants_t>(kNestedArray);
    unit_tests::equal("Variant array should preserve nested objects and arrays", std::string{kNestedArray},
                      nbn::core::serialization::value::serialize(arrayValue));

    const auto emptyArray = nbn::core::serialization::value::deserialize<serializable_variants_t>("[]");
    unit_tests::equal("Empty variant array should round-trip", std::string{"[]"},
                      nbn::core::serialization::value::serialize(emptyArray));

    std::unordered_map<std::string, std::string> properties{};
    size_t position{0};
    nbn::core::serialization::json::parseObject(
        properties, R"({"text":"line\nvalue","unicode":"\u0041","surrogate":"\uD83D\uDE00"})", position);
    unit_tests::equal("Object parser should retain escaped string values", std::string{"\"line\\nvalue\""}, properties.at("text"));
    unit_tests::equal("Object parser should retain unicode escape values", std::string{"\"\\u0041\""}, properties.at("unicode"));
    unit_tests::equal("Object parser should retain surrogate escapes", std::string{"\"\\uD83D\\uDE00\""},
                      properties.at("surrogate"));

    properties.clear();
    position = 0;
    nbn::core::serialization::json::parseObject(properties, R"({"empty":{}})", position);
}

void test_serialization_covers_nested_serializable_vector_instantiations_and_variants() {
    constexpr char kCharacterValue{'c'};
    constexpr long long kLongLongValue{42};
    constexpr unsigned long kUnsignedLongValue{84U};

    const nbn::core::serializable_variants_t character{kCharacterValue};
    const nbn::core::serializable_variants_t longLong{kLongLongValue};
    const nbn::core::serializable_variants_t unsignedLong{kUnsignedLongValue};
    const nbn::core::serializable_vector_t inner{character, longLong, unsignedLong};
    const std::vector<nbn::core::serializable_vector_t> nested{inner};

    const auto serialized = nbn::core::serialization::serialize(nested);
    const auto deserialized = nbn::core::serialization::deserialize<std::vector<nbn::core::serializable_vector_t>>(serialized);
    nbn::core::unit_tests::equal("Nested serializable vectors should round-trip", serialized,
                                 nbn::core::serialization::serialize(deserialized));

    const nbn::core::serializable_variants_t nestedVector{inner};
    nbn::core::serializable_variants_t nullObject{std::in_place_type<nbn::core::serializable_ptr_t>};
    const nbn::core::serializable_variants_t duration{std::in_place_type<std::chrono::nanoseconds>, std::chrono::nanoseconds{7}};
    nbn::core::unit_tests::equal("Variant serializable pointer should serialize as null", std::string{"null"},
                                 nbn::core::serialization::value::serialize(nullObject));
    nbn::core::unit_tests::equal("Variant duration should serialize as nanoseconds", std::string{"7"},
                                 nbn::core::serialization::value::serialize(duration));
    const auto deserializedVariant = nbn::core::serialization::value::deserialize<nbn::core::serializable_variants_t>(
        nbn::core::serialization::value::serialize(nestedVector));
    nbn::core::unit_tests::equal("Variant serializable vector should serialize as an array",
                                 nbn::core::serialization::value::serialize(nestedVector),
                                 nbn::core::serialization::value::serialize(deserializedVariant));
}

void test_serialization_rejects_malformed_variant_objects_and_arrays() {
    unit_tests::isThrowing<std::exception>("String JSON object parser should reject a missing separator", [] {
        size_t position{0};
        (void)nbn::core::serialization::json::parseObject(R"({"first":1 "second":2})", position);
    });
    unit_tests::isThrowing<std::exception>("String JSON object parser should reject a non-string key", [] {
        size_t position{0};
        (void)nbn::core::serialization::json::parseObject(R"({1:2})", position);
    });
    unit_tests::isThrowing<std::exception>("Malformed variant object member should throw", [] {
        (void)nbn::core::serialization::value::deserialize<serializable_variants_t>(R"({"value" 1})");
    });
    unit_tests::isThrowing<std::exception>("Malformed variant array separator should throw", [] {
        (void)nbn::core::serialization::value::deserialize<serializable_variants_t>(R"([1 2])");
    });
    unit_tests::isThrowing<std::exception>("Malformed skipped array separator should throw", [] {
        std::unordered_map<std::string, std::string> properties{};
        size_t position{0};
        nbn::core::serialization::json::parseObject(properties, R"({"items":[1 2]})", position);
    });
    unit_tests::isThrowing<std::exception>("Malformed variant object separator should throw", [] {
        (void)nbn::core::serialization::value::deserialize<serializable_variants_t>(R"({"first":1 "second":2})");
    });
    unit_tests::isThrowing<std::exception>("Malformed variant array trailing comma should throw", [] {
        (void)nbn::core::serialization::value::deserialize<serializable_variants_t>(R"([1,])");
    });
    unit_tests::isThrowing<std::exception>("Unterminated variant string should throw", [] {
        (void)nbn::core::serialization::value::deserialize<serializable_variants_t>(R"({"value":"unterminated})");
    });
}
