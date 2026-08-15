#!/usr/bin/env python3
"""Generate linked Markdown summaries from Valgrind reports."""

from __future__ import annotations

import argparse
import os
import re
import shlex
import shutil
import subprocess
from collections import Counter
from dataclasses import dataclass
from pathlib import Path


LEAK_LINE = re.compile(r"==\d+== (\d+) bytes in (\d+) blocks are (definitely lost|indirectly lost|possibly lost|still reachable)")
ERROR_SUMMARY = re.compile(r"ERROR SUMMARY: (\d+) errors from (\d+) contexts")
COMMAND = re.compile(r"==\d+== Command: (.+)")
MASSIF_VALUE = re.compile(r"^mem_heap_B=(\d+)$")
UNKNOWN_COMMAND = "unknown command"
CALLGRIND_TOTAL = re.compile(r"^([0-9,]+)[ \t]+\(100\.0%\)[ \t]+PROGRAM TOTALS$")
NBN_SOURCE_MARKER = "src/nbn/"
TEST_SOURCE_MARKER = "tests/nbn/"
DETAILS_OPEN = "<details>"
DETAILS_CLOSE = "</details>"
CALLGRIND_TABLE_SEPARATOR = "|---:|---|"
REPOSITORY_ROOT = Path(__file__).resolve().parents[4]


@dataclass
class MemcheckReport:
    path: Path
    command: str
    errors: int
    contexts: int
    leaks: Counter[str]
    invalid_lines: int
    locations: Counter[str]
    warnings: Counter[str]
    reachable_records: list[str]


@dataclass
class MassifReport:
    path: Path
    command: str
    peak_heap: int
    snapshots: int


@dataclass
class CallgrindReport:
    path: Path
    command: str
    total_instructions: int
    hotspots: list[tuple[int, str]]


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="replace")


def command_from(text: str) -> str:
    match = COMMAND.search(text)
    return match.group(1).strip() if match else UNKNOWN_COMMAND


def source_locations(text: str) -> Counter[str]:
    locations: Counter[str] = Counter()
    for line in text.splitlines():
        if not (line.startswith("==") and ("    at " in line or "    by " in line)):
            continue
        start = line.find("src/")
        if start < 0:
            continue
        candidate = line[start:].split(")", 1)[0].split()[0].rstrip(",")
        source_path, separator, line_number = candidate.rpartition(":")
        if separator and source_path and line_number.isdigit():
            locations[f"{display_location(source_path)}:{line_number}"] += 1
    return locations


def display_location(path: str) -> str:
    """Return a stable repository-relative source path for generated Markdown."""
    for marker in (NBN_SOURCE_MARKER, TEST_SOURCE_MARKER):
        index = path.rfind(marker)
        if index >= 0:
            return path[index:]
    return path


def parse_warnings(lines: list[str]) -> Counter[str]:
    warnings: Counter[str] = Counter()
    for index, line in enumerate(lines):
        if "== Warning: " not in line:
            continue
        warning = line.split("== Warning: ", 1)[1].strip()
        if index + 1 < len(lines) and lines[index + 1].strip().startswith("=="):
            warning += " " + lines[index + 1].split("==", 2)[-1].strip()
        warnings[warning] += 1
    return warnings


def parse_reachable_records(text: str) -> list[str]:
    records = []
    leak_matches = list(LEAK_LINE.finditer(text))
    for index, match in enumerate(leak_matches):
        if match.group(3) != "still reachable":
            continue
        end = leak_matches[index + 1].start() if index + 1 < len(leak_matches) else len(text)
        location = source_locations(text[match.end():end]).most_common(1)
        suffix = f"; first project frame `{location[0][0]}`" if location else ""
        records.append(f"{match.group(1)} bytes in {match.group(2)} block(s){suffix}")
    return records


