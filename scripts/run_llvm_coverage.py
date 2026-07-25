#!/usr/bin/env python3
"""Run instrumented tests and generate LLVM source-based coverage reports."""

from __future__ import annotations

import argparse
import html
import json
import os
import re
import shutil
import subprocess
import sys
from urllib.parse import quote
from pathlib import Path
from typing import NoReturn


LLVM_COV_WARNING_FLAGS = ["--no-pgo-warn-mismatch"]
LLVM_COV_AGGREGATE_FILTER = "--ignore-filename-regex=.*/tests/.*"
HTML_INDEX_FILENAME = "index.html"
IGNORED_LLVM_WARNING = re.compile(
    r"^warning: (?:\d+ functions have mismatched data|.*profile data may be out of date - object is newer)$"
)


def fail(message: str) -> NoReturn:
    print(message, file=sys.stderr)
    raise SystemExit(1)


def require_tool(name: str) -> str:
    tool = shutil.which(name)
    if tool is None:
        fail(f"Missing LLVM coverage tool: {name}")
    return tool


def run_command(
    command: list[str], *, environment: dict[str, str] | None = None
) -> subprocess.CompletedProcess[str]:
    return subprocess.run(command, env=environment, check=False, text=True)


def run_and_tee(command: list[str], report: Path) -> None:
    with report.open("w", encoding="utf-8") as report_file:
        process = subprocess.Popen(
            command,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
        )
        assert process.stdout is not None
        for line in process.stdout:
            if IGNORED_LLVM_WARNING.match(line.rstrip()):
                continue
            print(line, end="")
            report_file.write(line)
        return_code = process.wait()
    if return_code != 0:
        raise subprocess.CalledProcessError(return_code, command)


def run_llvm_cov(command: list[str]) -> subprocess.CompletedProcess[str]:
    result = subprocess.run(command, check=False, text=True, capture_output=True)
    for line in result.stdout.splitlines(keepends=True):
        print(line, end="")
    for line in result.stderr.splitlines(keepends=True):
        if not IGNORED_LLVM_WARNING.match(line.rstrip()):
            print(line, end="", file=sys.stderr)
    return result


def validate_build_directory(build_dir: Path) -> None:
    if not (build_dir / "CMakeCache.txt").is_file():
        fail(
            f"Coverage build directory is not configured: {build_dir}\n"
            "Run the coverage configure and build tasks before generating coverage."
        )
    if not (build_dir / "CTestTestfile.cmake").is_file():
        fail(
            f"Coverage build directory has no CTest registration: {build_dir}\n"
            "Configure the build with NBN_BUILD_TESTS=ON before generating coverage."
        )


def validate_registered_tests(build_dir: Path) -> list[str]:
    listing = subprocess.run(
        ["ctest", "--test-dir", str(build_dir), "--show-only=json-v1"],
        check=False,
        text=True,
        capture_output=True,
    )
    if listing.stderr:
        print(listing.stderr, end="", file=sys.stderr)
    if listing.returncode != 0:
        raise subprocess.CalledProcessError(listing.returncode, listing.args)
    try:
        test_names = [test["name"] for test in json.loads(listing.stdout)["tests"]]
    except (KeyError, TypeError, json.JSONDecodeError) as error:
        raise RuntimeError("CTest returned an invalid JSON test listing") from error
    if not test_names:
        fail(
            f"No tests are registered in the coverage build directory: {build_dir}\n"
            "The coverage build must be configured and built with NBN_BUILD_TESTS=ON."
        )
    print(f"Registered CTest tests: {len(test_names)}")
    return test_names


def prepare_coverage_directory(build_dir: Path) -> tuple[Path, Path, Path]:
    coverage_dir = (build_dir / "coverage-llvm").resolve()
    profile_dir = coverage_dir / "profiles"
    report_dir = coverage_dir / "html"
    shutil.rmtree(coverage_dir, ignore_errors=True)
    profile_dir.mkdir(parents=True)
    report_dir.mkdir(parents=True)
    return coverage_dir, profile_dir, report_dir


def merge_profiles(
    profiles: list[Path], llvm_profdata: str, output: Path
) -> None:
    if not profiles:
        fail("No LLVM .profraw files were produced.")
    result = run_command(
        [llvm_profdata, "merge", "-sparse", *(str(profile) for profile in profiles), "-o", str(output)]
    )
    if result.returncode != 0:
        raise subprocess.CalledProcessError(result.returncode, result.args)


def align_coverage_input_timestamps(
    test_binaries: list[Path], objects: list[Path], profiles: list[Path]
) -> None:
    """Avoid false stale-profile warnings when the system clock moves backwards."""
    profile_time = min(profile.stat().st_mtime for profile in profiles)
    for path in [*test_binaries, *objects]:
        os.utime(path, (profile_time, profile_time))


