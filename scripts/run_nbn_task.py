#!/usr/bin/env python3
"""Run nbn-cpp-api-core development tasks outside of VS Code."""

from __future__ import annotations

import argparse
import ast
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
DEBUG_PROFILE = "linux-clang19-debug"
COVERAGE_PROFILE = "linux-clang19-coverage"
ASAN_PROFILE = "linux-clang19-asan"
TSAN_PROFILE = "linux-clang19-tsan"
UBSAN_PROFILE = "linux-clang19-ubsan"
VALGRIND_PROFILE = "linux-clang19-valgrind"
BENCHMARK_TARGET = "benchmark_serialization"
COVERAGE_BUILD_NAME = COVERAGE_PROFILE
DEBUG_BUILD_NAME = DEBUG_PROFILE
ASAN_BUILD_NAME = ASAN_PROFILE
TSAN_BUILD_NAME = TSAN_PROFILE
UBSAN_BUILD_NAME = UBSAN_PROFILE
VALGRIND_BUILD_NAME = VALGRIND_PROFILE
QUALITY_REPORTS_NAME = "quality-reports"
CMAKE_CLANG_TIDY_ON = "-DNBN_CLANG_TIDY_ENABLE=ON"
BUILD_SYSTEM_ROOT_ENV = "NBN_CPP_API_BUILD_SYSTEM_ROOT"
BUILD_SYSTEM_PROFILES_ENV = "NBN_CPP_API_PROFILES_DIR"
COPILOT_WORKSPACE_ENV = "NBN_COPILOT_WORKSPACE"
MAX_BUILD_JOBS = os.cpu_count() or 1


def resolve_shared_scripts(workspace: Path) -> Path:
    """Find quality scripts from the build-system package or checkout."""
    candidates = []
    configured_root = os.environ.get(BUILD_SYSTEM_ROOT_ENV)
    if configured_root:
        candidates.append(Path(configured_root).expanduser() / "scripts")
    candidates.append(
        workspace.parent / "nbn-cpp-api-build-system" / "scripts"
    )
    for candidate in candidates:
        if (candidate / "run_llvm_coverage.py").is_file():
            return candidate.resolve()
    locations = ", ".join(str(candidate) for candidate in candidates)
    raise ValueError(
        "Cannot find nbn-cpp-api-build-system scripts. Source the Conan "
        f"environment or provide a sibling checkout. Checked: {locations}"
    )


def resolve_analyzer_script(
    workspace: Path,
    skill_name: str,
    script_name: str,
) -> Path:
    """Find an analyzer in a configured or sibling skill collection."""
    roots = []
    configured_workspace = os.environ.get(COPILOT_WORKSPACE_ENV)
    if configured_workspace:
        roots.append(Path(configured_workspace).expanduser())
    configured_build_system = os.environ.get(BUILD_SYSTEM_ROOT_ENV)
    if configured_build_system:
        roots.append(Path(configured_build_system).expanduser())
    roots.extend(
        [
            workspace.parent / "nbn-main-vscode-workspace",
            workspace.parent / "nbn-cpp-api-build-system",
            workspace,
        ]
    )
    candidates = [
        root / ".github" / "skills" / skill_name / "scripts" / script_name
        for root in roots
    ]
    for candidate in candidates:
        if candidate.is_file():
            return candidate.resolve()
    locations = ", ".join(str(candidate) for candidate in candidates)
    raise ValueError(
        f"Cannot find analyzer script {script_name}. Checked: {locations}"
    )


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
    def ubsan_build(self) -> Path:
        return self.build_root / UBSAN_BUILD_NAME

    @property
    def valgrind_build(self) -> Path:
        return self.build_root / VALGRIND_BUILD_NAME

    @property
    def quality_reports(self) -> Path:
        return self.workspace / "tmp" / QUALITY_REPORTS_NAME

    @property
    def shared_scripts(self) -> Path:
        """Return the shared build-system task helper directory."""
        return resolve_shared_scripts(self.workspace)

    @property
    def production_library_prefixes(self) -> tuple[str, ...]:
        """Return production library prefixes included in coverage reports."""
        repository_prefix = "nbn-cpp-api-"
        component = (
            self.workspace.name.removeprefix(repository_prefix)
            if self.workspace.name.startswith(repository_prefix)
            else "core"
        )
        library = f"libnbn-{component}"
        return (f"{library}.so", f"{library}.a")

    @property
    def coverage_header_only(self) -> bool:
        """Return whether coverage comes only from test executables."""
        conanfile = self.workspace / "conanfile.py"
        if not conanfile.is_file():
            return False
        try:
            module = ast.parse(conanfile.read_text(encoding="utf-8"))
        except SyntaxError as error:
            raise ValueError(f"Cannot parse Conan recipe: {conanfile}") from error
        for class_node in module.body:
            if not isinstance(class_node, ast.ClassDef):
                continue
            for node in class_node.body:
                targets = node.targets if isinstance(node, ast.Assign) else []
                if isinstance(node, ast.AnnAssign):
                    targets = [node.target]
                if (
                    any(
                        isinstance(target, ast.Name)
                        and target.id == "package_type"
                        for target in targets
                    )
                    and isinstance(node.value, ast.Constant)
                    and node.value.value == "header-library"
                ):
                    return True
        return False

    @property
    def include_benchmark_report(self) -> bool:
        """Return whether the aggregate report includes benchmark results."""
        return True

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


