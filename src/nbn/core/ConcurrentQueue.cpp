#include "ConcurrentQueue.h"

#ifdef NBN_CONCURRENT_QUEUE_TEST_HOOK
#include <atomic>

namespace nbn::core::detail::concurrent_queue_test {

auto hookStorage() noexcept -> std::atomic<hook_t>& {
    static std::atomic<hook_t> hook{nullptr};
    return hook;
}

auto setHook(hook_t callback) noexcept -> void {
    hookStorage().store(callback, std::memory_order_release);
}

auto invokeHook() noexcept -> void {
    const auto callback = hookStorage().load(std::memory_order_acquire);
    if (callback != nullptr) {
        callback();
    }
}

}  // namespace nbn::core::detail::concurrent_queue_test
#endif
