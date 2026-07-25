#pragma once

#include "../Task.h"

namespace nbn::core {

class nbn_export_core Thread : public Object, public interfaces::IRunnable {
   public:
    Thread();
    explicit Thread(std::shared_ptr<Task> task);
    explicit Thread(Task::task_body_t f, Task::RunMode mode = Task::RunMode::SingleShot);

    ~Thread() override;

    void run(Task::task_body_t taskBody, Task::RunMode mode = Task::RunMode::SingleShot);
    void run(std::shared_ptr<Task> task);

    auto signalStarted() -> signal_ptr<> { return signal<>(); }
    auto signalStopRequested() -> signal_ptr<> { return signal<>(); }
    auto signalStopped() -> signal_ptr<> { return signal<>(); }

    // IRunnable interface
    void run() override;
    void stop() override;
    [[nodiscard]] auto isRunning() const -> bool override;
    [[nodiscard]] auto isStopRequested() const -> bool override;
    void wait() override;

    // Signals the thread to stop without waiting for it to finish. Unlike
    // stop(), this does not join; the caller is responsible for joining via
    // wait(). Use this when another thread already owns the join operation.
    void requestStop();

    // Returns true if the calling thread is this thread's own OS thread.
    [[nodiscard]] auto isCurrentThread() const -> bool;
    // Detaches the underlying OS thread. Only safe to call from the thread itself
    // (crash / self-quit scenario) so that ~jthread does not attempt a self-join.
    auto detach() -> void;

   private:
    class Impl;
    std::unique_ptr<Impl> m_spImpl{};
};

}  // namespace nbn::core
