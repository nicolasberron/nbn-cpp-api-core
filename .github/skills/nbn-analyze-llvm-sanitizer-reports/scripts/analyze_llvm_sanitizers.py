#!/usr/bin/env python3
"""Generate linked Markdown summaries from LLVM sanitizer reports."""

from __future__ import annotations

import argparse
import os
import re
from collections import Counter
from dataclasses import dataclass
from pathlib import Path

REPOSITORY_ROOT = Path(__file__).resolve().parents[4]
SOURCE_MARKERS = ("src/", "tests/")
SANITIZER_MARKERS = ("AddressSanitizer", "ThreadSanitizer")
SUMMARY_RE = re.compile(r"SUMMARY: (AddressSanitizer|ThreadSanitizer):?\s*(.*)")
SOURCE_RE = re.compile(r"(?P<path>(?:/[^\s()]+/)?(?:src|tests)/[^\s():]+):(?P<line>\d+)(?::(?P<column>\d+))?")
TEST_START_RE = re.compile(r"(?:Testing|Test):\s+(test_[A-Za-z0-9_]+)|Start\s+\d+:\s+(test_[A-Za-z0-9_]+)")
FAILED_TEST_LIST_RE = re.compile(r"^\s*\d+:(test_[A-Za-z0-9_]+)\s*$")


@dataclass
class Finding:
    sanitizer: str
    summary: str
    source: str
    line: int | None
    report: Path
    report_line: int
    test: str
    severity: str
    detail: str


@dataclass
class Report:
    path: Path
    sanitizer: str
    findings: list[Finding]
    tests: Counter[str]
    failures: Counter[str]
    passed_tests: set[str]
    failure_details: list[tuple[str, str]]


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="replace")


def sanitizer_for(text: str, path: Path) -> str:
    path_parts = {part.lower() for part in path.parts}
    if "ThreadSanitizer" in text or "tsan" in path.name.lower() or any("tsan" in part for part in path_parts):
        return "ThreadSanitizer"
    if "AddressSanitizer" in text or "asan" in path.name.lower() or any("asan" in part for part in path_parts):
        return "AddressSanitizer"
    return "Unclassified"


def display_source(path: str) -> str:
    for marker in SOURCE_MARKERS:
        index = path.rfind(marker)
        if index >= 0:
            return path[index:]
    return path


def source_location(text: str, start: int = 0) -> tuple[str, int | None]:
    match = SOURCE_RE.search(text, start)
    if not match:
        return "", None
    return display_source(match.group("path")), int(match.group("line"))


def current_test(lines: list[str], index: int) -> str:
    for line in reversed(lines[: index + 1]):
        match = TEST_START_RE.search(line)
        if match:
            return match.group(1) or match.group(2) or "unknown test"
    return "unknown test"


def parse_report(path: Path) -> Report:
    text = read_text(path)
    lines = text.splitlines()
    sanitizer = sanitizer_for(text, path)
    findings: list[Finding] = []
    tests: Counter[str] = Counter()
    failures: Counter[str] = Counter()
    passed_tests: set[str] = set()
    failure_details: list[tuple[str, str]] = []

    for index, line in enumerate(lines):
        test_match = TEST_START_RE.search(line)
        if test_match:
            test = test_match.group(1) or test_match.group(2)
            if test:
                tests[test] += 1
        failed_list_match = FAILED_TEST_LIST_RE.match(line) if path.name == "LastTestsFailed.log" else None
        if failed_list_match:
            test = failed_list_match.group(1)
            failures[test] += 1
            failure_details.append((test, "Listed by CTest in LastTestsFailed.log"))
        elif line.strip() == "Test Failed.":
            test = current_test(lines, index)
            failures[test] += 1
            detail = line.strip()
            for previous_line in reversed(lines[max(0, index - 20):index]):
                if "Test time =" in previous_line or "Subprocess killed" in previous_line or "Timeout" in previous_line:
                    detail = f"{detail} ({previous_line.strip()})"
                    break
            failure_details.append((test, detail))
        elif line.strip() == "Test Passed.":
            passed_tests.add(current_test(lines, index))
        summary_match = SUMMARY_RE.search(line)
        if not summary_match:
            continue
        finding_sanitizer = summary_match.group(1)
        detail_start = index
        for candidate in range(index - 1, max(-1, index - 500), -1):
            if "WARNING: ThreadSanitizer" in lines[candidate] or "ERROR: AddressSanitizer" in lines[candidate]:
                detail_start = candidate
                break
        detail_lines = lines[detail_start:index + 1]
        detail = " ".join(line.strip() for line in detail_lines if line.strip())
        source, source_line = source_location("\n".join(lines), sum(len(item) + 1 for item in lines[:index]))
        severity = "error" if finding_sanitizer == "AddressSanitizer" else "race"
        findings.append(Finding(finding_sanitizer, summary_match.group(2).strip() or "sanitizer finding",
                                source, source_line, path, index + 1, current_test(lines, index), severity, detail))

    return Report(path, sanitizer, findings, tests, failures, passed_tests, failure_details)


