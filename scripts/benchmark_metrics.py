#!/usr/bin/env python3
"""Summarize and compare nbn dependency-free benchmark JSON reports."""

from __future__ import annotations

import argparse
import json
import math
import sys
from pathlib import Path
from typing import Any


def load_report(path: Path) -> dict[str, Any]:
    with path.open(encoding="utf-8") as stream:
        report = json.load(stream)
    if not isinstance(report, dict) or not isinstance(report.get("results"), list):
        raise ValueError(f"invalid benchmark report: {path}")
    return report


def results_by_name(report: dict[str, Any]) -> dict[str, dict[str, Any]]:
    results = {}
    for result in report["results"]:
        if not isinstance(result, dict) or not isinstance(result.get("name"), str):
            raise ValueError("benchmark result has no string name")
        results[result["name"]] = result
    return results


def finite_number(result: dict[str, Any], key: str) -> float:
    value = result.get(key)
    if not isinstance(value, (int, float)) or not math.isfinite(float(value)):
        raise ValueError(f"missing or invalid {key} for {result['name']}")
    return float(value)


def summarize(path: Path) -> int:
    report = load_report(path)
    for result in results_by_name(report).values():
        print(
            f"{result['name']}: median={finite_number(result, 'median_latency_seconds'):.9g}s "
            f"p95={finite_number(result, 'p95_latency_seconds'):.9g}s "
            f"throughput={finite_number(result, 'median_throughput_ops_per_second'):.9g} ops/s "
            f"iterations={result.get('iterations')} samples={result.get('sample_runs')}"
        )
    return 0


def compare(before_path: Path, after_path: Path, threshold: float) -> int:
    before = results_by_name(load_report(before_path))
    after = results_by_name(load_report(after_path))
    missing = sorted(set(before) - set(after))
    added = sorted(set(after) - set(before))
    if missing or added:
        raise ValueError(f"benchmark names differ: missing={missing} added={added}")

    rows = []
    failed = False
    for name in sorted(before):
        old = before[name]
        new = after[name]
        old_latency = finite_number(old, "median_latency_seconds")
        new_latency = finite_number(new, "median_latency_seconds")
        old_p95 = finite_number(old, "p95_latency_seconds")
        new_p95 = finite_number(new, "p95_latency_seconds")
        old_throughput = finite_number(old, "median_throughput_ops_per_second")
        new_throughput = finite_number(new, "median_throughput_ops_per_second")
        latency_delta = (new_latency / old_latency - 1.0) * 100.0
        p95_delta = (new_p95 / old_p95 - 1.0) * 100.0
        throughput_delta = (new_throughput / old_throughput - 1.0) * 100.0
        regression = latency_delta > threshold or p95_delta > threshold or throughput_delta < -threshold
        failed |= regression
        rows.append((name, latency_delta, p95_delta, throughput_delta, regression))

    print("name | median latency | p95 latency | median throughput | status")
    print("--- | ---: | ---: | ---: | ---")
    for name, latency_delta, p95_delta, throughput_delta, regression in rows:
        status = "REGRESSION" if regression else "OK"
        print(f"{name} | {latency_delta:+.2f}% | {p95_delta:+.2f}% | {throughput_delta:+.2f}% | {status}")
    return 1 if failed else 0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)
    summarize_parser = subparsers.add_parser("summarize")
    summarize_parser.add_argument("report", type=Path)
    compare_parser = subparsers.add_parser("compare")
    compare_parser.add_argument("before", type=Path)
    compare_parser.add_argument("after", type=Path)
    compare_parser.add_argument("--threshold", type=float, default=5.0, help="allowed regression percentage")
    args = parser.parse_args()
    try:
        if args.command == "summarize":
            return summarize(args.report)
        return compare(args.before, args.after, args.threshold)
    except (OSError, ValueError) as error:
        print(f"benchmark metrics error: {error}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())