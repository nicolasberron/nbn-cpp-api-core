#include <nbn/core/Color.h>
#include <nbn/core/UnitTests.h>
#include <nbn/core/utils.h>

#include <format>
#include <memory>
#include <string>

using namespace nbn::core;

class TestColorHolder : public Object {
   public:
    TestColorHolder() { (void)color(); }

    auto color() -> property_ptr<Color> { return property<Color>(); }
};

// Factory registration
static auto const registerToFactoryTestColorHolder{[]() { return Object::factoryRegister<TestColorHolder>(); }()};

void test_color_sync_between_hex_and_rgba() {
    constexpr std::string_view kHexInput{"#11223344"};
    constexpr std::string_view kHexAfterRedUpdate{"#AA223344"};
    constexpr int kExpectedSingleChange{1};
    constexpr int kExpectedDoubleChange{2};

    Color color;

    int redChanges{0};
    int greenChanges{0};
    int blueChanges{0};
    int alphaChanges{0};
    int hexChanges{0};

    color.red()->signalValueChanged()->connect([&](unsigned char oldRed, unsigned char newRed) {
        (void)oldRed;
        (void)newRed;
        ++redChanges;
    });
    color.green()->signalValueChanged()->connect([&](unsigned char oldGreen, unsigned char newGreen) {
        (void)oldGreen;
        (void)newGreen;
        ++greenChanges;
    });
    color.blue()->signalValueChanged()->connect([&](unsigned char oldBlue, unsigned char newBlue) {
        (void)oldBlue;
        (void)newBlue;
        ++blueChanges;
    });
    color.alpha()->signalValueChanged()->connect([&](unsigned char oldAlpha, unsigned char newAlpha) {
        (void)oldAlpha;
        (void)newAlpha;
        ++alphaChanges;
    });
    color.hex()->signalValueChanged()->connect([&](std::string oldHex, std::string newHex) {
        (void)oldHex;
        (void)newHex;
        ++hexChanges;
    });

    color.hex()->set(std::string{kHexInput});

    unit_tests::equal("Color::hex() should update from a hex input", std::string{kHexInput}, color.hex()->get());
    unit_tests::equal("Color::channels() should match the hex serialization", color.hex()->get(),
                      std::format("#{:02X}{:02X}{:02X}{:02X}", color.red()->get(), color.green()->get(), color.blue()->get(),
                                  color.alpha()->get()));
    unit_tests::equal("Color::hex() should emit once for the input change", kExpectedSingleChange, hexChanges);
    unit_tests::equal("Color::red() should emit once for the input change", kExpectedSingleChange, redChanges);
    unit_tests::equal("Color::green() should emit once for the input change", kExpectedSingleChange, greenChanges);
    unit_tests::equal("Color::blue() should emit once for the input change", kExpectedSingleChange, blueChanges);
    unit_tests::equal("Color::alpha() should emit once for the input change", kExpectedSingleChange, alphaChanges);

    Color expected;
    expected.hex()->set(std::string{kHexAfterRedUpdate});
    color.red()->set(expected.red()->get());

    unit_tests::equal("Color::hex() should update from an RGB input", std::string{kHexAfterRedUpdate}, color.hex()->get());
    unit_tests::equal("Color::red() should emit once more for the RGB input change", kExpectedDoubleChange, redChanges);
    unit_tests::equal("Color::green() should not loop for the RGB input change", kExpectedSingleChange, greenChanges);
    unit_tests::equal("Color::blue() should not loop for the RGB input change", kExpectedSingleChange, blueChanges);
    unit_tests::equal("Color::alpha() should not loop for the RGB input change", kExpectedSingleChange, alphaChanges);
    unit_tests::equal("Color::hex() should emit once more for the RGB input change", kExpectedDoubleChange, hexChanges);
}