def cmake_build_command(build_directory: Path, target: str | None = None) -> list[str]:
    """Build with all CPUs available to the host."""
    command = ["cmake", "--build", str(build_directory), "--parallel", str(MAX_BUILD_JOBS)]
    if target is not None:
        command.extend(["--target", target])
    return command


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


def reset_build_directory(
    build_directory: Path, *, dry_run: bool = False
) -> None:
    """Remove CMake state before regenerating a quality build directory."""
    if dry_run:
        print(f"[nbn-task] remove {build_directory}", flush=True)
        return
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
    reset_build_directory(context.debug_build, dry_run=context.dry_run)
    result = install_dependencies(context, context.debug_build)
    if result != 0:
        return result
    return configure_cmake(
        context,
        context.debug_build,
        [
            "-DCMAKE_BUILD_TYPE=Debug",
            "-DNBN_BUILD_TESTS=ON",
            CMAKE_CLANG_TIDY_ON,
        ],
    )


def build_debug(context: TaskContext) -> int:
    """Build the standard Conan debug preset."""
    if require_build_tree(context, context.debug_build, "debug build", require_ninja=True):
        return 1
    return run_command(context, cmake_build_command(context.debug_build))


def conan_install(context: TaskContext) -> int:
    """Install Conan dependencies for the selected profile."""
    return install_dependencies(context, context.build_root / context.profile)


def resolve_conan_profile(context: TaskContext, conan_profile: str) -> str:
    """Resolve a packaged, sibling, or Conan-cached profile name."""
    profile_path = Path(conan_profile).expanduser()
    if profile_path.is_file():
        return str(profile_path.resolve())

    candidates = []
    configured_profiles = os.environ.get(BUILD_SYSTEM_PROFILES_ENV)
    if configured_profiles:
        candidates.append(Path(configured_profiles).expanduser())
    candidates.append(context.workspace.parent / "nbn-cpp-api-build-system" / "profiles")
    for profiles_directory in candidates:
        candidate = profiles_directory / conan_profile
        if candidate.is_file():
            return str(candidate.resolve())
    return conan_profile


def install_dependencies(
    context: TaskContext,
    output_directory: Path,
    conan_profile: str | None = None,
) -> int:
    """Install editable Conan dependencies into one build directory."""
    selected_profile = conan_profile or context.conan_profile
    return run_command(
        context,
        [
            "conan",
            "install",
            ".",
            "-of",
            str(output_directory),
            "-pr",
            resolve_conan_profile(
                context,
                selected_profile,
            ),
            "-c:h",
            f"user.nbn:profile={Path(selected_profile).name}",
            "-o",
            "nbn-cpp-api-ui/*:shared=True",
        ],
    )


def open_integrated_terminal(context: TaskContext) -> int:
    """Open the selected profile build directory with the VS Code CLI."""
    return run_command(context, ["code", str(context.build_root / context.profile)])


