#include "Application.h"
#include "Logger.h"
#include "private/Application.h"

namespace nbn::core {

auto Application::signalInitialized() -> signal_ptr<> {
    return private_ns::Application::self().signalInitialized();
}

auto Application::signalAboutToQuit() -> signal_ptr<> {
    return private_ns::Application::self().signalAboutToQuit();
}

auto Application::signalInterrupted() -> signal_ptr<int> {
    return private_ns::Application::self().signalInterrupted();
}

auto Application::signalUncaughtAsyncException() -> signal_ptr<> {
    return private_ns::Application::self().signalUncaughtAsyncException();
}

auto Application::signalLogDebug() -> signal_ptr<std::string> {
    return private_ns::Application::self().signalLogDebug();
}

auto Application::signalLogInfo() -> signal_ptr<std::string> {
    return private_ns::Application::self().signalLogInfo();
}

auto Application::signalLogWarning() -> signal_ptr<std::string> {
    return private_ns::Application::self().signalLogWarning();
}

auto Application::signalLogError() -> signal_ptr<std::string> {
    return private_ns::Application::self().signalLogError();
}

auto Application::signalLogFatal() -> signal_ptr<std::string> {
    return private_ns::Application::self().signalLogFatal();
}

auto Application::setTerminateHandler(std::function<void()> handler) -> void {
    private_ns::Application::self().setTerminateHandler(handler);
}

void Application::init(std::vector<std::string> args, std::string_view logFilePath) {
    nbn::log::fatal_if(args.size() < 1, "Application::init() requires at least one argument");

    const bool wasAlreadyInitialized = private_ns::Application::self().isApplicationInitialized();

    private_ns::Application::self().init(args);
    private_ns::Application::self().logger()->start(logFilePath);

    // Wire up signal connections only once, even if init() is called multiple times, to avoid duplicate handlers.
    if (!wasAlreadyInitialized) {
        // Forward logger text signals to Application log signals
        auto spLogger = private_ns::Application::self().logger();
        spLogger->signalDebugText()->connect([](const std::string& text) { nbn::core::Application::signalLogDebug()->emit(text); });
        spLogger->signalInfoText()->connect([](const std::string& text) { nbn::core::Application::signalLogInfo()->emit(text); });
        spLogger->signalWarningText()->connect(
            [](const std::string& text) { nbn::core::Application::signalLogWarning()->emit(text); });
        spLogger->signalErrorText()->connect([](const std::string& text) { nbn::core::Application::signalLogError()->emit(text); });
        spLogger->signalFatalText()->connect([](const std::string& text) { nbn::core::Application::signalLogFatal()->emit(text); });

        private_ns::Application::self().signalInitialized()->connect([]() { nbn::log::debug("Application initialized"); });
        private_ns::Application::self().signalAboutToQuit()->connect([]() { nbn::log::debug("Application about to quit"); });
    }

    private_ns::Application::self().signalInitialized()->emit();
}

[[nodiscard]] auto Application::programName() -> const std::string& {
    return private_ns::Application::self().programName();
}

[[nodiscard]] auto Application::programPath() -> const std::filesystem::path& {
    return private_ns::Application::self().programPath();
}

[[nodiscard]] auto Application::arguments() -> const std::vector<std::string>& {
    return private_ns::Application::self().arguments();
}

auto Application::runSync(std::shared_ptr<Task> spTask) -> void {
    private_ns::Application::self().submit(std::move(spTask));
}

auto Application::runSync(Task::task_body_t taskBody, Task::RunMode mode) -> std::shared_ptr<interfaces::IRunnable> {
    auto spTask{std::make_shared<Task>(taskBody, mode)};
    private_ns::Application::self().submit(spTask);
    return spTask;
}

auto Application::runAsync(std::shared_ptr<Task> spTask) -> void {
    private_ns::Application::self().submit(std::make_unique<Thread>(std::move(spTask)));
}

auto Application::runAsync(Task::task_body_t taskBody, Task::RunMode mode) -> std::shared_ptr<interfaces::IRunnable> {
    auto spThread{std::make_shared<Thread>(std::make_shared<Task>(taskBody, mode))};
    private_ns::Application::self().submit(spThread);
    return spThread;
}

[[nodiscard]] auto Application::exec() -> int {
    return private_ns::Application::self().exec();
}

auto Application::quit(int exitCode) -> void {
    private_ns::Application::self().quit(exitCode);
}

}  // namespace nbn::core

namespace nbn::log {
void debug([[maybe_unused]] std::string_view message, [[maybe_unused]] const std::source_location& location) {
#ifdef NBN_DEBUG
    auto spLogger{nbn::core::private_ns::Application::self().logger()};
    if (spLogger != nullptr) {
        spLogger->debug(location.file_name(), location.line(), location.column(), location.function_name(), message);
    }
#endif
}

void info(std::string_view message, const std::source_location& location) {
    auto spLogger{nbn::core::private_ns::Application::self().logger()};
    if (spLogger != nullptr) {
        spLogger->info(location.file_name(), location.line(), location.column(), location.function_name(), message);
    }
}

void warning(std::string_view message, const std::source_location& location) {
    auto spLogger{nbn::core::private_ns::Application::self().logger()};
    if (spLogger != nullptr) {
        spLogger->warning(location.file_name(), location.line(), location.column(), location.function_name(), message);
    }
}

void error(std::string_view message, const std::source_location& location) {
    auto spLogger{nbn::core::private_ns::Application::self().logger()};
    if (spLogger != nullptr) {
        spLogger->error(location.file_name(), location.line(), location.column(), location.function_name(), message);
    }
}

void fatal(std::string_view message, const std::source_location& location) {
    auto spLogger{nbn::core::private_ns::Application::self().logger()};
    if (spLogger != nullptr) {
        const auto fatalMessage{nbn::core::private_ns::stackTraceMessage(std::format("Fatal: {}", message))};
        try {
            spLogger->fatal(location.file_name(), location.line(), location.column(), location.function_name(), fatalMessage);
        } catch (const std::runtime_error&) {
            throw std::runtime_error(std::string(message));
        }
    }
}

void debug_if(bool isCondition, std::string_view message, const std::source_location& location) {
    if (isCondition) {
        debug(message, location);
    }
}

void info_if(bool isCondition, std::string_view message, const std::source_location& location) {
    if (isCondition) {
        info(message, location);
    }
}

void warning_if(bool isCondition, std::string_view message, const std::source_location& location) {
    if (isCondition) {
        warning(message, location);
    }
}

void error_if(bool isCondition, std::string_view message, const std::source_location& location) {
    if (isCondition) {
        error(message, location);
    }
}

void fatal_if(bool isCondition, std::string_view message, const std::source_location& location) {
    if (isCondition) {
        fatal(message, location);
    }
}

}  // namespace nbn::log