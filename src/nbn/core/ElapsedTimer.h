#pragma once

#include <chrono>

#include "Object.h"

namespace nbn::core {

/**
 * @file ElapsedTimer.h
 * @brief Monotonic elapsed-time measurement helper.
 */

/**
 * @class ElapsedTimer
 * @brief Utility timer for duration and expiration checks.
 *
 * @par Usage example
 * @code{.cpp}
 * nbn::core::ElapsedTimer timer;
 * timer.start();
 *
 * // ... work ...
 * auto elapsed = timer.elapsed();
 * if (timer.hasExpired(std::chrono::milliseconds(10))) {
 *     nbn::log::debug("operation took at least 10ms");
 * }
 * @endcode
 */

class nbn_export_core ElapsedTimer : public Object {
   public:
    /** @brief Constructs an elapsed timer in a stopped state. */
    ElapsedTimer();
    /** @brief Destroys the elapsed timer. */
    ~ElapsedTimer() override;

    /** @brief Starts or restarts elapsed-time measurement. */
    void start();
    /** @brief Returns the duration since the timer was started. */
    [[nodiscard]] auto elapsed() const -> std::chrono::nanoseconds;
    /**
     * @brief Checks whether the requested duration has elapsed.
     * @param time Duration to compare with the measured elapsed time.
     * @return true when elapsed() is at least @p time.
     */
    [[nodiscard]] auto hasExpired(std::chrono::nanoseconds time) const -> bool;

   private:
    class Impl;
    std::unique_ptr<Impl> m_spImpl{};
};

}  // namespace nbn::core