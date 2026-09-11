# Benchmark report

- **Tool:** normal
- **Executable:** `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/core/utilities/benchmark_serialization`
- **Started:** 2026-09-05T17:27:20-04:00

The raw output for this run is stored in `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/benchmark-results`. This page is the published entry point for the run.

[PASS]

## Results

| Benchmark | Iterations | Samples | Median latency (s) | P90 latency (s) | P95 latency (s) | Median throughput (ops/s) |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| deserialize<string> escaped | 10000 | 15 | 0.106059 | 0.108839 | 0.112929 | 94287.50 |
| deserialize<variant> numeric array | 5000 | 15 | 0.441474 | 0.445092 | 0.448629 | 11325.70 |
| json::parseObject map extraction | 5000 | 15 | 0.806608 | 0.816999 | 0.832148 | 6198.80 |
| serialize<string> escaped | 10000 | 15 | 0.017889 | 0.018769 | 0.018937 | 559015.00 |
