#pragma once

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <iomanip>
#include <limits>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace nbn::benchmark {

struct Configuration {
    std::size_t warmupIterations{100U};
    std::size_t sampleRuns{15U};
};

struct Result {
    std::string name;
    std::size_t iterations{};
    std::size_t warmupIterations{};
    std::size_t sampleRuns{};
    double medianLatencySeconds{};
    double p90LatencySeconds{};
    double p95LatencySeconds{};
    double medianThroughput{};
    double p90Throughput{};
    double p95Throughput{};
};

namespace detail {

inline auto percentileIndex(std::size_t sampleCount, std::size_t percentile) -> std::size_t {
    constexpr std::size_t percentage{100U};
    const auto rank = (percentile * sampleCount + percentage - 1U) / percentage;
    return rank == 0U ? 0U : std::min(rank - 1U, sampleCount - 1U);
}

inline auto percentileValue(std::vector<double> values, std::size_t percentile) -> double {
    std::sort(values.begin(), values.end());
    return values.at(percentileIndex(values.size(), percentile));
}

inline auto escapeJson(std::string_view value) -> std::string {
    std::string escaped;
    for (const char character : value) {
        if (character == '\\' || character == '"') {
            escaped.push_back('\\');
        }
        escaped.push_back(character);
    }
    return escaped;
}

}  // namespace detail

template <typename Callable>
auto measure(std::string_view name, std::size_t iterations, const Configuration& configuration, Callable&& callable) -> Result {
    auto&& operation = std::forward<Callable>(callable);
    for (std::size_t iteration{}; iteration < configuration.warmupIterations; ++iteration) {
        operation();
    }

    constexpr std::size_t medianPercentile{50U};
    constexpr std::size_t p90Percentile{90U};
    constexpr std::size_t p95Percentile{95U};
    std::vector<double> latencies;
    std::vector<double> throughputs;
    latencies.reserve(configuration.sampleRuns);
    throughputs.reserve(configuration.sampleRuns);

    for (std::size_t run{}; run < configuration.sampleRuns; ++run) {
        const auto start = std::chrono::steady_clock::now();
        for (std::size_t iteration{}; iteration < iterations; ++iteration) {
            operation();
        }
        const auto elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
        const auto safeElapsed = std::max(elapsed, std::numeric_limits<double>::min());
        latencies.push_back(safeElapsed);
        throughputs.push_back(static_cast<double>(iterations) / safeElapsed);
    }

    return {std::string{name},
            iterations,
            configuration.warmupIterations,
            configuration.sampleRuns,
            detail::percentileValue(latencies, medianPercentile),
            detail::percentileValue(latencies, p90Percentile),
            detail::percentileValue(latencies, p95Percentile),
            detail::percentileValue(throughputs, medianPercentile),
            detail::percentileValue(throughputs, p90Percentile),
            detail::percentileValue(throughputs, p95Percentile)};
}

inline auto writeHuman(std::ostream& output, const Result& result) -> void {
    constexpr int latencyPrecision{6};
    output << result.name << '\n'
           << "  samples=" << result.sampleRuns << " warmup=" << result.warmupIterations << " iterations=" << result.iterations
           << '\n'
           << std::fixed << std::setprecision(latencyPrecision) << "  latency(s): median=" << result.medianLatencySeconds
           << " p90=" << result.p90LatencySeconds << " p95=" << result.p95LatencySeconds << '\n'
           << std::setprecision(2) << "  throughput(ops/s): median=" << result.medianThroughput << " p90=" << result.p90Throughput
           << " p95=" << result.p95Throughput << '\n';
}

inline auto writeJson(std::ostream& output, const std::vector<Result>& results) -> void {
    output << "{\n  \"framework\": \"nbn-dependency-free-benchmark\",\n  \"results\": [\n";
    for (std::size_t index{}; index < results.size(); ++index) {
        const auto& result = results.at(index);
        output << "    {\n      \"name\": \"" << detail::escapeJson(result.name) << "\",\n"
               << "      \"iterations\": " << result.iterations << ",\n"
               << "      \"warmup_iterations\": " << result.warmupIterations << ",\n"
               << "      \"sample_runs\": " << result.sampleRuns << ",\n"
               << "      \"median_latency_seconds\": " << result.medianLatencySeconds << ",\n"
               << "      \"p90_latency_seconds\": " << result.p90LatencySeconds << ",\n"
               << "      \"p95_latency_seconds\": " << result.p95LatencySeconds << ",\n"
               << "      \"median_throughput_ops_per_second\": " << result.medianThroughput << ",\n"
               << "      \"p90_throughput_ops_per_second\": " << result.p90Throughput << ",\n"
               << "      \"p95_throughput_ops_per_second\": " << result.p95Throughput << '\n'
               << "    }" << (index + 1U == results.size() ? "\n" : ",\n");
    }
    output << "  ]\n}\n";
}

}  // namespace nbn::benchmark