void test_color_serialization_deserialization() {
    constexpr unsigned char kTestRed{0xAAU};
    constexpr unsigned char kTestGreen{0xBBU};
    constexpr unsigned char kTestBlue{0xCCU};
    constexpr unsigned char kTestAlpha{0xDDU};
    constexpr std::string_view kExpectedHex{"#AABBCCDD"};

    // Use class property auto-shared_ptr behavior.
    auto holder = std::make_shared<TestColorHolder>();
    holder->color()->get()->red()->set(kTestRed);
    holder->color()->get()->green()->set(kTestGreen);
    holder->color()->get()->blue()->set(kTestBlue);
    holder->color()->get()->alpha()->set(kTestAlpha);

    // Serialize through the holder and deserialize through generic factory-backed path.
    auto serialized = holder->serialize();
    nbn::log::debug(std::format("Serialized Color holder: {}", serialized));

    // Verify serialized content contains expected values
    unit_tests::equal("Serialized Color should contain hex value", true,
                      serialized.find(std::string{kExpectedHex}) != std::string::npos);

    auto deserializedHolder = nbn::core::serialization::deserialize<std::shared_ptr<TestColorHolder>>(serialized);
    auto color = deserializedHolder->color()->get();

    // Verify the deserialized Color is not null and has the same values
    unit_tests::equal("Deserialized holder should not be null", true, deserializedHolder != nullptr);
    unit_tests::equal("Deserialized Color should not be null", true, color != nullptr);
    unit_tests::equal("Deserialized Color red component should match original", kTestRed, color->red()->get());
    unit_tests::equal("Deserialized Color green component should match original", kTestGreen, color->green()->get());
    unit_tests::equal("Deserialized Color blue component should match original", kTestBlue, color->blue()->get());
    unit_tests::equal("Deserialized Color alpha component should match original", kTestAlpha, color->alpha()->get());
    unit_tests::equal("Deserialized Color hex should match original", std::string{kExpectedHex}, color->hex()->get());
}

void test_class_property_auto_shared_ptr_serialization_deserialization() {
    constexpr std::string_view kExpectedHex{"#12345678"};

    auto holder = std::make_shared<TestColorHolder>();

    // Property<Color> should auto-create its shared instance.
    unit_tests::equal("Class property should auto-create shared_ptr instance", true, holder->color()->get() != nullptr);

    holder->color()->get()->hex()->set(std::string{kExpectedHex});

    const auto serialized = holder->serialize();
    auto deserialized = nbn::core::serialization::deserialize<std::shared_ptr<TestColorHolder>>(serialized);

    unit_tests::equal("Deserialized holder should not be null", true, deserialized != nullptr);
    unit_tests::equal("Deserialized class property should not be null", true, deserialized->color()->get() != nullptr);
    unit_tests::equal("Deserialized class property should preserve hex", std::string{kExpectedHex},
                      deserialized->color()->get()->hex()->get());
}

void test_color_ignores_invalid_hex_values_and_supports_rgb_alpha_forms() {
    constexpr unsigned char kExpectedRed{0xA1U};
    constexpr unsigned char kExpectedGreen{0xB2U};
    constexpr unsigned char kExpectedBlue{0xC3U};
    constexpr unsigned char kExpectedAlpha{0xD4U};

    Color color;
    color.hex()->set("#010203");
    unit_tests::equal("RGB hex should preserve its three-channel form", std::string{"#010203"}, color.hex()->get());

    color.hex()->set("#A1B2C3D4");
    unit_tests::equal("RGBA hex should update all channels", kExpectedRed, color.red()->get());
    unit_tests::equal("RGBA hex should update green", kExpectedGreen, color.green()->get());
    unit_tests::equal("RGBA hex should update blue", kExpectedBlue, color.blue()->get());
    unit_tests::equal("RGBA hex should update alpha", kExpectedAlpha, color.alpha()->get());

    const auto previousRed = color.red()->get();
    const auto previousGreen = color.green()->get();
    const auto previousBlue = color.blue()->get();
    const auto previousAlpha = color.alpha()->get();
    color.hex()->set("invalid");
    unit_tests::equal("Invalid hex should preserve red", previousRed, color.red()->get());
    unit_tests::equal("Invalid hex should preserve green", previousGreen, color.green()->get());
    unit_tests::equal("Invalid hex should preserve blue", previousBlue, color.blue()->get());
    unit_tests::equal("Invalid hex should preserve alpha", previousAlpha, color.alpha()->get());
    color.hex()->set("#123");
    unit_tests::equal("Short hex should preserve red", previousRed, color.red()->get());
    unit_tests::equal("Short hex should preserve green", previousGreen, color.green()->get());
    unit_tests::equal("Short hex should preserve blue", previousBlue, color.blue()->get());
    unit_tests::equal("Short hex should preserve alpha", previousAlpha, color.alpha()->get());

    color.hex()->set("#GGGGGG");
    unit_tests::equal("Non-hex RGB data should preserve red", previousRed, color.red()->get());
    unit_tests::equal("Non-hex RGB data should preserve green", previousGreen, color.green()->get());
    unit_tests::equal("Non-hex RGB data should preserve blue", previousBlue, color.blue()->get());
    unit_tests::equal("Non-hex RGB data should preserve alpha", previousAlpha, color.alpha()->get());
}

void test_color_destructor() {
    std::unique_ptr<nbn::core::Object> color{std::make_unique<nbn::core::Color>()};
    color.reset();
    unit_tests::isTrue("Color destructor should complete without throwing", true);
}