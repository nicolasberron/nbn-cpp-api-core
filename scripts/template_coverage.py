#!/usr/bin/env python3
"""Report uncovered LLVM coverage lines grouped by C++ template instantiation."""

from __future__ import annotations

import argparse
import re
import shutil
import subprocess
import sys
from pathlib import Path


# LLVM prints a source-file heading ending in ':' before the function records.
# Restrict records to Itanium C++ mangled symbols so uncovered lines are not
# accidentally attributed to the source-file heading.
FUNCTION_HEADER = re.compile(r"^\s*\|\s*(?P<symbol>_Z[^:]+):\s*$")
UNCOVERED_LINE = re.compile(r"^\s*\|?\s*(?P<line>\d+)\|\s*0\|")


def find_single(build_dir: Path, pattern: str, description: str) -> Path:
    matches = sorted(build_dir.glob(pattern))
    if len(matches) != 1:
        raise ValueError(
            f"expected exactly one {description} matching {pattern!r} in {build_dir}, "
            f"found {len(matches)}"
        )
    return matches[0]


def demangle(symbols: list[str], demangler: str | None) -> dict[str, str]:
    if demangler is None or not symbols:
        return {symbol: symbol for symbol in symbols}
    result = subprocess.run(
        [demangler, "-n", *symbols],
        text=True,
        capture_output=True,
        check=True,
    )
    names = result.stdout.splitlines()
    if len(names) != len(symbols):
        return {symbol: symbol for symbol in symbols}
    return dict(zip(symbols, names, strict=True))


def collect_uncovered(
    report: str,
) -> dict[str, list[int]]:
    current_symbol: str | None = None
    uncovered: dict[str, list[int]] = {}

    for line in report.splitlines():
        header = FUNCTION_HEADER.match(line)
        if header:
            current_symbol = header.group("symbol")
            continue
        match = UNCOVERED_LINE.match(line)
        if match is None or current_symbol is None:
            continue
        uncovered.setdefault(current_symbol, []).append(int(match.group("line")))

    return uncovered


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "build_dir",
        type=Path,
        help="Clang coverage build directory containing src/ and coverage-llvm/",
    )
    parser.add_argument(
        "--binary",
        type=Path,
        help="instrumented binary; defaults to the unique libnbn-core.so under src/",
    )
    parser.add_argument(
        "--profile",
        type=Path,
        help="merged .profdata file; defaults to coverage-llvm/coverage.profdata",
    )
    parser.add_argument(
        "--source",
        help="regular expression selecting template symbols, e.g. ConcurrentQueue",
    )
    parser.add_argument(
        "--llvm-cov",
        default="llvm-cov-19",
        help="llvm-cov executable (default: llvm-cov-19)",
    )
    parser.add_argument(
        "--demangler",
        default="llvm-cxxfilt-19",
        help="LLVM demangler, or empty to keep mangled names",
    )
    args = parser.parse_args()

    try:
        build_dir = args.build_dir.resolve()
        binary = args.binary or find_single(build_dir / "src", "**/libnbn-core.so", "production binary")
        profile = args.profile or build_dir / "coverage-llvm" / "coverage.profdata"
        llvm_cov = shutil.which(args.llvm_cov)
        if llvm_cov is None:
            raise ValueError(f"LLVM coverage tool not found: {args.llvm_cov}")
        demangler = None if args.demangler == "" else shutil.which(args.demangler)

        command = [
            llvm_cov,
            "show",
            str(binary),
            f"-instr-profile={profile}",
            "-show-line-counts-or-regions",
            "-show-instantiations=true",
        ]
        result = subprocess.run(command, text=True, capture_output=True, check=True)
        uncovered = collect_uncovered(result.stdout)
        names = demangle(list(uncovered), demangler)
        if args.source:
            source_filter = re.compile(args.source)
            uncovered = {
                symbol: lines
                for symbol, lines in uncovered.items()
                if source_filter.search(names[symbol])
            }

        if not uncovered:
            print("No uncovered template-instantiation lines found.")
            return 0

        for symbol, lines in uncovered.items():
            print(names[symbol])
            print("  uncovered lines: " + ", ".join(str(line) for line in sorted(set(lines))))
        return 0
    except (OSError, ValueError, subprocess.CalledProcessError) as error:
        print(f"template coverage error: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
