#include <nbn/core/Timer.h>
#include <nbn/core/UnitTests.h>

#include <condition_variable>
#include <mutex>

// Test Signal

using namespace nbn::core;

namespace {

constexpr auto kTimerCallbackTimeout = std::chrono::seconds{5};

class CallbackSignal {
   public:
    auto notify() -> void {
        {
            std::lock_guard lock{m_mutex};
            m_fired = true;
        }
        m_condition.notify_one();
    }

    [[nodiscard]] auto wait() -> bool {
        std::unique_lock lock{m_mutex};
        return m_condition.wait_for(lock, kTimerCallbackTimeout, [this]() { return m_fired; });
    }

   private:
    std::mutex m_mutex;
    std::condition_variable m_condition;
    bool m_fired{false};
};

}  // namespace

void test_timer_basic() {
    CallbackSignal callbackFired;
    const int timerInterval = 10;
    nbn::core::Timer timer(std::chrono::milliseconds(timerInterval), [&callbackFired]() { callbackFired.notify(); });

    timer.run();
    const auto didFire = callbackFired.wait();
    timer.stop();
    unit_tests::isTrue("Timer time out should have been called at least once", didFire);
}

void test_timer_change_interval() {
    CallbackSignal callbackFired;
    const int timerInterval = 10;
    nbn::core::Timer timer(std::chrono::milliseconds(timerInterval), [&callbackFired]() { callbackFired.notify(); });

    timer.run();
    unit_tests::isTrue("Timer callback should fire before changing its interval", callbackFired.wait());

    const int newTimerInterval = 20;
    unit_tests::isThrowing<std::runtime_error>(
        "Timer should throw when changing interval while running",
        [&timer, newTimerInterval]() { timer.run(std::chrono::milliseconds(newTimerInterval), nullptr); });

    nbn::log::info("Previous log should be the exception thrown by the timer");

    timer.stop();
    unit_tests::isTrue("Timer should be stopped", !timer.isRunning());
}

void test_timer_no_callback() {
    const int timerInterval = 10;
    nbn::core::Timer timer(std::chrono::milliseconds(timerInterval), nullptr, 1U);

    timer.run();
    timer.wait();
}

void test_timer_multiple_timers() {
    CallbackSignal timer1Fired;
    CallbackSignal timer2Fired;
    const int timer1Interval = 10;
    nbn::core::Timer timer1(std::chrono::milliseconds(timer1Interval), [&timer1Fired]() { timer1Fired.notify(); });
    const int timer2Interval = 20;
    nbn::core::Timer timer2(std::chrono::milliseconds(timer2Interval), [&timer2Fired]() { timer2Fired.notify(); });

    timer1.run();
    timer2.run();
    const auto timer1DidFire = timer1Fired.wait();
    const auto timer2DidFire = timer2Fired.wait();

    timer1.stop();
    timer2.stop();
    unit_tests::isTrue("Timer 1 time out should have been called at least once", timer1DidFire);
    unit_tests::isTrue("Timer 2 time out should have been called at least once", timer2DidFire);
}

void test_timer_repeat_count() {
    // Create a timer with a 10ms interval and a callback that increments a counter
    int count = 0;
    const int timerInterval = 10;
    const int repeatCount = 3;
    nbn::core::Timer timer(std::chrono::milliseconds(timerInterval), [&count]() { ++count; }, repeatCount);

    // Start the timer and wait for it to run for a while
    timer.run();
    timer.wait();

    unit_tests::isTrue("Timer time out should have been called the expected number of times", count == repeatCount);
    unit_tests::equal("Timer repeat count property should be accessible", static_cast<size_t>(0), timer.repeatCount()->get());
}

void test_timer_single_shot_and_lifecycle_signals() {
    constexpr auto kSingleShotInterval{std::chrono::milliseconds{1}};
    constexpr int kSingleShotRepeatCount{1};
    constexpr int kExpectedTimeoutCount{1};
    constexpr int kExpectedStartedCount{1};

    int timeoutCount{0};
    int startedCount{0};
    int stoppedCount{0};
    nbn::core::Timer timer(kSingleShotInterval, [&]() { ++timeoutCount; }, kSingleShotRepeatCount);
    timer.signalStarted()->connect([&]() { ++startedCount; });
    timer.signalStopped()->connect([&]() { ++stoppedCount; });

    unit_tests::isTrue("Repeat count one should identify a single-shot timer", timer.isSingleShot());
    unit_tests::isFalse("New timer should not be running", timer.isRunning());
    timer.stop();
    timer.run();
    timer.wait();

    unit_tests::equal("Single-shot timer should invoke its callback once", kExpectedTimeoutCount, timeoutCount);
    unit_tests::equal("Timer should emit started once", kExpectedStartedCount, startedCount);
    unit_tests::isFalse("Single-shot timer should stop after waiting", timer.isRunning());
    timer.stop();
    unit_tests::isTrue("Timer stop should emit a stopped signal", stoppedCount > 0);
}

void test_timer_default_and_runtime_configuration() {
    Timer timer{};
    int timeoutCount{0};
    timer.run(std::chrono::milliseconds{1}, [&timeoutCount]() { ++timeoutCount; }, 1U);
    timer.wait();

    unit_tests::equal("Runtime-configured timer should invoke its callback once", 1, timeoutCount);
    unit_tests::isTrue("Completed timer should have received a stop request", timer.isStopRequested());
}
