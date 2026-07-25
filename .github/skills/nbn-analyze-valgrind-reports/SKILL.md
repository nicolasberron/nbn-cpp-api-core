---
name: analyze-valgrind-reports
description: "Analyze Valgrind Memcheck, Massif, and Callgrind reports from this repository and generate linked Markdown findings, status summaries, and actionable recommendations. Use when reviewing quality-reports/valgrind, memory errors, leaks, heap usage, or callgrind profiles."
argument-hint: "[report directory] [output directory]"
---

# Analyze Valgrind reports

Use the bundled [analyzer](./scripts/analyze_valgrind.py) to turn raw Valgrind output into a small Markdown report set. The final review must state what is confirmed, what is informational, and the exact next action; do not merely tell the user to inspect raw logs.

This skill is analysis-only and does not change source code. If analysis identifies a fix, the implementation MUST use strict TDD and must restore 100% LLVM production coverage before handoff.

## Procedure

This skill is analysis-only. Do **not** launch Valgrind, CTest, builds, or profiling tasks. It consumes the reports that already exist in the supplied report directory. If no reports are present, explain that collection must be run separately and stop without starting it.

1. From the repository root, run:

   ```text
   python3 .github/skills/analyze-valgrind-reports/scripts/analyze_valgrind.py \\
   --reports quality-reports/valgrind/raw \\
   --output quality-reports/valgrind
   ```

2. Open `quality-reports/valgrind/README.md`.
3. Review `memcheck.md` for correctness issues, `massif.md` for peak heap observations, `callgrind.md` for instruction hotspots, and `recommendations.md` for prioritized follow-up work.
4. Treat `still reachable` allocations as informational, but list each allocation record and explain whether it is project-owned, runtime-owned, or an intentional process-lifetime allocation. Prioritize invalid accesses and `definitely lost` or `indirectly lost` blocks.
5. Treat Callgrind instruction counts as profiler observations, not wall-clock timings. Startup and dynamic-loader symbols are reported separately from project symbols.
6. Ensure recommendations identify the affected executable, linked report, project source location when available, severity, and next debugging or validation action. Deduplicate repeated Valgrind PIDs into one root-cause conclusion where possible.
7. Extract and report every Valgrind `Warning:`. Explain whether it is expected platform behavior or actionable configuration/code. For ignored `SIGKILL`/`SIGSTOP` handler registration, recommend skipping those uncatchable signals rather than suppressing the warning.

The script uses only Python's standard library. `ms_print` and `callgrind_annotate` are optional; raw report parsing still produces summaries when they are unavailable.

## Output

The generated directory contains:

- `README.md`: linked entry point and overall status.
- `overview.md`: counts across all report families.
- `memcheck.md`: errors, leak classes, and clean runs.
- `massif.md`: peak heap and stack measurements.
- `callgrind.md`: total instructions and top annotated functions.
- `recommendations.md`: prioritized recommendations based on observed findings.

Raw reports should remain in the build directory, for example `<build-dir>/valgrind-results`. Generated Markdown is written to `quality-reports/valgrind`; its `README.md` is the main Valgrind page.