def parse_memcheck(path: Path) -> MemcheckReport:
    text = read_text(path)
    summary = ERROR_SUMMARY.search(text)
    leaks: Counter[str] = Counter()
    for match in LEAK_LINE.finditer(text):
        leaks[match.group(3)] += int(match.group(1))
    invalid_lines = len(re.findall(r"==\d+== (?:Invalid read|Invalid write|Invalid free|Invalid jump)", text))
    locations = source_locations(text)
    lines = text.splitlines()
    warnings = parse_warnings(lines)
    reachable_records = parse_reachable_records(text)
    return MemcheckReport(path, command_from(text), int(summary.group(1)) if summary else 0,
                          int(summary.group(2)) if summary else 0, leaks, invalid_lines, locations, warnings,
                          reachable_records)


def parse_massif(path: Path) -> MassifReport:
    text = read_text(path)
    values = [int(match.group(1)) for line in text.splitlines() if (match := MASSIF_VALUE.fullmatch(line.strip()))]
    return MassifReport(path, next((line[5:].strip() for line in text.splitlines() if line.startswith("cmd: ")), "unknown command"),
                        max(values, default=0), len(re.findall(r"^snapshot=", text, re.MULTILINE)))


def parse_callgrind(path: Path) -> CallgrindReport:
    text = read_text(path)
    command = next((line[4:].strip() for line in text.splitlines() if line.startswith("cmd:")), UNKNOWN_COMMAND)
    annotated = text
    if shutil_which := shutil.which("callgrind_annotate"):
        result = subprocess.run([shutil_which, "--inclusive=yes", str(path)], capture_output=True, text=True, check=False)
        if result.returncode == 0:
            annotated = result.stdout
            command = next((line.split(":", 1)[1].strip() for line in annotated.splitlines() if line.startswith("Profiled target:")), command)
    total = 0
    hotspots: list[tuple[int, str]] = []
    for line in annotated.splitlines():
        match = CALLGRIND_TOTAL.match(line.strip())
        if match:
            total = int(match.group(1).replace(",", ""))
            continue
        row = line.strip()
        if row and row[0].isdigit() and "(" in row and ")" in row:
            count_text, remainder = row.split("(", 1)
            _, symbol = remainder.split(")", 1)
            if not count_text.strip().replace(",", "").isdigit():
                continue
            symbol = symbol.strip()
            if symbol != "PROGRAM TOTALS":
                hotspots.append((int(count_text.strip().replace(",", "")), symbol))
    return CallgrindReport(path, command, total, hotspots[:10])


def format_bytes(value: int) -> str:
    units = ("B", "KiB", "MiB", "GiB")
    amount = float(value)
    for unit in units:
        if amount < 1024 or unit == units[-1]:
            return f"{amount:.1f} {unit}" if unit != "B" else f"{value} B"
        amount /= 1024
    return f"{value} B"


def md_escape(value: str) -> str:
    return value.replace("|", "\\|").replace("\n", " ")


def executable_name(command: str) -> str:
    try:
        tokens = shlex.split(command)
    except ValueError:
        tokens = command.split()
    return Path(tokens[0]).name if tokens else UNKNOWN_COMMAND


def is_intentional_exit_test(command: str) -> bool:
    return executable_name(command) in {"test_ApplicationSegfault", "test_ApplicationUncaughtException", "test_CommandLineParserHelp"}


def is_api_symbol(symbol: str) -> bool:
    """Return whether a Callgrind row belongs to the nbn API implementation."""
    return NBN_SOURCE_MARKER in symbol


def normalize_callgrind_symbol(symbol: str) -> tuple[str, Path | None]:
    """Normalize duplicate source/binary-qualified Callgrind symbols."""
    clean_symbol = symbol.split(" [", 1)[0].strip()
    marker_index = clean_symbol.find(NBN_SOURCE_MARKER)
    if marker_index < 0:
        return clean_symbol, None
    source_and_function = clean_symbol[marker_index:]
    source, separator, function = source_and_function.partition(":")
    if not separator:
        return clean_symbol, None
    source_path = REPOSITORY_ROOT / source
    return f"{source_path.as_posix()}:{function}", source_path


