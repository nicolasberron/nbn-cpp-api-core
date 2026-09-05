#!/usr/bin/env python3
"""Write the published quality-report index and summarize generated failures."""

from __future__ import annotations

import argparse
import re
from pathlib import Path


MAX_FAILURE_LINES = 80
REPORT_README = "README.md"
PASS_BADGE = '<span style="color:green">✅ PASS</span>'
ERROR_BADGE = '<span style="color:red">❌ ERROR</span>'
REVIEW_BADGE = '<span style="color:#9a6700">⚠️ REVIEW</span>'
CAPTURED_CTEST_OUTPUT = "Captured CTest output:"


def read_lines(path: Path) -> list[str]:
    if not path.is_file():
        return []
    return path.read_text(encoding="utf-8", errors="replace").splitlines()


def ctest_status(build_dir: Path) -> tuple[str, list[str]]:
    failed = read_lines(build_dir / "Testing" / "Temporary" / "LastTestsFailed.log")
    last_test = build_dir / "Testing" / "Temporary" / "LastTest.log"
    captured = read_lines(build_dir / "quality-ctest.log")
    last_test_lines = read_lines(last_test) if last_test.is_file() else captured
    last_test_text = "\n".join(last_test_lines)
    if captured and re.search(
        r"\b[1-9]\d*\s+tests failed\b|Test Failed\.",
        "\n".join(captured),
        re.IGNORECASE,
    ):
        return "FAILED", [CAPTURED_CTEST_OUTPUT, *captured]
    passed = set(re.findall(r"Test Passed\.\s*\n\"([^\"]+)\" end time", last_test_text))
    passed.update(re.findall(r"Test\s+#?\d+:\s+(test_[A-Za-z0-9_]+).*Passed", last_test_text))
    failed = [
        entry
        for entry in failed
        if entry.strip() and entry.split(":", 1)[-1] not in passed
    ]
    details: list[str] = []
    if failed:
        details.extend([f"CTest failed tests: {', '.join(failed)}", *read_lines(last_test)])
        return "FAILED", [*details, CAPTURED_CTEST_OUTPUT, *captured]
    if last_test.is_file():
        if re.search(r"\b[1-9]\d*\s+tests failed\b|Test Failed\.", last_test_text, re.IGNORECASE):
            return "FAILED", ["Latest CTest output:", *read_lines(last_test)]
        if (
            passed
            or "Test Passed." in last_test_text
            or re.search(r"Ran\s+\d+\s+tests?:.*\b0\s+failed\b", last_test_text)
        ):
            return "PASSED", []
    if any(
        re.search(r"\b[1-9]\d*\s+tests failed", line.lower())
        or "test failed." in line.lower()
        or "errors while running ctest" in line.lower()
        for line in captured
    ):
        return "FAILED", [CAPTURED_CTEST_OUTPUT, *captured]
    if last_test.is_file():
        return "PASSED", captured
    if captured:
        if re.search(r"\b[1-9]\d*\s+tests failed\b|Test Failed\.", last_test_text, re.IGNORECASE):
            return "FAILED", [CAPTURED_CTEST_OUTPUT, *captured]
        if (
            "Test Passed." in last_test_text
            or re.search(r"\b\d+\/\d+\s+Test\s+#?\d+:.*Passed", last_test_text)
            or re.search(r"\b0\s+tests failed\b", last_test_text, re.IGNORECASE)
        ):
            return "PASSED", []
    return "NOT RUN", [f"CTest log not found: {last_test}", *captured]


def report_status(path: Path, expected: list[str]) -> tuple[str, list[str]]:
    missing = [name for name in expected if not (path / name).exists()]
    if not path.is_dir():
        return "NOT GENERATED", [f"Report directory not found: {path}"]
    if missing:
        return "INCOMPLETE", [f"Missing expected output: {name}" for name in missing]
    return "GENERATED", []


def section(title: str, status: str, details: list[str]) -> list[str]:
    lines = [f"### {title}: {status}", ""]
    if details:
        lines.append("<details><summary>Failure/output details</summary>")
        lines.append("")
        lines.append("```text")
        lines.extend(details[:MAX_FAILURE_LINES])
        if len(details) > MAX_FAILURE_LINES:
            lines.append(f"... {len(details) - MAX_FAILURE_LINES} additional lines omitted")
        lines.extend(["```", "", "</details>"])
    else:
        lines.append("No failure output was recorded.")
    lines.append("")
    return lines


def badge(status: str) -> str:
    if status in {"PASSED", "GENERATED"}:
        return PASS_BADGE
    if status in {"FAILED", "INCOMPLETE", "NOT GENERATED"}:
        return ERROR_BADGE
    return REVIEW_BADGE


def detail_text(details: list[str], fallback: str) -> str:
    if not details:
        return fallback
    text = " ".join(line.strip() for line in details[:3] if line.strip())
    return text.replace("|", "\\|") or fallback


def coverage_rows(production: Path) -> list[str]:
    rows = [
        "| File | Regions | Functions | Lines | Branches |",
        "| --- | ---: | ---: | ---: | ---: |",
    ]
    percentage = re.compile(r"\d{1,3}(?:\.\d+)?%")
    for line in read_lines(production):
        if line.lstrip().startswith("TOTAL"):
            continue
        matches = list(percentage.finditer(line))
        if len(matches) < 3:
            continue
        filename = line[: matches[0].start()].rstrip().rsplit(None, 2)[0]
        values = [match.group() for match in matches[:4]]
        if len(values) < 4:
            values.append("-")
        rows.append(f"| `{filename.strip()}` | {values[0]} | {values[1]} | "
                    f"{values[2]} | {values[3]} |")
    return rows