def find_coverage_inputs(build_dir: Path) -> tuple[list[Path], list[Path]]:
    test_binaries = sorted(
        path
        for path in (build_dir / "tests").rglob("test_*")
        if path.is_file() and os.access(path, os.X_OK)
    )
    objects = sorted(
        path
        for path in (build_dir / "src").rglob("*")
        if path.is_file()
        and (path.name.startswith("libnbn-core.so") or path.name == "libnbn-core.a")
    )
    if not test_binaries:
        fail("No instrumented test executables were found.")
    return test_binaries, objects


def generate_reports(
    test_binaries: list[Path],
    objects: list[Path],
    llvm_cov: str,
    profdata: Path,
    coverage_dir: Path,
    report_dir: Path,
    test_profiles: dict[str, Path],
) -> None:
    if not objects:
        fail("No instrumented production library was found.")
    production_binary = objects[0]
    aggregate_inputs = [production_binary, *test_binaries]

    def coverage_object_arguments(inputs: list[Path]) -> list[str]:
        primary, *additional = inputs
        return [
            str(primary),
            *(f"--object={path}" for path in additional),
        ]

    try:
        run_and_tee(
            [
                llvm_cov,
                "report",
                *coverage_object_arguments(aggregate_inputs),
                f"-instr-profile={profdata}",
                *LLVM_COV_WARNING_FLAGS,
                LLVM_COV_AGGREGATE_FILTER,
            ],
            coverage_dir / "production.txt",
        )
    except subprocess.CalledProcessError as error:
        raise SystemExit(error.returncode) from error

    aggregate_dir = report_dir / "aggregate"
    result = run_llvm_cov(
        [
            llvm_cov,
            "show",
            *coverage_object_arguments(aggregate_inputs),
            f"-instr-profile={profdata}",
            *LLVM_COV_WARNING_FLAGS,
            LLVM_COV_AGGREGATE_FILTER,
            "-format=html",
            f"-output-dir={aggregate_dir}",
        ]
    )
    if result.returncode != 0:
        raise SystemExit(result.returncode)

    for binary in test_binaries:
        name = binary.name
        test_profile = test_profiles.get(name)
        if test_profile is None:
            fail(f"No per-test LLVM profile was generated for {name}.")
        report = coverage_dir / f"{name}.txt"
        try:
            run_and_tee(
                [
                    llvm_cov,
                    "report",
                    *coverage_object_arguments([production_binary, binary]),
                    f"-instr-profile={test_profile}",
                    *LLVM_COV_WARNING_FLAGS,
                ],
                report,
            )
        except subprocess.CalledProcessError as error:
            raise SystemExit(error.returncode) from error
        result = run_llvm_cov(
            [
                llvm_cov,
                "show",
                *coverage_object_arguments([production_binary, binary]),
                f"-instr-profile={test_profile}",
                *LLVM_COV_WARNING_FLAGS,
                "-format=html",
                f"-output-dir={report_dir / name}",
            ]
        )
        if result.returncode != 0:
            raise SystemExit(result.returncode)

    write_html_index(report_dir, aggregate_dir)


def coverage_link(filename: str, report_dir: Path, aggregate_dir: Path) -> str:
    candidates = [path for path in aggregate_dir.rglob("*.html") if path.is_file()]
    matching = [path for path in candidates if f"{filename}.html" in path.as_posix()]
    if not matching:
        matching = [path for path in candidates if path.name == f"{Path(filename).name}.html"]
    if not matching:
        return ""
    return quote(matching[0].relative_to(report_dir).as_posix())


def coverage_rows(
    production_report: Path, report_dir: Path, aggregate_dir: Path
) -> tuple[list[str], list[str]]:
    source_rows = []
    build_rows = []
    if not production_report.is_file():
        return source_rows, build_rows
    percentage = re.compile(r"\d{1,3}(?:\.\d+)?%")
    for line in production_report.read_text(encoding="utf-8", errors="replace").splitlines():
        matches = list(percentage.finditer(line))
        if len(matches) < 3 or line.lstrip().startswith("TOTAL"):
            continue
        metric_prefix = line[: matches[0].start()]
        filename = metric_prefix.rstrip().rsplit(None, 2)[0]
        values = [match.group() for match in matches[:4]]
        if len(values) < 4:
            values.append("-")
        link = coverage_link(filename, report_dir, aggregate_dir)
        filename_cell = (
            f'<a href="{html.escape(link)}">{html.escape(filename)}</a>'
            if link
            else html.escape(filename)
        )
        function_class = coverage_class(values[1])
        line_class = coverage_class(values[2])
        region_class = coverage_class(values[0])
        branch_class = coverage_class(values[3])
        row = (
            f"<tr><td>{filename_cell}</td>"
            f'<td class="{function_class}">{html.escape(values[1])}</td>'
            f'<td class="{line_class}">{html.escape(values[2])}</td>'
            f'<td class="{region_class}">{html.escape(values[0])}</td>'
            f'<td class="{branch_class}">{html.escape(values[3])}</td></tr>'
        )
        if "/src/" in f"/{filename}":
            source_rows.append(row)
        else:
            build_rows.append(row)
    return source_rows, build_rows