def rel_link(path: Path, output: Path) -> str:
    return Path(os.path.relpath(path, output)).as_posix()


def source_link(source: str, line: int | None, output: Path) -> str:
    if not source:
        return "unknown project location"
    source_path = REPOSITORY_ROOT / source
    if source_path.is_file():
        anchor = f"#L{line}" if line else ""
        return f"[{source}:{line or ''}]({rel_link(source_path, output)}{anchor})"
    return f"`{source}:{line or ''}`"


def markdown_finding(finding: Finding, output: Path) -> str:
    report_link = f"[{finding.report.name}:L{finding.report_line}]({rel_link(finding.report, output)}#L{finding.report_line})"
    location = source_link(finding.source, finding.line, output)
    detail = finding.detail.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")
    return (f"- **{finding.severity}** `{finding.test}`: `{finding.summary}`; "
            f"location: {location}; report: {report_link}\n\n"
            f"  <details><summary>Detailed sanitizer diagnostic</summary>\n\n"
            f"  <pre>{detail}</pre>\n\n  </details>")


def finding_recommendation(finding: Finding) -> tuple[str, str]:
    """Return a concise root-cause explanation and a concrete repair action."""
    summary = finding.summary.lower()
    source = finding.source.lower()
    if "stack-use-after-scope" in summary and source.endswith("tests/nbn/core/filesystemwatcher.cpp"):
        cause = (
            "The test declares `watcher` before the local `stopped` flag. Local objects are "
            "destroyed in reverse declaration order, so `stopped` is destroyed first. "
            "Watcher::~Watcher() then calls wait(), which emits signalStopped() and invokes "
            "the callback that still captures `stopped` by reference."
        )
        fix = (
            "Declare `stopped` before `filesystem::Watcher watcher` so the watcher is "
            "destroyed first, while the callback capture remains valid. Keep the mutex and "
            "condition variable alive until after the watcher as they are now. Then rerun "
            "test_FilesystemWatcher under ASan and verify the focused test completes cleanly."
        )
        return cause, fix
    if "heap-use-after-free" in summary and source.endswith("src/nbn/core/private/thread.cpp"):
        cause = (
            "The worker lambda still uses Thread::Impl after the owning Thread can be "
            "destroyed. The detached-worker notification only means that detach() was "
            "called; it does not mean that the worker has finished its final accesses."
        )
        fix = (
            "Keep the worker state alive until the worker has returned, or replace the "
            "raw Impl capture with shared lifetime state. Do not destroy Thread::Impl "
            "while a detached worker can still execute, then rerun test_Thread under "
            "both ASan and TSan."
        )
        return cause, fix
    if "heap-use-after-free" in summary or "use-after-free" in summary:
        return (
            "A thread accesses storage after its owning object has been released. "
            "The failure is a lifetime defect, not sanitizer noise.",
            "Identify the owner and worker completion boundary, retain the state until "
            "all worker accesses finish, and rerun the focused test under the failing sanitizer.",
        )
    if "data race" in summary or "race" in summary:
        return (
            "Multiple threads access shared state without a complete happens-before "
            "relationship.",
            "Protect the shared state with the existing ownership or synchronization "
            "protocol, avoid unsynchronized lifetime transitions, and rerun the focused test.",
        )
    return (
        "The sanitizer reported an actionable defect at the first project source location "
        "shown below.",
        "Inspect the complete diagnostic and call chain, fix the ownership or synchronization "
        "defect, and rerun the affected test and sanitizer suite.",
    )


def all_reports(reports_dirs: list[Path], output: Path) -> list[Report]:
    paths = {
        path for reports_dir in reports_dirs for path in reports_dir.rglob("*")
        if path.is_file() and output not in path.parents
    }
    return [parse_report(path) for path in sorted(paths)]


def deduplicated_findings(reports: list[Report]) -> list[Finding]:
    result: list[Finding] = []
    seen: set[tuple[str, str, str, int | None]] = set()
    for report in reports:
        for finding in report.findings:
            key = (finding.sanitizer, finding.summary, finding.source, finding.line)
            if key not in seen:
                seen.add(key)
                result.append(finding)
    return result


