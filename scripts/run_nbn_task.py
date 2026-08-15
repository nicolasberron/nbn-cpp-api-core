#!/usr/bin/env python3
"""Run nbn-cpp-api-core development tasks outside of VS Code."""

from __future__ import annotations

import argparse
import os
import shlex
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Callable, Sequence


DEFAULT_BUILD_ROOT_NAME = "nbn-cpp-api-core"
DEFAULT_PROFILE = "linux-clang19-debug"
CORE_TEST_TARGET = "tests/nbn/core/all"
BENCHMARK_TARGET = "benchmark_serialization"
COVERAGE_BUILD_NAME = "linux-clang19-coverage"
DEBUG_BUILD_NAME = "linux-clang19-debug"
ASAN_BUILD_NAME = "linux-clang19-asan"
TSAN_BUILD_NAME = "linux-clang19-tsan"
QUALITY_REPORTS_NAME = "quality-reports"


@dataclass(frozen=True)
class TaskContext:
    """Paths and command options shared by all tasks."""

    workspace: Path
    build_root: Path
    profile: str
    dry_run: bool = False

    @property
    def debug_build(self) -> Path:
        return self.build_root / DEBUG_BUILD_NAME

    @property
    def coverage_build(self) -> Path:
        return self.build_root / COVERAGE_BUILD_NAME

    @property
    def asan_build(self) -> Path:
        return self.build_root / ASAN_BUILD_NAME

    @property
    def tsan_build(self) -> Path:
        return self.build_root / TSAN_BUILD_NAME

    @property
    def quality_reports(self) -> Path:
        return self.workspace / "doc" / QUALITY_REPORTS_NAME

    def command_environment(self) -> dict[str, str]:
        """Return a copy of the process environment for a child command."""
        return os.environ.copy()

    def conan_toolchain(self) -> Path:
        """Return the generated Conan toolchain or explain how to create it."""
        toolchain = self.debug_build / "conan_toolchain.cmake"
        if self.dry_run:
            return toolchain
        if not toolchain.is_file():
            raise FileNotFoundError(
                f"Conan toolchain not found: {toolchain}. "
                f"Run conan-install with profile {self.profile!r} first."
            )
        return toolchain

    def conan_environment_script(self) -> Path:
        """Return the generated Conan environment script for this platform."""
        scripts = sorted(self.debug_build.glob("conanbuildenv-*.sh"))
        if self.dry_run:
            return self.debug_build / "conanbuildenv-<configuration>.sh"
        if not scripts:
            raise FileNotFoundError(
                f"Conan build environment not found in {self.debug_build}. "
                f"Run conan-install with profile {self.profile!r} first."
            )
        return scripts[0]


TaskFunction = Callable[[TaskContext], int]


def display_command(command: Sequence[str], cwd: Path) -> None:
    """Print a command before executing it."""
    print(f"[nbn-task] cwd={cwd}", flush=True)
    print(f"[nbn-task] $ {shlex.join(command)}", flush=True)


def run_command(
    context: TaskContext,
    command: Sequence[str],
    *,
    cwd: Path | None = None,
    environment: dict[str, str] | None = None,
    output_file: Path | None = None,
) -> int:
    """Run a command with inherited output and optionally duplicate it to a file."""
    working_directory = cwd or context.workspace
    display_command(command, working_directory)
    if context.dry_run:
        return 0

    working_directory.mkdir(parents=True, exist_ok=True)
    if output_file is None:
        completed = subprocess.run(
            command,
            cwd=working_directory,
            env=environment,
            check=False,
        )
        return completed.returncode

    output_file.parent.mkdir(parents=True, exist_ok=True)
    with output_file.open("w", encoding="utf-8") as log:
        process = subprocess.Popen(
            command,
            cwd=working_directory,
            env=environment,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            bufsize=1,
        )
        assert process.stdout is not None
        for line in process.stdout:
            print(line, end="", flush=True)
            log.write(line)
        return process.wait()


def shell_command(command: str) -> list[str]:
    """Run a shell command through bash without relying on VS Code expansion."""
    return ["bash", "-o", "pipefail", "-c", command]


def run_python(context: TaskContext, script: Path, arguments: Sequence[str]) -> int:
    """Run a repository Python script with the current interpreter."""
    return run_command(context, [sys.executable, str(script), *arguments])


