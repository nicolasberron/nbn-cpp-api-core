#include <stdexcept>

#include <nbn/core/Task.h>
#include <nbn/core/UnitTests.h>

using namespace nbn::core;

namespace {

constexpr int kExpectedLoopIterations{3};
constexpr int kExpectedSignalCount{1};

}  // namespace

void test_task_initializes_and_runs_single_shot() {
    auto bodyWasCalled = false;
    Task task{};

    task.init([&bodyWasCalled]() { bodyWasCalled = true; }, Task::RunMode::SingleShot);

    unit_tests::equal("Task run mode should be single-shot", Task::RunMode::SingleShot, task.getRunMode());
    unit_tests::isTrue("Task body should be initialized", task.getTaskBody() != nullptr);
    unit_tests::isFalse("Task should not be running before run", task.isRunning());
    unit_tests::isFalse("Task should not have a stop request before run", task.isStopRequested());

    task.run();
    task.wait();

    unit_tests::isTrue("Single-shot task body should run", bodyWasCalled);
    unit_tests::isFalse("Single-shot task should finish", task.isRunning());
}

void test_task_runs_loop_until_stopped_and_emits_lifecycle_signals() {
    auto loopIterations = 0;
    auto startedCount = 0;
    auto stoppedCount = 0;
    Task task{[&]() {
                  ++loopIterations;
                  if (loopIterations >= kExpectedLoopIterations) {
                      task.stop();
                  }
              },
              Task::RunMode::Loop};

    task.signalStarted()->connect([&startedCount]() { ++startedCount; });
    task.signalStopped()->connect([&stoppedCount]() { ++stoppedCount; });

    task.run();
    task.wait();

    unit_tests::equal("Loop task should run the expected number of iterations", kExpectedLoopIterations, loopIterations);
    unit_tests::equal("Task should emit started once", kExpectedSignalCount, startedCount);
    unit_tests::equal("Task should emit stopped once", kExpectedSignalCount, stoppedCount);
    unit_tests::isTrue("Loop task should record a stop request", task.isStopRequested());
    unit_tests::isFalse("Loop task should finish after stop", task.isRunning());
}

void test_task_transfers_body_exceptions_through_wait() {
    const std::string expectedMessage{"task body failure"};
    Task task{[&expectedMessage]() { throw std::runtime_error(expectedMessage); }, Task::RunMode::SingleShot};

    task.run();

    auto didThrow{false};
    auto receivedMessage = std::string{};
    try {
        task.wait();
    } catch (const std::runtime_error& exception) {
        didThrow = true;
        receivedMessage = exception.what();
    }

    unit_tests::isTrue("Task wait should rethrow the body exception", didThrow);
    unit_tests::equal("Task wait should preserve the body exception", expectedMessage, receivedMessage);
    task.wait();
}

void test_task_without_body_throws_when_run() {
    Task task{};

    unit_tests::isThrowing<std::exception>("Running an uninitialized task should throw", [&task]() { task.run(); });
}

void test_task_preserves_exceptions_from_lifecycle_signals() {
    const std::string startedMessage{"task started signal failure"};
    Task task{[]() {}, Task::RunMode::SingleShot};
    task.signalStarted()->connect([&startedMessage]() { throw std::runtime_error(startedMessage); });
    task.signalStopped()->connect([]() { throw std::runtime_error{"task stopped signal failure"}; });

    task.run();

    auto receivedMessage = std::string{};
    try {
        task.wait();
    } catch (const std::runtime_error& exception) {
        receivedMessage = exception.what();
    }

    unit_tests::equal("Task should preserve the lifecycle signal exception", startedMessage, receivedMessage);
    unit_tests::isFalse("Task should finish after a lifecycle signal exception", task.isRunning());
}
