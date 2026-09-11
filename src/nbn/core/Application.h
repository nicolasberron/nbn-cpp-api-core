#pragma once

#include <filesystem>
#include <vector>

#include "ConcurrentQueue.h"
#include "Logger.h"
#include "Task.h"
#include "interfaces/concurrency.h"

namespace nbn::core {

/**
 * @file Application.h
 * @brief Application lifecycle and task scheduling API.
 */

/**
 * @class Application
 * @brief Global runtime entry point for initialization, event-loop execution, and task dispatch.
 *
 * @par Usage example
 * @code{.cpp}
 * int main(int argc, char** argv) {
 *     std::vector<std::string> args(argv, argv + argc);
 *     nbn::core::Application::init(args);
 *
 *     nbn::core::Application::runAsync([] {
 *         nbn::log::info("background work started");
 *         nbn::core::Application::quit(0);
 *     });
 *
 *     return nbn::core::Application::exec();
 * }
 * @endcode
 */
class nbn_export_core Application final : public Object {
   public:
    // Signals
    static auto signalInitialized() -> signal_ptr<>;
    static auto signalAboutToQuit() -> signal_ptr<>;
    static auto signalInterrupted() -> signal_ptr<int>;
    static auto signalUncaughtAsyncException() -> signal_ptr<>;
    static auto signalLogDebug() -> signal_ptr<std::string>;
    static auto signalLogInfo() -> signal_ptr<std::string>;
    static auto signalLogWarning() -> signal_ptr<std::string>;
    static auto signalLogError() -> signal_ptr<std::string>;
    static auto signalLogFatal() -> signal_ptr<std::string>;

    // Terminate handler
    static auto setTerminateHandler(std::function<void()> handler) -> void;

    // Initialization
    static void init(std::vector<std::string> args, std::string_view logFilePath = std::string());

    // Getters
    [[nodiscard]] static auto programName() -> const std::string&;
    [[nodiscard]] static auto programPath() -> const std::filesystem::path&;
    [[nodiscard]] static auto arguments() -> const std::vector<std::string>&;

    // Tasks processing
    static auto runSync(std::shared_ptr<Task> spTask) -> void;
    static auto runSync(Task::task_body_t taskBody, Task::RunMode mode = Task::RunMode::SingleShot)
        -> std::shared_ptr<interfaces::IRunnable>;

    static auto runAsync(std::shared_ptr<Task> spTask) -> void;
    static auto runAsync(Task::task_body_t taskBody, Task::RunMode mode = Task::RunMode::SingleShot)
        -> std::shared_ptr<interfaces::IRunnable>;

    // Execution
    [[nodiscard]] static auto exec() -> int;
    static auto quit(int exitCode = 0) -> void;
};

}  // namespace nbn::core

namespace nbn::log {

/**
 * @brief Writes a debug-level message.
 */
nbn_export_core void debug(std::string_view message, const std::source_location& location = std::source_location::current());
/**
 * @brief Writes an info-level message.
 */
nbn_export_core void info(std::string_view message, const std::source_location& location = std::source_location::current());
/**
 * @brief Writes a warning-level message.
 */
nbn_export_core void warning(std::string_view message, const std::source_location& location = std::source_location::current());
/**
 * @brief Writes an error-level message.
 */
nbn_export_core void error(std::string_view message, const std::source_location& location = std::source_location::current());
/**
 * @brief Writes a fatal-level message.
 */
[[noreturn]] nbn_export_core void fatal(std::string_view message,
                                        const std::source_location& location = std::source_location::current());

nbn_export_core void debug_if(bool isCondition,
                              std::string_view message,
                              const std::source_location& location = std::source_location::current());
nbn_export_core void info_if(bool isCondition,
                             std::string_view message,
                             const std::source_location& location = std::source_location::current());
nbn_export_core void warning_if(bool isCondition,
                                std::string_view message,
                                const std::source_location& location = std::source_location::current());
nbn_export_core void error_if(bool isCondition,
                              std::string_view message,
                              const std::source_location& location = std::source_location::current());
nbn_export_core void fatal_if(bool isCondition,
                              std::string_view message,
                              const std::source_location& location = std::source_location::current());

}  // namespace nbn::log