def source_line_for_symbol(symbol: str, source_path: Path) -> int | None:
    """Find the definition line for a normalized API symbol when available."""
    match = re.search(r"([A-Za-z_~][A-Za-z0-9_~]*)(?:\[[^]]+\])?\([^()]*\)\s*$", symbol)
    function = match.group(1) if match else ""
    if not function or not source_path.is_file():
        return None
    pattern = re.compile(rf"\b{re.escape(function)}\s*\(")
    candidates: list[int] = []
    for line_number, line in enumerate(read_text(source_path).splitlines(), start=1):
        if not pattern.search(line):
            continue
        candidates.append(line_number)
        if "{" in line or "->" in line:
            return line_number
    return candidates[0] if candidates else None


def callgrind_symbol_markdown(symbol: str, output: Path) -> str:
    """Render an API symbol with a relative link to its source file."""
    display_symbol, source_path = normalize_callgrind_symbol(symbol)
    if source_path is None:
        return f"`{md_escape(display_symbol)}`"
    line_number = source_line_for_symbol(display_symbol, source_path)
    anchor = f"#L{line_number}" if line_number is not None else ""
    relative_source = source_path.relative_to(REPOSITORY_ROOT).as_posix()
    function = display_symbol.split(f"{source_path.as_posix()}:", 1)[-1]
    line_label = f" (line {line_number})" if line_number is not None else ""
    label = f"{relative_source}:{function}{line_label}"
    return f"[{md_escape(label)}]({rel_link(source_path, output)}{anchor})"


def normalized_callgrind_hotspots(hotspots: list[tuple[int, str]]) -> list[tuple[int, str]]:
    """Drop duplicate source/binary-qualified rows while preserving hotness order."""
    normalized: list[tuple[int, str]] = []
    seen: set[str] = set()
    for count, symbol in hotspots:
        display_symbol, _ = normalize_callgrind_symbol(symbol)
        if display_symbol in seen:
            continue
        seen.add(display_symbol)
        normalized.append((count, display_symbol))
    return normalized


def rel_link(path: Path, output: Path) -> str:
    return Path(os.path.relpath(path, output)).as_posix()


def write(path: Path, content: str) -> None:
    path.write_text(content.rstrip() + "\n", encoding="utf-8")


