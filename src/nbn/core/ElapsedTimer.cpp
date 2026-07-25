#include <thread>

#include "ElapsedTimer.h"
#include "private/Thread.h"

namespace nbn::core {

// Factory registration
static auto const registerToFactory{[]() { return Object::factoryRegister<ElapsedTimer>(); }()};

class ElapsedTimer::Impl : public Thread {
   public:
    Impl() = default;

    void start() {
        m_start = std::chrono::high_resolution_clock::now();
        run([]() { std::this_thread::yield(); });
    }

    [[nodiscard]] auto elapsed() -> std::chrono::nanoseconds {
        if (m_start.time_since_epoch().count() == 0) {
            return std::chrono::nanoseconds::zero();
        }
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_start);
    }

   private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start{};
};

// ElapsedTimer class implementation

ElapsedTimer::ElapsedTimer() : m_spImpl{std::make_unique<Impl>()} {}

ElapsedTimer::~ElapsedTimer() = default;

void ElapsedTimer::start() {
    m_spImpl->start();
}

[[nodiscard]] auto ElapsedTimer::elapsed() const -> std::chrono::nanoseconds {
    return m_spImpl->elapsed();
}

[[nodiscard]] auto ElapsedTimer::hasExpired(std::chrono::nanoseconds nanoSeconds) const -> bool {
    return elapsed() >= nanoSeconds;
}

}  // namespace nbn::core