# Benchmark report

- **Tool:** normal
- **Executable:** `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/core/utilities/benchmark_serialization`
- **Started:** 2026-08-15T13:43:00-04:00

The raw output for this run is stored in `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/benchmark-results`. This page is the published entry point for the run.

[PASS]

## Results

| Benchmark | Iterations | Samples | Median latency (s) | P90 latency (s) | P95 latency (s) | Median throughput (ops/s) |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| deserialize<string> escaped | 10000 | 15 | 0.097197 | 0.104363 | 0.132626 | 102884.00 |
| deserialize<variant> numeric array | 5000 | 15 | 0.475472 | 0.661859 | 0.668430 | 10515.90 |
| json::parseObject map extraction | 5000 | 15 | 0.843615 | 0.880625 | 1.122110 | 5926.88 |
| serialize<string> escaped | 10000 | 15 | 0.018464 | 0.018771 | 0.019492 | 541596.00 |
