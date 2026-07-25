#include <atomic>
#include <chrono>
#include <stdexcept>

#include <nbn/core/Object.h>
#include <nbn/core/UnitTests.h>
#include <nbn/core/utils.h>

// Test Signal

using namespace nbn::core;

// Define some test arguments
constexpr int arg1 = 42;
constexpr unsigned char additionalByteValue = static_cast<unsigned char>(arg1);
constexpr size_t additionalCountValue = static_cast<size_t>(arg1);
constexpr auto additionalDurationValue = std::chrono::nanoseconds{arg1};
constexpr auto deserializedDurationValue = std::chrono::nanoseconds{13};
constexpr std::string_view serializedAdditionalDuration{"42"};
constexpr std::string_view serializedDeserializedDuration{"13"};

void test_signal_connect_disconnect() {
    nbn::core::Signal<int> signal{"signal"};
    int count = 0;

    // Connect a slot and emit the signal
    auto slot = [&](int) { count++; };
    signal.connect(slot);
    signal.emit(arg1);
    unit_tests::equal("Signal::emit() should call the slot once", 1, count);

    // Disconnect the slot and emit the signal again
    signal.disconnect(slot);
    signal.emit(arg1);
    unit_tests::equal("Signal::emit() should not call the slot anymore", 1, count);
}

void test_signal_handler_can_reenter_signal() {
    nbn::core::Signal<int> signal{"signal"};
    int count = 0;
    std::function<void(int)> slot;
    slot = [&](int) {
        ++count;
        signal.disconnect(slot);
    };

    signal.connect(slot);
    signal.emit(arg1);
    signal.emit(arg1);

    unit_tests::equal("Signal handlers should be able to disconnect during dispatch", 1, count);
}

void test_signal_emit_restores_sender_after_exception() {
    nbn::core::Signal<int> signal{"signal"};
    signal.connect([](int) { throw std::runtime_error{"signal failure"}; });

    unit_tests::isThrowing<std::runtime_error>("Signal emission should propagate slot exceptions",
                                               [&signal]() { signal.emit(arg1); });
}

void test_signal_named_slot_connections_dispatch_and_ignore_expired_slots() {
    nbn::core::Signal<int> signal{"signal"};
    auto liveSlot = std::make_shared<nbn::core::Slot<int>>("liveSlot");
    auto expiredSlot = std::make_shared<nbn::core::Slot<int>>("expiredSlot");
    int count{0};
    signal.connect(liveSlot, [&count](int) { ++count; });
    signal.connect(expiredSlot, [&count](int) { ++count; });
    expiredSlot.reset();

    signal.emit(arg1);
    unit_tests::equal("Live named slot connections should dispatch", 1, count);
}

void test_signal_named_slot_connection_propagates_exceptions() {
    nbn::core::Signal<int> signal{"signal"};
    auto slot = std::make_shared<nbn::core::Slot<int>>("slot");
    signal.connect(slot, [](int) { throw std::runtime_error{"named slot failure"}; });

    unit_tests::isThrowing<std::runtime_error>("Named slot exceptions should propagate", [&signal]() { signal.emit(arg1); });
}

template <typename... Args>
void exercise_named_signal(Args... args) {
    nbn::core::Signal<Args...> signal{"signal"};
    auto liveSlot = std::make_shared<nbn::core::Slot<Args...>>("liveSlot");
    auto expiredSlot = std::make_shared<nbn::core::Slot<Args...>>("expiredSlot");
    auto count = 0;
    signal.connect(liveSlot, [&count](auto&&...) { ++count; });
    signal.connect(expiredSlot, [&count](auto&&...) { ++count; });
    expiredSlot.reset();
    signal.emit(args...);
    unit_tests::equal("Named slots should dispatch for every production signal type", 1, count);

    nbn::core::Signal<Args...> throwingSignal{"throwingSignal"};
    auto throwingSlot = std::make_shared<nbn::core::Slot<Args...>>("throwingSlot");
    throwingSignal.connect(throwingSlot, [](auto&&...) { throw std::runtime_error{"named slot failure"}; });
    unit_tests::isThrowing<std::runtime_error>("Named slot exceptions should restore the sender",
                                               [&throwingSignal, &args...] { throwingSignal.emit(args...); });
}

void test_signal_named_slot_connections_cover_all_production_types() {
    exercise_named_signal(std::string{"value"});
    exercise_named_signal(std::string{"old"}, std::string{"new"});
    exercise_named_signal(static_cast<unsigned char>(1), static_cast<unsigned char>(2));
    exercise_named_signal(std::string{"value"}, static_cast<unsigned int>(1), static_cast<unsigned int>(2), std::string{"old"},
                          std::string{"new"});
    exercise_named_signal(std::chrono::nanoseconds{1}, std::chrono::nanoseconds{2});
}

void test_signal_emitAsync() {
    Signal<int> sig{"signal"};
    std::atomic<int> count{0};

    // Connect a slot that increments count
    sig.connect([&count](int) { count.fetch_add(1, std::memory_order_relaxed); });

    // Emit asynchronously and wait for completion
    const int signalValue = 42;
    auto fut = sig.emitAsync(signalValue);
    fut.wait();

    // Check that count was incremented
    unit_tests::equal("Signal::emitAsync() should have been called", 1, count.load());

    // Emit asynchronously multiple times and wait for completion
    std::vector<std::future<void>> futures{};
    const int numSignals = 10;
    for (int i = 0; i < numSignals; ++i) {
        futures.emplace_back(sig.emitAsync(i));
    }
    for (auto& fut : futures) {
        fut.wait();
    }

    // Check that count was incremented 11 times
    const int expectedCount = 1 + numSignals;
    unit_tests::equal("Signal::emitAsync() should have been called 11 times", expectedCount, count.load());
}

