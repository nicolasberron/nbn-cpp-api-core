#pragma once

#include <source_location>

#include "ConcurrentQueue.h"
#include "Object.h"

namespace nbn::core {

class Logger;

#ifdef NBN_LOGGER_TEST_HOOK
namespace detail::logger_test {

using hook_t = void (*)(Logger*);

nbn_export_core auto setHook(hook_t callback) noexcept -> void;

}  // namespace detail::logger_test
#endif

/**
 * @file Logger.h
 * @brief Asynchronous logging API.
 */

/**
 * @class Logger
 * @brief Structured logger with level-specific signals.
 *
 * @par Usage example
 * @code{.cpp}
 * nbn::core::Logger logger;
 * logger.start("app.log");
 *
 * logger.info("main.cpp", 42, 1, "main", "application started");
 * logger.warning("main.cpp", 43, 1, "main", "demo warning");
 *
 * logger.stop();
 * @endcode
 */

class nbn_export_core Logger : public Object {
   public:
    Logger();
    ~Logger() override;

    // Define log levels
    enum class Level { DEBUG, INFO, WARNING, ERROR, FATAL };

    using LogSignal = Signal<std::string, uint_least32_t, uint_least32_t, std::string, std::string>;

    // Signals — full structured args
    auto signalDebug() -> std::shared_ptr<LogSignal> { return logSignal(); }
    auto signalInfo() -> std::shared_ptr<LogSignal> { return logSignal(); }
    auto signalWarning() -> std::shared_ptr<LogSignal> { return logSignal(); }
    auto signalError() -> std::shared_ptr<LogSignal> { return logSignal(); }
    auto signalFatal() -> std::shared_ptr<LogSignal> { return logSignal(); }

    // Signals — formatted log line text (same text written to file)
    auto signalDebugText() -> signal_ptr<std::string> { return signal<std::string>(); }
    auto signalInfoText() -> signal_ptr<std::string> { return signal<std::string>(); }
    auto signalWarningText() -> signal_ptr<std::string> { return signal<std::string>(); }
    auto signalErrorText() -> signal_ptr<std::string> { return signal<std::string>(); }
    auto signalFatalText() -> signal_ptr<std::string> { return signal<std::string>(); }

    // Logging
    void debug(std::string_view file,
               uint_least32_t line,
               uint_least32_t column,
               std::string_view function,
               std::string_view message);

    void info(std::string_view file,
              uint_least32_t line,
              uint_least32_t column,
              std::string_view function,
              std::string_view message);

    void warning(std::string_view file,
                 uint_least32_t line,
                 uint_least32_t column,
                 std::string_view function,
                 std::string_view message);

    void error(std::string_view file,
               uint_least32_t line,
               uint_least32_t column,
               std::string_view function,
               std::string_view message);

    void fatal(std::string_view file,
               uint_least32_t line,
               uint_least32_t column,
               std::string_view function,
               std::string_view message);

    void start(std::string_view logFilePath = "");

    void stop();

#ifdef NBN_LOGGER_TEST_HOOK
    void requestStopForTest();
    void enqueueForTest(std::string message);
    [[nodiscard]] auto popForTest(std::string& message) -> bool;
#endif

   private:
    class Impl;
    std::unique_ptr<Impl> m_spImpl{};

    auto logSignal(const std::source_location& location = std::source_location::current()) -> std::shared_ptr<LogSignal> {
        return signal<std::string, uint_least32_t, uint_least32_t, std::string, std::string>(location);
    }
};

}  // namespace nbn::core
