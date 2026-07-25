#include <charconv>
#include <cstddef>
#include <exception>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <nbn/core/Application.h>
#include <nbn/core/detail/serialization.h>

#include "benchmark_support.h"

namespace {

constexpr std::size_t stringIterations{10000U};
constexpr std::size_t arrayIterations{5000U};
constexpr std::size_t objectIterations{5000U};
constexpr int exitSuccess{0};
constexpr int exitFailure{1};

template <typename T>
inline auto doNotOptimize(const T& value) -> void {
#if defined(__GNUC__) || defined(__clang__)
    asm volatile("" : : "g"(value) : "memory");
#else
    volatile const auto* sink = &value;
    (void)sink;
#endif
}

[[nodiscard]] auto parsePositiveSize(std::string_view value, std::size_t& out) -> bool {
    out = 0U;
    if (value.empty()) {
        return false;
    }
    const auto [ptr, error] = std::from_chars(value.data(), value.data() + value.size(), out);
    return error == std::errc{} && ptr == value.data() + value.size() && out > 0U;
}

}  // namespace

auto main(int argc, char** argv) -> int {
    try {
        using namespace nbn::core;
        using nbn::benchmark::Configuration;
        using nbn::benchmark::Result;

        Configuration configuration;
        std::string jsonPath;
        std::vector<std::string> args(argv, std::next(argv, argc));
        Application::init(args);
        const auto& cliArgs = Application::arguments();
        for (std::size_t index{}; index < cliArgs.size(); ++index) {
            const std::string_view argument{cliArgs.at(index)};
            if (argument == "--help" || argument == "-h") {
                std::cout << "benchmark_serialization options:\n"
                             "  --samples N   Number of measured runs (default: 15)\n"
                             "  --warmup N    Number of warmup iterations (default: 100)\n"
                             "  --json PATH   Write machine-readable timing results\n";
                return exitSuccess;
            }
            if ((argument == "--samples" || argument == "--warmup" || argument == "--json") && index + 1U < cliArgs.size()) {
                const auto value = std::string_view{cliArgs.at(++index)};
                if (argument == "--json") {
                    jsonPath = value;
                    continue;
                }
                std::size_t parsedValue{};
                if (!parsePositiveSize(value, parsedValue)) {
                    std::cerr << std::format("Invalid numeric value '{}' for option '{}'.\n", value, argument);
                    return exitFailure;
                }
                (argument == "--samples" ? configuration.sampleRuns : configuration.warmupIterations) = parsedValue;
                continue;
            }
            std::cerr << std::format("Unknown or incomplete option '{}'. Use --help for usage.\n", argument);
            return exitFailure;
        }

        const std::string escapedInput = R"("Line1\nLine2\tTabbed\\Slash\/Quote\"Unicode:\u00DF")";
        const std::string arrayInput = R"([1,-2,3.14159,0,42,6.02e23,-9.1e-3,8,9,10])";
        const std::string objectInput =
            R"({"id":12345,"name":"benchmark","active":true,"payload":{"k1":"v1","k2":"v2","arr":[1,2,3,4,5],"nested":{"x":1,"y":2}},"tags":["a","b","c"]})";
        std::vector<Result> results;
        results.push_back(nbn::benchmark::measure("deserialize<string> escaped", stringIterations, configuration, [&]() {
            const auto value = serialization::deserialize<std::string>(escapedInput);
            doNotOptimize(value);
        }));
        results.push_back(nbn::benchmark::measure("deserialize<variant> numeric array", arrayIterations, configuration, [&]() {
            const auto value = serialization::deserialize<serializable_variants_t>(arrayInput);
            doNotOptimize(value);
        }));
        results.push_back(nbn::benchmark::measure("json::parseObject map extraction", objectIterations, configuration, [&]() {
            std::unordered_map<std::string, std::string> output;
            std::size_t position{};
            serialization::json::parseObject(output, objectInput, position);
            doNotOptimize(output);
            doNotOptimize(position);
        }));
        results.push_back(nbn::benchmark::measure("serialize<string> escaped", stringIterations, configuration, [&]() {
            const auto serialized = serialization::serialize(std::string{"Line1\nLine2\tTabbed\\Slash/Quote\"Unicode:ß"});
            doNotOptimize(serialized);
        }));

        for (const auto& result : results) {
            nbn::benchmark::writeHuman(std::cout, result);
        }
        if (!jsonPath.empty()) {
            std::ofstream output(jsonPath);
            if (!output) {
                std::cerr << std::format("Could not open JSON output '{}'.\n", jsonPath);
                return exitFailure;
            }
            nbn::benchmark::writeJson(output, results);
        }
        return exitSuccess;
    } catch (const std::exception& exception) {
        std::cerr << std::format("benchmark_serialization failed: {}\n", exception.what());
        return exitFailure;
    } catch (...) {
        std::cerr << "benchmark_serialization failed: unknown exception\n";
        return exitFailure;
    }
}
