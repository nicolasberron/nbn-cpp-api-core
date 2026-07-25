#include <nbn/core/Timer.h>
#include <nbn/core/UnitTests.h>

// Test Signal

using namespace nbn::core;

void test_timer_basic() {
    // Create a timer with a 10ms interval and a callback that increments a counter
    int count = 0;
    const int timerInterval = 10;
    nbn::core::Timer timer(std::chrono::milliseconds(timerInterval), [&count]() { ++count; });

    // Start the timer and wait for it to run for a while
    timer.run();
    const int timeToSleep = 50;
    std::this_thread::sleep_for(std::chrono::milliseconds(timeToSleep));

    // Stop the timer and check that the counter has been incremented at least once
    timer.stop();
    unit_tests::isTrue("Timer time out should have been called at least once", count > 0);
}

void test_timer_change_interval() {
    // Create a timer with a 10ms interval and a callback that increments a counter
    int count = 0;
    const int timerInterval = 10;
    nbn::core::Timer timer(std::chrono::milliseconds(timerInterval), [&count]() { ++count; });

    // Start the timer and wait for it to run for a while
    timer.run();
    const int timeToSleep = 50;
    std::this_thread::sleep_for(std::chrono::milliseconds(timeToSleep));

    // Change the interval to 20ms and wait for the timer to run again
    const int newTimerInterval = 20;
    unit_tests::isThrowing<std::runtime_error>(
        "Timer should throw when changing interval while running",
        [&timer, newTimerInterval]() { timer.run(std::chrono::milliseconds(newTimerInterval), nullptr); });

    const int newTimeToSleep = 50;
    std::this_thread::sleep_for(std::chrono::milliseconds(newTimeToSleep));
    nbn::log::info("Previous log should be the exception thrown by the timer");

    // Stop the timer and check that the counter has been incremented at least three times
    timer.stop();
    unit_tests::isTrue("Timer should be stopped", !timer.isRunning());
}

void test_timer_no_callback() {
    // Create a timer with a 10ms interval and no callback
    const int timerInterval = 10;
    nbn::core::Timer timer(std::chrono::milliseconds(timerInterval), nullptr);

    // Start the timer and wait for it to run for a while
    timer.run();
    const int timeToSleep = 50;
    std::this_thread::sleep_for(std::chrono::milliseconds(timeToSleep));

    // Stop the timer and expect no errors
    timer.stop();
}

void test_timer_multiple_timers() {
    // Create two timers with different intervals and callbacks that increment different counters
    int count1 = 0;
    int count2 = 0;
    const int timer1Interval = 10;
    nbn::core::Timer timer1(std::chrono::milliseconds(timer1Interval), [&count1]() { ++count1; });
    const int timer2Interval = 20;
    nbn::core::Timer timer2(std::chrono::milliseconds(timer2Interval), [&count2]() { ++count2; });

    // Start both timers and wait for them to run for a while
    timer1.run();
    timer2.run();
    const int timeToSleep = 50;
    std::this_thread::sleep_for(std::chrono::milliseconds(timeToSleep));

    // Stop both timers and check that each counter has been incremented at least once
    timer1.stop();
    timer2.stop();
    unit_tests::isTrue("Timer 1 time out should have been called at least once", count1 > 0);
    unit_tests::isTrue("Timer 2 time out should have been called at least once", count2 > 0);
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
