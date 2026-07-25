---
name: nbn-implement-auto-benchmarking
description: "Use when improving serialization performance, comparing benchmark baselines, investigating profiling results, or adding focused benchmark cases in this repository."
---

# Implement automatic benchmark and profiling improvements

Use this workflow for measurement-driven performance changes in the nbn-cpp-api
repository. The goal is to make one small change at a time, measure it against
the same baseline, and keep only changes supported by reproducible evidence.

## Scope and constraints

- Work only in the main codebase
- Follow strict test-driven development for every production-code change: write or update a failing regression test first, make it pass with the smallest change, then refactor while tests remain green.
- Maintain 100% LLVM production coverage for all production code added or modified by the benchmark task. Benchmark-only harness code must still have deterministic tests where practical and must not be used to hide production coverage deficits.
- Keep `nbn-core` free of benchmark and profiler dependencies.
- Prefer focused benchmark cases over broad synthetic workloads.
- For extreme-performance work, isolate each hot path in a dedicated benchmark
  before changing production code. Optimize allocation count, copies, parsing
  passes, and type-erasure overhead in that order, and preserve a representative
  end-to-end workload as a regression guard.
- Do not use magic numbers in production C++ or benchmark code. Define workload
  sizes, thresholds, iteration counts, and other meaningful numeric values as
  descriptive `constexpr` variables at the narrowest appropriate scope, or reuse
  an existing named project constant.
- Prefer `emplace_back` over `push_back` when appending constructed elements to
  standard containers. Use `push_back` when `emplace_back` is unavailable (for
  example, `std::string`) or when it is the clearer operation.
- Preserve public APIs unless a measured improvement requires an API change.
- Use `NOLINT` only when no design change avoids the warning.
- Keep configure, build, benchmark, and profiler output visible. Do not hide
  output or replace repository scripts with ad-hoc commands when an equivalent
  project script exists.
- Do not compare instrumented runs with normal timing runs. Sanitizers,
  Valgrind, Heaptrack, and fuzzing are diagnostic workflows.
- Use the LLVM `linux-clang19-debug` build for repeatable normal serialization
  baselines on Linux.
- Use the Windows `clang-cl` build with the MSVC ABI/toolchain for Windows
  baselines; do not compare it with a standalone MSVC compiler build.
- Use LLVM/Clang for libFuzzer on Linux and Windows where the platform
  toolchain supports it. Keep host-specific results separate.
- Maintain the benchmark history in a tracked CSV file, normally
  `doc/benchmark-history.csv`. Do not store generated benchmark output in the
  repository unless it is explicitly requested.
- Every accepted measurement iteration must be committed. Use a monotonically
  increasing four-digit iteration number in both the CSV row and commit
  subject, for example `benchmark-0001: improve serialization throughput`.
  The commit subject must explicitly refer to the matching CSV iteration.
- Do not claim "no more performance gain" from a noisy mixed benchmark. If a
  candidate improves one hot path but another workload is unstable, add or use
  an isolated benchmark and repeat matched runs before accepting or rejecting
  the candidate.

## Required helpers

- `scripts/benchmark_metrics.py` summarizes benchmark JSON, compares two runs,
  and optionally enforces a regression threshold.
- `scripts/profiling_metrics.py` extracts stable counters from `perf stat`
  output and records profile metadata without requiring third-party Python
  packages.

## Workflow

1. Inspect the current repository state and read the relevant serializer,
   benchmark, and test files before editing.
2. Read the tracked benchmark CSV, determine the next iteration number as one
  greater than its largest iteration, and confirm the worktree is clean before
  starting. Never reuse an iteration number. If the CSV does not exist, create
  its header and use iteration `0001`.
3. Confirm the build directory, compiler, C++ standard, build type, CPU model,
   benchmark arguments, warmup count, sample count, and workload data. Do not
   compare results with different settings without labeling the comparison.
4. Build only the required target, normally `benchmark_serialization`. Do not
  perform a full build. Keep the complete build output visible.
5. Run a baseline with `scripts/run_benchmarks.sh --tool normal` and write the
   JSON report into a uniquely named result directory. Summarize it with
   `scripts/benchmark_metrics.py summarize`.
6. Run a focused profiler, normally `perf` or Heaptrack, against the same
   executable and arguments. Use `scripts/profiling_metrics.py` for `perf stat`
   output and retain the raw profiler files.
