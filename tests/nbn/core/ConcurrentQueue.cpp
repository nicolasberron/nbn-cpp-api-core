#include <atomic>
#include <barrier>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include <nbn/core/ConcurrentQueue.h>
#include <nbn/core/ElapsedTimer.h>
#include <nbn/core/Task.h>
#include <nbn/core/UnitTests.h>
#include <nbn/core/private/ApplicationMainThread.h>
#include <nbn/core/private/ApplicationThreadsManager.h>
#include <nbn/core/private/Thread.h>

using namespace nbn::core;

namespace {

constexpr int kFirstQueuedValue = 1;
constexpr int kSecondQueuedValue = 2;
constexpr std::ptrdiff_t kStressProducerCount = 4;
constexpr std::ptrdiff_t kStressConsumerCount = 4;
constexpr int kStressItemsPerProducer = 2500;
constexpr int kStressRoundCount = 3;
constexpr auto kStressTimeout = std::chrono::seconds{5};

auto queueHookCondition() -> std::condition_variable& {
    static std::condition_variable condition;
    return condition;
}

auto queueHookMutex() -> std::mutex& {
    static std::mutex mutex;
    return mutex;
}

auto queueNodeLinked() -> bool& {
    static bool linked{false};
    return linked;
}

auto queueHookReleased() -> bool& {
    static bool released{false};
    return released;
}

auto queuePauseNextPush() -> bool& {
    static bool pause{false};
    return pause;
}

void pauseAfterQueueNodeLinked() {
    std::unique_lock lock(queueHookMutex());
    if (!queuePauseNextPush()) {
        return;
    }
    queuePauseNextPush() = false;
    queueNodeLinked() = true;
    queueHookCondition().notify_all();
    queueHookCondition().wait(lock, [] { return queueHookReleased(); });
}

template <typename Value, typename PushValue, typename ReadValue>
auto runQueueStress(PushValue pushValue, ReadValue readValue) -> bool {
    const auto itemCount = static_cast<std::size_t>(kStressProducerCount * kStressItemsPerProducer);
    ConcurrentQueue<Value> queue;
    std::vector<std::atomic<int>> seen(itemCount);
    for (auto& count : seen) {
        count.store(0);
    }
    std::atomic<std::size_t> consumed{0};
    std::atomic<bool> invalidValue{false};
    const auto participantCount = kStressProducerCount + kStressConsumerCount + 1;
    const auto deadline = std::chrono::steady_clock::now() + kStressTimeout;
    std::barrier startGate{participantCount};
    std::vector<std::thread> workers;
    workers.reserve(static_cast<std::size_t>(participantCount - 1));

    for (std::ptrdiff_t producer = 0; producer < kStressProducerCount; ++producer) {
        workers.emplace_back([&queue, &startGate, pushValue, producer]() {
            startGate.arrive_and_wait();
            for (int item = 0; item < kStressItemsPerProducer; ++item) {
                pushValue(queue, static_cast<std::size_t>(producer * kStressItemsPerProducer + item));
            }
        });
    }

    for (std::ptrdiff_t consumer = 0; consumer < kStressConsumerCount; ++consumer) {
        workers.emplace_back([&queue, &startGate, &consumed, &seen, &invalidValue, readValue, deadline]() {
            startGate.arrive_and_wait();
            while (consumed.load(std::memory_order_acquire) < seen.size() && std::chrono::steady_clock::now() < deadline) {
                Value value{};
                if (!queue.pop(value)) {
                    std::this_thread::yield();
                    continue;
                }
                const auto item = readValue(value);
                if (item >= seen.size()) {
                    invalidValue.store(true, std::memory_order_release);
                    continue;
                }
                seen[item].fetch_add(1, std::memory_order_acq_rel);
                consumed.fetch_add(1, std::memory_order_acq_rel);
            }
            if (consumed.load(std::memory_order_acquire) < seen.size()) {
                invalidValue.store(true, std::memory_order_release);
            }
        });
    }

    startGate.arrive_and_wait();
    for (auto& worker : workers) {
        worker.join();
    }

    if (invalidValue.load(std::memory_order_acquire) || consumed.load(std::memory_order_acquire) != itemCount) {
        return false;
    }
    for (const auto& count : seen) {
        if (count.load(std::memory_order_acquire) != 1) {
            return false;
        }
    }
    return true;
}

}  // namespace

void test_concurrent_queue_push_pop() {
    nbn::core::ConcurrentQueue<int> queue;
    queue.push(1);
    queue.push(2);
    queue.push(3);

    int value{0};
    bool isResult = queue.pop(value);
    unit_tests::isTrue("First value should be 1", isResult && value == 1);

    isResult = queue.pop(value);
    unit_tests::isTrue("Second value should be 2", isResult && value == 2);

    isResult = queue.pop(value);
    unit_tests::isTrue("Third value should be 3", isResult && value == 3);

    isResult = queue.pop(value);
    unit_tests::isFalse("Queue should be empty", isResult);
}

