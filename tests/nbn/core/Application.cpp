#include <sys/wait.h>
#include <unistd.h>
#include <array>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <exception>
#include <format>
#include <stdexcept>
#include <string>
#include <thread>

#include <nbn/core/Application.h>
#include <nbn/core/UnitTests.h>
#include <nbn/core/private/Application.h>
#include <nbn/core/private/Thread.h>

// Test Application

using namespace nbn::core;

void test_application_init() {
    auto binaryDir = std::filesystem::path(NBN_TEST_BINARY_DIR);
    auto testApplication = binaryDir / "test_Application";
    Application::init({testApplication, "arg1", "arg2"});
    unit_tests::equal("Application::programName() should return \"test_Application\"", std::string("test_Application"),
                      Application::programName());
    unit_tests::equal("Application::programPath() should return \"" + binaryDir.string() + "\"", binaryDir.string(),
                      Application::programPath().string());
    unit_tests::equal("Application::arguments() should return a vector of 2 arguments", static_cast<size_t>(2),
                      Application::arguments().size());
    unit_tests::equal("Application::arguments()[0] should return \"arg1\"", std::string("arg1"), Application::arguments()[0]);
    unit_tests::equal("Application::arguments()[1] should return \"arg2\"", std::string("arg2"), Application::arguments()[1]);
}

void test_application_exposes_runtime_signals() {
    unit_tests::isTrue("Application initialized signal should be available", Application::signalInitialized() != nullptr);
    unit_tests::isTrue("Application about-to-quit signal should be available", Application::signalAboutToQuit() != nullptr);
    unit_tests::isTrue("Application interrupted signal should be available", Application::signalInterrupted() != nullptr);
    unit_tests::isTrue("Application async-exception signal should be available",
                       Application::signalUncaughtAsyncException() != nullptr);
    unit_tests::isTrue("Application debug log signal should be available", Application::signalLogDebug() != nullptr);
    unit_tests::isTrue("Application info log signal should be available", Application::signalLogInfo() != nullptr);
    unit_tests::isTrue("Application warning log signal should be available", Application::signalLogWarning() != nullptr);
    unit_tests::isTrue("Application error log signal should be available", Application::signalLogError() != nullptr);
    unit_tests::isTrue("Application fatal log signal should be available", Application::signalLogFatal() != nullptr);
}

void test_application_forwards_terminate_handler() {
    Application::setTerminateHandler([]() {});
}

void test_application_runs_shared_tasks_and_schedules_sync_tasks() {
    auto asyncTask = std::make_shared<Task>([]() {}, Task::RunMode::SingleShot);
    Application::runAsync(asyncTask);
    asyncTask->wait();

    auto syncTask = std::make_shared<Task>([]() {}, Task::RunMode::SingleShot);
    Application::runSync(syncTask);

    auto scheduledTask = Application::runSync([]() {}, Task::RunMode::SingleShot);
    unit_tests::isTrue("Synchronous task overload should return a runnable", scheduledTask != nullptr);
}

void test_application_run_async() {
    auto isTaskExecuted = false;
    auto task = Application::runAsync(
        [&isTaskExecuted]() {
            nbn::log::info("Task started");
            isTaskExecuted = true;
        },
        Task::RunMode::SingleShot);
    task->wait();
    unit_tests::equal("Task should be executed", true, isTaskExecuted);
}

void test_application_conditional_logging_helpers() {
    nbn::log::debug_if(true, "conditional debug message");
    nbn::log::info_if(true, "conditional info message");
    nbn::log::warning_if(true, "conditional warning message");
    nbn::log::error_if(true, "conditional error message");
    nbn::log::debug_if(false, "suppressed debug message");
    nbn::log::info_if(false, "suppressed info message");
    nbn::log::warning_if(false, "suppressed warning message");
    nbn::log::error_if(false, "suppressed error message");
    unit_tests::isTrue("Conditional logging helpers should return safely", true);
}

void test_application_fatal_logging_includes_stack_trace() {
    const std::string fatalMessage = "fatal stack trace message";
    auto loggedFatalMessage = std::string{};
    auto exceptionMessage = std::string{};
    std::function<void(const std::string&)> fatalLogger = [&loggedFatalMessage](const std::string& text) {
        loggedFatalMessage = text;
    };
    auto fatalSignal = Application::signalLogFatal();
    fatalSignal->connect(fatalLogger);

    nbn::log::fatal_if(false, "suppressed fatal message");
    try {
        nbn::log::fatal(fatalMessage);
    } catch (const std::runtime_error& exception) {
        exceptionMessage = exception.what();
    }
    unit_tests::isTrue("fatal should rethrow after logging a stack trace", !exceptionMessage.empty());
    unit_tests::equal("fatal should preserve the exception message", fatalMessage, exceptionMessage);
    unit_tests::isTrue("fatal should log its message", loggedFatalMessage.find(fatalMessage) != std::string::npos);
    unit_tests::isTrue("fatal should log a stack trace",
                       loggedFatalMessage.find("test_application_fatal_logging_includes_stack_trace") != std::string::npos);

    loggedFatalMessage.clear();
    exceptionMessage.clear();
    try {
        nbn::log::fatal_if(true, fatalMessage);
    } catch (const std::runtime_error& exception) {
        exceptionMessage = exception.what();
    }
    unit_tests::equal("fatal_if should preserve the exception message", fatalMessage, exceptionMessage);
    unit_tests::isTrue("fatal_if should log a stack trace",
                       loggedFatalMessage.find("test_application_fatal_logging_includes_stack_trace") != std::string::npos);
    fatalSignal->disconnect(fatalLogger);
}

