#include <thread>

#include <nbn/core/ElapsedTimer.h>
#include <nbn/core/UnitTests.h>

using namespace nbn::core;

void test_ElapsedTimer() {
    ElapsedTimer timer;
    unit_tests::isTrue("ElapsedTimer should be 0.", timer.elapsed() == std::chrono::nanoseconds::zero());

    timer.start();
    const auto timeToSleep = 100;
    std::this_thread::sleep_for(std::chrono::milliseconds(timeToSleep));
    const int MINIMUM_ELAPSED_TIME = 100;
    unit_tests::isTrue("ElapsedTimer should be greater than 100 ms.",
                       timer.elapsed() > std::chrono::milliseconds(MINIMUM_ELAPSED_TIME));
    unit_tests::isTrue("ElapsedTimer should report an expired duration", timer.hasExpired(std::chrono::milliseconds{1}));
    unit_tests::isFalse("ElapsedTimer should not report an unreachable duration as expired",
                        timer.hasExpired(std::chrono::hours{1}));
}