void test_concurrent_queue_destructor_drains_pending_values() {
    constexpr int firstValue = 1;
    constexpr int secondValue = 2;
    constexpr int thirdValue = 3;

    {
        nbn::core::ConcurrentQueue<int> queue;
        queue.push(firstValue);
        queue.push(secondValue);
        queue.push(thirdValue);
    }
}

void test_concurrent_queue_multiple_threads() {
    constexpr int queueSize = 10000;
    nbn::core::ConcurrentQueue<int> queue;
    std::vector<std::thread> pushers;
    std::condition_variable cv;
    std::mutex mtx;
    int ready_threads = 0;

    for (int i = 0; i < 4; ++i) {
        pushers.emplace_back([&queue, &cv, &mtx, &ready_threads, i]() {
            for (int j = 0; j < queueSize; ++j) {
                queue.push(i * queueSize + j);
            }
            std::unique_lock<std::mutex> lock(mtx);
            ready_threads++;
            cv.notify_all();
        });
    }

    std::thread popper([&queue, &cv, &mtx, &ready_threads]() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&ready_threads] { return ready_threads == 4; });
        int count = 0;
        int value{0};
        while (queue.pop(value)) {
            ++count;
        }
        constexpr int expectedCount = 4 * queueSize;
        unit_tests::equal("Popper should have popped 40000 values", expectedCount, count);
    });

    // Wait for all threads to finish
    for (auto& thread : pushers) {
        thread.join();
    }
    popper.join();
}

void test_concurrent_queue_high_contention_preserves_unique_values() {
    auto pushValue = [](ConcurrentQueue<int>& queue, std::size_t item) { queue.push(static_cast<int>(item)); };
    auto readValue = [](const int value) { return static_cast<std::size_t>(value); };
    auto allRoundsPassed = true;
    for (int round = 0; round < kStressRoundCount; ++round) {
        const auto roundPassed = runQueueStress<int>(pushValue, readValue);
        allRoundsPassed = roundPassed && allRoundsPassed;
    }
    unit_tests::isTrue("Queue stress should preserve every unique value", allRoundsPassed);
}

void test_concurrent_queue_high_contention_preserves_unique_move_only_values() {
    auto pushValue = [](ConcurrentQueue<std::unique_ptr<int>>& queue, std::size_t item) {
        queue.push(std::make_unique<int>(static_cast<int>(item)));
    };
    auto readValue = [](const std::unique_ptr<int>& value) { return static_cast<std::size_t>(*value); };
    auto allRoundsPassed = true;
    for (int round = 0; round < kStressRoundCount; ++round) {
        const auto roundPassed = runQueueStress<std::unique_ptr<int>>(pushValue, readValue);
        allRoundsPassed = roundPassed && allRoundsPassed;
    }
    unit_tests::isTrue("Queue stress should preserve every unique move-only value", allRoundsPassed);
}

void test_unique_ptr() {
    nbn::core::ConcurrentQueue<std::unique_ptr<int>> queue;
    queue.push(std::make_unique<int>(1));
    queue.push(std::make_unique<int>(2));
    queue.push(std::make_unique<int>(3));

    std::unique_ptr<int> value;
    bool isResult = queue.pop(value);
    unit_tests::isTrue("First value should be 1", isResult && *value == 1);

    isResult = queue.pop(value);
    unit_tests::isTrue("Second value should be 2", isResult && *value == 2);

    isResult = queue.pop(value);
    unit_tests::isTrue("Third value should be 3", isResult && *value == 3);

    isResult = queue.pop(value);
    unit_tests::isFalse("Queue should be empty", isResult);
}

void test_shared_ptr_destructor_drains_pending_values() {
    {
        nbn::core::ConcurrentQueue<std::shared_ptr<nbn::core::Task>> queue;
        queue.push(std::make_shared<nbn::core::Task>([] {}, nbn::core::Task::RunMode::SingleShot));
        queue.push(std::make_shared<nbn::core::Task>([] {}, nbn::core::Task::RunMode::SingleShot));
    }
}

void test_other_queue_destructors_drain_pending_values() {
    {
        nbn::core::ConcurrentQueue<std::string> queue;
        queue.push("pending string");
    }

    {
        nbn::core::ConcurrentQueue<std::shared_ptr<nbn::core::interfaces::IRunnable>> queue;
        std::shared_ptr<nbn::core::interfaces::IRunnable> runnable{std::make_shared<nbn::core::Thread>()};
        queue.push(std::move(runnable));
    }
}

