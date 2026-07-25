#pragma once

#include <condition_variable>
#include <mutex>
#include <utility>

#include "Application.h"
#include "Thread.h"

namespace nbn::core {

class Task::Impl : public Object {
   public:
    explicit Impl(Task* pDecl) : m_pDecl{pDecl} {}

    auto init(task_body_t f, RunMode mode) -> void {
        m_body = f;
        m_mode = mode;
    }

    [[nodiscard]] auto getTaskBody() const -> task_body_t { return m_body; }

    [[nodiscard]] auto getRunMode() const -> RunMode { return m_mode; }

    void run() {
        nbn::log::fatal_if(m_body == nullptr, "Task body is not set");
        m_isRunning = true;
        try {
            m_pDecl->signalStarted()->emit();
        } catch (...) {
            {
                std::lock_guard lock{m_exceptionMutex};
                m_exception_ptr = std::current_exception();
            }
            markFinished();
            try {
                m_pDecl->signalStopped()->emit();
            } catch (const std::exception& e) {
                nbn::log::error(std::format("signalStopped exception: {}", e.what()));
            }
            return;
        }
        try {
            if (m_mode == Task::RunMode::SingleShot) {
                m_body();
            } else {
                while (!m_isStopRequested && m_isRunning) {
                    m_body();
                }
            }
        } catch (...) {
            // Transfer the exception to the calling thread via wait()
            std::lock_guard lock{m_exceptionMutex};
            m_exception_ptr = std::current_exception();
        }
        markFinished();
        m_pDecl->signalStopped()->emit();
    }

    void stop() { m_isStopRequested = true; }

    [[nodiscard]] auto isRunning() const -> bool { return m_isRunning; }

    [[nodiscard]] auto isStopRequested() const -> bool { return m_isStopRequested; }

    void wait() {
        // Block on a condition variable instead of busy-spinning with std::this_thread::yield().
        // A long-running task (e.g. a blocking server loop) would otherwise pin a CPU core at 100%
        // for the whole time another thread waits on it.
        {
            std::unique_lock<std::mutex> lock(m_doneMutex);
            m_doneCondVar.wait(lock, [this] { return !m_isRunning.load(); });
        }
        std::exception_ptr exceptionPtr{};
        {
            std::lock_guard lock{m_exceptionMutex};
            exceptionPtr = std::exchange(m_exception_ptr, {});
        }
        if (exceptionPtr) {
            std::rethrow_exception(exceptionPtr);
        }
    }

   private:
    auto markFinished() noexcept -> void {
        {
            std::lock_guard<std::mutex> lock(m_doneMutex);
            m_isRunning = false;
        }
        m_doneCondVar.notify_all();
    }

    Task* m_pDecl{nullptr};
    task_body_t m_body;
    std::atomic<bool> m_isStopRequested{false};
    std::atomic<bool> m_isRunning{false};
    Task::RunMode m_mode{Task::RunMode::SingleShot};
    std::exception_ptr m_exception_ptr;
    std::mutex m_exceptionMutex;
    std::mutex m_doneMutex;
    std::condition_variable m_doneCondVar;
};

}  // namespace nbn::core