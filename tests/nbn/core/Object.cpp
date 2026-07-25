#include <chrono>
#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <nbn/core/Object.h>
#include <nbn/core/UnitTests.h>
#include <nbn/core/private/Object.h>

namespace nbn::core {}  // namespace nbn::core

namespace {

constexpr auto kInitialValue = 17;
constexpr auto kDeserializedValue = 23;
constexpr auto kDeserializedUuid = "coverage-uuid";
constexpr auto kUnquotedUuid = "unquoted-uuid";
constexpr auto kExpectedPropertyCount = 1U;
constexpr auto kThrowingSlotName = "throwingSlot";

class ObjectCoverageFixture final : public nbn::core::Object {
   public:
    ObjectCoverageFixture() {
        value();
        changed();
        activated();
    }

    auto value() -> nbn::core::property_ptr<int> { return property<int>(); }
    auto duration() -> nbn::core::property_ptr<std::chrono::nanoseconds> { return property<std::chrono::nanoseconds>(); }
    auto changed() -> nbn::core::signal_ptr<int> { return signal<int>(); }
    auto activated() -> nbn::core::slot_ptr<> { return slot<>(); }
    auto addThrowingSlot() -> void;
};

class ThrowingSlot final : public nbn::core::interfaces::ISlot {
   public:
    [[nodiscard]] auto getName() const -> std::string_view override { return kThrowingSlotName; }
    [[nodiscard]] auto getOwner() const -> nbn::core::Object* override { return nullptr; }
    [[nodiscard]] auto getArgTypeNames() const -> std::vector<std::string> override { return {}; }
    [[nodiscard]] auto serialize() const -> std::string override { return "null"; }
    auto deserialize(std::string_view) -> void override { throw std::runtime_error{"slot deserialization failed"}; }
};

auto ObjectCoverageFixture::addThrowingSlot() -> void {
    addSlot(std::source_location::current(), std::make_shared<ThrowingSlot>());
}

static auto const registerObjectCoverageFixture{nbn::core::Object::factoryRegister<ObjectCoverageFixture>()};

}  // namespace

void test_object_reports_identity_and_serializes_features() {
    ObjectCoverageFixture object{};
    object.value()->set(kInitialValue);

    const auto className = object.getClassName();
    const auto uuid = object.getUuid();
    const auto serialized = object.serialize();

    nbn::core::unit_tests::isTrue("Object class name should be available", !className.empty());
    nbn::core::unit_tests::isTrue("Object UUID should be available", !uuid.empty());
    nbn::core::unit_tests::equal("Object property should be discoverable", kExpectedPropertyCount, object.getProperties().size());
    nbn::core::unit_tests::isTrue("Named property lookup should return the registered property",
                                  object.getProperty("value") == object.value());
    nbn::core::unit_tests::isTrue("Serialized object should contain its class name",
                                  serialized.find(className) != std::string::npos);
    nbn::core::unit_tests::isTrue("Serialized object should contain its UUID", serialized.find(uuid) != std::string::npos);
    nbn::core::unit_tests::isTrue("Serialized object should contain its signal", serialized.find("changed") != std::string::npos);
    nbn::core::unit_tests::isTrue("Serialized object should contain its slot", serialized.find("activated") != std::string::npos);
    nbn::core::unit_tests::equal("Repeated class-name lookup should be stable", className, object.getClassName());
}

void test_object_deserializes_quoted_and_unquoted_uuids() {
    ObjectCoverageFixture object{};
    object.deserialize(std::string{"{\"uuid\":\""} + kDeserializedUuid + "\",\"value\":" + std::to_string(kDeserializedValue) +
                       "}");

    nbn::core::unit_tests::equal("Quoted UUID should be restored", std::string{kDeserializedUuid}, std::string{object.getUuid()});
    nbn::core::unit_tests::equal("Property value should be restored", kDeserializedValue, object.value()->get());

    nbn::core::Object plainObject{};
    plainObject.deserialize(std::string{"{\"uuid\":"} + kUnquotedUuid + "}");
    nbn::core::unit_tests::equal("Unquoted UUID should be retained", std::string{kUnquotedUuid},
                                 std::string{plainObject.getUuid()});
}

