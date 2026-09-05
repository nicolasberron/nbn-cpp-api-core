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
DEFAULT_PROFILE = "conan-debug"
DEFAULT_CONAN_PROFILE = "linux-clang19-debug"
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
    conan_profile: str
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

    @property
    def copilot_workspace(self) -> Path:
        """Return the workspace containing shared Copilot skills."""
        configured_workspace = os.environ.get("NBN_COPILOT_WORKSPACE")
        if configured_workspace:
            return Path(configured_workspace).expanduser().resolve()
        sibling_workspace = self.workspace.parent / "nbn-main-vscode-workspace"
        if (sibling_workspace / ".github/skills").is_dir():
            return sibling_workspace
        return self.workspace

    def command_environment(self) -> dict[str, str]:
        """Return a copy of the process environment for a child command."""
        return os.environ.copy()


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


def require_build_tree(
    context: TaskContext,
    build_directory: Path,
    description: str,
    *,
    require_ninja: bool = False,
) -> int:
    """Reject commands that require a configured CMake build tree."""
    if context.dry_run:
        return 0
    required = [build_directory / "CMakeCache.txt"]
    if require_ninja:
        required.append(build_directory / "build.ninja")
    missing = [path.name for path in required if not path.is_file()]
    if missing:
        print(
            f"[nbn-task] {description} is unavailable; missing "
            f"{', '.join(missing)} in {build_directory}. "
            "Run the matching configure task first.",
            file=sys.stderr,
            flush=True,
        )
        return 1
    return 0


def reset_build_directory(build_directory: Path) -> None:
    """Remove CMake state before regenerating a quality build directory."""
    if build_directory.exists():
        shutil.rmtree(build_directory)


def configure_cmake(
    context: TaskContext,
    build_directory: Path,
    options: Sequence[str],
) -> int:
    """Configure one build directory with only its generated Conan files."""
    cmake_command = [
        "cmake",
        "-S",
        str(context.workspace),
        "-B",
        str(build_directory),
        "-G",
        "Ninja",
        "-DCMAKE_TOOLCHAIN_FILE="
        + str(build_directory / "conan_toolchain.cmake"),
        *options,
    ]
    command = (
        "unset CMAKE_PREFIX_PATH CMAKE_MODULE_PATH"
        " && source "
        + shlex.quote(str(build_directory / "conanbuildenv-debug-x86_64.sh"))
        + " && "
        + shlex.join(cmake_command)
    )
    return run_command(context, shell_command(command))


def configure_debug(context: TaskContext) -> int:
    """Configure the standard Conan debug build."""
    reset_build_directory(context.debug_build)
    result = install_dependencies(context, context.debug_build)
    if result != 0:
        return result
    return configure_cmake(
        context,
        context.debug_build,
        [
            "-DCMAKE_BUILD_TYPE=Debug",
            "-DNBN_CLANG_TIDY_ENABLE=ON",
        ],
    )


def build_debug(context: TaskContext) -> int:
    """Build the standard Conan debug preset."""
    if require_build_tree(context, context.debug_build, "debug build", require_ninja=True):
        return 1
    return run_command(context, ["cmake", "--build", str(context.debug_build)])


def conan_install(context: TaskContext) -> int:
    """Install Conan dependencies for the selected profile."""
    return install_dependencies(context, context.build_root / context.profile)


def install_dependencies(context: TaskContext, output_directory: Path) -> int:
    """Install editable Conan dependencies into one build directory."""
    return run_command(
        context,
        [
            "conan",
            "install",
            ".",
            "-of",
            str(output_directory),
            "-pr",
            context.conan_profile,
            "-o",
            "nbn-cpp-api-ui/*:shared=True",
        ],
    )


def open_integrated_terminal(context: TaskContext) -> int:
    """Open the selected profile build directory with the VS Code CLI."""
    return run_command(context, ["code", str(context.build_root / context.profile)])


def configure_coverage(context: TaskContext) -> int:
    """Configure the LLVM coverage build."""
    reset_build_directory(context.coverage_build)
    result = install_dependencies(context, context.coverage_build)
    if result != 0:
        return result
    return configure_cmake(
        context,
        context.coverage_build,
        [
            "-DCMAKE_BUILD_TYPE=Debug",
            "-DNBN_BUILD_TESTS=ON",
            "-DNBN_LLVM_COVERAGE_ENABLE=ON",
            "-DNBN_CLANG_TIDY_ENABLE=OFF",
        ],
    )


def build_coverage(context: TaskContext) -> int:
    """Build the coverage test target."""
    if require_build_tree(context, context.coverage_build, "coverage build", require_ninja=True):
        return 1
    return run_command(
        context,
        ["cmake", "--build", str(context.coverage_build)],
        cwd=context.workspace,
    )


