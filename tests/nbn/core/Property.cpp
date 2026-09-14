#include <string>
#include <utility>
#include <vector>

#include <nbn/core/Object.h>
#include <nbn/core/UnitTests.h>
#include <nbn/core/detail/serialization.h>

namespace test_types {

enum class Layout { Vertical, Horizontal, Grid };

}  // namespace test_types

namespace nbn::core {

template <>
struct enum_serialization<test_types::Layout>
    : enum_serialization_xref<enum_serialization<test_types::Layout>, test_types::Layout> {
    using enum_type = test_types::Layout;
    using self = enum_serialization<enum_type>;
    using base = enum_serialization_xref<self, enum_type>;

    enum_serialization()
        : base({{enum_type::Vertical, "vertical"}, {enum_type::Horizontal, "horizontal"}, {enum_type::Grid, "grid"}}) {}
};

}  // namespace nbn::core

namespace {

constexpr auto kInitialNumber = 7;
constexpr auto kUpdatedNumber = 11;
constexpr auto kDeserializedNumber = 13;
constexpr auto kFirstIndex = 0;
constexpr auto kSecondIndex = 1;
constexpr auto kInvalidIndex = -1;
constexpr auto kOutOfRangeIndex = 2;
constexpr auto kNonStringValue = 42;
constexpr auto kValueCount = 2U;
constexpr auto kSingleChangeCount = 1;
constexpr auto kTwoChangeCount = 2;

class PropertyTestObject final : public nbn::core::Object {
   public:
    auto number() -> nbn::core::property_ptr<int> { return property<int>(); }
    auto text() -> nbn::core::property_ptr<std::string> { return property<std::string>(); }
    auto layout() -> nbn::core::enum_property_ptr<test_types::Layout> { return enumProperty<test_types::Layout>(); }

    template <typename Enum>
    auto enumValue() -> nbn::core::enum_property_ptr<Enum> {
        return enumProperty<Enum>();
    }
};

template <typename Enum>
void exercise_enum_property(const std::string& name,
                            PropertyTestObject& object,
                            const std::vector<Enum>& values,
                            const nbn::core::enum_property_ptr<Enum>& property) {
    auto anyChangedValue = std::string{};
    property->connectAnyChanged([&anyChangedValue](std::string value) { anyChangedValue = std::move(value); });
    property->setEnumValues(values);
    property->setIndex(kSecondIndex);

    nbn::core::unit_tests::equal("Enum property name should be available", name, std::string{property->getName()});
    nbn::core::unit_tests::isTrue("Enum property owner should be available", property->getOwner() == &object);
    nbn::core::unit_tests::equal("Enum property should expose its selected index", kSecondIndex, property->index());
    nbn::core::unit_tests::equal("Enum property should expose its configured values", values.size(), property->values().size());
    nbn::core::unit_tests::equal("Enum property should expose its selected value", values[kSecondIndex], property->get());
    nbn::core::unit_tests::isTrue("Enum property should serialize its state", !property->serialize().empty());
    nbn::core::unit_tests::isTrue("Enum value signal should be available", property->signalValueChanged() != nullptr);
    nbn::core::unit_tests::isTrue("Enum index signal should be available", property->signalIndexChanged() != nullptr);
    property->set(values[kFirstIndex]);
    nbn::core::unit_tests::isTrue("Enum callback should receive a value", !anyChangedValue.empty());
}

}  // namespace

void test_property_scalar_accessors_and_notifications() {
    PropertyTestObject object{};
    const auto number = object.number();
    int changes = 0;
    number->signalValueChanged()->connect([&](int oldValue, int newValue) {
        if (oldValue == kInitialNumber && newValue == kUpdatedNumber) {
            ++changes;
        }
    });

    number->set(kInitialNumber);
    number->set(kInitialNumber);
    number->set(kUpdatedNumber);

    nbn::core::unit_tests::equal("Property should return its updated value", kUpdatedNumber, number->get());
    nbn::core::unit_tests::equal("Property should emit only for a changed value", kSingleChangeCount, changes);
    nbn::core::unit_tests::equal("Property name should be available", std::string{"number"}, std::string{number->getName()});
    nbn::core::unit_tests::isTrue("Property owner should be available", number->getOwner() == &object);
    nbn::core::unit_tests::equal("Property serialization should round-trip", std::string{"11"}, number->serialize());
    number->deserialize("13");
    nbn::core::unit_tests::equal("Property deserialization should update its value", kDeserializedNumber, number->get());
}

