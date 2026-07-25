# Benchmark report

- **Tool:** normal
- **Executable:** `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/benchmark/benchmark_serialization`
- **Started:** 2026-08-05T11:29:25-04:00

The raw output for this run is stored in `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/benchmark-results`. This page is the published entry point for the run.

<span style="color:green">✅ PASS</span>

## Results

| Benchmark | Iterations | Samples | Median latency (s) | P90 latency (s) | P95 latency (s) | Median throughput (ops/s) |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| deserialize<string> escaped | 10000 | 15 | 0.098509 | 0.108525 | 0.119713 | 101514.00 |
| deserialize<variant> numeric array | 5000 | 15 | 0.480050 | 0.513104 | 0.514446 | 10415.60 |
| json::parseObject map extraction | 5000 | 15 | 0.850534 | 0.950261 | 0.994204 | 5878.66 |
| serialize<string> escaped | 10000 | 15 | 0.018168 | 0.018868 | 0.018937 | 550405.00 |
