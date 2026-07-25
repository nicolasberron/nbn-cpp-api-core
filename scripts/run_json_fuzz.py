#!/usr/bin/env python3
"""Run or minimize the libFuzzer JSON corpus with bounded resource settings."""

from __future__ import annotations

import argparse
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

DEFAULT_MAX_LENGTH = 16 * 1024 * 1024
DEFAULT_SECONDS = 300


def run(command: list[str]) -> int:
    print("[json-fuzz] $ " + " ".join(command), flush=True)
    return subprocess.run(command, check=False).returncode


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("binary", type=Path)
    parser.add_argument("corpus", type=Path)
    parser.add_argument("--seconds", type=int, default=DEFAULT_SECONDS)
    parser.add_argument("--max-len", type=int, default=DEFAULT_MAX_LENGTH)
    parser.add_argument("--minimize", type=Path)
    args = parser.parse_args()

    if not args.binary.is_file():
        parser.error(f"fuzzer binary does not exist: {args.binary}")
    if not args.corpus.is_dir():
        parser.error(f"corpus directory does not exist: {args.corpus}")
    if args.seconds <= 0 or args.max_len <= 0:
        parser.error("--seconds and --max-len must be positive")

    corpus = args.corpus
    if args.minimize is not None:
        args.minimize.parent.mkdir(parents=True, exist_ok=True)
        temporary: Path | None = Path(tempfile.mkdtemp(prefix=f"{args.minimize.name}.", dir=args.minimize.parent))
        try:
            command = [
                str(args.binary),
                f"-merge=1",
                str(temporary),
                str(args.corpus),
                f"-max_len={args.max_len}",
            ]
            result = run(command)
            if result == 0:
                if args.minimize.exists():
                    shutil.rmtree(args.minimize)
                temporary.replace(args.minimize)
                temporary = None
            return result
        finally:
            if temporary is not None and temporary.exists():
                shutil.rmtree(temporary)

    command = [
        str(args.binary),
        f"-max_total_time={args.seconds}",
        f"-timeout=10",
        f"-rss_limit_mb=2048",
        f"-max_len={args.max_len}",
        str(corpus),
    ]
    return run(command)


if __name__ == "__main__":
    sys.exit(main())
