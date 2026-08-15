---
name: analyze-llvm-sanitizer-reports
description: "Analyze LLVM AddressSanitizer and ThreadSanitizer reports from this repository and generate linked Markdown findings, status summaries, and actionable recommendations. Use when reviewing sanitizer-results, ASan failures, TSan data races, sanitizer test logs, or sanitizer regressions."
argument-hint: "[report directory] [output directory]"
---

# Analyze LLVM sanitizer reports

Use the bundled [analyzer](./scripts/analyze_llvm_sanitizers.py) to turn existing LLVM sanitizer output into a small Markdown report set. The final review must state what is confirmed, what is informational, and the exact next action; do not merely tell the user to inspect raw logs.

## Supported toolchains

This skill is analysis-only and does not change source code. If analysis identifies a fix, the implementation MUST use strict TDD and must restore 100% LLVM production coverage before handoff.

- Linux validation uses LLVM/Clang, currently the `linux-clang19-*` build directories.
- Windows validation uses LLVM `clang-cl` with the MSVC-compatible ABI, MSVC headers/libraries, and linker integration required by CUDA.
- Do not describe GCC or standalone MSVC compiler runs as supported sanitizer evidence for this repository.
- Record the host, compiler frontend, ABI/toolchain, sanitizer, and test subset separately. A clean Linux Clang run does not establish equivalent Windows `clang-cl` coverage.
- Valgrind remains a Linux-only supplemental diagnostic; it is not a Windows validation requirement.

## Procedure

This skill is analysis-only. Do **not** launch ASan, TSan, CTest, builds, or sanitizer tasks. It consumes reports that already exist in the supplied report directory. If a supplied directory is empty, still generate the complete Markdown output and state that collection produced no input; do not omit the report set.

### Locate existing reports

The report directory is not necessarily named `sanitizer-results` and may be outside the repository. Before concluding that reports are absent, inspect workspace build roots and search for existing CTest sanitizer logs, including:

- `<build-root>/*-asan/Testing/Temporary/LastTest.log`
- `<build-root>/*-tsan/Testing/Temporary/LastTest.log`
- `<build-root>/*-tsan/Testing/Temporary/LastTestsFailed.log`
- `<build-root>/*-asan/Testing/Temporary/analysis/`
- `<build-root>/*-tsan/Testing/Temporary/analysis/`

If a generated `analysis/` directory already exists, treat its parent `Testing/Temporary` directory as the report directory and review the generated Markdown first. A valid report location may therefore be:

`/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-tsan/Testing/Temporary`

When the user supplies a report path or an attachment links directly to an analysis file, use that path even when it is outside the workspace. Do not report that no reports exist until these build-tree locations have also been checked.

When ASan and TSan were run by a combined sanitizer task, discover both sibling report directories and analyze them together. Do not analyze only the TSan directory, because that produces a page that correctly reports TSan but says ASan was not supplied.

1. From the repository root, run the analyzer against the discovered report directory. The following is only the default when the repository contains that directory:

   ```text
   python3 .github/skills/analyze-llvm-sanitizer-reports/scripts/analyze_llvm_sanitizers.py \\
       --reports sanitizer-results \\
    --output doc/quality-reports/sanitizers
   ```

   For a CTest build-tree report, use the corresponding absolute paths instead:

   ```text
   python3 .github/skills/analyze-llvm-sanitizer-reports/scripts/analyze_llvm_sanitizers.py \
       --reports /home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-tsan/Testing/Temporary \
    --output doc/quality-reports/sanitizers
   ```

   For combined ASan and TSan output, pass both report directories after `--reports`:

   ```text
   python3 .github/skills/analyze-llvm-sanitizer-reports/scripts/analyze_llvm_sanitizers.py \
       --reports /home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-asan/Testing/Temporary \
                 /home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-tsan/Testing/Temporary \
    --output doc/quality-reports/sanitizers
   ```

2. Open `doc/quality-reports/sanitizers/README.md`.
3. Review `sanitizers.md` as the single ASan/TSan page, then review `failures.md` for every recorded CTest failure and its classification and `recommendations.md` for prioritized follow-up work. The legacy `asan.md` and `tsan.md` files may also be generated for compatibility, but are not the primary report.
4. Treat each sanitizer error as actionable unless the report explicitly identifies an intentional test-process termination. Do not suppress a report merely because the test is instrumented.
5. Deduplicate repeated reports by sanitizer, normalized summary, and first project source location where possible. Distinguish one root cause repeated across tests from independent defects.
6. Link project source locations when they are present. Preserve the raw report path and line range for every finding.
7. Identify whether a test failed because the sanitizer detected an error, because the process timed out, or because the process exited with an expected signal/status. A large failed-test count can be a cascade from one common race.
8. Recommendations must identify the affected sanitizer, executable/test, linked report, project source location when available, severity, the confirmed or likely root cause, how to fix it, and the next debugging or validation action. Generic advice to inspect the raw log is insufficient when the diagnostic identifies a lifetime, ownership, synchronization, or signal-handling pattern.

The script uses only Python's standard library. It accepts raw ASan/TSan output, CTest `LastTest.log`, CTest `LastTestsFailed.log`, and individual files under test-log directories. It searches test logs recursively, associates each finding with its test executable, preserves the detailed sanitizer diagnostic and stack frames, and links back to the relevant raw-log line. If no sanitizer marker is found in a file, it is reported as an unclassified log rather than silently discarded.

CTest's `LastTestsFailed.log` list is the starting point for the failed-test count, but it can contain stale entries when a later successful CTest run does not clear the file. The analyzer parses entries in the form `<test-number>:<test-name>` and reconciles them with explicit `Test Passed.` and `Test Failed.` markers in `LastTest.log`; an explicit pass removes a stale list entry unless the same log also contains an explicit failure. Findings are then classified using sanitizer output from all analyzed logs rather than only the file containing the failure list.

## Output

The generated directory contains:

- `README.md`: linked entry point and overall status.
- `sanitizers.md`: single-page ASan and TSan status and findings.
- `overview.md`: report counts, test outcomes, and deduplicated findings.
- `failures.md`: every recorded failed-test entry, classified as sanitizer-detected or non-sanitizer.
- `asan.md`: AddressSanitizer errors and clean ASan runs.
- `tsan.md`: ThreadSanitizer races and clean TSan runs.
- `recommendations.md`: prioritized recommendations based on observed findings.

Generated Markdown analysis is written to `doc/quality-reports/sanitizers`; its `README.md` is the main sanitizer page. Raw CTest logs remain in the external build directories and are used as analyzer inputs. The task runner creates empty sanitizer input directories before analysis, so `README.md`, `sanitizers.md`, `overview.md`, `failures.md`, `asan.md`, `tsan.md`, and `recommendations.md` are generated even when configuration, compilation, or CTest fails. A missing sanitizer diagnostic must be reported as missing input or a non-sanitizer failure, never silently treated as clean.
