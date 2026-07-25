#pragma once

#include "ApplicationMainThread.h"

namespace nbn::core {

class ApplicationThreadsManager final : public Object {
   public:
    explicit ApplicationThreadsManager();
    ~ApplicationThreadsManager() noexcept override;

    auto run() -> void;
    auto stop() -> void;
    auto wait() -> void;

    auto submit(std::shared_ptr<Task> spTask) noexcept -> void;
    auto submit(std::shared_ptr<Thread> spTask) noexcept -> void;

#ifdef NBN_LOGGER_TEST_HOOK
    [[nodiscard]] auto popForTest(std::shared_ptr<Thread>& spTask) -> bool;
#endif

   private:
    class Impl;
    std::unique_ptr<Impl> m_spImpl{};
};

}  // namespace nbn::core
