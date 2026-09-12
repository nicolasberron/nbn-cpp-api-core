#include <nbn/core/FiniteStateMachine.h>
#include <nbn/core/Object.h>
#include <nbn/core/Timer.h>
#include <nbn/core/UnitTests.h>

#include <functional>
#include <thread>

// Test Slot

using namespace nbn::core;

// Keep declarations visible to the CMake test-main generator when it scans
// this source file.
void test_slot_disconnect();
void test_slot_free_function();
void test_signal_free_function();
void test_slot_bind_raw_pointer();
void test_slot_bind_shared_ptr();
void test_slot_name();
void test_slot_serialization_round_trip();
void test_sender_is_set_during_slot_invocation();
void test_timer_signal_timeout_fires();
void test_object_serialize_contains_slot_keys();
void test_slot_arg_type_names_empty();
void test_signal_handler_const_ref_arg();
void test_slot_const_value_arg_type_name();

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

namespace {

// Object exposing a parameterised slot, a no-arg slot and a signal
class TestObject : public nbn::core::Object {
   public:
    auto signalFired() -> signal_ptr<int> { return signal<int>(); }
    auto signalPing() -> signal_ptr<> { return signal<>(); }
    auto signalMessage() -> signal_ptr<std::string> { return signal<std::string>(); }
    auto signalMulti() -> signal_ptr<int, bool> { return signal<int, bool>(); }
    auto signalConstValue() -> signal_ptr<const int> { return signal<const int>(); }
    auto signalConstRef() -> signal_ptr<const std::string&> { return signal<const std::string&>(); }
    auto signalMutableRef() -> signal_ptr<int&> { return signal<int&>(); }
    auto onFired() -> slot_ptr<int> { return slot<int>(); }
    auto onPing() -> slot_ptr<> { return slot<>(); }
    auto onMessage() -> slot_ptr<std::string> { return slot<std::string>(); }
    auto onMulti() -> slot_ptr<int, bool> { return slot<int, bool>(); }
    auto onConstValue() -> slot_ptr<const int> { return slot<const int>(); }
    auto onConstRef() -> slot_ptr<const std::string&> { return slot<const std::string&>(); }
    auto onMutableRef() -> slot_ptr<int&> { return slot<int&>(); }
};

// Handler class used in std::bind tests
class Receiver {
   public:
    int count{0};
    int lastValue{-1};

    void onPing() { ++count; }
    void onValue(int v) {
        ++count;
        lastValue = v;
    }
};

auto freePingCount() -> int& {
    static int count{0};
    return count;
}

auto freeIntCount() -> int& {
    static int count{0};
    return count;
}

static void freePingHandler() {
    ++freePingCount();
}
static void freeIntHandler(int) {
    ++freeIntCount();
}

// Minimal two-state FSM: A → B (immediate conditional transition)
class TwoStateMachine : public nbn::core::fsm::Machine {
   public:
    std::shared_ptr<nbn::core::fsm::interfaces::IState> spA;
    std::shared_ptr<nbn::core::fsm::interfaces::IState> spB;

    TwoStateMachine() {
        spA = addState("A", nullptr, nullptr);
        spB = addState(
            "B",
            []() -> bool {
                return false;  // stop FSM after entering B
            },
            nullptr);
        addTransition(spA, spB, []() { return true; }, nullptr);
    }
};

}  // namespace

// ---------------------------------------------------------------------------
// Factory-registered class for object-level serialization tests
// ---------------------------------------------------------------------------

class SlotSerializableObject : public nbn::core::Object {
   public:
    SlotSerializableObject() {
        (void)onAction();
        (void)onValue();
    }
    auto onAction() -> slot_ptr<> { return slot<>(); }
    auto onValue() -> slot_ptr<int> { return slot<int>(); }
    auto signalTrigger() -> signal_ptr<> { return signal<>(); }
};

static auto const registerSlotSerializableObject{[]() { return Object::factoryRegister<SlotSerializableObject>(); }()};

// ---------------------------------------------------------------------------
// Basic slot connect / emit / disconnect
// ---------------------------------------------------------------------------

void run_slot_multiple_handlers() {
    TestObject obj{};
    int count{0};
    int received{-1};

    obj.signalPing()->connect([&]() { ++count; });
    obj.signalPing()->connect([&]() { ++count; });
    obj.signalPing()->emit();

    unit_tests::equal("slot should invoke all connected handlers", 2, count);

    obj.signalFired()->connect(obj.onFired(), [&](int v) { received = v; });
    constexpr int kTestValue{42};
    obj.signalFired()->emit(kTestValue);
    unit_tests::equal("slot::emit should invoke the connected handler", kTestValue, received);
}

