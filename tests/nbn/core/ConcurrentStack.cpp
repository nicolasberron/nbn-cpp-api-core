#include <atomic>
#include <barrier>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include <nbn/core/ConcurrentStack.h>
#include <nbn/core/ElapsedTimer.h>
#include <nbn/core/UnitTests.h>

using namespace nbn::core;

namespace {

constexpr std::ptrdiff_t kStressProducerCount = 4;
constexpr std::ptrdiff_t kStressConsumerCount = 4;
constexpr int kStressItemsPerProducer = 2500;
constexpr int kStressRoundCount = 3;
constexpr auto kStressTimeout = std::chrono::seconds{5};

template <typename Value, typename PushValue, typename ReadValue>
auto runStackStress(PushValue pushValue, ReadValue readValue) -> bool {
    const auto itemCount = static_cast<std::size_t>(kStressProducerCount * kStressItemsPerProducer);
    ConcurrentStack<Value> stack;
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
        workers.emplace_back([&stack, &startGate, pushValue, producer]() {
            startGate.arrive_and_wait();
            for (int item = 0; item < kStressItemsPerProducer; ++item) {
                pushValue(stack, static_cast<std::size_t>(producer * kStressItemsPerProducer + item));
            }
        });
    }

    for (std::ptrdiff_t consumer = 0; consumer < kStressConsumerCount; ++consumer) {
        workers.emplace_back([&stack, &startGate, &consumed, &seen, &invalidValue, readValue, deadline]() {
            startGate.arrive_and_wait();
            while (consumed.load(std::memory_order_acquire) < seen.size() && std::chrono::steady_clock::now() < deadline) {
                Value value{};
                if (!stack.pop(value)) {
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

void test_concurrent_stack_push_pop() {
    ConcurrentStack<int> stack;
    stack.push(1);
    stack.push(2);
    stack.push(3);

    int value{0};
    bool isResult = stack.pop(value);
    unit_tests::isTrue("First value should be 3", isResult && value == 3);

    isResult = stack.pop(value);
    unit_tests::isTrue("Second value should be 2", isResult && value == 2);

    isResult = stack.pop(value);
    unit_tests::isTrue("Third value should be 1", isResult && value == 1);

    isResult = stack.pop(value);
    unit_tests::isFalse("Stack should be empty", isResult);
}

void test_concurrent_stack_destructor_drains_pending_values() {
    constexpr int firstValue = 1;
    constexpr int secondValue = 2;

    {
        ConcurrentStack<int> stack;
        stack.push(firstValue);
        stack.push(secondValue);
    }
}

void test_concurrent_stack_multiple_threads() {
    constexpr int stackSize = 10000;
    ConcurrentStack<int> stack;
    std::vector<std::thread> pushers;
    std::condition_variable cv;
    std::mutex mtx;
    int ready_threads = 0;

    for (int i = 0; i < 4; ++i) {
        pushers.emplace_back([&stack, &cv, &mtx, &ready_threads, i]() {
            for (int j = 0; j < stackSize; ++j) {
                stack.push(i * stackSize + j);
            }
            std::unique_lock<std::mutex> lock(mtx);
            ready_threads++;
            cv.notify_all();
        });
    }

    std::thread popper([&stack, &cv, &mtx, &ready_threads]() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&ready_threads] { return ready_threads == 4; });
        int count = 0;
        int value{0};
        while (stack.pop(value)) {
            ++count;
        }
        constexpr int expectedCount = 4 * stackSize;
        unit_tests::equal("Popper should have popped 40000 values", expectedCount, count);
    });

    // Wait for all threads to finish
    for (auto& thread : pushers) {
        thread.join();
    }
    popper.join();
}

void test_concurrent_stack_high_contention_preserves_unique_values() {
    auto pushValue = [](ConcurrentStack<int>& stack, std::size_t item) { stack.push(static_cast<int>(item)); };
    auto readValue = [](const int value) { return static_cast<std::size_t>(value); };
    auto allRoundsPassed = true;
    for (int round = 0; round < kStressRoundCount; ++round) {
        const auto roundPassed = runStackStress<int>(pushValue, readValue);
        allRoundsPassed = roundPassed && allRoundsPassed;
    }
    unit_tests::isTrue("Stack stress should preserve every unique value", allRoundsPassed);
}

void test_concurrent_stack_high_contention_preserves_unique_move_only_values() {
    auto pushValue = [](ConcurrentStack<std::unique_ptr<int>>& stack, std::size_t item) {
        stack.push(std::make_unique<int>(static_cast<int>(item)));
    };
    auto readValue = [](const std::unique_ptr<int>& value) { return static_cast<std::size_t>(*value); };
    auto allRoundsPassed = true;
    for (int round = 0; round < kStressRoundCount; ++round) {
        const auto roundPassed = runStackStress<std::unique_ptr<int>>(pushValue, readValue);
        allRoundsPassed = roundPassed && allRoundsPassed;
    }
    unit_tests::isTrue("Stack stress should preserve every unique move-only value", allRoundsPassed);
}

void test_with_unique_ptr() {
    ConcurrentStack<std::unique_ptr<int>> stack;
    stack.push(std::make_unique<int>(1));
    stack.push(std::make_unique<int>(2));
    stack.push(std::make_unique<int>(3));

    std::unique_ptr<int> value;
    bool isResult = stack.pop(value);
    unit_tests::isTrue("First value should be 3", isResult && *value == 3);

    isResult = stack.pop(value);
    unit_tests::isTrue("Second value should be 2", isResult && *value == 2);

    isResult = stack.pop(value);
    unit_tests::isTrue("Third value should be 1", isResult && *value == 1);

    isResult = stack.pop(value);
    unit_tests::isFalse("Stack should be empty", isResult);
}

void test_concurrent_stack_unique_ptr_destructor_drains_pending_values() {
    {
        ConcurrentStack<std::unique_ptr<int>> stack;
        stack.push(std::make_unique<int>(1));
        stack.push(std::make_unique<int>(2));
    }
}