def configure_coverage(context: TaskContext) -> int:
    """Configure the LLVM coverage build."""
    reset_build_directory(context.coverage_build, dry_run=context.dry_run)
    result = install_dependencies(
        context,
        context.coverage_build,
        COVERAGE_PROFILE,
    )
    if result != 0:
        return result
    return configure_cmake(
        context,
        context.coverage_build,
        [
            "-DCMAKE_BUILD_TYPE=Debug",
            "-DNBN_BUILD_TESTS=ON",
            CMAKE_CLANG_TIDY_ON,
        ],
    )


def build_coverage(context: TaskContext) -> int:
    """Build the coverage test target."""
    if require_build_tree(context, context.coverage_build, "coverage build", require_ninja=True):
        return 1
    return run_command(
        context,
        cmake_build_command(context.coverage_build),
        cwd=context.workspace,
    )


def coverage(context: TaskContext) -> int:
    """Run LLVM coverage collection and publish the aggregate HTML report."""
    if require_build_tree(context, context.coverage_build, "coverage build", require_ninja=True):
        return 1
    return run_python(
        context,
        context.shared_scripts / "run_llvm_coverage.py",
        [
            str(context.coverage_build),
            "--output-dir",
            str(context.quality_reports / "coverage"),
            *(
                argument
                for prefix in context.production_library_prefixes
                for argument in ("--library-prefix", prefix)
            ),
            *(["--header-only"] if context.coverage_header_only else []),
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
                    str(context.shared_scripts / "coverage_metrics.py"),
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
    reset_build_directory(context.valgrind_build, dry_run=context.dry_run)
    result = install_dependencies(
        context,
        context.valgrind_build,
        VALGRIND_PROFILE,
    )
    if result != 0:
        return result
    return configure_cmake(
        context,
        context.valgrind_build,
        [
            "-DCMAKE_BUILD_TYPE=Debug",
            "-DNBN_BUILD_TESTS=ON",
            "-DNBN_BUILD_BENCHMARKS=OFF",
            "-DNBN_TEST_TIMEOUT_SECONDS=300",
            CMAKE_CLANG_TIDY_ON,
        ],
    )


def build_valgrind(context: TaskContext) -> int:
    """Build the default target containing the UI tests used by Valgrind."""
    if require_build_tree(
        context,
        context.valgrind_build,
        "Valgrind build",
        require_ninja=True,
    ):
        return 1
    return run_command(context, cmake_build_command(context.valgrind_build))


def valgrind_tool(context: TaskContext, tool: str) -> int:
    """Run one Valgrind tool through the repository CTest script."""
    if require_build_tree(
        context,
        context.valgrind_build,
        "Valgrind tests",
        require_ninja=True,
    ):
        return 1
    output_directory = context.valgrind_build / "valgrind-results" / tool
    if tool == "callgrind" and not context.dry_run:
        failed_tests = (
            context.valgrind_build / "valgrind-results" / "failed-tests.txt"
        )
        failed_tests.unlink(missing_ok=True)
    return run_command(
        context,
        [
            "ctest",
            "-S",
            str(context.shared_scripts / "run_valgrind_ctest.cmake"),
            "-VV",
            f"-DCTEST_BINARY_DIRECTORY={context.valgrind_build}",
            f"-DCTEST_SOURCE_DIRECTORY={context.workspace}",
            f"-DVALGRIND_TOOL={tool}",
            f"-DVALGRIND_OUTPUT_DIR={output_directory}",
        ],
    )


def valgrind_report(context: TaskContext) -> int:
    """Generate published Valgrind Markdown reports."""
    reports_directory = context.valgrind_build / "valgrind-results"
    if not context.dry_run and not reports_directory.is_dir():
        print(
            f"[nbn-task] Valgrind report inputs are unavailable: {reports_directory}",
            file=sys.stderr,
            flush=True,
        )
        return 1
    return run_python(
        context,
        resolve_analyzer_script(
            context.workspace,
            "nbn-analyze-valgrind-reports",
            "analyze_valgrind.py",
        ),
        [
            "--reports",
            str(context.valgrind_build / "valgrind-results"),
            "--output",
            str(context.quality_reports / "valgrind"),
        ],
    )


def benchmark_all(context: TaskContext) -> int:
    """Build and run the normal serialization benchmark for publication."""
    reset_build_directory(context.debug_build, dry_run=context.dry_run)
    result = install_dependencies(context, context.debug_build)
    if result != 0:
        return result
    result = configure_cmake(
        context,
        context.debug_build,
        [
            "-DCMAKE_BUILD_TYPE=Debug",
            "-DNBN_BUILD_BENCHMARKS=ON",
            CMAKE_CLANG_TIDY_ON,
        ],
    )
    if result != 0:
        return result
    result = run_command(
        context,
        cmake_build_command(context.debug_build, BENCHMARK_TARGET),
    )
    if result != 0:
        return result
    return run_command(
        context,
        [
            str(context.shared_scripts / "run_benchmarks.sh"),
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


def sanitizer_configuration(
    context: TaskContext,
    sanitizer: str,
) -> tuple[Path, str]:
    """Return the build directory and Conan profile for a sanitizer."""
    configurations = {
        "asan": (context.asan_build, ASAN_PROFILE),
        "tsan": (context.tsan_build, TSAN_PROFILE),
        "ubsan": (context.ubsan_build, UBSAN_PROFILE),
    }
    try:
        return configurations[sanitizer]
    except KeyError as error:
        raise ValueError(f"unsupported sanitizer: {sanitizer}") from error


def configure_sanitizer(context: TaskContext, sanitizer: str) -> int:
    """Configure a sanitizer build selected by its Conan profile."""
    build_directory, conan_profile = sanitizer_configuration(
        context,
        sanitizer,
    )
    reset_build_directory(build_directory, dry_run=context.dry_run)
    result = install_dependencies(context, build_directory, conan_profile)
    if result != 0:
        return result
    options = [
        "-DCMAKE_BUILD_TYPE=Debug",
        "-DNBN_BUILD_TESTS=ON",
        "-DNBN_BUILD_BENCHMARKS=OFF",
        CMAKE_CLANG_TIDY_ON,
        "-DNBN_TEST_TIMEOUT_SECONDS=300",
    ]
    return configure_cmake(context, build_directory, options)


def build_sanitizer(context: TaskContext, sanitizer: str) -> int:
    """Build the default target containing the UI tests for a sanitizer."""
    build_directory, _ = sanitizer_configuration(context, sanitizer)
    if require_build_tree(context, build_directory, f"{sanitizer} build", require_ninja=True):
        return 1
    return run_command(
        context,
        cmake_build_command(build_directory),
    )


def run_sanitizer(context: TaskContext, sanitizer: str) -> int:
    """Run sanitizer tests while preserving the complete visible CTest log."""
    build_directory, _ = sanitizer_configuration(context, sanitizer)
    if require_build_tree(context, build_directory, f"{sanitizer} tests", require_ninja=True):
        return 1
    options = {
        "asan": "detect_leaks=1:halt_on_error=1:verbosity=1",
        "tsan": "halt_on_error=1:verbosity=1",
        "ubsan": "halt_on_error=1:print_stacktrace=1",
    }[sanitizer]
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
    """Generate the combined sanitizer Markdown report."""
    report_directories = []
    for build_directory in (context.asan_build, context.tsan_build):
        report_directory = build_directory / "Testing" / "Temporary"
        if not context.dry_run:
            report_directory.mkdir(parents=True, exist_ok=True)
        report_directories.append(report_directory)
    analyzer = resolve_analyzer_script(
        context.workspace,
        "nbn-analyze-llvm-sanitizer-reports",
        "analyze_llvm_sanitizers.py",
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
    expected_parent = context.workspace / "tmp"
    if (
        context.quality_reports == context.workspace
        or context.quality_reports.parent != expected_parent
    ):
        raise ValueError(
            "quality report path must be directly below the workspace "
            "tmp directory"
        )
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
    arguments = [
        "--workspace",
        str(context.workspace),
        "--build-root",
        str(context.build_root),
    ]
    if not context.include_benchmark_report:
        arguments.append("--no-benchmark")
    return run_python(
        context,
        context.shared_scripts / "write_quality_report_readme.py",
        arguments,
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
    reset_build_directory(context.coverage_build, dry_run=context.dry_run)
    result = run_sequence(context, [configure_coverage, build_coverage])
    if result != 0:
        return result
    coverage_result = coverage(context)
    metrics_result = coverage_metrics(context)
    return first_failure([coverage_result, metrics_result])


def valgrind_all(context: TaskContext) -> int:
    """Run Valgrind tools until failure, then publish available diagnostics."""
    results_directory = context.valgrind_build / "valgrind-results"
    if not context.dry_run:
        results_directory.mkdir(parents=True, exist_ok=True)
        (results_directory / "failed-tests.txt").unlink(missing_ok=True)
        temporary_directory = context.valgrind_build / "Testing" / "Temporary"
        for failure_log in temporary_directory.glob("LastTestsFailed_*.log"):
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
    if context.dry_run or results_directory.is_dir():
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


class TaskRunner:
    """Extensible command registry for repository development tasks."""

    context_type = TaskContext
    default_workspace = Path(__file__).resolve().parents[1]

    @staticmethod
    def reset_build_directory(
        context: TaskContext, build_directory: Path
    ) -> None:
        """Remove CMake state without mutating the tree during a dry run."""
        reset_build_directory(build_directory, dry_run=context.dry_run)

    configure_debug = staticmethod(configure_debug)
    build_debug = staticmethod(build_debug)
    conan_install = staticmethod(conan_install)
    open_integrated_terminal = staticmethod(open_integrated_terminal)
    configure_coverage = staticmethod(configure_coverage)
    build_coverage = staticmethod(build_coverage)
    coverage = staticmethod(coverage)
    coverage_metrics = staticmethod(coverage_metrics)
    configure_valgrind = staticmethod(configure_valgrind)
    build_valgrind = staticmethod(build_valgrind)
    valgrind_tool = staticmethod(valgrind_tool)
    valgrind_report = staticmethod(valgrind_report)
    benchmark_all = staticmethod(benchmark_all)
    configure_sanitizer = staticmethod(configure_sanitizer)
    build_sanitizer = staticmethod(build_sanitizer)
    run_sanitizer = staticmethod(run_sanitizer)
    sanitizer_report = staticmethod(sanitizer_report)
    clean_quality_reports = staticmethod(clean_quality_reports)
    quality_readme = staticmethod(quality_readme)

    def coverage_all(self, context: TaskContext) -> int:
        """Run coverage and publish metrics after collection failures."""
        self.reset_build_directory(context, context.coverage_build)
        result = run_sequence(
            context, [self.configure_coverage, self.build_coverage]
        )
        if result != 0:
            return result
        coverage_result = self.coverage(context)
        metrics_result = self.coverage_metrics(context)
        return first_failure([coverage_result, metrics_result])

    def valgrind_all(self, context: TaskContext) -> int:
        """Run Valgrind tools and publish available diagnostics."""
        results_directory = context.valgrind_build / "valgrind-results"
        if not context.dry_run:
            results_directory.mkdir(parents=True, exist_ok=True)
            (results_directory / "failed-tests.txt").unlink(missing_ok=True)
            temporary_directory = (
                context.valgrind_build / "Testing" / "Temporary"
            )
            for failure_log in temporary_directory.glob(
                "LastTestsFailed_*.log"
            ):
                failure_log.unlink(missing_ok=True)
        results: list[int] = []
        for task in (
            self.configure_valgrind,
            self.build_valgrind,
            lambda item: self.valgrind_tool(item, "callgrind"),
            lambda item: self.valgrind_tool(item, "memcheck"),
            lambda item: self.valgrind_tool(item, "massif"),
        ):
            result = task(context)
            results.append(result)
            if result != 0:
                print(
                    f"[nbn-task] task failed with exit code {result}",
                    flush=True,
                )
                break
        if context.dry_run or results_directory.is_dir():
            results.append(self.valgrind_report(context))
        return first_failure(results)

    def sanitizers_all(self, context: TaskContext) -> int:
        """Run sanitizer suites and publish reports after failures."""
        results: list[int] = []
        for sanitizer in ("asan", "tsan"):
            for task in (
                lambda item, name=sanitizer: self.configure_sanitizer(
                    item, name
                ),
                lambda item, name=sanitizer: self.build_sanitizer(item, name),
                lambda item, name=sanitizer: self.run_sanitizer(item, name),
            ):
                result = task(context)
                results.append(result)
                if result != 0:
                    print(
                        f"[nbn-task] task failed with exit code {result}",
                        flush=True,
                    )
        results.append(self.sanitizer_report(context))
        return first_failure(results)

    def quality_reports(self, context: TaskContext) -> int:
        """Generate all quality reports and the final README."""
        results = [self.clean_quality_reports(context)]
        results.extend(
            [
                self.coverage_all(context),
                self.benchmark_all(context),
                self.valgrind_all(context),
                self.sanitizers_all(context),
            ]
        )
        results.append(self.quality_readme(context))
        return first_failure(results)

    def task_registry(self) -> dict[str, TaskFunction]:
        """Return the supported portable task names."""
        return {
            "cmake-configure-debug": self.configure_debug,
            "cmake-build-debug": self.build_debug,
            "conan-install": self.conan_install,
            "open-integrated-terminal": self.open_integrated_terminal,
            "coverage-configure": self.configure_coverage,
            "coverage-build": self.build_coverage,
            "coverage": self.coverage,
            "coverage-metrics": self.coverage_metrics,
            "coverage-all": self.coverage_all,
            "valgrind-configure": self.configure_valgrind,
            "valgrind-build": self.build_valgrind,
            "valgrind-callgrind": lambda context: self.valgrind_tool(
                context, "callgrind"
            ),
            "valgrind-massif": lambda context: self.valgrind_tool(
                context, "massif"
            ),
            "valgrind-memcheck": lambda context: self.valgrind_tool(
                context, "memcheck"
            ),
            "valgrind-report": self.valgrind_report,
            "valgrind-all": self.valgrind_all,
            "asan-configure": lambda context: self.configure_sanitizer(
                context, "asan"
            ),
            "asan-build": lambda context: self.build_sanitizer(
                context, "asan"
            ),
            "asan": lambda context: self.run_sanitizer(context, "asan"),
            "tsan-configure": lambda context: self.configure_sanitizer(
                context, "tsan"
            ),
            "tsan-build": lambda context: self.build_sanitizer(
                context, "tsan"
            ),
            "tsan": lambda context: self.run_sanitizer(context, "tsan"),
            "ubsan-configure": lambda context: self.configure_sanitizer(
                context, "ubsan"
            ),
            "ubsan-build": lambda context: self.build_sanitizer(
                context, "ubsan"
            ),
            "ubsan": lambda context: self.run_sanitizer(context, "ubsan"),
            "sanitizers-all": self.sanitizers_all,
            "sanitizer-report": self.sanitizer_report,
            "quality-reports-clean": self.clean_quality_reports,
            "quality-reports": self.quality_reports,
        }

    def main(self) -> int:
        """Parse arguments, create a context, and run the selected task."""
        registry = self.task_registry()
        arguments = parse_arguments(registry)
        if arguments.list:
            print("\n".join(sorted(registry)))
            return 0

        try:
            workspace = arguments.workspace or self.default_workspace
            workspace = workspace.expanduser().resolve()
            if not (workspace / "CMakeLists.txt").is_file():
                raise ValueError(
                    f"workspace does not contain CMakeLists.txt: {workspace}"
                )
            workspace_runner = workspace / "scripts" / "run_nbn_task.py"
            if (
                type(self) is TaskRunner
                and workspace_runner.is_file()
                and workspace_runner.resolve() != Path(__file__).resolve()
            ):
                command = [
                    sys.executable,
                    str(workspace_runner),
                    arguments.task,
                    "--workspace",
                    str(workspace),
                    "--profile",
                    arguments.profile,
                    "--conan-profile",
                    arguments.conan_profile,
                ]
                if arguments.build_root is not None:
                    command.extend(
                        ["--build-root", str(arguments.build_root.resolve())]
                    )
                if arguments.dry_run:
                    command.append("--dry-run")
                completed = subprocess.run(
                    command,
                    cwd=workspace,
                    check=False,
                )
                return completed.returncode
            context = self.context_type(
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


def task_registry() -> dict[str, TaskFunction]:
    """Return the supported portable task names."""
    return TaskRunner().task_registry()


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
    return TaskRunner().main()


if __name__ == "__main__":
    raise SystemExit(main())