void test_object_deserializes_uuid_without_a_closing_object_delimiter() {
    ObjectCoverageFixture object{};
    object.deserialize(R"({"uuid":   unquoted-uuid)");

    nbn::core::unit_tests::equal("UUID should be retained when the object delimiter is missing", std::string{kUnquotedUuid},
                                 std::string{object.getUuid()});
}

void test_object_rejects_serialized_objects_missing_a_property() {
    ObjectCoverageFixture object{};
    auto missingPropertyThrows = false;

    try {
        object.deserialize(R"({"uuid":"coverage-uuid"})");
    } catch (const std::exception&) {
        missingPropertyThrows = true;
    }

    nbn::core::unit_tests::isTrue("Deserialization should reject an object missing a declared property", missingPropertyThrows);
}

void test_object_logs_and_ignores_property_deserialization_errors() {
    ObjectCoverageFixture object{};
    object.value()->set(kInitialValue);

    object.deserialize(R"({"uuid":"coverage-uuid","value":invalid-value})");

    nbn::core::unit_tests::equal("Invalid property data should leave the existing value unchanged", kInitialValue,
                                 object.value()->get());
}

void test_object_logs_and_ignores_property_value_deserialization_errors() {
    ObjectCoverageFixture object{};
    object.value()->set(kInitialValue);

    object.deserialize(R"({"uuid":"coverage-uuid","value":null})");

    nbn::core::unit_tests::equal("Invalid property values should leave the existing value unchanged", kInitialValue,
                                 object.value()->get());
}

void test_object_logs_and_ignores_slot_deserialization_errors() {
    ObjectCoverageFixture object{};
    object.addThrowingSlot();

    object.deserialize(R"({"uuid":"coverage-uuid","value":23,"throwingSlot":null})");

    nbn::core::unit_tests::equal("Valid properties should deserialize when a slot fails", kDeserializedValue,
                                 object.value()->get());
}

void test_object_factory_create_with_serialized_options() {
    const ObjectCoverageFixture fixture{};
    const auto className = std::string{fixture.getClassName()};
    const auto serialized = std::string{R"({"uuid":"factory-uuid","value":31})"};
    nbn::core::unit_tests::isTrue("Registered object classes should be discoverable", nbn::core::Object::factoryHas(className));
    nbn::core::unit_tests::isFalse("Unknown object classes should not be discoverable",
                                   nbn::core::Object::factoryHas("MissingObjectCoverageFixture"));
    auto object = nbn::core::Object::factoryCreate(className, {serialized});
    nbn::core::unit_tests::isTrue("Factory create with options should return an object", object != nullptr);
    if (object != nullptr) {
        nbn::core::unit_tests::equal("Factory create should restore the UUID", std::string{"factory-uuid"},
                                     std::string{object->getUuid()});
    }
}

void test_object_returns_existing_features_when_requested_repeatedly() {
    ObjectCoverageFixture object{};
    nbn::core::unit_tests::isTrue("Repeated signal lookup should return the existing feature",
                                  object.changed() == object.changed());
    nbn::core::unit_tests::isTrue("Repeated property lookup should return the existing feature", object.value() == object.value());
    nbn::core::unit_tests::isTrue("Repeated slot lookup should return the existing feature",
                                  object.activated() == object.activated());
}

void test_object_creates_duration_property_on_first_request() {
    ObjectCoverageFixture object{};
    const auto duration = object.duration();

    nbn::core::unit_tests::isTrue("First duration property lookup should create a property", duration != nullptr);
    nbn::core::unit_tests::isTrue("Created duration property should be returned on repeated lookup", duration == object.duration());
}
