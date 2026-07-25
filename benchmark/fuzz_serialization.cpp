#include <cstddef>
#include <cstdint>
#include <string>

#include <nbn/core/Application.h>
#include <nbn/core/detail/serialization.h>

extern "C" auto LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size)
    -> int {  // NOLINT(readability-identifier-naming)
    constexpr int fuzzerSuccess{0};
    static const auto applicationInitialized = [] {
        nbn::core::Application::init({"fuzz_serialization"});
        return true;
    }();
    (void)applicationInitialized;
    const std::string input(reinterpret_cast<const char*>(data), size);  // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    try {
        const auto value = nbn::core::serialization::deserialize<nbn::core::serializable_variants_t>(input);
        (void)value;
    } catch (...) {
        return fuzzerSuccess;
    }
    return fuzzerSuccess;
}