void test_slot_disconnect() {
    run_slot_multiple_handlers();

    TestObject obj{};
    int count{0};
    auto handler = [&]() { ++count; };

    obj.signalPing()->connect(handler);
    obj.signalPing()->emit();
    unit_tests::equal("slot should fire after connect", 1, count);

    obj.signalPing()->disconnect(handler);
    obj.signalPing()->emit();
    unit_tests::equal("slot should not fire after disconnect", 1, count);
}

void test_slot_returns_existing_instances_for_all_argument_types() {
    TestObject object{};

    unit_tests::isTrue("const-value signal lookup should return the existing signal",
                       object.signalConstValue() == object.signalConstValue());
    unit_tests::isTrue("int slot lookup should return the existing slot", object.onFired() == object.onFired());
    unit_tests::isTrue("empty slot lookup should return the existing slot", object.onPing() == object.onPing());
    unit_tests::isTrue("string slot lookup should return the existing slot", object.onMessage() == object.onMessage());
    unit_tests::isTrue("multi-argument slot lookup should return the existing slot", object.onMulti() == object.onMulti());
    unit_tests::isTrue("const-value slot lookup should return the existing slot", object.onConstValue() == object.onConstValue());
    unit_tests::isTrue("const-reference slot lookup should return the existing slot", object.onConstRef() == object.onConstRef());
    unit_tests::isTrue("mutable-reference slot lookup should return the existing slot",
                       object.onMutableRef() == object.onMutableRef());
}

// ---------------------------------------------------------------------------
// Free functions — no lambda wrapper
// ---------------------------------------------------------------------------

void test_slot_free_function() {
    freePingCount() = 0;
    TestObject obj{};

    obj.signalPing()->connect(freePingHandler);
    obj.signalPing()->emit();
    unit_tests::equal("slot connected to free function should fire", 1, freePingCount());

    obj.signalPing()->disconnect(freePingHandler);
    obj.signalPing()->emit();
    unit_tests::equal("slot disconnected from free function should not fire", 1, freePingCount());
}

void test_signal_free_function() {
    TestObject obj{};

    freeIntCount() = 0;
    constexpr int kTestValue{7};

    obj.signalFired()->connect(freeIntHandler);
    obj.signalFired()->emit(kTestValue);
    unit_tests::equal("signal connected to free function should fire", 1, freeIntCount());

    obj.signalFired()->disconnect(freeIntHandler);
    obj.signalFired()->emit(kTestValue);
    unit_tests::equal("signal disconnected from free function should not fire", 1, freeIntCount());
}

// ---------------------------------------------------------------------------
// std::bind with raw pointer — no lambda wrapper
// ---------------------------------------------------------------------------

void test_slot_bind_raw_pointer() {
    Receiver r{};
    TestObject obj{};

    obj.signalPing()->connect(std::bind(&Receiver::onPing, &r));
    obj.signalPing()->emit();
    obj.signalPing()->emit();

    unit_tests::equal("slot with std::bind(raw ptr) should fire on each emit", 2, r.count);
}

void test_signal_bind_raw_pointer() {
    Receiver r{};
    TestObject obj{};
    constexpr int kTestValue{99};

    // _1 forwards the single int argument emitted by signalFired
    obj.signalFired()->connect(std::bind(&Receiver::onValue, &r, std::placeholders::_1));
    obj.signalFired()->emit(kTestValue);

    unit_tests::equal("signal with std::bind(raw ptr) should fire", 1, r.count);
    unit_tests::equal("signal std::bind handler should receive the correct argument", kTestValue, r.lastValue);
}

// ---------------------------------------------------------------------------
// std::bind with shared_ptr — no lambda wrapper
// ---------------------------------------------------------------------------

void test_slot_bind_shared_ptr() {
    auto spR = std::make_shared<Receiver>();
    TestObject obj{};

    obj.signalPing()->connect(std::bind(&Receiver::onPing, spR));
    obj.signalPing()->emit();

    unit_tests::equal("slot with std::bind(shared_ptr) should fire", 1, spR->count);
}

