#include <atomic>
#include <exception>
#if defined(PLATFORMIO_BUILD)
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#else
#include <stop_token>
#include <thread>
#endif

#include "Logger.h"
#include "Thread.h"
#include "format_bridge.h"
#include "private/Application.h"
#include "private/Task.h"

namespace nbn::core {

namespace {

// A worker can start executing before std::jthread's move-assignment into
// m_thread has completed. Do not use m_thread.get_id() to detect self-stop in
// that window; the member may still be non-joinable and make the worker try to
// join itself. Thread-local ownership is established before the task body runs.
auto currentThreadImpl() -> const void*& {
    thread_local const void* pCurrentThreadImpl = nullptr;
    return pCurrentThreadImpl;
}

}  // namespace

// Factory registration
static auto const registerToFactory{[]() { return Object::factoryRegister<Thread>(); }()};

// Thread::Impl class implementation

class Thread::Impl {
    Thread* m_pDecl{nullptr};
    std::atomic<bool> m_isWaiting{false};

   public:
    explicit Impl(Thread* pDecl, std::shared_ptr<Task> spTask) : m_pDecl{pDecl}, m_spTask{std::move(spTask)} {}

    void run() {
        nbn::log::fatal_if(m_spTask == nullptr, "Task is not set");
        nbn::log::fatal_if(isRunning(), std::format("{} thread is already running", m_pDecl->getClassName()));

        m_isStopRequested.store(false);
        m_isRunning.store(true);
        m_isStoppedEmitted.store(false);
        m_spDetached = std::make_shared<std::atomic<bool>>(false);

        // Create and connect the stop signal before starting the worker. Both the worker
        // and the caller can otherwise lazily request this signal concurrently, causing
        // duplicate feature creation on the owning Object.
        auto spTask = m_spTask;
        m_pDecl->signalStopRequested()->connect([spTask]() {
            if (spTask != nullptr) {
                spTask->stop();
            }
        });

#if defined(PLATFORMIO_BUILD)
        auto createStatus = xTaskCreate(&Impl::taskEntry, "nbn::Thread", 8192, this, 1, &m_taskHandle);
        nbn::log::fatal_if(createStatus != pdPASS, "Failed to create FreeRTOS task");
#else
        {
            std::lock_guard<std::mutex> lock(m_threadMutex);
            const auto spDetached = m_spDetached;
            m_thread = std::jthread([this, spTask = std::move(m_spTask), spDetached]() mutable {
                currentThreadImpl() = this;
                try {
                    spTask->run();
                    spTask->wait();
                } catch (...) {
                    // Transfer the exception to the calling thread via wait()
                    nbn::log::debug("Transfer exception caught in thread");
                    if (spDetached->load()) {
                        currentThreadImpl() = nullptr;
                        return;
                    }
                    m_exception_ptr = std::current_exception();
                    if (!m_isWaiting) {
                        stop();
                    }
                }
                if (spDetached->load()) {
                    currentThreadImpl() = nullptr;
                    return;
                }
                m_isRunning.store(false);
                // Signal natural completion so observers (e.g. the application thread manager) can react
                // without polling. emitStoppedOnce() guarantees a single emission even if stop() also runs.
                emitStoppedOnce();
                currentThreadImpl() = nullptr;
            });
        }
#endif
        m_pDecl->signalStarted()->emit();
    }

    void run(std::shared_ptr<Task> spTask) {
        m_spTask = std::move(spTask);
        run();
    }