def main() -> int:  # NOSONAR - coordinates independent report-family writers.
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--reports", type=Path, default=Path("build/valgrind-results"), help="Valgrind result directory")
    parser.add_argument("--output", type=Path, default=Path("doc/quality-reports/valgrind"), help="Markdown output directory")
    args = parser.parse_args()

    reports = args.reports.resolve()
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=True)
    memchecks = [parse_memcheck(path) for path in sorted((reports / "memcheck").glob("*.log"))]
    massifs = [parse_massif(path) for path in sorted((reports / "massif").glob("*.out"))]
    callgrinds = [parse_callgrind(path) for path in sorted((reports / "callgrind").glob("*.out"))]

    bad_memchecks = [item for item in memchecks if item.invalid_lines or item.leaks["definitely lost"] or item.leaks["indirectly lost"]]
    peak = max(massifs, key=lambda item: item.peak_heap, default=None)
    total_instructions = sum(item.total_instructions for item in callgrinds)

    write(output / "overview.md", f"""# Overview

| Report family | Reports | Status |
|---|---:|---|
| Memcheck | {len(memchecks)} | {'⚠️ findings' if bad_memchecks else '✅ no invalid-access or definite/indirect-loss findings'} |
| Massif | {len(massifs)} | {'✅ collected' if massifs else 'ℹ️ no reports'} |
| Callgrind | {len(callgrinds)} | {'✅ collected' if callgrinds else 'ℹ️ no reports'} |

- Peak observed heap: **{format_bytes(peak.peak_heap) if peak else 'n/a'}**{f' in `{executable_name(peak.command)}` ([{peak.path.name}]({rel_link(peak.path, output)}))' if peak else ''}
- Aggregate Callgrind instruction count: **{total_instructions:,}**
""")

    mem_lines = ["# Memcheck", "", "Memcheck is **OK** when there are no invalid-access errors and no definite or indirect leaks. `still reachable` memory is reported as informational.", ""]
    if bad_memchecks:
        mem_lines.extend(["## ⚠️ Investigate", "", "See [Recommendations](recommendations.md) for next actions.", "", "| Test | Errors | Invalid accesses | Definitely lost | Indirectly lost | Possibly lost | Still reachable | Report |", "|---|---:|---:|---:|---:|---:|---:|---|"])
        for item in memchecks:
            if item not in bad_memchecks and not item.invalid_lines:
                continue
            mem_lines.append(f"| `{executable_name(item.command)}` | {item.errors} | {item.invalid_lines} | {format_bytes(item.leaks['definitely lost'])} | {format_bytes(item.leaks['indirectly lost'])} | {format_bytes(item.leaks['possibly lost'])} | {format_bytes(item.leaks['still reachable'])} | [{item.path.name}]({rel_link(item.path, output)}) |")
    mem_lines.extend(["", "## ✅ Clean", "", "| Test | Errors | Invalid accesses | Still reachable | Report |", "|---|---:|---:|---:|---|"])
    for item in memchecks:
        if item in bad_memchecks or item.invalid_lines:
            continue
        mem_lines.append(f"| `{executable_name(item.command)}` | {item.errors} | {item.invalid_lines} | {format_bytes(item.leaks['still reachable'])} | [{item.path.name}]({rel_link(item.path, output)}) |")
    if not memchecks or len(bad_memchecks) == len(memchecks):
        mem_lines.append("| _none_ |  |  |  | ℹ️ none |")
    warning_items = [(item, warning, count) for item in memchecks for warning, count in item.warnings.items()]
    reachable_items = [(item, record) for item in memchecks for record in item.reachable_records]
    memcheck_status = "🔴 **Action required**" if bad_memchecks else "🟢 **Clear**"
    memcheck_summary = (f"{len(bad_memchecks)} report(s) contain invalid accesses or definite/indirect leaks"
                        if bad_memchecks else "No invalid accesses or definite/indirect leaks were detected")
    mem_lines.extend(["", "## ℹ️ Informational allocations and Valgrind warnings", "", "Still-reachable memory is not a lost allocation, but each record is listed so its lifetime can be reviewed. Warnings describe instrumentation or platform behavior and should not be silently discarded.", ""])
    if reachable_items:
        mem_lines.extend(["### Still reachable", "", "| Test | Allocation | Report |", "|---|---|---|"])
        for item, record in reachable_items:
            mem_lines.append(f"| `{executable_name(item.command)}` | {record} | [{item.path.name}]({rel_link(item.path, output)}) |")
    else:
        mem_lines.append("No still-reachable allocation records were found.")
    mem_lines.extend(["", "### Warnings", ""])
    if warning_items:
        mem_lines.extend(["| Test | Warning | Count | Report |", "|---|---|---:|---|"])
        for item, warning, count in warning_items:
            mem_lines.append(f"| `{executable_name(item.command)}` | {md_escape(warning)} | {count} | [{item.path.name}]({rel_link(item.path, output)}) |")
    else:
        mem_lines.append("No Valgrind warnings were found.")
    write(output / "memcheck.md", "\n".join(mem_lines))

    massif_lines = ["# Massif", "", "Massif reports heap trends. The peak is an observation, not automatically a defect; compare it with the workload and repeat runs before optimizing.", ""]
    review_massifs = [item for item in massifs if item.peak_heap == 0]
    if review_massifs:
        massif_lines.extend(["## ⚠️ Review", "", "| Test | Peak heap | Snapshots | Report |", "|---|---:|---:|---|"])
        for item in review_massifs:
            massif_lines.append(f"| `{executable_name(item.command)}` | {format_bytes(item.peak_heap)} | {item.snapshots} | [{item.path.name}]({rel_link(item.path, output)}) |")
    massif_lines.extend(["", "## ✅ Collected", "", "| Test | Peak heap | Snapshots | Report |", "|---|---:|---:|---|"])
    for item in sorted((item for item in massifs if item.peak_heap != 0), key=lambda item: item.peak_heap, reverse=True):
        if item.peak_heap == 0:
            continue
        massif_lines.append(f"| `{executable_name(item.command)}` | {format_bytes(item.peak_heap)} | {item.snapshots} | [{item.path.name}]({rel_link(item.path, output)}) |")
    if not massifs or all(item.peak_heap == 0 for item in massifs):
        massif_lines.append("| _none_ |  |  | ℹ️ none |")
    write(output / "massif.md", "\n".join(massif_lines))

    call_lines = ["# Callgrind", "", "Callgrind instruction counts are useful for locating hot functions. They are not wall-clock timings, and dynamic-loader/startup symbols should not be optimized as application code.", "", "## ✅ Profiles collected", ""]
    for item in callgrinds:
        hotspots = normalized_callgrind_hotspots(item.hotspots)
        api_hotspots = [(count, symbol) for count, symbol in hotspots if is_api_symbol(symbol)]
        external_hotspots = [(count, symbol) for count, symbol in hotspots if not is_api_symbol(symbol)]
        call_lines.extend([f"## `{executable_name(item.command)}`", "", f"- Report: [{item.path.name}]({rel_link(item.path, output)})", f"- Command: `{md_escape(item.command)}`", f"- Program instructions: **{item.total_instructions:,}**", "", DETAILS_OPEN, "<summary>Callgrind hotspots by ownership</summary>", "", "### nbn API", "", "| Instructions | Function / location |", CALLGRIND_TABLE_SEPARATOR])
        if api_hotspots:
            call_lines.extend(f"| {count:,} | {callgrind_symbol_markdown(symbol, output)} |" for count, symbol in api_hotspots)
        else:
            call_lines.append("| _none_ | No nbn API functions were identified in the parsed rows |")
        call_lines.extend(["", "### External and runtime code", "", "| Instructions | Function / location |", CALLGRIND_TABLE_SEPARATOR])
        if external_hotspots:
            call_lines.extend(f"| {count:,} | {callgrind_symbol_markdown(symbol, output)} |" for count, symbol in external_hotspots)
        else:
            call_lines.append("| _none_ | No external or runtime functions were identified in the parsed rows |")
        call_lines.extend(["", DETAILS_CLOSE, ""])
    if not callgrinds:
        call_lines.append("No Callgrind reports were found.")
    write(output / "callgrind.md", "\n".join(call_lines))

    warning_items = [(item, warning, count) for item in memchecks for warning, count in item.warnings.items()]
    reachable_items = [(item, record) for item in memchecks for record in item.reachable_records]
    recommendations = [
        "# Recommendations",
        "",
        "> Prioritized actions generated from the analyzed Valgrind reports. Start with confirmed correctness defects; treat profiling observations as hypotheses until validated outside Valgrind.",
        "",
        "## At a glance",
        "",
        "| Area | Status | What it means | Next action |",
        "|---|---|---|---|",
        f"| Memcheck | {memcheck_status} | {memcheck_summary} | {'Fix and rerun the affected test(s)' if bad_memchecks else 'Continue monitoring possibly lost and still-reachable memory'} |",
        f"| Still reachable | {'🟡 **Review**' if reachable_items else '🟢 **None found**'} | {len(reachable_items)} allocation record(s) remain reachable at process exit | Confirm ownership and intended lifetime |",
        f"| Valgrind warnings | {'🟡 **Review**' if warning_items else '🟢 **None found**'} | {len(warning_items)} warning occurrence group(s) were recorded | Correct actionable warnings, then rerun Memcheck |",
        f"| Massif | {'🔵 **Observation**' if peak else '⚪ **Unavailable**'} | {'Peak heap: ' + format_bytes(peak.peak_heap) if peak else 'No Massif peak was available'} | Compare against a workload baseline before optimizing |",
        f"| Callgrind | {'🔵 **Observation**' if callgrinds else '⚪ **Unavailable**'} | {'Instruction hotspots were collected' if callgrinds else 'No Callgrind reports were available'} | Validate candidates with a normal benchmark |",
        "",
    ]
    if bad_memchecks:
        recommendations += ["## 1. Confirmed correctness defects", "", f"{len(bad_memchecks)} report(s) contain invalid accesses or definite/indirect leaks. These findings take priority over performance work.", "", "| Priority | Test | Evidence | First project frame | Reports |", "|---|---|---|---|---|"]
        locations = Counter()
        grouped: dict[tuple[str, str], list[MemcheckReport]] = {}
        for item in bad_memchecks:
            locations.update(item.locations)
            location = item.locations.most_common(1)[0][0] if item.locations else "no project source location in stack"
            grouped.setdefault((executable_name(item.command), location), []).append(item)
        for (executable, location), items in grouped.items():
            details = []
            invalid_count = sum(item.invalid_lines for item in items)
            if invalid_count:
                details.append(f"{invalid_count} invalid-access report(s) across {len(items)} run(s)")
            for kind in ("definitely lost", "indirectly lost"):
                amount = sum(item.leaks[kind] for item in items)
                if amount:
                    details.append(f"{format_bytes(amount)} {kind} across {len(items)} run(s)")
            report_links = " ".join(f"[{item.path.name}]({rel_link(item.path, output)})" for item in items)
            recommendations.append(f"| 🔴 High | `{executable}` | {md_escape('; '.join(details))} | `{md_escape(location)}` | {report_links} |")
        if locations:
            recommendations += ["", "**Required next step:** reproduce each affected test alone, inspect the first project frame and its ownership/teardown path, then add a regression test for the failing lifecycle.", "", DETAILS_OPEN, "<summary>Repeated project frames</summary>", "", "\n".join(f"- `{location}` — {count} occurrence(s)" for location, count in locations.most_common(5)), "", DETAILS_CLOSE, ""]
    else:
        recommendations += ["## 1. Memcheck status: clear", "", "No invalid-access errors or definite/indirect leaks were detected in the analyzed logs. Review `possibly lost` and `still reachable` allocations only if they grow across repeated runs or violate the intended lifetime model.", ""]
    if reachable_items:
        reachable_reports = sorted({item.path.name for item, _ in reachable_items})
        intentional_items = [(item, record) for item, record in reachable_items if is_intentional_exit_test(item.command)]
        normal_items = [(item, record) for item, record in reachable_items if not is_intentional_exit_test(item.command)]
        recommendations += ["## 2. Still-reachable allocations", "", f"{len(reachable_items)} record(s) were found in {len(reachable_reports)} report(s). `still reachable` means Valgrind can still find a pointer at process exit; it is not proof of a leak.", "", "| Classification | Records | Interpretation | Action |", "|---|---:|---|---|"]
        if intentional_items:
            recommendations.append(f"| 🟢 Expected process lifetime | {len(intentional_items)} | Intentional exit tests: {', '.join(f'`{name}`' for name in sorted({executable_name(item.command) for item, _ in intentional_items}))} | Do not add artificial production cleanup; preserve the exit semantics |")
        if normal_items:
            recommendations.append(f"| 🟡 Ownership review | {len(normal_items)} | Records occur in non-terminating test runs | Review the first project frame; add teardown only for project-owned persistent objects |")
        recommendations += ["", DETAILS_OPEN, "<summary>Representative records and raw reports</summary>", "", "| Test | Allocation record | Report |", "|---|---|---|"]
        for item, record in reachable_items[:10]:
            recommendations.append(f"| `{executable_name(item.command)}` | {record} | [{item.path.name}]({rel_link(item.path, output)}) |")
        recommendations += ["", f"All affected reports: {', '.join(f'[{name}]({rel_link(next(item.path for item, _ in reachable_items if item.path.name == name), output)})' for name in reachable_reports)}.", "", DETAILS_CLOSE, ""]
    if warning_items:
        warning_totals: Counter[str] = Counter()
        warning_runs: Counter[str] = Counter()
        for item, warning, count in warning_items:
            warning_totals[warning] += count
            warning_runs[warning] += 1
        recommendations += ["## 3. Valgrind warnings", "", f"{len(warning_totals)} distinct warning type(s) were recorded. The current warnings concern `SIGKILL` and `SIGSTOP`, which POSIX makes uncatchable.", "", "| Severity | Warning | Occurrences | Runs |", "|---|---|---:|---:|"]
        for warning in warning_totals:
            recommendations.append(f"| 🟡 Review | {md_escape(warning)} | {warning_totals[warning]} | {warning_runs[warning]} |")
        recommendations += ["", "**Required next step:** restrict signal registration to catchable signals (or explicitly skip `SIGKILL` and `SIGSTOP`), then rerun Memcheck and confirm these warnings disappear. Do not suppress them without fixing or documenting the registration logic.", ""]
    if peak:
        recommendations += ["## 4. Massif peak", "", "| Test | Peak heap | Report |", "|---|---:|---|", f"| `{executable_name(peak.command)}` | **{format_bytes(peak.peak_heap)}** | [{peak.path.name}]({rel_link(peak.path, output)}) |", "", "This is a measured peak, not proof of a leak. Inspect the peak snapshot’s allocation tree, identify the owning container or buffer, and compare the same test against a baseline before changing allocation behavior.", ""]
    if callgrinds:
        recommendations += ["## 5. Callgrind hotspots", "", "Instruction counts locate hot functions but are not wall-clock timings. Validate any optimization with a normal benchmark.", ""]
        project_hotspots = Counter()
        for item in callgrinds:
            for count, symbol in item.hotspots:
                if ("src/nbn/" in symbol or "tests/nbn/" in symbol) and "libnbn-core.so" not in symbol:
                    project_hotspots[symbol] += count
        if project_hotspots:
            recommendations += ["| Instructions | Project-owned hotspot |", "|---:|---|"]
            recommendations.extend(f"| {count:,} | `{md_escape(symbol)}` |" for symbol, count in project_hotspots.most_common(5))
            recommendations += ["", "**Required next step:** add a focused benchmark and compare wall-clock timings outside Valgrind before changing code."]
        else:
            recommendations.append("No project-owned hotspot was identified in the parsed top rows. Loader, libc, and standard-library startup rows are instrumentation context rather than actionable application targets.")
        recommendations.append("")
    recommendations += ["## Repeatable workflow", "", "1. Keep raw reports beside the generated Markdown so every finding remains traceable.", "2. Reproduce one affected executable with the same build and Valgrind options.", "3. Apply one focused change, rerun the relevant profile, and compare the result.", "4. Use normal benchmark timings—not Valgrind instruction counts—for performance decisions.", ""]
    write(output / "recommendations.md", "\n".join(recommendations))

    write(output / "README.md", f"""# Valgrind Analysis

Generated from `{reports}`.

## Status

- **Memcheck:** {'⚠️ findings require investigation' if bad_memchecks else '✅ no invalid-access or definite/indirect-loss findings'}
- **Massif:** {'✅ reports analyzed' if massifs else 'ℹ️ no reports found'}
- **Callgrind:** {'✅ reports analyzed' if callgrinds else 'ℹ️ no reports found'}

## Reports

- [Overview](overview.md)
- [Memcheck findings](memcheck.md)
- [Massif heap observations](massif.md)
- [Callgrind hotspots](callgrind.md)
- [Recommendations](recommendations.md)
""")
    print(f"Generated Markdown analysis in {output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