def coverage_class(value: str) -> str:
    if value == "-":
        return "not-applicable"
    percentage = float(value.rstrip("%"))
    if percentage >= 100.0:
        return "covered"
    if percentage >= 90.0:
        return "partial"
    return "uncovered"


def write_html_index(report_dir: Path, aggregate_dir: Path) -> None:
    source_rows, build_rows = coverage_rows(
        report_dir.parent / "production.txt", report_dir, aggregate_dir
    )
    source_table = "\n".join(source_rows)
    build_table = "\n".join(build_rows)
    (report_dir / HTML_INDEX_FILENAME).write_text(
        "<!doctype html>\n<html lang=\"en\"><head><meta charset=\"utf-8\">"
        "<title>LLVM coverage reports</title>"
        "<style>body{font-family:Arial,sans-serif;background:#202020;color:#eee}"
        "table{border-collapse:collapse;width:100%}th,td{padding:6px;border:1px solid #555;text-align:left}"
        "th{background:#333}a{color:#9cf}.covered{background:#116b19}.partial{background:#807b0b}"
        ".uncovered{background:#7b1717}.not-applicable{background:#555}</style></head><body>"
        "<h1>LLVM coverage reports</h1>"
        "<table><thead><tr><th>Filename</th><th>Function Coverage</th>"
        "<th>Line Coverage</th><th>Region Coverage</th><th>Branch Coverage</th>"
        f"</tr></thead><tbody><tr><th colspan=\"5\">Source files</th></tr>"
        f"{source_table}<tr><th colspan=\"5\">Build files</th></tr>"
        f"{build_table}</tbody></table></body></html>\n",
        encoding="utf-8",
    )


def remove_published_html(output_dir: Path) -> None:
    """Keep generated HTML coverage artifacts in the build directory only."""
    shutil.rmtree(output_dir / "html", ignore_errors=True)
    output_dir.mkdir(parents=True, exist_ok=True)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("build_dir", nargs="?", type=Path, default=Path("."))
    parser.add_argument("--output-dir", type=Path, default=Path("quality-reports/coverage"))
    args = parser.parse_args()
    build_dir = args.build_dir.resolve()
    llvm_cov = require_tool(os.environ.get("LLVM-COV", "llvm-cov-19"))
    llvm_profdata = require_tool(os.environ.get("LLVM_PROFDATA", "llvm-profdata-19"))

    validate_build_directory(build_dir)
    test_names = validate_registered_tests(build_dir)
    coverage_dir, profile_dir, report_dir = prepare_coverage_directory(build_dir)

    print("Running instrumented CTest tests:")
    environment = os.environ.copy()
    environment["LLVM_PROFILE_FILE"] = str(profile_dir / "%p.profraw")
    test_profile_files: dict[str, list[Path]] = {}
    for test_name in test_names:
        profile_pattern = profile_dir / f"{test_name}-%p.profraw"
        environment["LLVM_PROFILE_FILE"] = str(profile_pattern)
        print(f"Running instrumented CTest test: {test_name}")
        tests = run_command(
            [
                "ctest",
                "--test-dir",
                str(build_dir),
                "--output-on-failure",
                "-R",
                f"^{re.escape(test_name)}$",
            ],
            environment=environment,
        )
        if tests.returncode != 0:
            return tests.returncode
        test_profile_files[test_name] = sorted(
            profile_dir.glob(f"{test_name}-*.profraw")
        )

    test_binaries, objects = find_coverage_inputs(build_dir)
    all_profiles = sorted(
        profile for profiles in test_profile_files.values() for profile in profiles
    )
    align_coverage_input_timestamps(test_binaries, objects, all_profiles)

    profdata = coverage_dir / "coverage.profdata"
    merge_profiles(
        all_profiles,
        llvm_profdata,
        profdata,
    )
    test_profdata_dir = coverage_dir / "test-profdata"
    test_profdata_dir.mkdir()
    test_profiles = {}
    for test_name, profiles in test_profile_files.items():
        test_profile = test_profdata_dir / f"{test_name}.profdata"
        merge_profiles(profiles, llvm_profdata, test_profile)
        test_profiles[test_name] = test_profile
    generate_reports(
        test_binaries,
        objects,
        llvm_cov,
        profdata,
        coverage_dir,
        report_dir,
        test_profiles,
    )
    remove_published_html(args.output_dir)
    print(f"LLVM coverage reports generated at {coverage_dir}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