    void stop() {
        // A worker may request its own stop while std::jthread is still being assigned
        // to m_thread. Do not touch m_thread or acquire the join mutex in that window;
        // the task stop signal is sufficient to stop the worker without racing the
        // caller that will join it.
        if (currentThreadImpl() == this) {
            if (isRunning() && !isStopRequested()) {
                m_isStopRequested.store(true);
                m_pDecl->signalStopRequested()->emit();
            }
            return;
        }

#if defined(PLATFORMIO_BUILD)
        if (isRunning()) {
#else
        bool hasThread{false};
        {
            std::lock_guard<std::mutex> lock(m_threadMutex);
            hasThread = m_thread.joinable();
        }
        if (isRunning() || hasThread) {
#endif
            {
                std::lock_guard<std::mutex> lock(m_stopMutex);
                if (!isStopRequested()) {
                    m_isStopRequested.store(true);
#if !defined(PLATFORMIO_BUILD)
                    m_thread.request_stop();
#endif
                    if (m_isStopRequested) {
                        m_pDecl->signalStopRequested()->emit();
                    }
                }
            }
#if defined(PLATFORMIO_BUILD)
            auto* pCurrentTaskHandle = xTaskGetCurrentTaskHandle();
            if (pCurrentTaskHandle != m_taskHandle) {
                while (m_isRunning.load()) {
                    vTaskDelay(1);
                }
            }
#else
            if (currentThreadImpl() != this) {
                std::lock_guard<std::mutex> lock(m_threadMutex);
                if (m_thread.joinable()) {
                    m_thread.join();
                    emitStoppedOnce();
                }
                m_isRunning.store(false);
            }
#endif
        }
    }

    [[nodiscard]] auto isRunning() const -> bool { return m_isRunning; }

    void wait() {
        m_isWaiting.store(true);
#if defined(PLATFORMIO_BUILD)
        while (m_isRunning.load()) {
            vTaskDelay(1);
        }
#else
        std::unique_lock<std::mutex> lock(m_waitMutex);
        std::lock_guard<std::mutex> threadLock(m_threadMutex);
        if (m_thread.joinable()) {
            m_thread.join();
        }
#endif
        m_isWaiting.store(false);
        auto exceptionPtr = m_exception_ptr;
        m_exception_ptr = {};
        if (exceptionPtr) {
            nbn::log::debug("Rethrow exception caught in thread");
            std::rethrow_exception(exceptionPtr);
        }
    }

    [[nodiscard]] auto isStopRequested() const -> bool { return m_isStopRequested; }

    // Signals stop without joining. Use when another thread is already waiting to join.
    void requestStop() {
        if (!isRunning()) {
            return;
        }

        bool expected{false};
        if (!m_isStopRequested.compare_exchange_strong(expected, true)) {
            return;
        }

        m_pDecl->signalStopRequested()->emit();
        // Intentionally no join here — the caller (or another thread in wait()) joins.
    }

    [[nodiscard]] auto isCurrentThread() const -> bool { return currentThreadImpl() == this; }

    // Detach the underlying jthread so that ~jthread does not try to join the current thread.
    // Only call this from the thread itself in crash / self-quit scenarios.
    auto detach() -> void {
        std::lock_guard<std::mutex> lock(m_threadMutex);
        if (m_thread.joinable()) {
            m_spDetached->store(true);
            m_thread.detach();
            m_isRunning.store(false);
            emitStoppedOnce();
        }
    }

   private:
    // Emits signalStopped() at most once per run, regardless of whether completion is reached
    // naturally inside the worker or through a concurrent stop()/join().
    auto emitStoppedOnce() -> void {
        bool expected{false};
        if (m_isStoppedEmitted.compare_exchange_strong(expected, true)) {
            m_pDecl->signalStopped()->emit();
        }
    }

#if defined(PLATFORMIO_BUILD)
    static void taskEntry(void* pArg) {
        auto* pSelf = static_cast<Impl*>(pArg);
        auto spTask = pSelf->m_spTask;
        try {
            if (spTask != nullptr) {
                spTask->run();
                spTask->wait();
            }
        } catch (...) {
            nbn::log::debug("Transfer exception caught in thread");
            pSelf->m_exception_ptr = std::current_exception();
        }

        pSelf->m_isRunning.store(false);
        pSelf->m_taskHandle = nullptr;
        pSelf->emitStoppedOnce();
        vTaskDelete(nullptr);
    }

    TaskHandle_t m_taskHandle{nullptr};
#else
    std::jthread m_thread;
    mutable std::mutex m_threadMutex;
#endif
    std::atomic<bool> m_isRunning{false};
    std::atomic<bool> m_isStopRequested{false};
    std::atomic<bool> m_isStoppedEmitted{false};
    std::exception_ptr m_exception_ptr;
    std::shared_ptr<Task> m_spTask{nullptr};
    std::shared_ptr<std::atomic<bool>> m_spDetached{std::make_shared<std::atomic<bool>>(false)};
    mutable std::mutex m_stopMutex;
    mutable std::mutex m_waitMutex;
};

// Thread class implementation

Thread::Thread() : m_spImpl{std::make_unique<Impl>(this, nullptr)} {}

Thread::Thread(std::shared_ptr<Task> task) : m_spImpl{std::make_unique<Impl>(this, std::move(task))} {}

Thread::Thread(Task::task_body_t f, Task::RunMode mode) : m_spImpl{std::make_unique<Impl>(this, std::make_unique<Task>(f, mode))} {}

Thread::~Thread() {
    stop();
}

void Thread::run(Task::task_body_t taskBody, Task::RunMode mode) {
    m_spImpl->run(std::make_unique<Task>(taskBody, mode));
}

void Thread::run(std::shared_ptr<Task> spTask) {
    m_spImpl->run(std::move(spTask));
}

void Thread::run() {
    m_spImpl->run();
}

void Thread::stop() {
    m_spImpl->stop();
}

auto Thread::isRunning() const -> bool {
    return m_spImpl->isRunning();
}

auto Thread::isStopRequested() const -> bool {
    return m_spImpl->isStopRequested();
}

void Thread::wait() {
    m_spImpl->wait();
}

void Thread::requestStop() {
    m_spImpl->requestStop();
}

auto Thread::isCurrentThread() const -> bool {
    return m_spImpl->isCurrentThread();
}

auto Thread::detach() -> void {
    m_spImpl->detach();
}

}  // namespace nbn::core