class TestSignal : public nbn::core::Object {
   public:
    TestSignal() = default;
    ~TestSignal() override = default;

    auto signalInt() -> signal_ptr<int> { return signal<int>(); }
};

void test_signal_in_class() {
    TestSignal testSignal;
    int count = 0;

    // Connect a slot and emit the signal
    auto slot = [&](int) { count++; };
    testSignal.signalInt()->connect(slot);
    testSignal.signalInt()->emit(arg1);
    unit_tests::equal("Signal::emit() should call the slot once", 1, count);

    // Disconnect the slot and emit the signal again
    testSignal.signalInt()->disconnect(slot);
    testSignal.signalInt()->emit(arg1);
    unit_tests::equal("Signal::emit() should not call the slot anymore", 1, count);

    // The name of the signal should be "signalInt"
    unit_tests::equal("Signal::getName() should return the name of the signal", "signalInt", testSignal.signalInt()->getName());

    // The owner of the signal should be the TestSignal instance
    auto isTheOwner = testSignal.signalInt()->getOwner() == &testSignal;
    unit_tests::isTrue("Signal::getOwner() should return the owner of the signal", isTheOwner);
}

class TestProperty : public nbn::core::Object {
   public:
    TestProperty() {}
    ~TestProperty() override = default;

    auto propertyInt() -> property_ptr<int> { return property<int>(); }
    auto propertyString() -> property_ptr<std::string> { return property<std::string>(); }
};

class AdditionalPropertyTypes final : public nbn::core::Object {
   public:
    auto byte() -> property_ptr<unsigned char> { return property<unsigned char>(); }
    auto count() -> property_ptr<size_t> { return property<size_t>(); }
    auto duration() -> property_ptr<std::chrono::nanoseconds> { return property<std::chrono::nanoseconds>(); }
};

void test_property_additional_core_value_types() {
    AdditionalPropertyTypes object{};
    const auto byte = object.byte();
    const auto count = object.count();
    const auto duration = object.duration();
    auto durationChanges = 0;

    duration->signalValueChanged()->connect([&durationChanges](const auto&, const auto&) { ++durationChanges; });
    duration->connectAnyChanged([](std::string) {});

    byte->set(additionalByteValue);
    count->set(additionalCountValue);
    duration->set(additionalDurationValue);

    unit_tests::equal("Unsigned byte property should return its value", additionalByteValue, byte->get());
    unit_tests::isTrue("Unsigned byte property should expose its owner", byte->getOwner() == &object);
    unit_tests::equal("Size property should return its value", additionalCountValue, count->get());
    unit_tests::isTrue("Duration property should expose its owner", duration->getOwner() == &object);
    unit_tests::isTrue("Unsigned byte property lookup should return the existing property", byte == object.byte());
    unit_tests::isTrue("Size property lookup should return the existing property", count == object.count());
    unit_tests::isTrue("Duration property lookup should return the existing property", duration == object.duration());
    unit_tests::equal("Duration property should emit its value signal", 1, durationChanges);
    unit_tests::equal("Duration property should serialize its value", std::string{serializedAdditionalDuration},
                      duration->serialize());
    duration->deserialize(serializedDeserializedDuration);
    unit_tests::equal("Duration property should deserialize its value", deserializedDurationValue, duration->get());
}

void test_property_in_class() {
    TestProperty testProperty;

    // The name of the property should be "propertyInt"
    unit_tests::equal("Property::getName() should return the name of the property", "propertyInt",
                      testProperty.propertyInt()->getName());

    // The owner of the property should be the TestProperty instance
    auto isTheOwner = testProperty.propertyInt()->getOwner() == &testProperty;
    unit_tests::isTrue("Property::getOwner() should return the owner of the property", isTheOwner);

    int oldValue{-1};
    int newValue{-1};

    // Connect a slot and set the property
    auto slot = [&](int old, int newV) {
        oldValue = old;
        newValue = newV;
    };
    testProperty.propertyInt()->signalValueChanged()->connect(slot);

    // The value of the property should be zero
    const auto zero = 0;
    unit_tests::equal("Property::get() should return the value of the property", zero, testProperty.propertyInt()->get());

    // The value of the property should be fortyTwo
    const auto fortyTwo = 42;
    testProperty.propertyInt()->set(fortyTwo);
    unit_tests::equal("Property::get() should return the value of the property", fortyTwo, testProperty.propertyInt()->get());

    // The old value should be zero and the new value should be fortyTwo
    unit_tests::equal("Property::set() should emit the signal", zero, oldValue);
    unit_tests::equal("Property::set() should emit the signal", fortyTwo, newValue);

    const auto stringProperty = testProperty.propertyString();
    unit_tests::isTrue("String property lookup should return the existing property",
                       stringProperty == testProperty.propertyString());
    stringProperty->set("value");
    unit_tests::equal("Non-trivial property should return its value", std::string{"value"}, stringProperty->get());

    nbn::log::debug(std::format("42 == {}", utils::string::fromString<int>("42")));
    nbn::log::debug(std::format("3.14 == {}", utils::string::fromString<float>("3.14")));
    nbn::log::debug(std::format("hello == {}", utils::string::fromString<std::string>("hello")));
    nbn::log::debug(std::format("true == {}", utils::string::fromString<bool>(serialization::json::constants::TRUE)));
    nbn::log::debug(std::format("false == {}", utils::string::fromString<bool>(serialization::json::constants::FALSE)));
    nbn::log::debug(std::format("A == {}", utils::string::fromString<char>("A")));
}
