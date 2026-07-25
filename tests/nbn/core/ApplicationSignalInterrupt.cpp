#include <csignal>
#include <cstdlib>
#include <format>

#include <nbn/core/Application.h>
#include <nbn/core/UnitTests.h>

// Test Application

using namespace nbn::core;

void test_application_signal_interrupt() {
    auto receivedSignal = 0;

    Application::signalInterrupted()->connect([&](int signal) {
        if (signal == SIGUSR1) {
            nbn::log::info("SIGUSR1 received");
            receivedSignal = signal;
        } else {
            nbn::log::error(std::format("Invalid signal received: {}. expected SIGUSR1", signal));
            std::exit(1);
        }
    });

    nbn::log::info("Raising SIGUSR1");
    std::raise(SIGUSR1);

    unit_tests::equal("SIGUSR1 should be forwarded through Application::signalInterrupted", SIGUSR1, receivedSignal);
}