def write(path: Path, content: str) -> None:
    path.write_text(content.rstrip() + "\n", encoding="utf-8")


def sanitizer_page(sanitizer: str, reports: list[Report], findings: list[Finding], output: Path,
                   failed_tests: Counter[str]) -> list[str]:
    sanitizer_reports = [report for report in reports if report.sanitizer == sanitizer]
    sanitizer_findings = [finding for finding in findings if finding.sanitizer == sanitizer]
    name = "AddressSanitizer (ASan)" if sanitizer == "AddressSanitizer" else "ThreadSanitizer (TSan)"
    lines = [f"## {name}", ""]
    if not sanitizer_reports:
        lines += ["**Status:** no report for this sanitizer was supplied.", ""]
        return lines
    if sanitizer_findings:
        lines += [f"**Status:** {len(sanitizer_findings)} finding(s) detected.", ""]
        lines += [markdown_finding(finding, output) for finding in sanitizer_findings]
        return lines

    passed_tests = {test for report in sanitizer_reports for test in report.passed_tests}
    lines += ["**Status:** clean; no sanitizer diagnostic was emitted.", ""]
    lines += [
        "A clean sanitizer run is normally silent. This status is based on the absence of sanitizer diagnostics "
        "in the supplied logs, the recorded CTest results, and the sanitizer build-directory context.",
        "",
        f"- Reports classified as {name}: {len(sanitizer_reports)}",
        f"- Recorded passed CTest executables: {len(passed_tests)}",
        f"- Unresolved failed tests: {len(failed_tests)}",
        "- Expected clean process status: 0; the supplied Markdown logs do not independently record the shell exit code.",
        "",
    ]
    return lines


