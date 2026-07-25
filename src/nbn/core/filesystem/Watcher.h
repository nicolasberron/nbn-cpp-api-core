#pragma once

#include <filesystem>
#include <memory>
#include <string>

#include "../Object.h"
#include "../interfaces/concurrency.h"

namespace nbn::core::filesystem {

/**
 * @class Watcher
 * @brief Watches the immediate files in a directory for changes.
 *
 * Watcher emits signalChanged() from its worker thread when a regular file is
 * created, removed, or modified. The watcher is non-recursive.
 */
class nbn_export_core Watcher : public Object, public interfaces::IRunnable {
    class Impl;
    std::unique_ptr<Impl> m_spImpl{};

   public:
    /**
     * @brief Constructs a watcher for an existing directory.
     * @param directory Directory whose immediate regular files are watched.
     * @throws std::invalid_argument when directory is not an existing directory.
     */
    explicit Watcher(std::filesystem::path directory);

    /** @brief Stops the watcher and waits for its worker to finish. */
    ~Watcher() override;

    /** @brief Returns the directory being watched. */
    [[nodiscard]] auto directory() const -> const std::filesystem::path&;

    /** @brief Returns the signal emitted with the path of a changed file. */
    auto signalChanged() -> signal_ptr<std::string> { return signal<std::string>(); }

    /** @brief Returns the signal emitted when watching starts. */
    auto signalStarted() -> signal_ptr<> { return signal<>(); }

    /** @brief Returns the signal emitted when watching stops. */
    auto signalStopped() -> signal_ptr<> { return signal<>(); }

    void run() override;
    void stop() override;
    [[nodiscard]] auto isRunning() const -> bool override;
    [[nodiscard]] auto isStopRequested() const -> bool override;
    void wait() override;
};

}  // namespace nbn::core::filesystem