void test_signal_bind_shared_ptr() {
    auto spR = std::make_shared<Receiver>();
    TestObject obj{};

    obj.signalFired()->connect(std::bind(&Receiver::onValue, spR, std::placeholders::_1));
    constexpr int kTestValue{55};
    obj.signalFired()->emit(kTestValue);

    unit_tests::equal("signal with std::bind(shared_ptr) should fire", 1, spR->count);
    unit_tests::equal("signal std::bind(shared_ptr) should forward the argument", kTestValue, spR->lastValue);
}

// ---------------------------------------------------------------------------
// Slot identity (name / owner)
// ---------------------------------------------------------------------------

void test_slot_name() {
    TestObject obj{};
    unit_tests::equal("slot::getName should return the method name 'onFired'", std::string{"onFired"},
                      std::string{obj.onFired()->getName()});
    unit_tests::equal("slot::getName should return the method name 'onPing'", std::string{"onPing"},
                      std::string{obj.onPing()->getName()});
}

void test_slot_owner() {
    TestObject obj{};
    unit_tests::isTrue("slot::getOwner should return the declaring object", obj.onPing()->getOwner() == &obj);
}

// ---------------------------------------------------------------------------
// Slot serialization — name and arg types
// ---------------------------------------------------------------------------

void test_slot_serialization_round_trip() {
    TestObject obj{};
    const auto spSlot = obj.onPing();

    // Serialized form: JSON object with name and arg-types array
    unit_tests::equal("slot::serialize should include name and empty args array", std::string{R"({"name":"onPing","args":[]})"},
                      spSlot->serialize());

    // deserialize is a no-op: name and args are fixed at construction
    spSlot->deserialize(R"("anything")");
    unit_tests::equal("slot::serialize should be unchanged after deserialize", std::string{R"({"name":"onPing","args":[]})"},
                      spSlot->serialize());
}

// ---------------------------------------------------------------------------
// sender() — identifies the emitting object inside a slot handler
// ---------------------------------------------------------------------------

void test_sender_is_set_during_slot_invocation() {
    TestObject obj{};
    nbn::core::Object* capturedSender{nullptr};

    obj.signalFired()->connect(obj.onFired(), [&](int) { capturedSender = nbn::core::sender(); });
    obj.signalFired()->emit(0);

    unit_tests::isTrue("sender() should return the signal owner during slot invocation", capturedSender == &obj);
}

void test_sender_is_null_outside_emit() {
    unit_tests::isTrue("sender() should return nullptr outside of signal dispatch", nbn::core::sender() == nullptr);
}
// ---------------------------------------------------------------------------
// Timer — signalTimeout and onTimeout slot (manually chained)
// ---------------------------------------------------------------------------

void test_timer_signal_timeout_fires() {
    int count{0};
    nbn::core::Timer timer{};

    // signalTimeout is emitted by the timer on every tick
    timer.signalTimeout()->connect([&]() { ++count; });

    constexpr int kRepeatCount{3};
    constexpr std::chrono::milliseconds kInterval{20};
    timer.run(kInterval, nullptr, kRepeatCount);
    timer.wait();

    unit_tests::equal("signalTimeout should fire exactly 3 times for repeatCount=3", kRepeatCount, count);
}

// ---------------------------------------------------------------------------
// Object serialization — slots appear in serialized JSON
// ---------------------------------------------------------------------------

void test_object_serialize_contains_slot_keys() {
    TestObject obj{};
    // Access slots to register them before serialization
    (void)obj.onFired();
    (void)obj.onPing();

    const auto json = obj.serialize();

    unit_tests::equal("serialized object JSON should contain onFired slot with arg type", true,
                      json.find(R"("onFired":{"name":"onFired","args":["int"]})") != std::string::npos);
    unit_tests::equal("serialized object JSON should contain onPing slot with no args", true,
                      json.find(R"("onPing":{"name":"onPing","args":[]})") != std::string::npos);
}

void test_object_round_trip_with_slots() {
    auto spObj = std::make_shared<SlotSerializableObject>();
    const auto json = spObj->serialize();

    auto spDeserialized = nbn::core::serialization::deserialize<std::shared_ptr<SlotSerializableObject>>(json);
    unit_tests::isTrue("deserialized object should not be null", spDeserialized != nullptr);

    int count{0};
    spDeserialized->signalTrigger()->connect(spDeserialized->onAction(), [&]() { ++count; });
    spDeserialized->signalTrigger()->emit();
    unit_tests::equal("onAction slot should fire after object round-trip deserialization", 1, count);
}

