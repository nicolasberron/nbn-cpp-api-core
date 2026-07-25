/*
 * Timer.h
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

#pragma once

#include <chrono>
#include <functional>

#include "Object.h"
#include "interfaces/concurrency.h"

namespace nbn::core {

/**
 * @class Timer
 * @brief The Timer class represents a timer that can run a callback function at regular intervals.
 *
 * @par Usage example
 * @code{.cpp}
 * nbn::core::Timer timer(std::chrono::milliseconds(100), [] {
 *     nbn::log::debug("tick");
 * }, 5);
 *
 * timer.run();
 * timer.wait();
 * @endcode
 */
class nbn_export_core Timer : public Object, public interfaces::IRunnable {
    class Impl;
    std::unique_ptr<Impl> m_spImpl{};

   public:
    /**
     * @brief Constructs a Timer object.
     */
    Timer();

    /**
     * @brief Constructs a Timer object with the specified interval, callback function, and repeat count.
     * @param interval The time interval between each callback invocation.
     * @param callback The function to be called at each interval.
     * @param repeatCount The number of times the callback should be repeated. A value of 0 means infinite repetitions.
     */
    explicit Timer(std::chrono::nanoseconds interval, std::function<void()> callback, size_t repeatCount = 0);

    /**
     * @brief Destroys the Timer object.
     */
    ~Timer() override;

    // Properties

    /**
     * @brief Returns the interval property of the Timer.
     * @return The interval property.
     */
    auto interval() -> property_ptr<std::chrono::nanoseconds> { return property<std::chrono::nanoseconds>(); }

    /**
     * @brief Returns the repeat count property of the Timer.
     * @return The repeat count property.
     */
    auto repeatCount() -> property_ptr<size_t> { return property<size_t>(); }

    // Signals

    /**
     * @brief Returns the timeout signal of the Timer.
     * @return The timeout signal.
     */
    auto signalTimeout() -> signal_ptr<> { return signal<>(); }

    // Methods

    /**
     * @brief Runs the Timer with the specified interval, callback function, and repeat count.
     * @param interval The time interval between each callback invocation.
     * @param callback The function to be called at each interval.
     * @param repeatCount The number of times the callback should be repeated. A value of 0 means infinite repetitions.
     */
    void run(std::chrono::nanoseconds interval, std::function<void()> callback, size_t repeatCount = 0);

    /**
     * @brief Checks if the Timer is a single-shot timer.
     * @return True if the Timer is a single-shot timer, false otherwise.
     */
    [[nodiscard]] auto isSingleShot() const -> bool;

    // Signals

    /**
     * @brief Returns the signal that is emitted when the Timer is started.
     * @return The signal that is emitted when the Timer is started.
     */
    auto signalStarted() -> signal_ptr<> { return signal<>(); }

    /**
     * @brief Returns the signal that is emitted when the Timer is stopped.
     * @return The signal that is emitted when the Timer is stopped.
     */
    auto signalStopped() -> signal_ptr<> { return signal<>(); }

    // IRunnable interface

    /**
     * @brief Runs the Timer.
     */
    void run() override;

    /**
     * @brief Stops the Timer.
     */
    void stop() override;

    /**
     * @brief Checks if the Timer is running.
     * @return True if the Timer is running, false otherwise.
     */
    [[nodiscard]] auto isRunning() const -> bool override;

    /**
     * @brief Checks if a stop request has been made for the Timer.
     * @return True if a stop request has been made, false otherwise.
     */
    [[nodiscard]] auto isStopRequested() const -> bool override;

    /**
     * @brief Waits for the Timer to finish.
     */
    void wait() override;
};

}  // namespace nbn::core
