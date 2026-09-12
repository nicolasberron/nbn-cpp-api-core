#include <nbn/core/Object.h>
#include <nbn/core/ObjectSync.h>
#include <nbn/core/UnitTests.h>

using namespace nbn::core;

void test_objectsync_a_to_b();

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

namespace {

// Object with text + count + extra (extra has no match in ObjB)
class ObjA : public nbn::core::Object {
   public:
    auto text() -> nbn::core::property_ptr<std::string> { return property<std::string>(); }
    auto count() -> nbn::core::property_ptr<int> { return property<int>(); }
    auto extra() -> nbn::core::property_ptr<bool> { return property<bool>(); }
};

// Object with text + count + other (other has no match in ObjA)
class ObjB : public nbn::core::Object {
   public:
    auto text() -> nbn::core::property_ptr<std::string> { return property<std::string>(); }
    auto count() -> nbn::core::property_ptr<int> { return property<int>(); }
    auto other() -> nbn::core::property_ptr<double> { return property<double>(); }
};

// Initializes all properties so they appear in getProperties()
auto initA(std::shared_ptr<ObjA> a) -> void {
    a->text()->set("");
    a->count()->set(0);
    a->extra()->set(false);
}

auto initB(std::shared_ptr<ObjB> b) -> void {
    b->text()->set("");
    b->count()->set(0);
    b->other()->set(0.0);
}

}  // namespace

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

void test_objectsync_a_to_b() {
    auto a = std::make_shared<ObjA>();
    auto b = std::make_shared<ObjB>();
    initA(a);
    initB(b);

    auto sync = ObjectSync::create({a, b});

    a->text()->set("hello");
    unit_tests::equal("B.text follows A.text", std::string{"hello"}, b->text()->get());

    constexpr auto newCount = 42;
    a->count()->set(newCount);
    unit_tests::equal("B.count follows A.count", newCount, b->count()->get());
}

void test_objectsync_b_to_a() {
    auto a = std::make_shared<ObjA>();
    auto b = std::make_shared<ObjB>();
    initA(a);
    initB(b);

    auto sync = ObjectSync::create({a, b});

    b->text()->set("world");
    unit_tests::equal("A.text follows B.text", std::string{"world"}, a->text()->get());

    constexpr auto newCount = 7;
    b->count()->set(newCount);
    unit_tests::equal("A.count follows B.count", newCount, a->count()->get());
}

void test_objectsync_no_infinite_loop() {
    auto a = std::make_shared<ObjA>();
    auto b = std::make_shared<ObjB>();
    initA(a);
    initB(b);

    auto sync = ObjectSync::create({a, b});

    int aChanges = 0;
    int bChanges = 0;
    a->text()->signalValueChanged()->connect([&](const std::string&, const std::string&) { ++aChanges; });
    b->text()->signalValueChanged()->connect([&](const std::string&, const std::string&) { ++bChanges; });

    a->text()->set("ping");

    // A fired once, B propagated once; no further echoes
    unit_tests::equal("A.text changed exactly once", 1, aChanges);
    unit_tests::equal("B.text changed exactly once", 1, bChanges);
}

void test_objectsync_unmatched_properties_ignored() {
    auto a = std::make_shared<ObjA>();
    auto b = std::make_shared<ObjB>();
    initA(a);
    initB(b);

    auto sync = ObjectSync::create({a, b});

    // 'extra' exists only on A — changing it must not crash
    a->extra()->set(true);
    unit_tests::isTrue("A.extra can be set freely", a->extra()->get());

    // 'other' exists only on B — changing it must not crash
    constexpr auto newOther = 3.14;
    constexpr auto three = 3.0;
    b->other()->set(newOther);
    unit_tests::isTrue("B.other can be set freely", b->other()->get() > three);
}

void test_objectsync_deactivation() {
    auto a = std::make_shared<ObjA>();
    auto b = std::make_shared<ObjB>();
    initA(a);
    initB(b);

    {
        auto sync = ObjectSync::create({a, b});
        a->text()->set("active");
        unit_tests::equal("B.text follows while sync is alive", std::string{"active"}, b->text()->get());
    }  // sync destroyed here

    a->text()->set("after-sync");
    unit_tests::notEqual("B.text does not follow after sync is destroyed", std::string{"after-sync"}, b->text()->get());

    b->text()->set("after-sync-from-b");
    unit_tests::notEqual("A.text does not follow after sync is destroyed", std::string{"after-sync-from-b"}, a->text()->get());
}

void test_objectsync_initial_values_not_propagated_on_create() {
    auto a = std::make_shared<ObjA>();
    auto b = std::make_shared<ObjB>();
    initA(a);
    initB(b);

    a->text()->set("A-initial");
    b->text()->set("B-initial");

    // Creating the sync must NOT overwrite either side
    auto sync = ObjectSync::create({a, b});

    unit_tests::equal("A.text unchanged after sync creation", std::string{"A-initial"}, a->text()->get());
    unit_tests::equal("B.text unchanged after sync creation", std::string{"B-initial"}, b->text()->get());
}

void test_objectsync_ignores_changes_after_peer_destruction() {
    auto a = std::make_shared<ObjA>();
    auto b = std::make_shared<ObjB>();
    initA(a);
    initB(b);
    auto sync = ObjectSync::create({a, b});
    const auto bText = b->text();
    b.reset();

    a->text()->set("peer-destroyed");
    unit_tests::equal("Sync should ignore changes when the peer is destroyed", std::string{}, bText->get());
}

void test_objectsync_ignores_changes_after_the_other_peer_is_destroyed() {
    auto a = std::make_shared<ObjA>();
    auto b = std::make_shared<ObjB>();
    initA(a);
    initB(b);
    auto sync = ObjectSync::create({a, b});
    a.reset();

    b->text()->set("peer-destroyed");
    unit_tests::equal("Remaining peer should still accept changes", std::string{"peer-destroyed"}, b->text()->get());
}
