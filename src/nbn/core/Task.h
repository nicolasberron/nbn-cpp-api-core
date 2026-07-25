#pragma once

#include <functional>

#include "Object.h"
#include "interfaces/concurrency.h"

namespace nbn::core {

/**
 * @file Task.h
 * @brief Runnable task abstraction used by the application runtime.
 */

/**
 * @class Task
 * @brief Wraps executable work in single-shot or loop run modes.
 *
 * @par Usage example
 * @code{.cpp}
 * auto task = std::make_shared<nbn::core::Task>([] {
 *     nbn::log::info("task body executed");
 * }, nbn::core::Task::RunMode::SingleShot);
 *
 * task->run();
 * task->wait();
 * @endcode
 */

class nbn_export_core Task : public Object, public interfaces::IRunnable {
   public:
    using task_body_t = std::function<void()>;

    enum class RunMode { SingleShot, Loop };

    Task();
    explicit Task(task_body_t f, RunMode mode);
    ~Task() override;

    auto init(task_body_t f, RunMode mode) -> void;

    [[nodiscard]] auto getTaskBody() const -> task_body_t;
    [[nodiscard]] auto getRunMode() const -> RunMode;

    auto signalStarted() -> signal_ptr<> { return signal<>(); }
    auto signalStopped() -> signal_ptr<> { return signal<>(); }

    // IRunnable interface
    void run() override;
    void stop() override;
    [[nodiscard]] auto isRunning() const -> bool override;
    [[nodiscard]] auto isStopRequested() const -> bool override;
    void wait() override;

   private:
    class Impl;
    std::unique_ptr<Impl> m_spImpl{};
};

}  // namespace nbn::core
