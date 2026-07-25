#!/usr/bin/env python3
"""Summarize LLVM coverage reports and enforce production coverage thresholds."""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path


PERCENTAGE = r"(?:100(?:\.0+)?|\d{1,2}(?:\.\d+)?)%"
TOTAL_PATTERN = re.compile(
    rf"^\s*TOTAL\s+.*?({PERCENTAGE})\s+.*?({PERCENTAGE})\s+.*?({PERCENTAGE})(?:\s|$)"
)


def find_report(coverage_dir: Path, requested: Path | None) -> Path:
    if requested is not None:
        report = requested
    else:
        report = coverage_dir / "production.txt"
        if not report.is_file():
            reports = sorted(coverage_dir.glob("*.txt"))
            if len(reports) == 1:
                report = reports[0]
    if not report.is_file():
        raise ValueError(f"LLVM coverage report not found: {report}")
    return report


def parse_total(report: Path) -> tuple[float, float, float]:
    totals: list[tuple[float, float, float]] = []
    with report.open(encoding="utf-8", errors="replace") as stream:
        for line in stream:
            match = TOTAL_PATTERN.match(line)
            if match:
                totals.append(tuple(float(value.rstrip("%")) for value in match.groups()))
    if not totals:
        raise ValueError(f"no LLVM TOTAL row found in {report}")
    if len(totals) > 1:
        raise ValueError(f"multiple LLVM TOTAL rows found in {report}")
    return totals[0]


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("coverage_dir", type=Path)
    parser.add_argument(
        "--report",
        type=Path,
        help="coverage report to parse; defaults to <coverage-dir>/production.txt",
    )
    parser.add_argument(
        "--minimum",
        type=float,
        default=100.0,
        help="minimum required percentage for regions, functions, and lines",
    )
    args = parser.parse_args()
    try:
        report = find_report(args.coverage_dir, args.report)
        regions, functions, lines = parse_total(report)
        metrics = {
            "regions": regions,
            "functions": functions,
            "lines": lines,
        }
        failed = any(value < args.minimum for value in metrics.values())
        status = "PASS" if not failed else "FAIL"
        print(f"coverage: {status} ({report})")
        for name, value in metrics.items():
            print(f"{name}: {value:.2f}%")
        return 1 if failed else 0
    except (OSError, ValueError) as error:
        print(f"coverage metrics error: {error}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
