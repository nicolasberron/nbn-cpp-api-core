#pragma once

#include "../Task.h"
#include "Thread.h"

namespace nbn::core {

class ApplicationMainThread final : public Thread {
   public:
    explicit ApplicationMainThread();
    ~ApplicationMainThread() noexcept override;

    auto run() -> void override;
    auto wait() -> void override;

    auto submit(std::shared_ptr<Task> spTask) noexcept -> void;

   private:
    class Impl;
    std::unique_ptr<Impl> m_spImpl{};
};

}  // namespace nbn::core