void test_object_deserialize_missing_slot_key_is_tolerated() {
    // Build minimal JSON without slot keys, simulating pre-0.12.0 JSON
    auto spRef = std::make_shared<SlotSerializableObject>();
    const std::string minimalJson = std::string{R"({")"} + std::string{spRef->getClassName()} + R"(":{"uuid":"legacy-uuid-0001"}})";

    auto spObj = nbn::core::serialization::deserialize<std::shared_ptr<SlotSerializableObject>>(minimalJson);
    unit_tests::isTrue("deserialized object from JSON without slot keys should not be null", spObj != nullptr);

    int count{0};
    spObj->signalTrigger()->connect(spObj->onAction(), [&]() { ++count; });
    spObj->signalTrigger()->emit();
    unit_tests::equal("onAction slot should be usable after deserializing JSON without slot keys", 1, count);
}

// ---------------------------------------------------------------------------
// Slot and Signal argument type introspection
// ---------------------------------------------------------------------------

void test_slot_arg_type_names_empty() {
    TestObject obj{};
    const auto types = obj.onPing()->getArgTypeNames();
    unit_tests::equal("no-arg slot should report zero arg types", true, types.empty());
}

void test_slot_arg_type_names_single() {
    TestObject obj{};
    const auto types = obj.onFired()->getArgTypeNames();
    unit_tests::equal("int slot should report exactly one arg type", 1, static_cast<int>(types.size()));
    unit_tests::equal("int slot arg type name should be 'int'", std::string{"int"}, types[0]);
}

void test_slot_serialize_includes_arg_types() {
    TestObject obj{};
    unit_tests::equal("no-arg slot serialize should have empty args array", std::string{R"({"name":"onPing","args":[]})"},
                      obj.onPing()->serialize());
    unit_tests::equal("int slot serialize should include int in args array", std::string{R"({"name":"onFired","args":["int"]})"},
                      obj.onFired()->serialize());
}

void test_signal_arg_type_names_empty() {
    TestObject obj{};
    const auto types = obj.signalPing()->getArgTypeNames();
    unit_tests::equal("no-arg signal should report zero arg types", true, types.empty());
}

void test_signal_arg_type_names_single() {
    TestObject obj{};
    const auto types = obj.signalFired()->getArgTypeNames();
    unit_tests::equal("int signal should report exactly one arg type", 1, static_cast<int>(types.size()));
    unit_tests::equal("int signal arg type name should be 'int'", std::string{"int"}, types[0]);
}

// ---------------------------------------------------------------------------
// Handler argument qualifications (const ref, by value, multi-arg)
// ---------------------------------------------------------------------------

void test_signal_handler_const_ref_arg() {
    TestObject obj{};
    int received{-1};

    // Handler takes int by const-ref; signal<int> should still invoke it correctly
    obj.signalFired()->connect([&](const int& v) { received = v; });
    constexpr int kValue{77};
    obj.signalFired()->emit(kValue);

    unit_tests::equal("handler taking const int& should receive the emitted value", kValue, received);
}

void test_signal_handler_string_by_value() {
    TestObject obj{};
    std::string received{};

    obj.signalMessage()->connect([&](std::string v) { received = std::move(v); });
    obj.signalMessage()->emit("hello");

    unit_tests::equal("handler taking std::string by value should receive the emitted string", std::string{"hello"}, received);
}

void test_signal_handler_string_const_ref() {
    TestObject obj{};
    std::string received{};

    obj.signalMessage()->connect([&](const std::string& v) { received = v; });
    obj.signalMessage()->emit("world");

    unit_tests::equal("handler taking const std::string& should receive the emitted string", std::string{"world"}, received);
}

void test_signal_multi_arg_emit() {
    TestObject obj{};
    int capturedInt{-1};
    bool capturedBool{false};

    obj.signalMulti()->connect([&](int a, bool b) {
        capturedInt = a;
        capturedBool = b;
    });
    constexpr int kIntValue{42};
    constexpr bool kBoolValue{true};
    obj.signalMulti()->emit(kIntValue, kBoolValue);

    unit_tests::equal("multi-arg signal should deliver the int argument", kIntValue, capturedInt);
    unit_tests::isTrue("multi-arg signal should deliver the bool argument", capturedBool);
}

void test_signal_multi_arg_handler_const_ref() {
    TestObject obj{};
    int capturedInt{-1};
    bool capturedBool{false};

    // Handler takes first arg by const-ref
    obj.signalMulti()->connect([&](const int& a, bool b) {
        capturedInt = a;
        capturedBool = b;
    });
    constexpr int kIntValue{99};
    constexpr bool kBoolValue{true};
    obj.signalMulti()->emit(kIntValue, kBoolValue);

    unit_tests::equal("multi-arg handler with const int& should receive the int argument", kIntValue, capturedInt);
    unit_tests::isTrue("multi-arg handler with const int& should receive the bool argument", capturedBool);
}

void test_slot_arg_type_names_string() {
    TestObject obj{};
    const auto types = obj.onMessage()->getArgTypeNames();
    unit_tests::equal("std::string slot should report exactly one arg type", 1, static_cast<int>(types.size()));
    unit_tests::equal("std::string slot arg type name should be 'std::string'", std::string{"std::string"}, types[0]);
}

void test_slot_arg_type_names_multi() {
    TestObject obj{};
    const auto types = obj.onMulti()->getArgTypeNames();
    unit_tests::equal("int,bool slot should report two arg types", 2, static_cast<int>(types.size()));
    unit_tests::equal("first arg type should be 'int'", std::string{"int"}, types[0]);
    unit_tests::equal("second arg type should be 'bool'", std::string{"bool"}, types[1]);
}

void test_signal_arg_type_names_multi() {
    TestObject obj{};
    const auto types = obj.signalMulti()->getArgTypeNames();
    unit_tests::equal("int,bool signal should report two arg types", 2, static_cast<int>(types.size()));
    unit_tests::equal("first arg type should be 'int'", std::string{"int"}, types[0]);
    unit_tests::equal("second arg type should be 'bool'", std::string{"bool"}, types[1]);
}

void test_slot_serialize_multi_arg() {
    TestObject obj{};
    unit_tests::equal("multi-arg slot serialize should list all arg types",
                      std::string{R"({"name":"onMulti","args":["int","bool"]})"}, obj.onMulti()->serialize());
}

// ---------------------------------------------------------------------------
// Qualified argument types (const, &, const &) — type names and serialization
// ---------------------------------------------------------------------------

void test_slot_const_value_arg_type_name() {
    TestObject obj{};
    const auto types = obj.onConstValue()->getArgTypeNames();
    unit_tests::equal("const int slot should report one arg type", 1, static_cast<int>(types.size()));
    unit_tests::equal("const int slot arg type name should be 'const int'", std::string{"const int"}, types[0]);
}

void test_slot_const_ref_arg_type_name() {
    TestObject obj{};
    const auto types = obj.onConstRef()->getArgTypeNames();
    unit_tests::equal("const string& slot should report one arg type", 1, static_cast<int>(types.size()));
    unit_tests::equal("const string& slot arg type name should be 'const std::string&'", std::string{"const std::string&"},
                      types[0]);
}

void test_slot_mutable_ref_arg_type_name() {
    TestObject obj{};
    const auto types = obj.onMutableRef()->getArgTypeNames();
    unit_tests::equal("int& slot should report one arg type", 1, static_cast<int>(types.size()));
    unit_tests::equal("int& slot arg type name should be 'int&'", std::string{"int&"}, types[0]);
}

void test_slot_const_value_serialize() {
    TestObject obj{};
    unit_tests::equal("const int slot should serialize with qualifier",
                      std::string{R"({"name":"onConstValue","args":["const int"]})"}, obj.onConstValue()->serialize());
}

void test_slot_const_ref_serialize() {
    TestObject obj{};
    unit_tests::equal("const string& slot should serialize with qualifier",
                      std::string{R"({"name":"onConstRef","args":["const std::string&"]})"}, obj.onConstRef()->serialize());
}

void test_slot_mutable_ref_serialize() {
    TestObject obj{};
    unit_tests::equal("int& slot should serialize with qualifier", std::string{R"({"name":"onMutableRef","args":["int&"]})"},
                      obj.onMutableRef()->serialize());
}

void test_signal_const_ref_emit_and_receive() {
    TestObject obj{};
    std::string received{};

    obj.signalConstRef()->connect([&](const std::string& v) { received = v; });
    obj.signalConstRef()->emit("qualified");

    unit_tests::equal("const string& signal should deliver the value", std::string{"qualified"}, received);
}

void test_signal_mutable_ref_handler_mutates_value() {
    TestObject obj{};
    int value{0};

    constexpr int kValue{99};
    obj.signalMutableRef()->connect([](int& v) { v = kValue; });
    obj.signalMutableRef()->emit(value);

    unit_tests::equal("int& signal handler should be able to mutate the referenced value", kValue, value);
}
