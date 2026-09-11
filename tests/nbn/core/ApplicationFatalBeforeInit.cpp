#include <cstdlib>
#include <stdexcept>
#include <string_view>

#include <nbn/core/Application.h>

namespace {

constexpr auto kFatalMessage = "fatal before application initialization";

}  // namespace

auto main() -> int {
    try {
        nbn::log::fatal(kFatalMessage);
    } catch (const std::runtime_error& exception) {
        return std::string_view{exception.what()} == kFatalMessage ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    return EXIT_FAILURE;
}