def coverage(context: TaskContext) -> int:
    """Run LLVM coverage collection and publish the aggregate HTML report."""
    if require_build_tree(context, context.coverage_build, "coverage build", require_ninja=True):
        return 1
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
    if not context.dry_run and not (metrics_directory / "production.txt").is_file():
        print(
            f"[nbn-task] coverage metrics are unavailable; missing production.txt "
            f"in {metrics_directory}. Run the coverage task first.",
            file=sys.stderr,
            flush=True,
        )
        return 1
    result = run_command(
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
    failed_tests_file = metrics_directory / "failed-tests.txt"
    if not context.dry_run and failed_tests_file.is_file() and metrics_file.is_file():
        failed_tests = failed_tests_file.read_text(encoding="utf-8").splitlines()
        with metrics_file.open("a", encoding="utf-8") as output:
            output.write("coverage tests failed: " + ", ".join(failed_tests) + "\n")
    return result


def configure_valgrind(context: TaskContext) -> int:
    """Configure the debug build used by Valgrind."""
    reset_build_directory(context.debug_build)
    result = install_dependencies(context, context.debug_build)
    if result != 0:
        return result
    return configure_cmake(
        context,
        context.debug_build,
        [
            "-DCMAKE_BUILD_TYPE=Debug",
            "-DNBN_BUILD_TESTS=ON",
            "-DNBN_BUILD_BENCHMARKS=OFF",
            "-DNBN_TEST_TIMEOUT_SECONDS=300",
            "-DNBN_CLANG_TIDY_ENABLE=OFF",
        ],
    )


def build_valgrind(context: TaskContext) -> int:
    """Build the default target containing the UI tests used by Valgrind."""
    if require_build_tree(context, context.debug_build, "Valgrind build", require_ninja=True):
        return 1
    return run_command(
        context,
        [
            "cmake",
            "--build",
            str(context.debug_build),
        ],
    )


def valgrind_tool(context: TaskContext, tool: str) -> int:
    """Run one Valgrind tool through the repository CTest script."""
    if require_build_tree(context, context.debug_build, "Valgrind tests", require_ninja=True):
        return 1
    output_directory = context.debug_build / "valgrind-results" / tool
    if tool == "callgrind":
        (context.debug_build / "valgrind-results" / "failed-tests.txt").unlink(
            missing_ok=True
        )
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
    reports_directory = context.debug_build / "valgrind-results"
    if not context.dry_run and not reports_directory.is_dir():
        print(
            f"[nbn-task] Valgrind report inputs are unavailable: {reports_directory}",
            file=sys.stderr,
            flush=True,
        )
        return 1
    return run_python(
        context,
        context.copilot_workspace
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
    reset_build_directory(context.debug_build)
    result = install_dependencies(context, context.debug_build)
    if result != 0:
        return result
    result = configure_cmake(
        context,
        context.debug_build,
        [
            "-DCMAKE_BUILD_TYPE=Debug",
            "-DNBN_BUILD_BENCHMARKS=ON",
            "-DNBN_CLANG_TIDY_ENABLE=OFF",
        ],
    )
    if result != 0:
        return result
    result = run_command(
        context,
        ["cmake", "--build", str(context.debug_build), "--target", BENCHMARK_TARGET],
    )
    if result != 0:
        return result
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
    sanitizer_flag = "address" if sanitizer == "asan" else "thread"
    flags = f"-fsanitize={sanitizer_flag} -fno-omit-frame-pointer"
    extra_options = " -DNBN_TSAN_BUILD=ON" if sanitizer == "tsan" else ""
    reset_build_directory(build_directory)
    result = install_dependencies(context, build_directory)
    if result != 0:
        return result
    options = [
        "-DCMAKE_BUILD_TYPE=Debug",
        "-DCMAKE_C_COMPILER=clang-19",
        "-DCMAKE_CXX_COMPILER=clang++-19",
        f"-DCMAKE_C_FLAGS=-fsanitize={sanitizer_flag}",
        f"-DCMAKE_CXX_FLAGS={flags}",
        f"-DCMAKE_EXE_LINKER_FLAGS=-fsanitize={sanitizer_flag}",
        f"-DCMAKE_SHARED_LINKER_FLAGS=-fsanitize={sanitizer_flag}",
        "-DNBN_BUILD_TESTS=ON",
        "-DNBN_BUILD_BENCHMARKS=OFF",
        "-DNBN_LLVM_COVERAGE_ENABLE=OFF",
        "-DNBN_CLANG_TIDY_ENABLE=OFF",
        "-DNBN_TEST_TIMEOUT_SECONDS=300",
    ]
    if extra_options:
        options.append(extra_options.strip())
    return configure_cmake(context, build_directory, options)


def build_sanitizer(context: TaskContext, sanitizer: str) -> int:
    """Build the default target containing the UI tests for a sanitizer."""
    build_directory = context.asan_build if sanitizer == "asan" else context.tsan_build
    if require_build_tree(context, build_directory, f"{sanitizer} build", require_ninja=True):
        return 1
    return run_command(
        context,
        ["cmake", "--build", str(build_directory)],
    )


def run_sanitizer(context: TaskContext, sanitizer: str) -> int:
    """Run sanitizer tests while preserving the complete visible CTest log."""
    build_directory = context.asan_build if sanitizer == "asan" else context.tsan_build
    if require_build_tree(context, build_directory, f"{sanitizer} tests", require_ninja=True):
        return 1
    options = (
        "detect_leaks=1:halt_on_error=1:verbosity=1"
        if sanitizer == "asan"
        else "halt_on_error=1:verbosity=1"
    )
    environment = context.command_environment()
    environment[f"{sanitizer.upper()}_OPTIONS"] = options
    return run_command(
        context,
        [
            "ctest",
            "--test-dir",
            str(build_directory),
            "--output-on-failure",
            "--stop-on-failure",
        ],
        environment=environment,
        output_file=build_directory / "quality-ctest.log",
    )


def sanitizer_report(context: TaskContext) -> int:
    """Generate the combined ASan and TSan Markdown report."""
    report_directories = []
    for build_directory in (context.asan_build, context.tsan_build):
        report_directory = build_directory / "Testing" / "Temporary"
        if not context.dry_run:
            report_directory.mkdir(parents=True, exist_ok=True)
        report_directories.append(report_directory)
    analyzer = (
        context.copilot_workspace
        / ".github/skills/nbn-analyze-llvm-sanitizer-reports/scripts"
        / "analyze_llvm_sanitizers.py"
    )
    return run_python(
        context,
        analyzer,
        [
            "--reports",
            *(str(directory) for directory in report_directories),
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
    """Run dependent stages until the first failure."""
    for task in tasks:
        result = task(context)
        if result != 0:
            print(f"[nbn-task] task failed with exit code {result}", flush=True)
            return result
    return 0


def coverage_all(context: TaskContext) -> int:
    """Run coverage and publish metrics even when collection reports failures."""
    result = run_sequence(context, [configure_coverage, build_coverage])
    if result != 0:
        return result
    coverage_result = coverage(context)
    metrics_result = coverage_metrics(context)
    return first_failure([coverage_result, metrics_result])


def valgrind_all(context: TaskContext) -> int:
    """Run Valgrind tools until failure, then publish available diagnostics."""
    results_directory = context.debug_build / "valgrind-results"
    results_directory.mkdir(parents=True, exist_ok=True)
    (results_directory / "failed-tests.txt").unlink(missing_ok=True)
    for failure_log in (context.debug_build / "Testing" / "Temporary").glob(
        "LastTestsFailed_*.log"
    ):
        failure_log.unlink(missing_ok=True)
    results: list[int] = []
    for task in (
        configure_valgrind,
        build_valgrind,
        lambda item: valgrind_tool(item, "callgrind"),
        lambda item: valgrind_tool(item, "memcheck"),
        lambda item: valgrind_tool(item, "massif"),
    ):
        result = task(context)
        results.append(result)
        if result != 0:
            print(f"[nbn-task] task failed with exit code {result}", flush=True)
            break
    if context.dry_run or (context.debug_build / "valgrind-results").is_dir():
        results.append(valgrind_report(context))
    return first_failure(results)


def sanitizers_all(context: TaskContext) -> int:
    """Run both sanitizer suites and publish reports even after test failures."""
    results: list[int] = []
    for sanitizer in ("asan", "tsan"):
        for task in (
            lambda item, name=sanitizer: configure_sanitizer(item, name),
            lambda item, name=sanitizer: build_sanitizer(item, name),
            lambda item, name=sanitizer: run_sanitizer(item, name),
        ):
            result = task(context)
            results.append(result)
            if result != 0:
                print(f"[nbn-task] task failed with exit code {result}", flush=True)
    results.append(sanitizer_report(context))
    return first_failure(results)


def quality_reports(context: TaskContext) -> int:
    """Generate all quality reports and the final README despite stage failures."""
    results = [clean_quality_reports(context)]
    results.extend(
        [
            coverage_all(context),
            benchmark_all(context),
            valgrind_all(context),
            sanitizers_all(context),
        ]
    )
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
    parser.add_argument(
        "--profile", default=DEFAULT_PROFILE, help="CMake preset name"
    )
    parser.add_argument(
        "--conan-profile",
        default=DEFAULT_CONAN_PROFILE,
        help="Conan profile name used for dependency installation",
    )
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
            conan_profile=arguments.conan_profile,
            dry_run=arguments.dry_run,
        )
        return registry[arguments.task](context)
    except (OSError, ValueError) as error:
        print(f"[nbn-task] error: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
