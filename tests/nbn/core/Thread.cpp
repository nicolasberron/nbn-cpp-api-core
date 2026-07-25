#include <atomic>
#include <chrono>
#include <future>
#include <memory>
#include <stdexcept>
#include <thread>

#include <nbn/core/Task.h>
#include <nbn/core/UnitTests.h>
#include <nbn/core/private/Thread.h>

namespace {

constexpr auto kExpectedExecutions = 1;
constexpr int kShutdownStressRoundCount = 32;
constexpr auto kShutdownStressTimeout = std::chrono::seconds{2};

}  // namespace

void test_thread_runs_a_shared_task() {
    std::atomic<int> executions{0};
    const auto task = std::make_shared<nbn::core::Task>([&executions]() { ++executions; }, nbn::core::Task::RunMode::SingleShot);
    nbn::core::Thread thread{task};

    thread.run(task);
    thread.wait();

    nbn::core::unit_tests::equal("Thread should run a task supplied through shared ownership", kExpectedExecutions,
                                 executions.load());
    nbn::core::unit_tests::isFalse("Thread should finish after the shared task completes", thread.isRunning());
}

void test_thread_detaches_from_its_own_worker() {
    std::promise<void> detached;
    auto detachedFuture = detached.get_future();
    std::promise<void> releaseWorker;
    auto releaseWorkerFuture = releaseWorker.get_future();
    std::promise<void> workerFinished;
    auto workerFinishedFuture = workerFinished.get_future();

    {
        nbn::core::Thread thread{};

        thread.run(
            [&thread, &detached, &releaseWorkerFuture, &workerFinished]() {
                nbn::core::unit_tests::isTrue("Thread should identify its own worker", thread.isCurrentThread());
                thread.detach();
                detached.set_value();
                releaseWorkerFuture.wait();
                workerFinished.set_value();
            },
            nbn::core::Task::RunMode::SingleShot);

        detachedFuture.wait();
        nbn::core::unit_tests::isFalse("Detached thread should no longer be running", thread.isRunning());
    }

    releaseWorker.set_value();
    workerFinishedFuture.wait();
}

void test_thread_discards_exceptions_after_detaching() {
    std::promise<void> detached;
    auto detachedFuture = detached.get_future();
    std::promise<void> taskFinished;
    auto taskFinishedFuture = taskFinished.get_future();

    nbn::core::Thread thread{};
    const auto task = std::make_shared<nbn::core::Task>(
        [&thread, &detached]() {
            thread.detach();
            detached.set_value();
            throw std::runtime_error{"detached task failure"};
        },
        nbn::core::Task::RunMode::SingleShot);
    task->signalStopped()->connect([&taskFinished]() { taskFinished.set_value(); });

    thread.run(task);
    detachedFuture.wait();
    taskFinishedFuture.wait();

    nbn::core::unit_tests::isFalse("Detached thread should not remain running after an exception", thread.isRunning());
}

void test_thread_repeated_cancellation_and_join_are_safe() {
    auto allRoundsPassed = true;
    for (int round = 0; round < kShutdownStressRoundCount; ++round) {
        std::atomic<bool> started{false};
        nbn::core::Thread thread{[&started]() {
                                     started.store(true, std::memory_order_release);
                                     std::this_thread::yield();
                                 },
                                 nbn::core::Task::RunMode::Loop};
        thread.run();
        const auto deadline = std::chrono::steady_clock::now() + kShutdownStressTimeout;
        while (!started.load(std::memory_order_acquire) && std::chrono::steady_clock::now() < deadline) {
            std::this_thread::yield();
        }
        if (!started.load(std::memory_order_acquire)) {
            allRoundsPassed = false;
            thread.requestStop();
            thread.wait();
            continue;
        }
        thread.requestStop();
        thread.wait();
        allRoundsPassed = allRoundsPassed && thread.isStopRequested() && !thread.isRunning();
    }
    nbn::core::unit_tests::isTrue("Repeated thread cancellation should finish every worker", allRoundsPassed);
}
