---
name: nbn-implement-auto-coverage
description: "Use when improving production C++ test coverage, investigating llvm-cov uncovered lines, or adding focused tests for uncovered branches in this repository."
---

# Implement automatic coverage improvements

Use this workflow for LLVM source-based coverage changes in the nbn-cpp-api repository.

## Scope and constraints

- Work only in the main codebase;
- Follow strict test-driven development: add or update a deterministic failing test before changing production code, make it pass with the smallest implementation, then refactor under green tests.
- A coverage task is not complete until fresh LLVM metrics report 100% for production regions, functions, and lines. Do not accept a lower threshold or hide an uncovered reachable path with an exclusion.
- Prefer focused unit tests over production changes.
- Remove code only when it is demonstrably unreachable and has no references.
- Do not use magic numbers. Prefer named constants or existing project constants.
- Follow the repository's C++ Core Guidelines and existing test style.
- Use `NOLINT` and LLVM coverage exclusions only when no code or test design can avoid them.
- For templates, coverage MUST exercise every relevant project-supported type category and concrete instantiation. Interpret "all possible types" as all types the project supports or meaningfully relies on, not every type expressible in C++. Do not aggregate one template instantiation's coverage as evidence for another; retain a deterministic test for each concrete instantiation reported by the template coverage analysis.
- Remove GCC-specific coverage exclusions instead of carrying them into LLVM coverage. Retain only narrowly justified `LLVM-COV EXCL_LINE`, `LLVM-COV EXCL START`, and `LLVM-COV EXCL STOP` markers.
- This skill is explicitly authorized to run the project's CMake configure step, all CTest tests, and the coverage task as part of its iteration loop.
- Keep configure, build, test, and coverage output visible; never hide or redirect it.
- Coverage must use LLVM on both supported hosts: the Clang `linux-clang19-coverage` profile on Linux and a Windows `clang-cl` coverage profile using the MSVC ABI/toolchain. Do not use `default`, GCC, or standalone MSVC compiler coverage.
- On Windows, keep the MSVC headers, libraries, linker, and CUDA-compatible toolchain environment intact while using `clang-cl` as the compiler frontend.
- Enable `NBN_LLVM_COVERAGE_ENABLE` and run coverage from the generated LLVM build directory, where `.profraw`, `.profdata`, and LLVM HTML reports are generated. Record host, LLVM version, frontend, ABI/toolchain, and profile in the report.

## Workflow

1. Inspect the current repository state and read the relevant production and test files before editing.
2. Select the LLVM coverage profile for the host: `linux-clang19-coverage` on Linux or the Windows `clang-cl` coverage profile with the MSVC ABI/toolchain. Use the project-provided coverage task, which runs `scripts/run_llvm_coverage.py` from the generated build directory. Do not select `default`, GCC, standalone MSVC compiler coverage, or substitute an ad-hoc coverage command when the project-provided coverage task is available. Intermediate `.profraw`, `.profdata`, text reports, and HTML files are generated under `<coverage-build-dir>/coverage-llvm/`; only the HTML site is published to `quality-reports/coverage/html/`.
3. Identify uncovered executable production lines from the `llvm-cov-19 report` output. Ignore generated files and inspect every former GCC-only exclusion rather than preserving it automatically.
4. **Mandatory template coverage gate:** if the target is a `.h`/`.hpp` file containing `template`, a templated class/function, or the input is an LLVM HTML report for such a file, stop normal coverage analysis and run `template_coverage.py` first. This is required even when the debugger shows a breakpoint hit. Run:
   `python3 scripts/template_coverage.py <coverage-build-dir> --source '<TemplateName>'`
   For the queue, use `--source ConcurrentQueue`. Do not proceed to add tests, exclusions, or explain coverage until the script output has been inspected.
5. Use the script output as the authoritative mapping from uncovered source lines to concrete C++ types. The script reads the same instrumented production binary and merged `coverage.profdata`, enables `-show-instantiations=true`, demangles LLVM symbols, and reports uncovered lines per instantiation. A breakpoint in `T=int` does not cover `T=std::string`; never aggregate template instantiations by source line alone.
6. If the target instantiation is not present in the production report, run the script against the relevant test executable and its per-test profile, or inspect the per-test HTML report. Do not conclude that a template method is uninstantiated merely because it is absent from the production-library aggregate.
7. Trace each uncovered line to its controlling condition, migration step, error path, or dispatch branch.
8. Search existing tests and test registration before adding a new test. Tests named `test_*()` are discovered by the project's CMake test generation.
9. Add the smallest deterministic test fixture that reaches the branch. Use valid serialized JSON and the exact version or state needed to enter a migration path.
10. Preserve existing formatting and APIs. Use `apply_patch` for source edits.
11. Check diagnostics after editing. If a diagnostic points at an unrelated end-of-file location, inspect generated test registration and stale language-server state before changing valid source.
12. If test functions were added or removed, run the project's CMake configure step for the `linux-clang19-coverage` profile before testing so generated test mains and CTest registrations are refreshed.
13. Build only the affected target, keep all output visible, and run the relevant tests. Run all CTest tests before collecting aggregate coverage when the change affects shared production code.
14. Run the project coverage task after all tests complete. Inspect freshly generated LLVM reports, not an earlier report from another profile.
15. Run `python3 scripts/coverage_metrics.py <coverage-build-dir>/coverage-llvm` and repeat steps 3-15 until it reports 100% for regions, functions, and lines in `production.txt`. Each iteration must address the currently reported uncovered executable code rather than adding broad or redundant tests.
16. Report the final 100% coverage metrics, changed files, test result, and the visible configure/build/test/coverage commands or tasks executed.

