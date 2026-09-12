# Benchmark report

- **Tool:** normal
- **Executable:** `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/core/utilities/benchmark_serialization`
- **Started:** 2026-09-12T14:55:30-04:00

The raw output for this run is stored in `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/benchmark-results`. This page is the published entry point for the run.

[PASS]

## Results

| Benchmark | Iterations | Samples | Median latency (s) | P90 latency (s) | P95 latency (s) | Median throughput (ops/s) |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| deserialize<string> escaped | 10000 | 15 | 0.098706 | 0.100890 | 0.101319 | 101311.00 |
| deserialize<variant> numeric array | 5000 | 15 | 0.476297 | 0.502738 | 0.510354 | 10497.60 |
| json::parseObject map extraction | 5000 | 15 | 0.857585 | 0.945167 | 1.120960 | 5830.33 |
| serialize<string> escaped | 10000 | 15 | 0.018559 | 0.019878 | 0.020747 | 538818.00 |