void test_property_non_trivial_values_and_any_changed_callback() {
    PropertyTestObject object{};
    const auto text = object.text();
    auto serializedValue = std::string{};
    text->connectAnyChanged([&serializedValue](std::string value) { serializedValue = std::move(value); });

    text->set("first");
    text->set("second");

    nbn::core::unit_tests::equal("Non-trivial property should return its value", std::string{"second"}, text->get());
    nbn::core::unit_tests::equal("Any-changed callback should receive serialized data", std::string{"\"second\""}, serializedValue);
    nbn::core::unit_tests::equal("Non-trivial property should serialize", std::string{"\"second\""}, text->serialize());
}

void test_enum_property_values_indices_and_notifications() {
    PropertyTestObject object{};
    const auto layout = object.layout();
    nbn::core::unit_tests::equal("Enum property name should be available", std::string{"layout"}, std::string{layout->getName()});
    nbn::core::unit_tests::isTrue("Enum property owner should be available", layout->getOwner() == &object);
    int indexChanges = 0;
    int valueChanges = 0;
    auto serializedValue = std::string{};
    layout->signalIndexChanged()->connect([&](int, int) { ++indexChanges; });
    layout->signalValueChanged()->connect([&](auto, auto) { ++valueChanges; });
    layout->connectAnyChanged([&serializedValue](std::string value) { serializedValue = std::move(value); });

    layout->setEnumValues({test_types::Layout::Vertical, test_types::Layout::Horizontal});
    nbn::core::unit_tests::equal("Enum property should expose configured values", kValueCount, layout->values().size());
    layout->setIndex(kSecondIndex);
    nbn::core::unit_tests::equal("Enum property should select its index", test_types::Layout::Horizontal, layout->get());
    nbn::core::unit_tests::equal("Enum property should emit index changes", kSingleChangeCount, indexChanges);
    nbn::core::unit_tests::equal("Enum property should emit value changes", kSingleChangeCount, valueChanges);
    nbn::core::unit_tests::equal("Enum any-changed callback should receive the value", std::string{"horizontal"}, serializedValue);

    layout->set(test_types::Layout::Vertical);
    layout->set(test_types::Layout::Vertical);
    nbn::core::unit_tests::equal("Enum property should ignore unchanged values", kTwoChangeCount, indexChanges);
}

void test_enum_property_rejects_invalid_values_and_handles_empty_state() {
    PropertyTestObject object{};
    const auto layout = object.layout();
    layout->setValues({std::string{"vertical"}, kNonStringValue, std::string{"horizontal"}});
    nbn::core::unit_tests::equal("Enum values should filter non-string entries", kValueCount, layout->values().size());

    layout->setIndex(kInvalidIndex);
    layout->setIndex(kOutOfRangeIndex);
    layout->set(test_types::Layout::Grid);
    nbn::core::unit_tests::equal("Invalid enum selections should preserve the current index", kFirstIndex, layout->index());
    nbn::core::unit_tests::equal("Invalid enum selections should preserve the current value", test_types::Layout::Vertical,
                                 layout->get());

    layout->setValues({});
    layout->set(test_types::Layout::Horizontal);
    layout->setIndex(kFirstIndex);
    nbn::core::unit_tests::equal("Empty enum values should reset the index", kFirstIndex, layout->index());
    nbn::core::unit_tests::equal("Empty enum values should return the default", test_types::Layout::Vertical, layout->get());
}

void test_enum_property_serialization_and_deserialization_variants() {
    PropertyTestObject object{};
    const auto layout = object.layout();
    layout->setEnumValues({test_types::Layout::Vertical, test_types::Layout::Horizontal});
    layout->deserialize(R"({"values":["vertical","horizontal"],"index":1})");
    nbn::core::unit_tests::equal("Enum object payload should restore the index", kSecondIndex, layout->index());
    nbn::core::unit_tests::equal("Enum property should serialize its state",
                                 std::string{"{\"values\":[\"vertical\",\"horizontal\"],\"index\":1}"}, layout->serialize());
    layout->deserialize(R"({"values":["vertical"]})");
    nbn::core::unit_tests::equal("Missing enum index should default to zero", kFirstIndex, layout->index());
    layout->setEnumValues({test_types::Layout::Vertical, test_types::Layout::Horizontal});
    layout->deserialize("{}");
    nbn::core::unit_tests::equal("Missing enum values should clear the property", kFirstIndex, layout->index());
    nbn::core::unit_tests::equal("Missing enum values should return the default enum", test_types::Layout::Vertical, layout->get());
    layout->setEnumValues({test_types::Layout::Vertical, test_types::Layout::Horizontal});
    layout->deserialize(R"("horizontal")");
    nbn::core::unit_tests::equal("Scalar enum payload should be accepted", test_types::Layout::Horizontal, layout->get());
}

void test_enum_property_instantiations_expose_common_accessors() {
    PropertyTestObject object{};
    exercise_enum_property("enumValue", object, {test_types::Layout::Vertical, test_types::Layout::Horizontal},
                           object.enumValue<test_types::Layout>());
}