def configure_debug(context: TaskContext) -> int:
    """Configure the standard Conan debug build."""
    toolchain = context.conan_toolchain()
    return run_command(
        context,
        [
            "cmake",
            "-S",
            str(context.workspace),
            "-B",
            str(context.debug_build),
            "-G",
            "Ninja",
            "-DCMAKE_BUILD_TYPE=Debug",
            "-DCMAKE_TOOLCHAIN_FILE=" + str(toolchain),
            "-DCMAKE_C_COMPILER=clang-19",
            "-DCMAKE_CXX_COMPILER=clang++-19",
            "-DNBN_CLANG_TIDY_ENABLE=ON",
        ],
    )


def build_debug(context: TaskContext) -> int:
    """Build the standard Conan debug preset."""
    return run_command(context, ["cmake", "--build", str(context.debug_build)])


def conan_install(context: TaskContext) -> int:
    """Install Conan dependencies for the selected profile."""
    output_directory = context.debug_build
    return run_command(
        context,
        [
            "conan",
            "install",
            ".",
            "-of",
            str(output_directory),
            "-pr",
            context.profile,
        ],
    )


def open_integrated_terminal(context: TaskContext) -> int:
    """Open the selected profile build directory with the VS Code CLI."""
    return run_command(context, ["code", str(context.debug_build)])


def configure_coverage(context: TaskContext) -> int:
    """Configure the LLVM coverage build."""
    environment_script = context.conan_environment_script()
    toolchain = context.conan_toolchain()
    command = (
        "source "
        f"{shlex.quote(str(environment_script))}"
        f" && cmake -S {shlex.quote(str(context.workspace))}"
        f" -B {shlex.quote(str(context.coverage_build))} -G Ninja"
        " -DCMAKE_BUILD_TYPE=Debug"
        " -DCMAKE_TOOLCHAIN_FILE="
        f"{shlex.quote(str(toolchain))}"
        " -DNBN_BUILD_TESTS=ON -DNBN_LLVM_COVERAGE_ENABLE=ON"
        " -DNBN_CLANG_TIDY_ENABLE=OFF"
    )
    return run_command(context, shell_command(command))


def build_coverage(context: TaskContext) -> int:
    """Build the coverage test target."""
    return run_command(
        context,
        ["cmake", "--build", str(context.coverage_build)],
        cwd=context.workspace,
    )


def coverage(context: TaskContext) -> int:
    """Run LLVM coverage collection and publish the aggregate HTML report."""
    return run_python(
        context,
        context.workspace / "scripts/run_llvm_coverage.py",
        [
            str(context.coverage_build),
            "--output-dir",
            str(context.quality_reports / "coverage"),
        ],
    )


def coverage_metrics(context: TaskContext) -> int:
    """Generate coverage metrics in the coverage build directory."""
    metrics_directory = context.coverage_build / "coverage-llvm"
    metrics_file = metrics_directory / "metrics.txt"
    return run_command(
        context,
        shell_command(
            shlex.join(
                [
                    sys.executable,
                    str(context.workspace / "scripts/coverage_metrics.py"),
                    str(metrics_directory),
                ]
            )
        ),
        output_file=metrics_file,
    )


def configure_valgrind(context: TaskContext) -> int:
    """Configure the debug build used by Valgrind."""
    toolchain = context.conan_toolchain()
    return run_command(
        context,
        [
            "cmake",
            "-S",
            str(context.workspace),
            "-B",
            str(context.debug_build),
            "-G",
            "Ninja",
            "-DCMAKE_BUILD_TYPE=Debug",
            "-DCMAKE_TOOLCHAIN_FILE=" + str(toolchain),
            "-DCMAKE_C_COMPILER=clang-19",
            "-DCMAKE_CXX_COMPILER=clang++-19",
            "-DNBN_BUILD_TESTS=ON",
            "-DNBN_BUILD_BENCHMARKS=OFF",
            "-DNBN_TEST_TIMEOUT_SECONDS=300",
            "-DNBN_CLANG_TIDY_ENABLE=OFF",
        ],
    )


def build_valgrind(context: TaskContext) -> int:
    """Build the core tests used by Valgrind."""
    return run_command(
        context,
        [
            "cmake",
            "--build",
            str(context.debug_build),
            "--target",
            CORE_TEST_TARGET,
        ],
    )


def valgrind_tool(context: TaskContext, tool: str) -> int:
    """Run one Valgrind tool through the repository CTest script."""
    output_directory = context.debug_build / "valgrind-results" / tool
    return run_command(
        context,
        [
            "ctest",
            "-S",
            str(context.workspace / "scripts/run_valgrind_ctest.cmake"),
            "-VV",
            f"-DCTEST_BINARY_DIRECTORY={context.debug_build}",
            f"-DVALGRIND_TOOL={tool}",
            f"-DVALGRIND_OUTPUT_DIR={output_directory}",
        ],
    )