void test_concurrent_queue_consumer_advances_stale_tail() {
    queueNodeLinked() = false;
    queueHookReleased() = false;
    queuePauseNextPush() = true;
    nbn::core::detail::concurrent_queue_test::setHook(&pauseAfterQueueNodeLinked);

    nbn::core::ConcurrentQueue<int> queue;
    std::thread producer{[&queue]() { queue.push(1); }};

    {
        std::unique_lock lock(queueHookMutex());
        queueHookCondition().wait(lock, [] { return queueNodeLinked(); });
    }

    int value{0};
    const auto popped = queue.pop(value);
    unit_tests::isTrue("Consumer should pop the node while the producer tail is stale", popped);
    unit_tests::equal("Consumer should receive the linked value", 1, value);

    {
        std::lock_guard lock(queueHookMutex());
        queueHookReleased() = true;
    }
    queueHookCondition().notify_all();
    producer.join();

    nbn::core::detail::concurrent_queue_test::setHook(nullptr);
}

void test_concurrent_queue_string_consumer_advances_stale_tail() {
    queueNodeLinked() = false;
    queueHookReleased() = false;
    queuePauseNextPush() = true;
    nbn::core::detail::concurrent_queue_test::setHook(&pauseAfterQueueNodeLinked);

    nbn::core::ConcurrentQueue<std::string> queue;
    std::thread producer{[&queue]() { queue.push("queued value"); }};

    {
        std::unique_lock lock(queueHookMutex());
        queueHookCondition().wait(lock, [] { return queueNodeLinked(); });
    }

    std::string value{};
    const auto popped = queue.pop(value);
    unit_tests::isTrue("String consumer should pop while the producer tail is stale", popped);
    unit_tests::equal("String consumer should receive the linked value", std::string{"queued value"}, value);

    {
        std::lock_guard lock(queueHookMutex());
        queueHookReleased() = true;
    }
    queueHookCondition().notify_all();
    producer.join();

    nbn::core::detail::concurrent_queue_test::setHook(nullptr);
}

void test_concurrent_queue_production_instantiation_advances_stale_tail() {
    queueNodeLinked() = false;
    queueHookReleased() = false;
    queuePauseNextPush() = true;
    nbn::core::detail::concurrent_queue_test::setHook(&pauseAfterQueueNodeLinked);

    nbn::core::ApplicationMainThread mainThread{};

    std::atomic<int> taskExecutions{0};
    auto firstTask =
        std::make_shared<nbn::core::Task>([&taskExecutions]() { ++taskExecutions; }, nbn::core::Task::RunMode::SingleShot);
    auto secondTask =
        std::make_shared<nbn::core::Task>([&taskExecutions]() { ++taskExecutions; }, nbn::core::Task::RunMode::SingleShot);

    std::thread firstProducer{[&mainThread, task = std::move(firstTask)]() mutable { mainThread.submit(std::move(task)); }};
    {
        std::unique_lock lock(queueHookMutex());
        queueHookCondition().wait(lock, [] { return queueNodeLinked(); });
    }

    mainThread.run();
    mainThread.submit(std::move(secondTask));
    {
        std::lock_guard lock(queueHookMutex());
        queueHookReleased() = true;
    }
    queueHookCondition().notify_all();
    firstProducer.join();

    mainThread.wait();
    nbn::core::detail::concurrent_queue_test::setHook(nullptr);

    unit_tests::equal("Production queue tasks should execute", 2, taskExecutions.load());
}

void test_concurrent_queue_production_producer_advances_stale_tail() {
    queueNodeLinked() = false;
    queueHookReleased() = false;
    queuePauseNextPush() = true;
    nbn::core::detail::concurrent_queue_test::setHook(&pauseAfterQueueNodeLinked);

    nbn::core::ApplicationMainThread mainThread{};
    std::atomic<int> taskExecutions{0};
    auto firstTask =
        std::make_shared<nbn::core::Task>([&taskExecutions]() { ++taskExecutions; }, nbn::core::Task::RunMode::SingleShot);
    auto secondTask =
        std::make_shared<nbn::core::Task>([&taskExecutions]() { ++taskExecutions; }, nbn::core::Task::RunMode::SingleShot);

    std::thread firstProducer{[&mainThread, task = std::move(firstTask)]() mutable { mainThread.submit(std::move(task)); }};
    {
        std::unique_lock lock(queueHookMutex());
        queueHookCondition().wait(lock, [] { return queueNodeLinked(); });
    }

    mainThread.submit(std::move(secondTask));
    {
        std::lock_guard lock(queueHookMutex());
        queueHookReleased() = true;
    }
    queueHookCondition().notify_all();
    firstProducer.join();

    mainThread.run();
    mainThread.wait();
    nbn::core::detail::concurrent_queue_test::setHook(nullptr);

    unit_tests::equal("Production queue tasks should execute", 2, taskExecutions.load());
}

