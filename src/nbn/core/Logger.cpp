#include <array>
#include <chrono>
#include <ctime>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <utility>

#if defined(PLATFORMIO_BUILD)
#include <Arduino.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#endif

#include "Application.h"
#include "Logger.h"
#include "private/Application.h"
#include "private/Thread.h"
#include "private/format_bridge.h"

namespace nbn::core {

namespace {

// std::localtime uses shared C runtime storage. Keep the portable API but serialize
// calls made by logger instances so concurrent logging cannot race through that state.
auto localTimeMutex() noexcept -> std::mutex& {
    static std::mutex mutex{};
    return mutex;
}

}  // namespace

#ifdef NBN_LOGGER_TEST_HOOK
namespace detail::logger_test {

auto hookStorage() noexcept -> std::atomic<hook_t>& {
    static std::atomic<hook_t> hook{nullptr};
    return hook;
}

auto setHook(hook_t callback) noexcept -> void {
    hookStorage().store(callback, std::memory_order_release);
}

auto invokeHook(Logger* logger) noexcept -> void {
    const auto callback = hookStorage().load(std::memory_order_acquire);
    if (callback != nullptr) {
        callback(logger);
    }
}

}  // namespace detail::logger_test
#endif

// Factory registration
static auto const registerToFactory{[]() { return Object::factoryRegister<Logger>(); }()};

// Helper function to convert log level to string
[[nodiscard]] auto toString(Logger::Level level) -> std::string_view {
    switch (level) {
        case Logger::Level::DEBUG:
            return "DEBUG:  ";
        case Logger::Level::INFO:
            return "INFO:   ";
        case Logger::Level::WARNING:
            return "WARNING:";
        case Logger::Level::ERROR:
            return "ERROR:  ";
        case Logger::Level::FATAL:
        default:
            return "FATAL:  ";
    }
}

// Define logger private implementation

class Logger::Impl : public Thread {
   public:
    struct Message {
        Logger::Level m_level{Logger::Level::INFO};
        std::string m_text;
    };

    // Constructor
    explicit Impl(Logger* pDecl) : Thread([this]() { writeLoop(); }, Task::RunMode::Loop), m_pDecl{pDecl} {}

    // Destructor
    ~Impl() override { stop(); }

    // Disable copy and move
    Impl(const Impl&) noexcept = delete;
    auto operator=(const Impl&) noexcept -> Impl& = delete;
    Impl(Impl&&) noexcept = delete;
    auto operator=(Impl&&) noexcept -> Impl& = delete;

    // Stop logging thread and flush queue
    void stop() override {
#if defined(PLATFORMIO_BUILD)
        flush();
        if (m_outputStream.is_open()) {
            m_outputStream.close();
        }
#else
        Thread::stop();
        Thread::wait();
        flush();
        if (m_outputStream.is_open()) {
            m_outputStream.close();
        }
#endif
    }

    // Flush queue
    void flush() {
        Message message{};
        while (true) {
            if (!m_queue.pop(message)) {
                break;
            }
            writeToFile(message.m_text, message.m_level);
        }
    }

#ifdef NBN_LOGGER_TEST_HOOK
    void enqueueForTest(std::string message) { m_queue.push(Message{Logger::Level::INFO, std::move(message)}); }
    auto popForTest(std::string& message) -> bool {
        Message queuedMessage{};
        if (!m_queue.pop(queuedMessage)) {
            return false;
        }
        message = std::move(queuedMessage.m_text);
        return true;
    }
#endif

    [[nodiscard]] static auto threadId() -> std::string {
        std::stringstream ss;

#if defined(_WIN32) || defined(_WIN64)
        // Windows implementation
        ss << GetCurrentThreadId();
#elif defined(PLATFORMIO_BUILD)
        // Avoid pthread-dependent thread-id APIs on embedded targets.
        ss << "embedded";
#elif defined(__linux__)
        ss << ::gettid();
#elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
        ss << std::this_thread::get_id();
#else
#error "Unsupported platform"
#endif

        return ss.str();
    }

