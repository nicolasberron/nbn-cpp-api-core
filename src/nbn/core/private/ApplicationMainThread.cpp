#include "ApplicationMainThread.h"
#include "../Application.h"

#include <exception>
#include <functional>
#include <mutex>

#include "../ConcurrentQueue.h"

namespace nbn::core {

class ApplicationMainThread::Impl {
   public:
    Impl(ApplicationMainThread* pDecl) : m_pDecl{pDecl} {}

    void run() {
        clearException();
        m_pDecl->Thread::run(
            [this]() {
                std::shared_ptr<Task> spTask{};
                while (!m_pDecl->isStopRequested() && m_tasksQueue.pop(spTask)) {
                    try {
                        spTask->run();
                        spTask->wait();
                    } catch (...) {
                        // First exception wins. Stop scheduling and drain pending work.
                        captureCurrentException();
                        m_pDecl->stop();
                        break;
                    }
                }
                drainPendingTasks();
            },
            Task::RunMode::SingleShot);
    }

    auto submit(std::shared_ptr<Task> spTask) noexcept -> void { m_tasksQueue.push(std::move(spTask)); }

    auto wait() -> void {
        m_pDecl->Thread::wait();
        drainPendingTasks();
        rethrowIfException();
    }

   private:
    auto captureCurrentException() noexcept -> void {
        std::lock_guard<std::mutex> lock(m_exceptionMutex);
        if (!m_rethrowFirstException) {
            m_rethrowFirstException = [exceptionPtr = std::current_exception()]() { std::rethrow_exception(exceptionPtr); };
        }
    }

    auto clearException() -> void {
        std::lock_guard<std::mutex> lock(m_exceptionMutex);
        m_rethrowFirstException = {};
    }

    auto rethrowIfException() -> void {
        std::function<void()> rethrowFirstException{};
        {
            std::lock_guard<std::mutex> lock(m_exceptionMutex);
            rethrowFirstException = m_rethrowFirstException;
        }
        if (rethrowFirstException) {
            rethrowFirstException();
        }
    }

    auto drainPendingTasks() noexcept -> void {
        std::shared_ptr<Task> spTask{};
        while (m_tasksQueue.pop(spTask)) {
            spTask->stop();
            spTask->wait();
        }
    }

    ApplicationMainThread* m_pDecl{nullptr};
    ConcurrentQueue<std::shared_ptr<Task>> m_tasksQueue{};
    std::mutex m_exceptionMutex{};
    std::function<void()> m_rethrowFirstException{};
};

ApplicationMainThread::ApplicationMainThread() : m_spImpl{std::make_unique<Impl>(this)} {}

ApplicationMainThread::~ApplicationMainThread() noexcept = default;

auto ApplicationMainThread::run() -> void {
    m_spImpl->run();
}

auto ApplicationMainThread::wait() -> void {
    m_spImpl->wait();
}

auto ApplicationMainThread::submit(std::shared_ptr<Task> spTask) noexcept -> void {
    m_spImpl->submit(std::move(spTask));
}

}  // namespace nbn::core