def valgrind_report(context: TaskContext) -> int:
    """Generate published Valgrind Markdown reports."""
    return run_python(
        context,
        context.workspace
        / ".github/skills/nbn-analyze-valgrind-reports/scripts/analyze_valgrind.py",
        [
            "--reports",
            str(context.debug_build / "valgrind-results"),
            "--output",
            str(context.quality_reports / "valgrind"),
        ],
    )


def benchmark_all(context: TaskContext) -> int:
    """Build and run the normal serialization benchmark for publication."""
    environment_script = context.conan_environment_script()
    toolchain = context.conan_toolchain()
    configure_command = (
        "source "
        f"{shlex.quote(str(environment_script))}"
        f" && cmake -S {shlex.quote(str(context.workspace))}"
        f" -B {shlex.quote(str(context.debug_build))} -G Ninja"
        " -DCMAKE_BUILD_TYPE=Debug"
        " -DCMAKE_TOOLCHAIN_FILE="
        f"{shlex.quote(str(toolchain))}"
        " -DNBN_BUILD_BENCHMARKS=ON"
        " -DNBN_BUILD_BENCHMARK_SANITIZERS=OFF"
        " -DNBN_BUILD_BENCHMARK_FUZZER=OFF"
        " -DNBN_CLANG_TIDY_ENABLE=OFF"
    )
    configure_result = run_command(context, shell_command(configure_command))
    if configure_result != 0:
        return configure_result
    build_result = run_command(
        context,
        ["cmake", "--build", str(context.debug_build), "--target", BENCHMARK_TARGET],
    )
    if build_result != 0:
        return build_result
    return run_command(
        context,
        [
            str(context.workspace / "scripts/run_benchmarks.sh"),
            "--build-dir",
            str(context.debug_build),
            "--executable",
            "tests/core/utilities/benchmark_serialization",
            "--tool",
            "normal",
            "--output-dir",
            str(context.debug_build / "benchmark-results"),
            "--report-dir",
            str(context.quality_reports / "benchmark"),
        ],
    )


def configure_sanitizer(context: TaskContext, sanitizer: str) -> int:
    """Configure an AddressSanitizer or ThreadSanitizer build."""
    build_directory = context.asan_build if sanitizer == "asan" else context.tsan_build
    environment_script = context.conan_environment_script()
    toolchain = context.conan_toolchain()
    sanitizer_flag = "address" if sanitizer == "asan" else "thread"
    flags = f"-fsanitize={sanitizer_flag} -fno-omit-frame-pointer"
    extra_options = " -DNBN_TSAN_BUILD=ON" if sanitizer == "tsan" else ""
    command = (
        "source "
        f"{shlex.quote(str(environment_script))}"
        f" && cmake -S {shlex.quote(str(context.workspace))}"
        f" -B {shlex.quote(str(build_directory))} -G Ninja"
        " -DCMAKE_BUILD_TYPE=Debug"
        " -DCMAKE_TOOLCHAIN_FILE="
        f"{shlex.quote(str(toolchain))}"
        " -DCMAKE_C_COMPILER=clang-19 -DCMAKE_CXX_COMPILER=clang++-19"
        f" -DCMAKE_C_FLAGS=-fsanitize={sanitizer_flag}"
        f" -DCMAKE_CXX_FLAGS={shlex.quote(flags)}"
        f" -DCMAKE_EXE_LINKER_FLAGS=-fsanitize={sanitizer_flag}"
        f" -DCMAKE_SHARED_LINKER_FLAGS=-fsanitize={sanitizer_flag}"
        " -DNBN_BUILD_TESTS=ON -DNBN_BUILD_BENCHMARKS=OFF"
        " -DNBN_LLVM_COVERAGE_ENABLE=OFF -DNBN_CLANG_TIDY_ENABLE=OFF"
        " -DNBN_TEST_TIMEOUT_SECONDS=300"
        f"{extra_options}"
    )
    return run_command(context, shell_command(command))


def build_sanitizer(context: TaskContext, sanitizer: str) -> int:
    """Build the core tests for a sanitizer."""
    build_directory = context.asan_build if sanitizer == "asan" else context.tsan_build
    return run_command(
        context,
        ["cmake", "--build", str(build_directory), "--target", CORE_TEST_TARGET],
    )