7. Identify the dominant operation, allocation pattern, or input shape from
   the measurements. Trace it to the smallest production or benchmark change.
8. Add or update a deterministic benchmark case when the workload does not
   expose the suspected cost. Keep input generation outside the timed region.
9. Apply the smallest code change. Preserve output semantics and error
   behavior. Check diagnostics after editing.
10. Rebuild only the affected benchmark target and rerun the exact baseline
   command. Use `benchmark_metrics.py compare` to compare median and tail
   latency, throughput, and workload identity.
11. Reject a change when the result is within noise, regresses a protected
    metric, changes semantics, or cannot be reproduced. Repeat measurements
    when variance is high rather than tuning thresholds immediately.
    For extreme-performance work, require the focused hot-path benchmark to
    improve and require every end-to-end guard benchmark to remain within the
    threshold.
12. Run the focused tests and fresh LLVM coverage after every
  correctness-sensitive production change. The production coverage report MUST
  remain at 100% for regions, functions, and lines. Run sanitizer, Memcheck, or
  fuzz diagnostics after correctness-sensitive serialization changes. Do not
  use their timings as performance evidence.
13. Record the accepted change, command lines, raw result paths, compiler/build
    settings, and measured improvement in the relevant documentation or change
    summary.
14. Append exactly one row to the tracked benchmark CSV for the accepted
   iteration. The row must include at least: `iteration`, `date`, `commit_ref`,
   `workload`, `benchmark_name`, `compiler`, `build_type`, `cpu`, `arguments`,
   `warmup_count`, `sample_count`, `baseline_report`, `result_report`,
   `median_latency_seconds`, `p95_latency_seconds`,
   `median_throughput_ops_per_second`, `latency_delta_percent`,
   `p95_delta_percent`, `throughput_delta_percent`, `threshold_percent`,
   `status`, and `notes`. Keep paths relative to the repository or record
   stable external artifact paths; do not use temporary absolute paths.
15. Commit the accepted source/test changes and the CSV row together. The
   commit subject must be `benchmark-NNNN: <short description>`, where `NNNN`
   is the CSV `iteration` and `commit_ref` is the same value (or the exact
   subject). Do not amend an earlier benchmark iteration and do not commit a
   failed or rejected measurement. Verify with `git show --stat --oneline`
   that the numbered commit contains the implementation and its matching CSV
   entry.

## Iteration rules

- Keep the original baseline immutable; create a new result directory per run.
- Compare the same benchmark names and sample configuration.
- Treat median latency and median throughput as primary metrics; inspect p95
  latency for tail regressions.
- A default regression threshold is 5 percent. Set a stricter threshold only
  after measuring run-to-run noise on the same machine.
- A performance improvement is not accepted without a correctness check.
- If results disagree with profiler evidence, repeat the run and inspect CPU
  frequency, thermal throttling, background load, and input setup.
- Do not optimize code solely because it appears in a profile; first establish
  that it affects a representative benchmark workload.
- Treat the CSV as append-only history. Never rewrite prior measurements,
  reorder rows, or overwrite the original baseline. If a measurement is
  invalid, record it as `status=invalid` with an explanation rather than
  deleting it; invalid iterations must not be committed as accepted changes.
- The iteration number is the stable cross-reference between a commit and the
  CSV. A Git hash may be added as supplemental metadata after committing, but
  it must not replace the numbered reference.

## Completion criteria

A benchmarking task is complete when:

- The changed benchmark or production code builds successfully.
- The relevant correctness tests and diagnostic runs pass.
- Before/after JSON reports and raw profiler artifacts are retained.
- `benchmark_metrics.py compare` reports the measured deltas and no protected
  regression, or the regression is explicitly justified.
- A tracked CSV row records the complete accepted measurement and uses the
  same numbered `commit_ref` as its commit subject.
- `git show --stat --oneline <numbered benchmark commit>` confirms that the
  commit contains the code change and matching CSV entry.
- The final report names the workload, compiler, build type, command, samples,
  profiler, observed result, CSV path, iteration number, and commit subject.
- No unrelated files, generated build files, raw result artifacts are committed; only the tracked CSV and relevant source, test, or
  documentation changes belong in the numbered commit.