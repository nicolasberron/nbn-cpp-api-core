#include <nbn/core/Application.h>
#include <nbn/core/Logger.h>
#include <nbn/core/UnitTests.h>

#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

// Test Logger

using namespace nbn::core;

namespace {

struct LoggerIdleState {
    std::condition_variable m_condition;
    std::mutex m_mutex;
    bool m_reached{false};
    bool m_release{false};
    unsigned int m_activeHooks{0};
    nbn::core::Logger* m_logger{nullptr};
};

struct LoggerQueueState {
    std::condition_variable condition;
    std::mutex mutex;
    bool nodeLinked{false};
    bool releasePush{false};
    bool pauseNextPush{false};
};

auto loggerIdleState() -> LoggerIdleState& {
    static LoggerIdleState state{};
    return state;
}

auto loggerQueueState() -> LoggerQueueState& {
    static LoggerQueueState state{};
    return state;
}

void pauseLoggerQueuePush() {
    auto& state = loggerQueueState();
    std::unique_lock lock{state.mutex};
    if (!state.pauseNextPush) {
        return;
    }
    state.pauseNextPush = false;
    state.nodeLinked = true;
    state.condition.notify_all();
    state.condition.wait(lock, [&state] { return state.releasePush; });
}
void pauseLoggerAtStopCheck(nbn::core::Logger* invokingLogger) {
    auto& state = loggerIdleState();
    nbn::core::Logger* targetLogger{nullptr};
    {
        std::lock_guard lock{state.m_mutex};
        if (invokingLogger != state.m_logger) {
            return;
        }
        targetLogger = state.m_logger;
        ++state.m_activeHooks;
        state.m_reached = true;
        state.m_condition.notify_one();
    }
    targetLogger->requestStopForTest();

    std::unique_lock lock{state.m_mutex};
    state.m_condition.wait(lock, [&state] { return state.m_release; });
    --state.m_activeHooks;
    state.m_condition.notify_all();
}

}  // namespace

void test_logger() {
    nbn::log::debug("This is a debug message");
    nbn::log::info("This is an info message");
    nbn::log::warning("This is a warning message");
    nbn::log::error("This is an error message");
}

void test_logger_concurrent_producers() {
    constexpr int kProducerCount{4};
    constexpr int kMessagesPerProducer{250};
    nbn::core::Logger logger{};

    std::vector<std::thread> producers;
    producers.reserve(kProducerCount);
    for (int producer{0}; producer < kProducerCount; ++producer) {
        producers.emplace_back([&logger, producer]() {
            for (int message{0}; message < kMessagesPerProducer; ++message) {
                logger.enqueueForTest(std::to_string(producer) + ":" + std::to_string(message));
            }
        });
    }

    for (auto& producer : producers) {
        producer.join();
    }
    int received{0};
    std::string message;
    while (logger.popForTest(message)) {
        ++received;
    }
    nbn::core::unit_tests::equal("Logger should accept every concurrent producer message", kProducerCount * kMessagesPerProducer,
                                 received);
}

void test_logger_emits_structured_and_formatted_messages() {
    nbn::core::Logger logger{};
    auto debugMessage = std::string{};
    auto structuredMessage = std::string{};
    auto errorMessage = std::string{};
    auto formattedMessage = std::string{};
    logger.signalDebug()->connect([&debugMessage](std::string_view, uint_least32_t, uint_least32_t, std::string_view,
                                                  std::string_view message) { debugMessage = message; });
    logger.signalInfo()->connect([&structuredMessage](std::string_view, uint_least32_t, uint_least32_t, std::string_view,
                                                      std::string_view message) { structuredMessage = message; });
    logger.signalError()->connect([&errorMessage](std::string_view, uint_least32_t, uint_least32_t, std::string_view,
                                                  std::string_view message) { errorMessage = message; });
    logger.signalInfoText()->connect([&formattedMessage](const std::string& message) { formattedMessage = message; });

    logger.start();
    logger.debug("logger-test.cpp", 1U, 2U, "test_logger", "debug message");
    logger.info("logger-test.cpp", 1U, 2U, "test_logger", "structured message");
    logger.error("logger-test.cpp", 1U, 2U, "test_logger", "error message");
    logger.stop();

    nbn::core::unit_tests::equal("Logger should emit the debug message", std::string{"debug message"}, debugMessage);
    nbn::core::unit_tests::equal("Logger should emit the structured message", std::string{"structured message"}, structuredMessage);
    nbn::core::unit_tests::equal("Logger should emit the error message", std::string{"error message"}, errorMessage);
    nbn::core::unit_tests::isTrue("Logger should emit a formatted message", formattedMessage.find("INFO:") != std::string::npos);
    nbn::core::unit_tests::isTrue("Formatted message should include the source file",
                                  formattedMessage.find("logger-test.cpp") != std::string::npos);
}