def write_coverage_page(quality: Path, coverage: Path, metrics: list[str], complete: bool) -> None:
    page = [
        "# LLVM coverage",
        "",
    ]
    if (coverage / "html/index.html").is_file():
        page.extend([f"[Build HTML details]({coverage / 'html/index.html'})", ""])
    page.extend(
        [
            PASS_BADGE if complete else ERROR_BADGE,
            "",
            *metrics,
            "",
            *coverage_rows(coverage / "production.txt"),
        ]
    )
    quality.mkdir(parents=True, exist_ok=True)
    (quality / "coverage.md").write_text("\n".join(page) + "\n", encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--workspace", type=Path, default=Path.cwd())
    parser.add_argument("--build-root", type=Path, required=True)
    args = parser.parse_args()

    workspace = args.workspace.resolve()
    build_root = args.build_root.resolve()
    quality = workspace / "doc" / "quality-reports"
    coverage = build_root / "linux-clang19-coverage" / "coverage-llvm"
    asan = build_root / "linux-clang19-asan"
    tsan = build_root / "linux-clang19-tsan"

    coverage_status, coverage_details = report_status(
        coverage, ["production.txt", "metrics.txt"]
    )
    metrics = read_lines(coverage / "metrics.txt")
    failed_coverage_tests = read_lines(coverage / "failed-tests.txt")
    if failed_coverage_tests:
        coverage_details.extend(
            [
                "Coverage tests failed:",
                *failed_coverage_tests,
            ]
        )
        coverage_status = "FAILED"
    if metrics:
        coverage_details.extend(["Coverage metrics output:", *metrics])
        if any("coverage: FAIL" in line or "coverage metrics error:" in line for line in metrics):
            coverage_status = "FAILED"
    valgrind_status, valgrind_details = report_status(
        quality / "valgrind", [REPORT_README]
    )
    valgrind_failures = read_lines(
        build_root / "linux-clang19-debug" / "valgrind-results" / "failed-tests.txt"
    )
    if not valgrind_failures:
        failure_logs = sorted(
            (build_root / "linux-clang19-debug" / "Testing" / "Temporary").glob(
                "LastTestsFailed_*.log"
            )
        )
        if failure_logs:
            valgrind_failures = read_lines(failure_logs[-1])
    if valgrind_failures:
        valgrind_status = "FAILED"
        valgrind_details.extend(["Valgrind tests failed:", *valgrind_failures])
    benchmark_status, benchmark_details = report_status(
        quality / "benchmark", [REPORT_README]
    )
    sanitizer_status, sanitizer_details = report_status(
        quality / "sanitizers", [REPORT_README]
    )
    asan_status, asan_details = ctest_status(asan)
    tsan_status, tsan_details = ctest_status(tsan)
    if asan_status == "FAILED" or tsan_status == "FAILED":
        sanitizer_status = "FAILED"
        sanitizer_details = [
            "At least one sanitizer test suite has unresolved failures.",
            *asan_details[:2],
            *tsan_details[:2],
        ]

    required_metrics = ("regions:", "functions:", "lines:")
    metrics_status = all(
        any(line.startswith(metric) and line.endswith("100.00%") for line in metrics)
        for metric in required_metrics
    )
    metrics_status = metrics_status and coverage_status == "GENERATED"
    coverage_link = ""
    write_coverage_page(quality, coverage, metrics, metrics_status)
    coverage_link = "[coverage.md](coverage.md)"

    report_rows = [
        ("Coverage", coverage_status, "Markdown coverage table", coverage_link),
        ("Benchmark", benchmark_status, "Serialization benchmark output", "benchmark/README.md"),
        ("Valgrind", valgrind_status, "Memcheck, Massif, and Callgrind output", "valgrind/README.md"),
        ("AddressSanitizer", asan_status, "CTest sanitizer output", "sanitizers/asan.md"),
        ("ThreadSanitizer", tsan_status, "CTest sanitizer output", "sanitizers/tsan.md"),
        ("Sanitizer analysis", sanitizer_status, "Findings and recommendations", "sanitizers/README.md"),
    ]
    lines = [
        "# Quality reports",
        "",
        "This directory contains published performance, correctness, sanitizer, and coverage reports.",
        "Raw and intermediate files remain in the build directories.",
        "",
        "## Reports",
        "",
        "| Report | Status | Output details | Individual page |",
        "| --- | --- | --- | --- |",
    ]
    details_by_title = {
        "Coverage": coverage_details,
        "Benchmark": benchmark_details,
        "Valgrind": valgrind_details,
        "AddressSanitizer": asan_details,
        "ThreadSanitizer": tsan_details,
        "Sanitizer analysis": sanitizer_details,
    }
    for title, status, output, page in report_rows:
        if page.startswith("["):
            page_link = page
        elif page:
            page_link = f"[{page}]({page})"
        else:
            page_link = "Markdown table below"
        details = detail_text(details_by_title[title], output)
        lines.append(f"| {title} | {badge(status)} | {details} | {page_link} |")
    lines.append("Generated by `scripts/write_quality_report_readme.py`.")

    quality.mkdir(parents=True, exist_ok=True)
    (quality / "README.md").write_text("\n".join(lines) + "\n", encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