def generate(reports_dirs: list[Path], output: Path) -> None:
    output.mkdir(parents=True, exist_ok=True)
    reports = all_reports(reports_dirs, output)
    findings = deduplicated_findings(reports)
    sanitizer_counts = Counter(report.sanitizer for report in reports)
    failure_map: dict[str, tuple[Report, str]] = {}
    failure_details: list[tuple[Report, str, str]] = []
    passed_tests = {test for report in reports for test in report.passed_tests}
    explicit_failed_tests = {
        test for report in reports if report.path.name != "LastTestsFailed.log"
        for test, _ in report.failure_details
    }
    for report in reports:
        for test, detail in report.failure_details:
            if (report.path.name == "LastTestsFailed.log" and test in passed_tests
                    and test not in explicit_failed_tests):
                continue
            if test not in failure_map or not detail.startswith("Listed by CTest"):
                failure_map[test] = (report, detail)
    failed_tests = Counter({test: 1 for test in failure_map})
    failure_details = [(report, test, detail) for test, (report, detail) in sorted(failure_map.items())]
    reconciled_failures = [
        (report, test, detail)
        for report in reports
        for test, detail in report.failure_details
        if report.path.name == "LastTestsFailed.log"
        and test in passed_tests
        and test not in explicit_failed_tests
    ]

    overview = ["# LLVM sanitizer overview", "", f"Reports analyzed: {len(reports)}", "",
                f"Deduplicated findings: {len(findings)}", f"Failed test entries: {sum(failed_tests.values())}", ""]
    if failed_tests:
        overview += ["## Failed tests", ""] + [f"- `{name}` ({count} report entry/entries)" for name, count in sorted(failed_tests.items())] + [""]
    else:
        overview += ["No failed-test entries were detected.", ""]
    overview += ["## Report families", ""] + [f"- {name}: {count}" for name, count in sorted(sanitizer_counts.items())]
    write(output / "overview.md", "\n".join(overview))

    failure_lines = ["# Test failures", "", f"Recorded failed tests: {sum(failed_tests.values())}", "", "See [Recommendations](recommendations.md) for the confirmed findings, informational context, and exact next action.", ""]
    if failure_details:
        failure_lines += [
            "Each entry below is a recorded CTest failure. A sanitizer finding is actionable; a failure without a sanitizer finding requires separate test/assertion investigation.",
            "",
        ]
        finding_tests = {finding.test for report in reports for finding in report.findings}
        for report, test, detail in failure_details:
            classification = "sanitizer-detected failure" if test in finding_tests else "non-sanitizer CTest failure"
            reasons = sorted({finding.summary for source_report in reports for finding in source_report.findings
                              if finding.test == test})
            reason = "; ".join(reasons) if reasons else detail
            report_link = f"[{report.path.name}]({rel_link(report.path, output)})"
            failure_lines.append(f"- **{classification}** `{test}` in {report_link}: {reason}")
    else:
        failure_lines.append("No failed-test entries were detected in the analyzed reports.")
    write(output / "failures.md", "\n".join(failure_lines))

    sanitizer_lines = [
        "# ASan and TSan report",
        "",
        "This is the consolidated sanitizer report. A sanitizer normally produces no console output when the "
        "instrumented process completes without findings; silence alone is not treated as proof of instrumentation.",
        "",
        f"- Reports analyzed: {len(reports)}",
        f"- Sanitizer findings: {len(findings)}",
        f"- Unresolved failed tests: {sum(failed_tests.values())}",
        "",
    ]
    sanitizer_lines += sanitizer_page("AddressSanitizer", reports, findings, output, failed_tests)
    sanitizer_lines += sanitizer_page("ThreadSanitizer", reports, findings, output, failed_tests)
    write(output / "sanitizers.md", "\n".join(sanitizer_lines))

    for sanitizer, filename in (("AddressSanitizer", "asan.md"), ("ThreadSanitizer", "tsan.md")):
        sanitizer_reports = [report for report in reports if report.sanitizer == sanitizer]
        sanitizer_findings = [finding for report in sanitizer_reports for finding in report.findings]
        lines = [f"# {sanitizer}", ""]
        if sanitizer_findings:
            lines += ["<span style=\"color:red\">❌ ERROR</span>", "", "See [Recommendations](recommendations.md) for next actions.", "", f"Findings: {len(sanitizer_findings)}", ""]
            lines += [markdown_finding(finding, output) for finding in sanitizer_findings]
        elif sanitizer_reports:
            lines += ["<span style=\"color:green\">✅ PASS</span>", "", "No sanitizer findings were detected in the analyzed reports.", ""]
        else:
            lines += ["<span style=\"color:#9a6700\">⚠️ REVIEW</span>", "", "No reports for this sanitizer were found.", ""]
        write(output / filename, "\n".join(lines))

    sanitizer_findings = Counter()
    for report in reports:
        sanitizer_findings[report.sanitizer] += len(report.findings)
    deduplicated_sanitizer_findings = Counter(finding.sanitizer for finding in findings)

    recommendations = ["# Recommendations", "", "## Confirmed", ""]
    if sanitizer_findings["AddressSanitizer"]:
        recommendations.append(f"- **ASan:** {sanitizer_findings['AddressSanitizer']} AddressSanitizer finding(s) were detected.")
    elif any(report.sanitizer == "AddressSanitizer" for report in reports):
        recommendations.append("- **ASan:** no AddressSanitizer finding marker was detected in the analyzed logs.")
    else:
        recommendations.append("- **ASan:** no AddressSanitizer report was supplied.")
    if sanitizer_findings["ThreadSanitizer"]:
        recommendations.append(
            f"- **TSan:** {sanitizer_findings['ThreadSanitizer']} raw finding(s), "
            f"and {deduplicated_sanitizer_findings['ThreadSanitizer']} deduplicated finding(s) "
            f"were detected across {sum(failed_tests.values())} failed test(s).")
    elif any(report.sanitizer == "ThreadSanitizer" for report in reports):
        if any("test_" in test and report.sanitizer == "ThreadSanitizer" for report, test, _ in failure_details):
            recommendations.append(
                "- **TSan:** no ThreadSanitizer finding marker was detected, but one or more ThreadSanitizer CTest failures require test/assertion investigation."
            )
        else:
            recommendations.append("- **TSan:** no ThreadSanitizer finding marker was detected in the analyzed logs.")
    else:
        recommendations.append("- **TSan:** no ThreadSanitizer report was supplied.")
    recommendations += ["", "## Informational", "", "- The consolidated status is based on sanitizer diagnostics, explicit CTest pass/fail markers, and sanitizer build-directory context.", "- CTest's `LastTestsFailed.log` entries are reconciled with explicit `Test Passed.` and `Test Failed.` markers in `LastTest.log` so stale entries do not remain failures.", "- Repeated sanitizer findings are deduplicated separately.", "- No timeout or expected-signal classification was inferred unless it was present in the supplied logs.", "", "## Exact next action", ""]
    if failure_details:
        recommendations += ["- Investigate every unresolved CTest failure listed below; a failed test without a sanitizer marker is still a failed validation and must not be reported as a clean sanitizer run.", "", "## CTest failures requiring follow-up", ""]
        finding_tests = {finding.test for report in reports for finding in report.findings}
        for report, test, detail in failure_details:
            classification = "sanitizer finding" if test in finding_tests else "non-sanitizer test failure"
            report_link = f"[{report.path.name}]({rel_link(report.path, output)})"
            if test in finding_tests:
                recommendations.append(
                    f"- **{classification}** `{test}`: {detail}; raw log: {report_link}; "
                    f"full failure entry: [failures.md](failures.md)."
                )
            else:
                recommendations.append(
                    f"- **{classification}** `{test}` ({report.sanitizer}): {detail}. "
                    "**Severity:** unresolved validation failure. **Cause:** CTest recorded "
                    "a failure without a sanitizer diagnostic, so this is a test assertion, "
                    "exit-status, timeout, or infrastructure failure. **How to fix:** rerun "
                    f"`{test}` with complete CTest output, classify the failure, fix the test "
                    "or production defect, and rerun the focused test. The failure list is "
                    f"{report_link}; full failure entry: [failures.md](failures.md)."
                )
        recommendations.append("")
    if reconciled_failures:
        recommendations += [
            "## Reconciled CTest entries",
            "",
            "The following entries were present in `LastTestsFailed.log` but had an explicit `Test Passed.` marker in `LastTest.log`. They are reported here for traceability and are not counted as unresolved failures.",
            "",
        ]
        for report, test, _ in reconciled_failures:
            report_link = f"[{report.path.name}]({rel_link(report.path, output)})"
            recommendations.append(
                f"- **stale failure entry** `{test}`: explicit CTest pass reconciled the failure; "
                f"failure list: {report_link}; full failure entry: [failures.md](failures.md)."
            )
        recommendations.append("")
    if findings or failure_details:
        if findings:
            recommendations.append("- Resolve the highest-frequency deduplicated sanitizer root cause first, then rerun the affected focused tests and the sanitizer suite.")
        else:
            recommendations.append("- Resolve each unresolved CTest failure, then rerun the affected focused tests and the sanitizer suite.")
        recommendations.append("")
        recommendations.append("## Detailed recommendations")
        recommendations.append("")
        for finding in findings:
            cause, fix = finding_recommendation(finding)
            recommendations.append(f"### {finding.sanitizer} - `{finding.test}`")
            recommendations.append("")
            recommendations.append(f"- **Finding:** {finding.summary}")
            recommendations.append(f"- **Cause:** {cause}")
            recommendations.append(f"- **How to fix:** {fix}")
            recommendations.append(
                f"- **Evidence:** {source_link(finding.source, finding.line, output)}; "
                f"raw report: [{finding.report.name}:L{finding.report_line}]"
                f"({rel_link(finding.report, output)}#L{finding.report_line})"
            )
            recommendations.append("")
            recommendations.append(markdown_finding(finding, output))
            recommendations.append("")
        finding_tests = {finding.test for report in reports for finding in report.findings}
        for report, test, detail in failure_details:
            if test in finding_tests:
                continue
            report_link = f"[{report.path.name}]({rel_link(report.path, output)})"
            recommendations.append(f"### CTest - `{test}`")
            recommendations.append("")
            recommendations.append(f"- **Sanitizer:** {report.sanitizer}")
            recommendations.append("- **Severity:** unresolved validation failure")
            recommendations.append(f"- **Failure:** {detail}")
            recommendations.append(
                "- **Cause:** The supplied report records a CTest failure but no sanitizer "
                "diagnostic, so the failure may be an assertion, exit-status, timeout, or "
                "infrastructure problem."
            )
            recommendations.append(
                f"- **How to fix:** Re-run `{test}` with its complete CTest output enabled, "
                "classify the failure, fix the test or production defect, and then rerun "
                f"the focused test. Raw report: {report_link}."
            )
            recommendations.append("- **Evidence:** Full failure entry: [failures.md](failures.md).")
            recommendations.append("")
    else:
        recommendations.append("No sanitizer findings or unresolved test failures were detected in the analyzed logs.")
    write(output / "recommendations.md", "\n".join(recommendations))

    index_status = (
        '<span style="color:red">❌ ERROR</span>'
        if findings or failed_tests
        else '<span style="color:green">✅ PASS</span>'
    )
    index = [
        "# LLVM sanitizer report",
        "",
        index_status,
        "",
        "- [ASan and TSan](sanitizers.md)",
        "- [Overview](overview.md)",
        "- [Test failures](failures.md)",
        "- [Recommendations](recommendations.md)",
        "",
    ]
    write(output / "README.md", "\n".join(index))


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--reports", type=Path, required=True, nargs="+")
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    missing = [path for path in args.reports if not path.is_dir()]
    if missing:
        parser.error(f"report directory does not exist: {', '.join(map(str, missing))}")
    generate(args.reports, args.output)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
