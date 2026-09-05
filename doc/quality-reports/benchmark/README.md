# Benchmark report

- **Tool:** normal
- **Executable:** `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/core/utilities/benchmark_serialization`
- **Started:** 2026-08-15T19:02:01-04:00

The raw output for this run is stored in `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/benchmark-results`. This page is the published entry point for the run.

[PASS]

## Results

| Benchmark | Iterations | Samples | Median latency (s) | P90 latency (s) | P95 latency (s) | Median throughput (ops/s) |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| deserialize<string> escaped | 10000 | 15 | 0.096713 | 0.099826 | 0.113996 | 103398.00 |
| deserialize<variant> numeric array | 5000 | 15 | 0.463465 | 0.484214 | 0.485946 | 10788.30 |
| json::parseObject map extraction | 5000 | 15 | 0.834781 | 0.852529 | 0.852849 | 5989.59 |
| serialize<string> escaped | 10000 | 15 | 0.018408 | 0.019410 | 0.019489 | 543232.00 |
