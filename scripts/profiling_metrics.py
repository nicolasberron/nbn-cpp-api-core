#!/usr/bin/env python3
"""Extract stable counters from a Linux perf stat text report."""

from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path


COUNTERS = (
    "task-clock",
    "context-switches",
    "cpu-migrations",
    "page-faults",
    "cycles",
    "instructions",
    "branches",
    "branch-misses",
    "cache-references",
    "cache-misses",
)
COUNTER_PATTERN = re.compile(r"^\s*(\d[\d,]*(?:\.\d+)?|<not counted>|<not supported>)\s+(\S+)")


def parse_perf(path: Path) -> dict[str, object]:
    counters: dict[str, object] = {}
    with path.open(encoding="utf-8", errors="replace") as stream:
        for line in stream:
            match = COUNTER_PATTERN.match(line)
            if not match:
                continue
            raw_value, counter = match.groups()
            counter = counter.rstrip(":")
            if counter not in COUNTERS:
                continue
            if raw_value.startswith("<"):
                counters[counter] = raw_value
            else:
                counters[counter] = float(raw_value.replace(",", ""))
    return counters


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("perf_report", type=Path)
    parser.add_argument("--json", type=Path, help="write normalized counters to this path")
    args = parser.parse_args()
    try:
        counters = parse_perf(args.perf_report)
        if not counters:
            raise ValueError(f"no supported perf counters found in {args.perf_report}")
        payload = {"source": str(args.perf_report), "counters": counters}
        text = json.dumps(payload, indent=2, sort_keys=True) + "\n"
        if args.json:
            args.json.write_text(text, encoding="utf-8")
        else:
            print(text, end="")
        return 0
    except (OSError, ValueError) as error:
        print(f"profiling metrics error: {error}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())