// NOTE: Any uncaught exception will be caught by the UnitTest Task in which the test is run.

void test_application_run_async_throw_and_catch_exception() {
    auto isExceptionThrown = false;
    auto isExceptionCaught = false;
    auto throwingTask = Application::runAsync(
        [&isExceptionThrown]() {
            nbn::log::info("Exception throwing task started");
            isExceptionThrown = true;
            throw std::runtime_error("Throwing exception");
        },
        Task::RunMode::SingleShot);
    try {
        throwingTask->wait();
    } catch (const std::exception& ex) {
        isExceptionCaught = true;
    }
    unit_tests::equal("Exception should be thrown", true, isExceptionThrown);
    unit_tests::equal("Exception should be caught", true, isExceptionCaught);

    const auto TIME_TO_SLEEP = 500;
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_TO_SLEEP));
}

namespace {

constexpr auto kPrivateApplicationName = "private-application";
constexpr auto kPrivateApplicationArgument = "coverage-argument";
constexpr int kSuccessExitCode = 0;
constexpr int kDefaultTerminateExitCode = 1;
constexpr int kUnknownExceptionExitCode = 2;

}  // namespace

void test_private_application_initializes_task_and_thread_managers() {
    nbn::core::private_ns::Application taskApplication{};
    taskApplication.init({kPrivateApplicationName});
    auto taskExecuted = false;
    taskApplication.submit(std::make_shared<Task>([&taskExecuted]() { taskExecuted = true; }, Task::RunMode::SingleShot));
    unit_tests::equal("Private application should execute submitted tasks", kSuccessExitCode, taskApplication.exec());
    unit_tests::isTrue("Submitted task should execute", taskExecuted);

    nbn::core::private_ns::Application threadApplication{};
    threadApplication.init({kPrivateApplicationName, kPrivateApplicationArgument});
    auto threadExecuted = false;
    auto thread = std::make_shared<Thread>([&threadExecuted]() { threadExecuted = true; }, Task::RunMode::SingleShot);
    threadApplication.submit(std::move(thread));
    unit_tests::equal("Private application should execute submitted threads", kSuccessExitCode, threadApplication.exec());
    unit_tests::isTrue("Submitted thread should execute", threadExecuted);
}

void test_private_application_exec_initializes_task_manager_when_no_task_is_submitted() {
    nbn::core::private_ns::Application application{};
    application.init({kPrivateApplicationName});

    unit_tests::equal("Private application without submitted tasks should exit successfully", kSuccessExitCode, application.exec());
}

void test_private_application_handles_unknown_task_exceptions() {
    nbn::core::private_ns::Application application{};
    application.init({kPrivateApplicationName});
    application.submit(std::make_shared<Task>([]() { throw kUnknownExceptionExitCode; }, Task::RunMode::SingleShot));

    unit_tests::equal("Unknown task exceptions should return the unknown-exception exit code", kUnknownExceptionExitCode,
                      application.exec());
}

void test_private_application_stores_terminate_handlers() {
    nbn::core::private_ns::Application application{};
    auto handlerCalled = false;
    application.setTerminateHandler([&handlerCalled]() { handlerCalled = true; });

    const auto handler = application.getTerminateHandler();
    unit_tests::isTrue("Private application should return its terminate handler", handler != nullptr);
    handler();
    unit_tests::isTrue("Returned terminate handler should remain callable", handlerCalled);
}

void test_private_application_default_terminate_handler_stops_application() {
    nbn::core::private_ns::Application application{};
    application.init({kPrivateApplicationName});

    const auto handler = application.getTerminateHandler();
    unit_tests::isTrue("Default terminate handler should be installed", handler != nullptr);
    handler();

    unit_tests::equal("Default terminate handler should set the quit code", kDefaultTerminateExitCode, application.exec());
}

void test_throw_no_catch_no_crash() {
    const std::string fatalMessage = "Log should contain this fatal message";

    auto isFatalLogged = false;
    auto fatalSignal = Application::signalLogFatal();
    std::function<void(const std::string&)> fatalLogger = [&isFatalLogged, fatalMessage](const std::string& text) {
        if (text.find(fatalMessage) != std::string::npos) {
            isFatalLogged = true;
        }
    };
    fatalSignal->connect(fatalLogger);

    // Launch a task that throws (nbn::log::fatal() logs and then throws) without catching the exception.
    // We deliberately do NOT call wait(): wait() would rethrow the exception into this thread. The point of
    // this test is that an uncaught async exception still logs its message and does not crash the application.
    auto throwingTask = Application::runAsync([fatalMessage]() { nbn::log::fatal(fatalMessage); }, Task::RunMode::SingleShot);

    const auto TIME_TO_SLEEP = 500;
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_TO_SLEEP));

    unit_tests::equal("Fatal message should be logged", true, isFatalLogged);
    fatalSignal->disconnect(fatalLogger);
}
