#include <atomic>
#include <chrono>
#include <memory>
#include <stdexcept>
#include <thread>

#include <nbn/core/UnitTests.h>
#include <nbn/core/private/ApplicationMainThread.h>
#include <nbn/core/private/ApplicationThreadsManager.h>

namespace {

constexpr auto kExpectedTaskExecutions = 1;
constexpr auto kExpectedAsyncTaskExecutions = 1;
constexpr auto kStoppedTaskExecutions = 0;
constexpr int kShutdownStressRoundCount = 16;
constexpr auto kShutdownStressTimeout = std::chrono::seconds{2};

}  // namespace

void test_application_threads_manager_runs_main_tasks() {
    std::atomic<int> taskExecutions{0};
    nbn::core::ApplicationThreadsManager manager{};
    manager.submit(
        std::make_shared<nbn::core::Task>([&taskExecutions]() { ++taskExecutions; }, nbn::core::Task::RunMode::SingleShot));

    manager.run();
    manager.wait();

    nbn::core::unit_tests::equal("Application threads manager should run the main task", kExpectedTaskExecutions,
                                 taskExecutions.load());
}

void test_application_threads_manager_runs_async_threads() {
    std::atomic<int> taskExecutions{0};
    nbn::core::ApplicationThreadsManager manager{};
    auto thread =
        std::make_shared<nbn::core::Thread>([&taskExecutions]() { ++taskExecutions; }, nbn::core::Task::RunMode::SingleShot);
    manager.submit(std::move(thread));

    manager.run();
    manager.wait();

    nbn::core::unit_tests::equal("Application threads manager should run the asynchronous task", kExpectedAsyncTaskExecutions,
                                 taskExecutions.load());
}

void test_application_threads_manager_rethrows_main_thread_exception() {
    nbn::core::ApplicationThreadsManager manager{};
    manager.submit(std::make_shared<nbn::core::Task>([]() { throw std::runtime_error{"Application main task failure"}; },
                                                     nbn::core::Task::RunMode::SingleShot));

    manager.run();

    nbn::core::unit_tests::isThrowing<std::runtime_error>("Application threads manager should rethrow main task exceptions",
                                                          [&manager]() { manager.wait(); });
}

void test_application_threads_manager_rethrows_async_thread_exception() {
    nbn::core::ApplicationThreadsManager manager{};
    auto thread = std::make_shared<nbn::core::Thread>([]() { throw std::runtime_error{"Async thread failure"}; },
                                                      nbn::core::Task::RunMode::SingleShot);
    manager.submit(std::move(thread));

    manager.run();

    nbn::core::unit_tests::isThrowing<std::runtime_error>("Application threads manager should rethrow async thread exceptions",
                                                          [&manager]() { manager.wait(); });
}

void test_application_threads_manager_stops_pending_async_threads() {
    nbn::core::ApplicationThreadsManager manager{};
    auto thread = std::make_shared<nbn::core::Thread>([]() {}, nbn::core::Task::RunMode::Loop);
    manager.submit(std::move(thread));

    manager.stop();
    manager.wait();
}

void test_application_threads_manager_stops_pending_main_tasks() {
    std::atomic<int> taskExecutions{0};
    nbn::core::ApplicationThreadsManager manager{};
    manager.submit(
        std::make_shared<nbn::core::Task>([&taskExecutions]() { ++taskExecutions; }, nbn::core::Task::RunMode::SingleShot));

    manager.stop();
    manager.wait();

    nbn::core::unit_tests::equal("Stopping the manager should drain pending main tasks", kStoppedTaskExecutions,
                                 taskExecutions.load());
}

void test_application_main_thread_stops_pending_tasks() {
    std::atomic<int> taskExecutions{0};
    nbn::core::ApplicationMainThread mainThread{};
    mainThread.submit(
        std::make_shared<nbn::core::Task>([&taskExecutions]() { ++taskExecutions; }, nbn::core::Task::RunMode::SingleShot));

    mainThread.stop();
    mainThread.wait();

    nbn::core::unit_tests::equal("Stopping the main thread should drain pending tasks", kStoppedTaskExecutions,
                                 taskExecutions.load());
}

void test_application_main_thread_rethrows_task_exception() {
    nbn::core::ApplicationMainThread mainThread{};
    mainThread.submit(std::make_shared<nbn::core::Task>([]() { throw std::runtime_error{"Main task failure"}; },
                                                        nbn::core::Task::RunMode::SingleShot));
    mainThread.run();

    nbn::core::unit_tests::isThrowing<std::runtime_error>("Main thread should rethrow task exceptions",
                                                          [&mainThread]() { mainThread.wait(); });
}

void test_application_threads_manager_repeated_shutdown_stops_async_workers() {
    auto allRoundsPassed = true;
    for (int round = 0; round < kShutdownStressRoundCount; ++round) {
        std::atomic<bool> started{false};
        nbn::core::ApplicationThreadsManager manager{};
        auto thread = std::make_shared<nbn::core::Thread>(
            [&started]() {
                started.store(true, std::memory_order_release);
                std::this_thread::yield();
            },
            nbn::core::Task::RunMode::Loop);
        auto threadForVerification = thread;
        manager.submit(std::move(thread));
        const auto deadline = std::chrono::steady_clock::now() + kShutdownStressTimeout;
        while (!started.load(std::memory_order_acquire) && std::chrono::steady_clock::now() < deadline) {
            std::this_thread::yield();
        }
        allRoundsPassed = started.load(std::memory_order_acquire) && allRoundsPassed;
        manager.run();
        manager.stop();
        manager.wait();
        const auto workerStopped = threadForVerification->isStopRequested() && !threadForVerification->isRunning();
        allRoundsPassed = allRoundsPassed && workerStopped;
    }
    nbn::core::unit_tests::isTrue("Repeated manager shutdown should stop every async worker", allRoundsPassed);
}
