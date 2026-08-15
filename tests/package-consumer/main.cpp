#include <nbn/benchmark/benchmark_support.h>
#include <nbn/core/Color.h>

#include <sstream>

int main() {
    nbn::core::Color color{};
    const auto result = nbn::benchmark::measure("consumer smoke", 1U, nbn::benchmark::Configuration{}, [] {});
    std::ostringstream output;
    nbn::benchmark::writeJson(output, {result});
    return color.red() != nullptr && output.str().find("consumer smoke") != std::string::npos ? 0 : 1;
}