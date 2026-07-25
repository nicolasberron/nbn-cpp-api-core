#!/usr/bin/env python3
"""Compare parser acceptance with Python's strict JSON decoder for a corpus."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path


def python_accepts(data: bytes) -> bool:
    try:
        json.loads(
            data.decode("utf-8"),
            object_pairs_hook=_reject_duplicate_keys,
            parse_int=_parse_int,
            parse_float=_parse_float,
            parse_constant=_reject_constant,
        )
    except (UnicodeDecodeError, ValueError, OverflowError):
        return False
    return True


def _parse_int(value: str) -> int:
    result = int(value)
    if result < -(2**63) or result > 2**63 - 1:
        raise OverflowError("integer outside the reference range")
    return result


def _parse_float(value: str) -> float:
    result = float(value)
    if result != result or result in (float("inf"), float("-inf")):
        raise ValueError("non-finite number")
    return result


def _reject_constant(value: str) -> object:
    raise ValueError(f"non-standard JSON constant: {value}")


def _reject_duplicate_keys(pairs: list[tuple[str, object]]) -> dict[str, object]:
    result: dict[str, object] = {}
    for key, value in pairs:
        if key in result:
            raise ValueError(f"duplicate key: {key}")
        result[key] = value
    return result


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("probe", type=Path)
    parser.add_argument("corpus", type=Path)
    args = parser.parse_args()
    failures = 0
    for path in sorted(args.corpus.rglob("*")):
        if not path.is_file():
            continue
        data = path.read_bytes()
        expected = python_accepts(data)
        result = subprocess.run([str(args.probe), str(path)], check=False, timeout=10)
        actual = result.returncode == 0
        if expected != actual:
            print(f"[FAIL] {path}: reference={expected} parser={actual}")
            failures += 1
        else:
            print(f"[PASS] {path}: accepted={actual}")
    return min(failures, 255)


if __name__ == "__main__":
    sys.exit(main())
