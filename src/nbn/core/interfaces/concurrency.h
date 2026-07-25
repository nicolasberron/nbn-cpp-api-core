#pragma once

#include "Interface.h"

namespace nbn::core::interfaces {

/**
 * @file interfaces/concurrency.h
 * @brief Concurrency-related interfaces.
 */

/**
 * @struct IRunnable
 * @brief Contract for start/stop/wait runnable components.
 */

struct nbn_export_core IRunnable : public Interface {
    virtual auto run() -> void = 0;
    virtual auto stop() -> void = 0;
    [[nodiscard]] virtual auto isRunning() const -> bool = 0;
    [[nodiscard]] virtual auto isStopRequested() const -> bool = 0;
    virtual auto wait() -> void = 0;
};

}  // namespace nbn::core::interfaces
