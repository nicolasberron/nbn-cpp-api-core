#include <exception>

#include <nbn/core/Application.h>
#include <nbn/core/UnitTests.h>

// Test Application

using namespace nbn::core;

class UncaughtException : public std::runtime_error {
   public:
    UncaughtException() : std::runtime_error("Test uncaught exception") {}
};

void test_application_run_async_throw_and_do_not_catch_exception() {
    Application::signalUncaughtAsyncException()->connect([]() {
        try {
            std::rethrow_exception(std::current_exception());
        } catch (const UncaughtException&) {
            nbn::log::info("Expected uncaught exception caught");
            exit(0);
        }
    });

    auto throwingTask = Application::runAsync(
        []() {
            nbn::log::info("Exception throwing task started");
            throw UncaughtException();
        },
        Task::RunMode::SingleShot);
}
