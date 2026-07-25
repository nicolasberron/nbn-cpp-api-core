#include <string>
#include <utility>
#include <vector>

#include <nbn/core/Object.h>
#include <nbn/core/UnitTests.h>
#include <nbn/ui/html/containers/Splitter.h>
#include <nbn/ui/html/dialogs/FileDialog.h>
#include <nbn/ui/html/dialogs/FileOpenDialog.h>
#include <nbn/ui/html/dialogs/MessageBox.h>
#include <nbn/ui/html/widgets/Spacer.h>
#include <nbn/ui/interfaces/Widget.h>

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
    auto layout() -> nbn::core::enum_property_ptr<nbn::ui::interfaces::LayoutType> {
        return enumProperty<nbn::ui::interfaces::LayoutType>();
    }

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

    layout->setEnumValues({nbn::ui::interfaces::LayoutType::Vertical, nbn::ui::interfaces::LayoutType::Horizontal});
    nbn::core::unit_tests::equal("Enum property should expose configured values", kValueCount, layout->values().size());
    layout->setIndex(kSecondIndex);
    nbn::core::unit_tests::equal("Enum property should select its index", nbn::ui::interfaces::LayoutType::Horizontal,
                                 layout->get());
    nbn::core::unit_tests::equal("Enum property should emit index changes", kSingleChangeCount, indexChanges);
    nbn::core::unit_tests::equal("Enum property should emit value changes", kSingleChangeCount, valueChanges);
    nbn::core::unit_tests::equal("Enum any-changed callback should receive the value", std::string{"horizontal"}, serializedValue);

    layout->set(nbn::ui::interfaces::LayoutType::Vertical);
    layout->set(nbn::ui::interfaces::LayoutType::Vertical);
    nbn::core::unit_tests::equal("Enum property should ignore unchanged values", kTwoChangeCount, indexChanges);
}

void test_enum_property_rejects_invalid_values_and_handles_empty_state() {
    PropertyTestObject object{};
    const auto layout = object.layout();
    layout->setValues({std::string{"vertical"}, kNonStringValue, std::string{"horizontal"}});
    nbn::core::unit_tests::equal("Enum values should filter non-string entries", kValueCount, layout->values().size());

    layout->setIndex(kInvalidIndex);
    layout->setIndex(kOutOfRangeIndex);
    layout->set(nbn::ui::interfaces::LayoutType::Grid);
    nbn::core::unit_tests::equal("Invalid enum selections should preserve the current index", kFirstIndex, layout->index());
    nbn::core::unit_tests::equal("Invalid enum selections should preserve the current value",
                                 nbn::ui::interfaces::LayoutType::Vertical, layout->get());

    layout->setValues({});
    layout->set(nbn::ui::interfaces::LayoutType::Horizontal);
    layout->setIndex(kFirstIndex);
    nbn::core::unit_tests::equal("Empty enum values should reset the index", kFirstIndex, layout->index());
    nbn::core::unit_tests::equal("Empty enum values should return the default", nbn::ui::interfaces::LayoutType::Vertical,
                                 layout->get());
}

void test_enum_property_serialization_and_deserialization_variants() {
    PropertyTestObject object{};
    const auto layout = object.layout();
    layout->setEnumValues({nbn::ui::interfaces::LayoutType::Vertical, nbn::ui::interfaces::LayoutType::Horizontal});
    layout->deserialize(R"({"values":["vertical","horizontal"],"index":1})");
    nbn::core::unit_tests::equal("Enum object payload should restore the index", kSecondIndex, layout->index());
    nbn::core::unit_tests::equal("Enum property should serialize its state",
                                 std::string{"{\"values\":[\"vertical\",\"horizontal\"],\"index\":1}"}, layout->serialize());
    layout->deserialize(R"({"values":["vertical"]})");
    nbn::core::unit_tests::equal("Missing enum index should default to zero", kFirstIndex, layout->index());
    layout->setEnumValues({nbn::ui::interfaces::LayoutType::Vertical, nbn::ui::interfaces::LayoutType::Horizontal});
    layout->deserialize("{}");
    nbn::core::unit_tests::equal("Missing enum values should clear the property", kFirstIndex, layout->index());
    nbn::core::unit_tests::equal("Missing enum values should return the default enum", nbn::ui::interfaces::LayoutType::Vertical,
                                 layout->get());
    layout->setEnumValues({nbn::ui::interfaces::LayoutType::Vertical, nbn::ui::interfaces::LayoutType::Horizontal});
    layout->deserialize(R"("horizontal")");
    nbn::core::unit_tests::equal("Scalar enum payload should be accepted", nbn::ui::interfaces::LayoutType::Horizontal,
                                 layout->get());
}

void test_enum_property_instantiations_expose_common_accessors() {
    PropertyTestObject object{};
    exercise_enum_property("enumValue", object,
                           {nbn::ui::interfaces::SizePolicy::Preferred, nbn::ui::interfaces::SizePolicy::MaximumExpanding},
                           object.enumValue<nbn::ui::interfaces::SizePolicy>());
    exercise_enum_property("enumValue", object, {nbn::ui::interfaces::IconSize::Px16, nbn::ui::interfaces::IconSize::Px24},
                           object.enumValue<nbn::ui::interfaces::IconSize>());
    exercise_enum_property(
        "enumValue", object,
        {nbn::ui::html::containers::SplitterOrientation::Vertical, nbn::ui::html::containers::SplitterOrientation::Horizontal},
        object.enumValue<nbn::ui::html::containers::SplitterOrientation>());
    exercise_enum_property("enumValue", object,
                           {nbn::ui::html::dialogs::FileBrowseMode::Server, nbn::ui::html::dialogs::FileBrowseMode::Client},
                           object.enumValue<nbn::ui::html::dialogs::FileBrowseMode>());
    exercise_enum_property("enumValue", object,
                           {nbn::ui::html::dialogs::MessageBoxResult::Unset, nbn::ui::html::dialogs::MessageBoxResult::Ok},
                           object.enumValue<nbn::ui::html::dialogs::MessageBoxResult>());
    exercise_enum_property("enumValue", object,
                           {nbn::ui::html::dialogs::MessageBoxButtons::Ok, nbn::ui::html::dialogs::MessageBoxButtons::OkCancel},
                           object.enumValue<nbn::ui::html::dialogs::MessageBoxButtons>());
    exercise_enum_property("enumValue", object,
                           {nbn::ui::html::dialogs::FileOpenDialogMode::Files, nbn::ui::html::dialogs::FileOpenDialogMode::Folders},
                           object.enumValue<nbn::ui::html::dialogs::FileOpenDialogMode>());
    exercise_enum_property(
        "enumValue", object,
        {nbn::ui::html::widgets::SpacerOrientation::Horizontal, nbn::ui::html::widgets::SpacerOrientation::Vertical},
        object.enumValue<nbn::ui::html::widgets::SpacerOrientation>());
}
