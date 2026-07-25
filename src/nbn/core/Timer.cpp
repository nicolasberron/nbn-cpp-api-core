/*
 * Timer.cpp
 * Description:
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more detail.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2023 Nicolas Berron <nicolas.berron@gmail.com>
 */

#include "Timer.h"
#include "Logger.h"
#include "private/Thread.h"

#include <mutex>

#if defined(PLATFORMIO_BUILD)
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#endif

namespace nbn::core {

// Factory registration
static auto const registerToFactory{[]() { return Object::factoryRegister<Timer>(); }()};

class Timer::Impl : public Thread {
    Timer* m_pDecl{nullptr};
    mutable std::mutex m_configurationMutex;
    std::mutex m_runMutex;
    std::chrono::nanoseconds m_interval{};
    size_t m_repeatCount{0};
    size_t m_repeatIndex{0};

   public:
    explicit Impl(Timer* pDecl) : Thread{[this]() { timerLoop(); }, Task::RunMode::Loop}, m_pDecl{pDecl} {
        signalStarted()->connect([this]() { m_pDecl->signalStarted()->emit(); });
        signalStopped()->connect([this]() { m_pDecl->signalStopped()->emit(); });
    }

    Impl(Timer* pDecl, std::chrono::nanoseconds interval, std::function<void()> callback, size_t repeatCount)
        : Thread{[this]() { timerLoop(); }, Task::RunMode::Loop}, m_pDecl{pDecl}, m_interval{interval}, m_repeatCount{repeatCount} {
        if (callback != nullptr) {
            m_pDecl->signalTimeout()->connect(callback);
        }
        signalStarted()->connect([this]() { m_pDecl->signalStarted()->emit(); });
        signalStopped()->connect([this]() { m_pDecl->signalStopped()->emit(); });
    }

    void timerLoop() {
        std::chrono::nanoseconds interval{};
        size_t repeatCount{0};
        {
            std::lock_guard lock(m_configurationMutex);
            interval = m_interval;
            repeatCount = m_repeatCount;
        }
#if defined(PLATFORMIO_BUILD)
        auto delayMs = std::chrono::duration_cast<std::chrono::milliseconds>(interval).count();
        if (delayMs <= 0) {
            delayMs = 1;
        }
        vTaskDelay(pdMS_TO_TICKS(static_cast<uint32_t>(delayMs)));
#else
        std::this_thread::sleep_for(interval);
#endif
        m_pDecl->signalTimeout()->emit();  // TODO: option to emit asynchronously
        bool shouldStop{false};
        {
            std::lock_guard lock(m_configurationMutex);
            if (repeatCount != 0 && ++m_repeatIndex == repeatCount) {
                shouldStop = true;
            }
        }
        if (shouldStop) {
            m_pDecl->stop();
        }
    }

    void run() override {
        std::lock_guard lock(m_runMutex);
        Thread::run();
    }

    void run(std::chrono::nanoseconds interval, std::function<void()> callback, size_t repeatCount) {
        std::lock_guard runLock(m_runMutex);
        {
            std::lock_guard configurationLock(m_configurationMutex);
            m_interval = interval;
            m_repeatCount = repeatCount;
            m_repeatIndex = 0;
            if (callback != nullptr) {
                m_pDecl->signalTimeout()->connect(callback);
            }
        }
        Thread::run();
    }

    auto isSingleShot() const -> bool {
        std::lock_guard lock(m_configurationMutex);
        return m_repeatCount == 1;
    }
};

// Timer class implementation

Timer::Timer() : m_spImpl(std::make_unique<Impl>(this)) {
    interval()->set(std::chrono::nanoseconds{0});
}

Timer::Timer(std::chrono::nanoseconds interval, std::function<void()> callback, size_t repeatCount)
    : m_spImpl(std::make_unique<Impl>(this, interval, callback, repeatCount)) {}

Timer::~Timer() {
    stop();
}

void Timer::run(std::chrono::nanoseconds interval, std::function<void()> callback, size_t repeatCount) {
    m_spImpl->run(interval, callback, repeatCount);
}

void Timer::run() {
    m_spImpl->run();
}

void Timer::stop() {
    m_spImpl->stop();
}

[[nodiscard]] auto Timer::isSingleShot() const -> bool {
    return m_spImpl->isSingleShot();
}

[[nodiscard]] auto Timer::isRunning() const -> bool {
    return m_spImpl->isRunning();
}

[[nodiscard]] auto Timer::isStopRequested() const -> bool {
    return m_spImpl->isStopRequested();
}

void Timer::wait() {
    m_spImpl->wait();
}

}  // namespace nbn::core