void test_concurrent_queue_production_consumer_advances_task_stale_tail() {
    queueNodeLinked() = false;
    queueHookReleased() = false;
    queuePauseNextPush() = true;
    nbn::core::detail::concurrent_queue_test::setHook(&pauseAfterQueueNodeLinked);

    nbn::core::ApplicationMainThread mainThread{};
    auto task = std::make_shared<nbn::core::Task>([] {}, nbn::core::Task::RunMode::SingleShot);
    std::thread producer{[&mainThread, task = std::move(task)]() mutable { mainThread.submit(std::move(task)); }};
    {
        std::unique_lock lock(queueHookMutex());
        queueHookCondition().wait(lock, [] { return queueNodeLinked(); });
    }

    mainThread.wait();
    {
        std::lock_guard lock(queueHookMutex());
        queueHookReleased() = true;
    }
    queueHookCondition().notify_all();
    producer.join();
    nbn::core::detail::concurrent_queue_test::setHook(nullptr);
}

void test_concurrent_queue_production_consumer_advances_thread_stale_tail() {
    queueNodeLinked() = false;
    queueHookReleased() = false;
    queuePauseNextPush() = true;
    nbn::core::detail::concurrent_queue_test::setHook(&pauseAfterQueueNodeLinked);

    nbn::core::ApplicationThreadsManager manager{};
    auto firstThread = std::make_shared<nbn::core::Thread>([] {}, nbn::core::Task::RunMode::Loop);
    auto secondThread = std::make_shared<nbn::core::Thread>([] {}, nbn::core::Task::RunMode::Loop);
    std::thread producer{[&manager, thread = std::move(firstThread)]() mutable { manager.submit(std::move(thread)); }};
    {
        std::unique_lock lock(queueHookMutex());
        queueHookCondition().wait(lock, [] { return queueNodeLinked(); });
    }

    manager.submit(std::move(secondThread));
    std::shared_ptr<nbn::core::Thread> consumedThread{};
    const auto consumed = manager.popForTest(consumedThread);
    unit_tests::isTrue("Production thread queue should pop while the producer tail is stale", consumed);
    {
        std::lock_guard lock(queueHookMutex());
        queueHookReleased() = true;
    }
    queueHookCondition().notify_all();
    producer.join();
    manager.wait();

    queueNodeLinked() = false;
    queueHookReleased() = false;
    queuePauseNextPush() = true;
    nbn::core::ApplicationThreadsManager consumerManager{};
    auto consumerThread = std::make_shared<nbn::core::Thread>([] {}, nbn::core::Task::RunMode::Loop);
    std::thread consumerProducer{
        [&consumerManager, thread = std::move(consumerThread)]() mutable { consumerManager.submit(std::move(thread)); }};
    {
        std::unique_lock lock(queueHookMutex());
        queueHookCondition().wait(lock, [] { return queueNodeLinked(); });
    }
    std::shared_ptr<nbn::core::Thread> staleTailThread{};
    const auto staleTailConsumed = consumerManager.popForTest(staleTailThread);
    unit_tests::isTrue("Production thread queue should advance a stale tail while popping", staleTailConsumed);
    {
        std::lock_guard lock(queueHookMutex());
        queueHookReleased() = true;
    }
    queueHookCondition().notify_all();
    consumerProducer.join();
    nbn::core::detail::concurrent_queue_test::setHook(nullptr);
}

void test_concurrent_queue_production_thread_destructor_drains_pending_value() {
    {
        nbn::core::ApplicationThreadsManager manager{};
        manager.submit(std::make_shared<nbn::core::Thread>([] {}, nbn::core::Task::RunMode::Loop));
    }
}

void test_concurrent_queue_producer_helps_stale_tail() {
    queueNodeLinked() = false;
    queueHookReleased() = false;
    queuePauseNextPush() = true;
    nbn::core::detail::concurrent_queue_test::setHook(&pauseAfterQueueNodeLinked);

    nbn::core::ConcurrentQueue<int> queue;
    std::thread firstProducer{[&queue]() { queue.push(kFirstQueuedValue); }};
    {
        std::unique_lock lock(queueHookMutex());
        queueHookCondition().wait(lock, [] { return queueNodeLinked(); });
    }

    std::thread secondProducer{[&queue]() { queue.push(kSecondQueuedValue); }};
    {
        std::lock_guard lock(queueHookMutex());
        queueHookReleased() = true;
    }
    queueHookCondition().notify_all();
    firstProducer.join();
    secondProducer.join();

    int value{0};
    unit_tests::isTrue("Queue should retain the first helped value", queue.pop(value));
    unit_tests::equal("Queue should retain the first helped value", kFirstQueuedValue, value);
    unit_tests::isTrue("Queue should retain the second helped value", queue.pop(value));
    unit_tests::equal("Queue should retain the second helped value", kSecondQueuedValue, value);
    nbn::core::detail::concurrent_queue_test::setHook(nullptr);
}