def run_sanitizer(context: TaskContext, sanitizer: str) -> int:
    """Run sanitizer tests while preserving the complete visible CTest log."""
    build_directory = context.asan_build if sanitizer == "asan" else context.tsan_build
    options = (
        "detect_leaks=1:halt_on_error=1:verbosity=1"
        if sanitizer == "asan"
        else "halt_on_error=1:verbosity=1"
    )
    environment = context.command_environment()
    environment[f"{sanitizer.upper()}_OPTIONS"] = options
    return run_command(
        context,
        ["ctest", "--test-dir", str(build_directory), "--output-on-failure"],
        environment=environment,
        output_file=build_directory / "quality-ctest.log",
    )


def sanitizer_report(context: TaskContext) -> int:
    """Generate the combined ASan and TSan Markdown report."""
    # Report generation must remain possible when configure, build, or CTest
    # failed. Empty input directories allow the analyzer to emit every expected
    # Markdown page with an explicit no-report status instead of omitting the
    # complete sanitizer report directory.
    for build_directory in (context.asan_build, context.tsan_build):
        (build_directory / "Testing" / "Temporary").mkdir(parents=True, exist_ok=True)
    analyzer = (
        context.workspace
        / ".github/skills/nbn-analyze-llvm-sanitizer-reports/scripts"
        / "analyze_llvm_sanitizers.py"
    )
    return run_python(
        context,
        analyzer,
        [
            "--reports",
            str(context.asan_build / "Testing/Temporary"),
            str(context.tsan_build / "Testing/Temporary"),
            "--output",
            str(context.quality_reports / "sanitizers"),
        ],
    )


def clean_quality_reports(context: TaskContext) -> int:
    """Remove previous published quality reports."""
    if context.dry_run:
        print(f"[nbn-task] remove {context.quality_reports}", flush=True)
        return 0
    expected_parent = context.workspace / "doc"
    if (
        context.quality_reports == context.workspace
        or context.quality_reports.parent != expected_parent
    ):
        raise ValueError("quality report path must be directly below the workspace doc directory")
    if not context.quality_reports.is_relative_to(context.workspace):
        raise ValueError("quality report path must be inside the workspace")
    history_path = context.quality_reports / "benchmark-history.csv"
    history_contents = history_path.read_bytes() if history_path.is_file() else None
    if context.quality_reports.exists():
        shutil.rmtree(context.quality_reports)
    context.quality_reports.mkdir(parents=True, exist_ok=True)
    if history_contents is not None:
        history_path.write_bytes(history_contents)
    return 0


def quality_readme(context: TaskContext) -> int:
    """Write the aggregate quality report README."""
    return run_python(
        context,
        context.workspace / "scripts/write_quality_report_readme.py",
        [
            "--workspace",
            str(context.workspace),
            "--build-root",
            str(context.build_root),
        ],
    )


def first_failure(results: Sequence[int]) -> int:
    """Return a stable non-zero status when any stage failed."""
    for result in results:
        if result != 0:
            return result
    return 0


def run_sequence(context: TaskContext, tasks: Sequence[TaskFunction]) -> int:
    """Run all stages and continue after failures."""
    results: list[int] = []
    for task in tasks:
        try:
            result = task(context)
        except (OSError, ValueError) as error:
            print(f"[nbn-task] error: {error}", file=sys.stderr, flush=True)
            result = 1
        results.append(result)
        if result != 0:
            print(f"[nbn-task] task failed with exit code {result}", flush=True)
    return first_failure(results)


def coverage_all(context: TaskContext) -> int:
    """Run coverage configuration, build, collection, and metrics."""
    return run_sequence(
        context, [configure_coverage, build_coverage, coverage, coverage_metrics]
    )


def valgrind_all(context: TaskContext) -> int:
    """Run all Valgrind tools and publish their report."""
    return run_sequence(
        context,
        [
            configure_valgrind,
            build_valgrind,
            lambda item: valgrind_tool(item, "callgrind"),
            lambda item: valgrind_tool(item, "memcheck"),
            lambda item: valgrind_tool(item, "massif"),
            valgrind_report,
        ],
    )