void test_logger_writes_messages_to_a_file_and_fatal_rethrows() {
    const auto path = std::filesystem::temp_directory_path() / "nbn-logger-coverage.log";
    nbn::core::Logger logger{};
    logger.start(path.string());
    logger.warning("logger-test.cpp", 3U, 4U, "test_logger", "file message");
    logger.stop();

    std::ifstream stream(path);
    const std::string contents{std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
    nbn::core::unit_tests::isTrue("Logger should write messages to the configured file",
                                  contents.find("file message") != std::string::npos);
    constexpr auto kFiveU = 5U;
    constexpr auto kSixU = 6U;
    nbn::core::unit_tests::isThrowing<std::runtime_error>("Logger fatal should rethrow the message", [&logger]() {
        logger.fatal("logger-test.cpp", kFiveU, kSixU, "test_logger", "fatal message");
    });

    std::error_code error;
    std::filesystem::remove(path, error);
}

// Regression test for a worker calling stop() while its std::jthread member is
// still being assigned. The worker must not mistake the not-yet-assigned member
// for another thread and attempt to join itself.
void test_logger_worker_stop_request_does_not_self_join() {
    nbn::core::Logger logger{};
    auto& state = loggerIdleState();
    {
        std::lock_guard lock{state.m_mutex};
        state.m_reached = false;
        state.m_release = false;
        state.m_activeHooks = 0;
        state.m_logger = &logger;
    }
    nbn::core::detail::logger_test::setHook(&pauseLoggerAtStopCheck);
    logger.start();

    bool reached{false};
    {
        std::unique_lock lock{state.m_mutex};
        reached = state.m_condition.wait_for(lock, std::chrono::seconds{1}, [&state] { return state.m_reached; });
    }

    {
        std::lock_guard lock{state.m_mutex};
        state.m_release = true;
    }
    state.m_condition.notify_one();
    logger.stop();
    nbn::core::detail::logger_test::setHook(nullptr);
    {
        std::unique_lock lock{state.m_mutex};
        state.m_condition.wait(lock, [&state] { return state.m_activeHooks == 0; });
    }
    nbn::core::unit_tests::isTrue("Logger worker should reach the stop-check hook", reached);
}

void test_logger_queue_covers_stale_tail_paths() {
    auto& state = loggerQueueState();
    {
        std::lock_guard lock{state.mutex};
        state.nodeLinked = false;
        state.releasePush = false;
        state.pauseNextPush = true;
    }

    nbn::core::detail::concurrent_queue_test::setHook(&pauseLoggerQueuePush);

    nbn::core::Logger logger{};
    logger.start();
    std::thread firstProducer{[&logger]() { logger.enqueueForTest("first message"); }};

    {
        std::unique_lock lock{state.mutex};
        state.condition.wait(lock, [&state] { return state.nodeLinked; });
    }

    logger.enqueueForTest("second message");
    std::string consumedMessage;
    const auto consumed = logger.popForTest(consumedMessage);
    nbn::core::unit_tests::isTrue("Logger should pop the first queued message while the producer tail is stale", consumed);

    {
        std::unique_lock lock{state.mutex};
        state.releasePush = true;
    }
    state.condition.notify_all();
    firstProducer.join();
    logger.stop();
    nbn::core::unit_tests::isFalse("Logger test queue should be empty after stop", logger.popForTest(consumedMessage));

    nbn::core::detail::concurrent_queue_test::setHook(nullptr);
}
