#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

#include <nbn/core/Application.h>
#include <nbn/core/detail/serialization.h>

extern "C" int llvmFuzzerTestOneInput(const std::uint8_t* data, std::size_t size) __asm__("LLVMFuzzerTestOneInput");

extern "C" auto llvmFuzzerTestOneInput(const std::uint8_t* data, std::size_t size) -> int {
    constexpr int fuzzerSuccess{0};
    static const auto applicationInitialized = [] {
        nbn::core::Application::init({"fuzz_serialization"});
        return true;
    }();
    (void)applicationInitialized;
    std::string input(size, '\0');
    if (!input.empty()) {
        std::memcpy(input.data(), data, size);
    }
    try {
        const auto value = nbn::core::serialization::deserialize<nbn::core::serializable_variants_t>(input);
        (void)value;
    } catch (...) {
        return fuzzerSuccess;
    }
    return fuzzerSuccess;
}