## Coverage investigation notes

- A stale coverage report can show old source line mappings. Confirm that the report was generated after the latest build and test run.
- LLVM may map compiler-generated regions differently between Linux Clang and Windows `clang-cl`. Remove old GCC-only exclusions first; use `LLVM-COV EXCL_*` only after the host-specific LLVM report demonstrates that a non-actionable region remains.
- For template coverage, use `scripts/template_coverage.py` against the same `linux-clang19-coverage` binary and `coverage.profdata` used by the aggregate report. It distinguishes concrete `ConcurrentQueue<T>` symbols and prevents treating one covered instantiation as coverage for another.
- Never skip `scripts/template_coverage.py` for a template header or template HTML report. The required order is: regenerate coverage, run `coverage_metrics.py`, run `template_coverage.py`, then inspect and fix the reported concrete instantiations. The template report is required evidence in the iteration, not an optional optimization.
- When the user provides a `file:///.../*.html` LLVM report, resolve the corresponding source file and coverage build directory, then run `template_coverage.py` before answering or editing. Do not answer from the HTML line count alone.
- The production aggregate may contain only template instantiations emitted into `libnbn-core.so`; test-only instantiations can live in test executables. If a type is absent, inspect the relevant per-test report or the test binary rather than assuming the production report includes it.
- The aggregate report must be investigated with the same input set used by `run_llvm_coverage.py`: the production library plus every instrumented `test_*` executable. A command using only `libnbn-core.so` cannot reproduce a deficit contributed by a test-only header instantiation.
- A header can report `100.00%` regions and functions while line coverage is lower. In this case, use `llvm-cov report` with each relevant test executable and request `--show-instantiations`; look for `Unexecuted instantiation:` entries. LLVM may omit the corresponding zero-count header line from normal `llvm-cov show` output, so the absence of a `0|` line is not proof of complete coverage.
- For an aggregate header deficit, identify the unexecuted concrete instantiation first, then add a deterministic call in the test binary that owns it. For example, an unused `Object::signal<const int>()` helper can leave `detail/Object.h` at `96.43%` even when all visible slot instantiations are covered; calling that helper twice covers both creation and cache-hit paths.
- `template_coverage.py` currently reports zero-count lines inside rendered mangled function records, but it may not report LLVM's separate `Unexecuted instantiation:` records. Always inspect `llvm-cov show --show-instantiations` when the file summary has a line deficit but the template script reports no uncovered lines.
- To determine template argument types from an uncovered symbol, first pass the complete mangled name through `c++filt`; it decodes the Itanium C++ ABI name. When inspecting an LLVM report, pass `-Xdemangler=c++filt` to `llvm-cov show` so the report displays demangled template arguments. Common ABI fragments include `A7_c` -> `char[7]`, `A10_c` -> `char[10]`, `A12_c` -> `char[12]`, `Dn` -> `decltype(nullptr)`, `i` -> `int`, `j` -> `unsigned int`, `m` -> `unsigned long`, `St6vectorIi...` -> `std::vector<int>`, and `INS0_4Task7RunMode...` -> `nbn::core::Task::RunMode`. Array extents include the null terminator for string literals. Use the full demangled signature, not a shortened symbol prefix, because mixed instantiations such as `equal<char[7], std::string>` and `equal<char[7], std::string_view>` require different tests.
- For each demangled instantiation, inspect the argument pair and the condition outcome. A zero count on the fatal line usually means the instantiation exists but every observed comparison was equal. Add an unequal test for each reachable pair. `std::nullptr_t` is an exception: it has only the value `nullptr`, so `equal<std::nullptr_t, std::nullptr_t>` cannot naturally reach the unequal branch. Prefer a compile-time no-op for that impossible failure path; use a narrowly justified LLVM exclusion only if the code cannot be structured to remove the unreachable instantiation-specific branch.
- After fixing the reported instantiations, regenerate the aggregate report and inspect it again. Do not stop after a per-test report: aggregate coverage can expose additional concrete template instantiations emitted by other test binaries. Treat every newly reported symbol as a separate type pair and repeat the demangle -> classify -> test-or-justify cycle until only accepted exclusions remain.
- A newly added `test_*()` function may not appear in the generated test main until CMake is reconfigured.
- Always run all CTest tests before collecting coverage; otherwise old `.gcda` counters can make the result incomplete or misleading.
- For migration code, verify which top-level version field takes precedence and whether earlier migration steps remove or rewrite later fields.
- For nested JSON, verify whether the parser expects a JSON string value such as `"#ffffff"` rather than an escaped string containing quote characters.
- When a factory branch is uncovered, verify that the concrete type is linked and registered in the test target before changing production code.
- If coverage does not improve after an iteration, inspect test discovery, generated test mains, build configuration, and counter files before adding another test.
- Do not declare success from source-level reasoning alone: the fresh LLVM report must confirm the required coverage target.

## Completion criteria

A coverage task is complete when:

- `python3 scripts/coverage_metrics.py <coverage-build-dir>/coverage-llvm` reports 100% for regions, functions, and lines.
- All CTest tests pass after the final configure/build/test cycle.
- A fresh LLVM coverage report confirms the target lines are covered; only explicitly accepted LLVM exclusions may remain.
- No unrelated files or generated files were modified.