    // Log message with given level, file, line and message
    void log(Logger::Level level,
             std::string_view file,
             uint_least32_t line,
             uint_least32_t column,
             std::string_view function,
             std::string_view message) {
#if defined(PLATFORMIO_BUILD)
        if (!isStopRequested()) {
#else
        if (isRunning()) {
#endif
            auto levelString = toString(level);
            std::string timeString;

#if defined(PLATFORMIO_BUILD)
            const unsigned long uptimeMs = millis();
            const auto uptimeSeconds = uptimeMs / 1000UL;
            const auto uptimeRemainderMs = uptimeMs % 1000UL;
            std::ostringstream uptimeStream;
            uptimeStream << "t+" << uptimeSeconds << "." << std::setw(3) << std::setfill('0') << uptimeRemainderMs << "s";
            timeString = uptimeStream.str();
#else
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            const auto oneThousands = std::chrono::milliseconds(1000);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % oneThousands;
            std::lock_guard<std::mutex> localTimeLock(localTimeMutex());
            std::ostringstream oss;
            oss << std::put_time(std::localtime(&time), "%Y:%m:%d-%H:%M:%S");
            timeString = oss.str() + "." + std::to_string(ms.count());
#endif

            auto message_with_info = std::format("| {} | {} | {} | {} | {} | {}:{}:{} | {} |", Application::programName(),
                                                 levelString, timeString, threadId(), message, file, line, column, function);

            // Emit formatted text signal synchronously before queuing
            if (m_pDecl != nullptr) {
                switch (level) {
                    case Logger::Level::DEBUG:
                        m_pDecl->signalDebugText()->emit(message_with_info);
                        break;
                    case Logger::Level::INFO:
                        m_pDecl->signalInfoText()->emit(message_with_info);
                        break;
                    case Logger::Level::WARNING:
                        m_pDecl->signalWarningText()->emit(message_with_info);
                        break;
                    case Logger::Level::ERROR:
                        m_pDecl->signalErrorText()->emit(message_with_info);
                        break;
                    case Logger::Level::FATAL:
                        m_pDecl->signalFatalText()->emit(message_with_info);
                        break;
                }
            }

#if defined(PLATFORMIO_BUILD)
            writeToFile(message_with_info, level);
#else
            m_queue.push(Message{level, std::move(message_with_info)});
#endif
        }
    }

    // Explicitly start logging thread
    void start(std::string_view logFilePath) {
        nbn::log::fatal_if(isRunning(), "Logger thread already running");

        if (!logFilePath.empty()) {
            m_outputStream.open(std::string(logFilePath), std::ios::out | std::ios::app);
            nbn::log::fatal_if(!m_outputStream.is_open(), std::format("Failed to open log file: {}", logFilePath));
            m_isWritingToFile = true;
        }

#if defined(PLATFORMIO_BUILD)
        return;
#else
        run();
#endif
    }

   private:
    void writeLoop() {
        Message message{};
        if (m_queue.pop(message)) {
            writeToFile(message.m_text, message.m_level);
        } else {
#ifdef NBN_LOGGER_TEST_HOOK
            detail::logger_test::invokeHook(m_pDecl);
#endif
            if (isStopRequested()) {
                return;
            }
            // Idle: sleep briefly instead of busy-spinning. A bare std::this_thread::yield()
            // re-runs this loop body immediately and pins a CPU core at 100% while no logs are queued.
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    // Write message to file or console
    void writeToFile(std::string_view message, Logger::Level level) {
        m_logLevel = level;
        if (m_isWritingToFile) {
            std::unique_lock<std::mutex> lock(m_writeMutex);
            if (m_outputStream.is_open()) {
                m_outputStream << message << std::endl;
            }
        } else {
#if defined(PLATFORMIO_BUILD)
            Serial.println(std::string(message).c_str());
#else
            auto& stream = m_logLevel == Logger::Level::ERROR ? std::cerr : std::cout;
            stream << message << std::endl;
            stream.flush();
#endif
        }
    }

    ConcurrentQueue<Message> m_queue;
    std::mutex m_writeMutex;
    bool m_isWritingToFile{false};
    std::ofstream m_outputStream;
    Logger::Level m_logLevel{Logger::Level::INFO};
    Logger* m_pDecl{nullptr};
};

// Define logger class implementation

Logger::Logger() : m_spImpl{std::make_unique<Logger::Impl>(this)} {
    signalDebug()->connect(
        [this](std::string_view file, uint_least32_t line, uint_least32_t column, std::string_view function,
               std::string_view message) { m_spImpl->log(Level::DEBUG, std::move(file), line, column, function, message); });

    signalInfo()->connect(
        [this](std::string_view file, uint_least32_t line, uint_least32_t column, std::string_view function,
               std::string_view message) { m_spImpl->log(Level::INFO, std::move(file), line, column, function, message); });

    signalWarning()->connect(
        [this](std::string_view file, uint_least32_t line, uint_least32_t column, std::string_view function,
               std::string_view message) { m_spImpl->log(Level::WARNING, std::move(file), line, column, function, message); });

    signalError()->connect(
        [this](std::string_view file, uint_least32_t line, uint_least32_t column, std::string_view function,
               std::string_view message) { m_spImpl->log(Level::ERROR, std::move(file), line, column, function, message); });

    signalFatal()->connect(
        [this](std::string_view file, uint_least32_t line, uint_least32_t column, std::string_view function,
               std::string_view message) { m_spImpl->log(Level::FATAL, std::move(file), line, column, function, message); });
}

Logger::~Logger() {
    m_spImpl->stop();
}

#ifdef NBN_LOGGER_TEST_HOOK
void Logger::requestStopForTest() {
    m_spImpl->Thread::requestStop();
}

void Logger::enqueueForTest(std::string message) {
    m_spImpl->enqueueForTest(std::move(message));
}

auto Logger::popForTest(std::string& message) -> bool {
    return m_spImpl->popForTest(message);
}
#endif

void Logger::debug(std::string_view file,
                   uint_least32_t line,
                   uint_least32_t column,
                   std::string_view function,
                   std::string_view message) {
    if (m_spImpl != nullptr && !m_spImpl->isStopRequested()) {
        signalDebug()->emit(std::string(file), line, column, std::string(function), std::string(message));
    }
}

void Logger::info(std::string_view file,
                  uint_least32_t line,
                  uint_least32_t column,
                  std::string_view function,
                  std::string_view message) {
    if (m_spImpl != nullptr && !m_spImpl->isStopRequested()) {
        signalInfo()->emit(std::string(file), line, column, std::string(function), std::string(message));
    }
}

void Logger::warning(std::string_view file,
                     uint_least32_t line,
                     uint_least32_t column,
                     std::string_view function,
                     std::string_view message) {
    if (m_spImpl != nullptr && !m_spImpl->isStopRequested()) {
        signalWarning()->emit(std::string(file), line, column, std::string(function), std::string(message));
    }
}

void Logger::error(std::string_view file,
                   uint_least32_t line,
                   uint_least32_t column,
                   std::string_view function,
                   std::string_view message) {
    if (m_spImpl != nullptr && !m_spImpl->isStopRequested()) {
        signalError()->emit(std::string(file), line, column, std::string(function), std::string(message));
    }
}

void Logger::fatal(std::string_view file,
                   uint_least32_t line,
                   uint_least32_t column,
                   std::string_view function,
                   std::string_view message) {
    if (m_spImpl != nullptr && !m_spImpl->isStopRequested()) {
        signalFatal()->emit(std::string(file), line, column, std::string(function), std::string(message));
    }
    throw std::runtime_error(std::string(message));
}

void Logger::start(std::string_view logFilePath) {
    if (!m_spImpl->isRunning()) {
        m_spImpl->start(logFilePath);
    }
}

void Logger::stop() {
    m_spImpl->stop();
}

}  // namespace nbn::core
