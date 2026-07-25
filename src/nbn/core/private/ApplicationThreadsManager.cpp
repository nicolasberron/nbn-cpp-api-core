#include "ApplicationThreadsManager.h"
#include "ApplicationMainThread.h"

#include <condition_variable>
#include <exception>
#include <functional>
#include <mutex>
#include <vector>

#include "../ConcurrentQueue.h"

namespace nbn::core {

class ApplicationThreadsManager::Impl : public Thread {
   public:
    Impl() {
        // Wake the state checker whenever the manager itself is asked to stop or the main thread finishes,
        // so it can re-evaluate its loop condition without polling.
        signalStopRequested()->connect([this]() { notifyStateChanged(); });
        m_mainThread.signalStopped()->connect([this]() { notifyStateChanged(); });
    }

    void run() override {
        clearException();
        m_mainThread.run();
        Thread::run([&] { threadStateChecker(); }, Task::RunMode::SingleShot);
    }

    void threadStateChecker() {
        while (!isStopRequested() && !m_mainThread.isStopRequested() && m_mainThread.isRunning()) {
            reapFinishedThreads();

            // Block until a relevant state change is signalled: a new async thread submitted, an async
            // thread completed, the main thread finished, or a stop was requested. This replaces the
            // previous busy-poll that pinned a CPU core at 100% while the application was idle.
            std::unique_lock<std::mutex> lock(m_stateMutex);
            m_stateCondVar.wait(lock, [this] {
                return m_stateChanged || isStopRequested() || m_mainThread.isStopRequested() || !m_mainThread.isRunning();
            });
            m_stateChanged = false;
        }
        m_mainThread.requestStop();
        // If the main thread is stopped, we need to wait for all the threads to finish.
        stopAndWaitAllAsyncTasks();
        // Skip joining the main thread when stop() was requested externally (crash or
        // quit-from-main-thread).  In that scenario the main thread is executing std::exit(),
        // and joining it here would deadlock with the destructor chain that is concurrently
        // trying to join this (threadStateChecker) thread.  Impl::wait() handles the join
        // on the non-crashing path (called from the main() thread).
        if (!isStopRequested()) {
            try {
                m_mainThread.wait();
            } catch (...) {
                captureCurrentException();
            }
        }
    }

    // Reaps every async thread that has finished (joining it and surfacing its exception) and keeps the
    // still-running ones queued. Each thread is inspected exactly once per call to avoid re-spinning.
    auto reapFinishedThreads() -> void {
        std::vector<std::shared_ptr<Thread>> stillRunning{};
        std::shared_ptr<Thread> spThread{};
        while (m_threadsQueue.pop(spThread)) {
            if (!spThread->isStopRequested() && spThread->isRunning()) {
                stillRunning.emplace_back(std::move(spThread));
            } else {
                // This blocks until worker completion and surfaces captured exceptions.
                try {
                    spThread->wait();
                } catch (...) {
                    // First exception wins: request cooperative stop and continue draining.
                    captureCurrentException();
                    stop();
                }
            }
        }
        for (auto& sp : stillRunning) {
            m_threadsQueue.push(std::move(sp));
        }
    }

    auto stopAndWaitAllAsyncTasks() -> void {
        ConcurrentQueue<std::shared_ptr<Thread>> threadsQueue{};
        std::shared_ptr<Thread> spThread{};
        while (m_threadsQueue.pop(spThread)) {
            spThread->stop();
            threadsQueue.push(std::move(spThread));
        }
        while (threadsQueue.pop(spThread)) {
            try {
                spThread->wait();
            } catch (...) {
                captureCurrentException();
            }
        }
    }

    auto submit(std::shared_ptr<Task> spTask) noexcept -> void { m_mainThread.submit(std::move(spTask)); }

    auto submit(std::shared_ptr<Thread> spThread) noexcept -> void {
        // Wake the state checker when this async thread completes, then on submission itself.
        spThread->signalStopped()->connect([this]() { notifyStateChanged(); });
        spThread->run();
        m_threadsQueue.push(std::move(spThread));
        notifyStateChanged();
    }

#ifdef NBN_LOGGER_TEST_HOOK
    [[nodiscard]] auto popForTest(std::shared_ptr<Thread>& spThread) -> bool { return m_threadsQueue.pop(spThread); }
#endif

    auto stop() -> void override {
        // Use requestStop() instead of Thread::stop() to avoid a double-join:
        // Thread 1 (main()) is already waiting in Impl::wait() → Thread::wait() to
        // join Thread 3 (threadStateChecker). Thread::stop() would also try to join
        // Thread 3 from this thread, causing undefined behaviour (two threads joining
        // the same jthread simultaneously).
        // requestStop() only signals Thread 3 to exit; Thread 1 performs the join.
        Thread::requestStop();
        // Do not join here: the state checker and the manager wait path must not compete
        // to join the application thread. The manager wait path owns that join.
        m_mainThread.requestStop();
        notifyStateChanged();
    }

    auto wait() -> void override {
        Thread::wait();
        stopAndWaitAllAsyncTasks();
        try {
            m_mainThread.wait();
        } catch (...) {
            captureCurrentException();
        }
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

    // Marks a state change and wakes the checker. Signalled on submit, on async/main thread completion,
    // and on stop request.
    auto notifyStateChanged() noexcept -> void {
        {
            std::lock_guard<std::mutex> lock(m_stateMutex);
            m_stateChanged = true;
        }
        m_stateCondVar.notify_all();
    }

    ConcurrentQueue<std::shared_ptr<Thread>> m_threadsQueue{};
    ApplicationMainThread m_mainThread{};
    std::mutex m_exceptionMutex{};
    std::mutex m_stateMutex{};
    std::condition_variable m_stateCondVar{};
    bool m_stateChanged{false};
    std::function<void()> m_rethrowFirstException{};
};

ApplicationThreadsManager::ApplicationThreadsManager() : m_spImpl{std::make_unique<Impl>()} {}

ApplicationThreadsManager::~ApplicationThreadsManager() noexcept = default;

auto ApplicationThreadsManager::run() -> void {
    m_spImpl->run();
}

auto ApplicationThreadsManager::stop() -> void {
    m_spImpl->stop();
}

auto ApplicationThreadsManager::wait() -> void {
    m_spImpl->wait();
}

auto ApplicationThreadsManager::submit(std::shared_ptr<Task> spTask) noexcept -> void {
    m_spImpl->submit(std::move(spTask));
}

auto ApplicationThreadsManager::submit(std::shared_ptr<Thread> spTask) noexcept -> void {
    m_spImpl->submit(std::move(spTask));
}

#ifdef NBN_LOGGER_TEST_HOOK
auto ApplicationThreadsManager::popForTest(std::shared_ptr<Thread>& spTask) -> bool {
    return m_spImpl->popForTest(spTask);
}
#endif

}  // namespace nbn::core