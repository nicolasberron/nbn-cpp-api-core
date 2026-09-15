# nbn benchmark utilities

This directory contains standalone, opt-in benchmarks and a reusable
header-only runner. The runner uses only the C++23 standard library; `nbn-core`
is linked only by benchmark executables, so the production library remains
dependency-free. The installed header is available to downstream Conan
packages as `nbn/benchmark/benchmark_support.h`, and the CMake package exports
the header-only `nbn::benchmark` target for reuse by downstream benchmarks.

## Build

Configure with `NBN_BUILD_BENCHMARKS=ON` and build only the desired target:

- `benchmark_serialization`: normal timing plus optional JSON output.
- `benchmark_serialization_sanitized`: available under the
  `linux-clang19-asan` and `linux-clang19-ubsan` Conan profiles.
- `fuzz_serialization`: available under the `linux-clang19-fuzz` Conan profile.

The repository's build directory and presets are Conan-managed. Select
instrumentation through its matching Conan profile and use the same profile
for the core library. Do not enable benchmarks in normal production builds.

## Running tools

After activating the Conan build environment,
`$NBN_CPP_API_BUILD_SYSTEM_SCRIPTS_DIR/run_benchmarks.sh` accepts `--build-dir`,
`--executable`, `--tool`, and `--output-dir`. Tools are `normal`, `perf`,
`heaptrack`, `callgrind`, `massif`, `memcheck`, `asan`, `ubsan`, and `fuzz`.
Optional tools are detected before execution. Results include metadata and are
written below the output directory, which defaults to
`tmp/quality-reports/benchmark`. The directory's `README.md` is the main page
for the run.

Normal and `perf` runs are timing runs. Heaptrack, Valgrind, sanitizers, and
fuzzing are diagnostic runs; their output must not be compared as performance
measurements because instrumentation changes execution substantially.

The normal executable writes a JSON timing report when invoked with
`--json PATH`, and supports `--samples N` and `--warmup N`.

## Tool installation

For Debian/Ubuntu systems, see [tools-install.md](tools-install.md). Its
fenced `bash` blocks can be executed directly from VS Code with the
`nbn-vscode-interactive-markdown` extension.

## Limitations

The framework is intentionally small: it does not pin CPUs, control scheduler
noise, or provide statistical confidence intervals. `perf`, heaptrack,
Valgrind, sanitizers, and libFuzzer are external optional tools and may require
permissions, platform-specific setup, or a separately instrumented target.