def sanitizers_all(context: TaskContext) -> int:
    """Run ASan and TSan configuration, builds, tests, and report generation."""
    result = run_sequence(
        context,
        [
            lambda item: configure_sanitizer(item, "asan"),
            lambda item: build_sanitizer(item, "asan"),
            lambda item: run_sanitizer(item, "asan"),
            lambda item: configure_sanitizer(item, "tsan"),
            lambda item: build_sanitizer(item, "tsan"),
            lambda item: run_sanitizer(item, "tsan"),
            sanitizer_report,
        ],
    )
    # Keep this explicit final attempt separate from the sequence result. This
    # protects the publication step if a future collection stage raises before
    # the analyzer is reached, while preserving the original failure status.
    report_result = sanitizer_report(context)
    return first_failure([result, report_result])


def quality_reports(context: TaskContext) -> int:
    """Generate all quality reports and the final README despite stage failures."""
    results = [clean_quality_reports(context)]
    results.extend([benchmark_all(context), coverage_all(context), valgrind_all(context), sanitizers_all(context)])
    results.append(quality_readme(context))
    return first_failure(results)


def task_registry() -> dict[str, TaskFunction]:
    """Return the supported portable task names."""
    return {
        "cmake-configure-debug": configure_debug,
        "cmake-build-debug": build_debug,
        "conan-install": conan_install,
        "open-integrated-terminal": open_integrated_terminal,
        "coverage-configure": configure_coverage,
        "coverage-build": build_coverage,
        "coverage": coverage,
        "coverage-metrics": coverage_metrics,
        "coverage-all": coverage_all,
        "valgrind-configure": configure_valgrind,
        "valgrind-build": build_valgrind,
        "valgrind-callgrind": lambda context: valgrind_tool(context, "callgrind"),
        "valgrind-massif": lambda context: valgrind_tool(context, "massif"),
        "valgrind-memcheck": lambda context: valgrind_tool(context, "memcheck"),
        "valgrind-report": valgrind_report,
        "valgrind-all": valgrind_all,
        "asan-configure": lambda context: configure_sanitizer(context, "asan"),
        "asan-build": lambda context: build_sanitizer(context, "asan"),
        "asan": lambda context: run_sanitizer(context, "asan"),
        "tsan-configure": lambda context: configure_sanitizer(context, "tsan"),
        "tsan-build": lambda context: build_sanitizer(context, "tsan"),
        "tsan": lambda context: run_sanitizer(context, "tsan"),
        "sanitizers-all": sanitizers_all,
        "sanitizer-report": sanitizer_report,
        "quality-reports-clean": clean_quality_reports,
        "quality-reports": quality_reports,
    }


def parse_arguments(registry: dict[str, TaskFunction]) -> argparse.Namespace:
    """Parse command-line arguments for the task runner."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "task", nargs="?", choices=sorted(registry), help="task to execute"
    )
    parser.add_argument(
        "--list", action="store_true", help="list supported tasks and exit"
    )
    parser.add_argument(
        "--workspace",
        type=Path,
        help="repository root (defaults to script parent)",
    )
    parser.add_argument(
        "--build-root",
        type=Path,
        help="external build root (defaults to NBN_BUILD_DIR_BASE/repository name)",
    )
    parser.add_argument("--profile", default=DEFAULT_PROFILE, help="Conan profile name")
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="print commands without executing them",
    )
    arguments = parser.parse_args()
    if not arguments.list and arguments.task is None:
        parser.error("a task is required unless --list is specified")
    return arguments


def resolve_workspace(arguments: argparse.Namespace) -> Path:
    """Resolve and validate the repository root."""
    workspace = arguments.workspace or Path(__file__).resolve().parents[1]
    workspace = workspace.expanduser().resolve()
    if not (workspace / "CMakeLists.txt").is_file():
        raise ValueError(f"workspace does not contain CMakeLists.txt: {workspace}")
    return workspace


def resolve_build_root(arguments: argparse.Namespace, workspace: Path) -> Path:
    """Resolve the external build root without requiring VS Code variables."""
    if arguments.build_root is not None:
        return arguments.build_root.expanduser().resolve()
    base = os.environ.get("NBN_BUILD_DIR_BASE")
    if base:
        return (Path(base).expanduser() / workspace.name).resolve()
    return (workspace / "build").resolve()


def main() -> int:
    """Run the selected task."""
    registry = task_registry()
    arguments = parse_arguments(registry)
    if arguments.list:
        print("\n".join(sorted(registry)))
        return 0

    try:
        workspace = resolve_workspace(arguments)
        context = TaskContext(
            workspace=workspace,
            build_root=resolve_build_root(arguments, workspace),
            profile=arguments.profile,
            dry_run=arguments.dry_run,
        )
        return registry[arguments.task](context)
    except (OSError, ValueError) as error:
        print(f"[nbn-task] error: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
