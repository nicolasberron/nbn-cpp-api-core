#!/usr/bin/env python3
"""Contract tests for profile-driven instrumentation tasks."""

from __future__ import annotations

import importlib.util
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch


SCRIPT_PATH = (
    Path(__file__).resolve().parents[2] / "scripts" / "run_nbn_task.py"
)
SPECIFICATION = importlib.util.spec_from_file_location(
    "run_nbn_task",
    SCRIPT_PATH,
)
if SPECIFICATION is None or SPECIFICATION.loader is None:
    raise RuntimeError(f"Cannot load task runner: {SCRIPT_PATH}")
TASKS = importlib.util.module_from_spec(SPECIFICATION)
sys.modules[SPECIFICATION.name] = TASKS
SPECIFICATION.loader.exec_module(TASKS)


class InstrumentationProfileTests(unittest.TestCase):
    """Verify instrumentation is selected through Conan profiles."""

    def setUp(self) -> None:
        self.temporary_directory = tempfile.TemporaryDirectory()
        root = Path(self.temporary_directory.name)
        self.context = TASKS.TaskContext(
            workspace=root / "workspace",
            build_root=root / "build",
            profile=TASKS.DEFAULT_PROFILE,
            conan_profile=TASKS.DEFAULT_CONAN_PROFILE,
            dry_run=True,
        )

    def tearDown(self) -> None:
        self.temporary_directory.cleanup()

    def assert_profile_driven_configure(
        self,
        configure,
        expected_profile: str,
    ) -> None:
        commands: list[list[str]] = []

        def capture_command(context, command, **kwargs):
            commands.append(list(command))
            return 0

        with (
            patch.dict("os.environ", {}, clear=True),
            patch.object(
                TASKS,
                "run_command",
                side_effect=capture_command,
            ),
        ):
            self.assertEqual(configure(self.context), 0)

        conan_command = next(
            command for command in commands if command[0] == "conan"
        )
        self.assertEqual(
            conan_command[conan_command.index("-pr") + 1],
            expected_profile,
        )
        self.assertEqual(
            conan_command[conan_command.index("-c:h") + 1],
            f"user.nbn:profile={expected_profile}",
        )
        rendered_commands = "\n".join(
            " ".join(command) for command in commands
        )
        self.assertNotIn("-fsanitize=", rendered_commands)
        self.assertNotIn("NBN_LLVM_COVERAGE_ENABLE", rendered_commands)

    def test_coverage_configure_uses_coverage_profile(self) -> None:
        self.assert_profile_driven_configure(
            TASKS.configure_coverage,
            TASKS.COVERAGE_PROFILE,
        )

    def test_asan_configure_uses_asan_profile(self) -> None:
        self.assert_profile_driven_configure(
            lambda context: TASKS.configure_sanitizer(context, "asan"),
            TASKS.ASAN_PROFILE,
        )

    def test_tsan_configure_uses_tsan_profile(self) -> None:
        self.assert_profile_driven_configure(
            lambda context: TASKS.configure_sanitizer(context, "tsan"),
            TASKS.TSAN_PROFILE,
        )

    def test_ubsan_configure_uses_ubsan_profile(self) -> None:
        self.assert_profile_driven_configure(
            lambda context: TASKS.configure_sanitizer(context, "ubsan"),
            TASKS.UBSAN_PROFILE,
        )

    def test_valgrind_configure_uses_valgrind_profile(self) -> None:
        self.assert_profile_driven_configure(
            TASKS.configure_valgrind,
            TASKS.VALGRIND_PROFILE,
        )

    def test_profile_resolves_from_packaged_directory(self) -> None:
        profiles_directory = Path(self.temporary_directory.name) / "profiles"
        profiles_directory.mkdir()
        profile = profiles_directory / TASKS.ASAN_PROFILE
        profile.touch()

        with patch.dict(
            "os.environ",
            {TASKS.BUILD_SYSTEM_PROFILES_ENV: str(profiles_directory)},
        ):
            resolved = TASKS.resolve_conan_profile(
                self.context,
                TASKS.ASAN_PROFILE,
            )

        self.assertEqual(resolved, str(profile.resolve()))

    def test_shared_scripts_resolve_from_build_system_root(self) -> None:
        build_system_root = (
            Path(self.temporary_directory.name) / "build-system"
        )
        scripts_directory = build_system_root / "scripts"
        scripts_directory.mkdir(parents=True)
        (scripts_directory / "run_llvm_coverage.py").touch()

        with patch.dict(
            "os.environ",
            {TASKS.BUILD_SYSTEM_ROOT_ENV: str(build_system_root)},
        ):
            resolved = TASKS.resolve_shared_scripts(self.context.workspace)

        self.assertEqual(resolved, scripts_directory.resolve())

    def test_shared_scripts_reject_missing_configured_root(self) -> None:
        missing_root = Path(self.temporary_directory.name) / "missing"

        with (
            patch.dict(
                "os.environ",
                {TASKS.BUILD_SYSTEM_ROOT_ENV: str(missing_root)},
            ),
            self.assertRaisesRegex(
                ValueError,
                "Cannot find nbn-cpp-api-build-system scripts",
            ),
        ):
            TASKS.resolve_shared_scripts(self.context.workspace)

    def test_shared_scripts_resolve_from_sibling_checkout(self) -> None:
        scripts_directory = (
            self.context.workspace.parent
            / "nbn-cpp-api-build-system"
            / "scripts"
        )
        scripts_directory.mkdir(parents=True)
        (scripts_directory / "run_llvm_coverage.py").touch()

        with patch.dict("os.environ", {}, clear=True):
            resolved = TASKS.resolve_shared_scripts(self.context.workspace)

        self.assertEqual(resolved, scripts_directory.resolve())

    def test_analyzer_falls_back_from_invalid_copilot_workspace(self) -> None:
        build_system_root = (
            self.context.workspace.parent / "nbn-cpp-api-build-system"
        )
        analyzer = (
            build_system_root
            / ".github/skills/nbn-analyze-valgrind-reports/scripts"
            / "analyze_valgrind.py"
        )
        analyzer.parent.mkdir(parents=True)
        analyzer.touch()

        with patch.dict(
            "os.environ",
            {TASKS.COPILOT_WORKSPACE_ENV: str(self.context.workspace.parent)},
            clear=True,
        ):
            resolved = TASKS.resolve_analyzer_script(
                self.context.workspace,
                "nbn-analyze-valgrind-reports",
                "analyze_valgrind.py",
            )

        self.assertEqual(resolved, analyzer.resolve())

    def test_report_tasks_use_build_system_analyzers(self) -> None:
        build_system_root = (
            Path(self.temporary_directory.name) / "build-system"
        )
        valgrind_analyzer = (
            build_system_root
            / ".github/skills/nbn-analyze-valgrind-reports/scripts"
            / "analyze_valgrind.py"
        )
        sanitizer_analyzer = (
            build_system_root
            / ".github/skills/nbn-analyze-llvm-sanitizer-reports/scripts"
            / "analyze_llvm_sanitizers.py"
        )
        for analyzer in (valgrind_analyzer, sanitizer_analyzer):
            analyzer.parent.mkdir(parents=True)
            analyzer.touch()

        with (
            patch.dict(
                "os.environ",
                {TASKS.BUILD_SYSTEM_ROOT_ENV: str(build_system_root)},
                clear=True,
            ),
            patch.object(TASKS, "run_command", return_value=0) as run_command,
        ):
            self.assertEqual(TASKS.valgrind_report(self.context), 0)
            self.assertEqual(TASKS.sanitizer_report(self.context), 0)

        commands = [call.args[1] for call in run_command.call_args_list]
        self.assertEqual(commands[0][1], str(valgrind_analyzer.resolve()))
        self.assertEqual(commands[1][1], str(sanitizer_analyzer.resolve()))
        for command in commands:
            self.assertEqual(
                command[command.index("--repository-root") + 1],
                str(self.context.workspace),
            )
        self.assertEqual(
            self.context.quality_reports,
            self.context.workspace / "tmp" / "quality-reports",
        )
        self.assertEqual(
            commands[0][commands[0].index("--output") + 1],
            str(self.context.quality_reports / "valgrind"),
        )
        self.assertEqual(
            commands[1][commands[1].index("--output") + 1],
            str(self.context.quality_reports / "sanitizers"),
        )

    def test_quality_tasks_use_shared_build_system_scripts(self) -> None:
        build_system_root = (
            Path(self.temporary_directory.name) / "build-system"
        )
        scripts_directory = build_system_root / "scripts"
        scripts_directory.mkdir(parents=True)
        for name in (
            "run_llvm_coverage.py",
            "coverage_metrics.py",
            "write_quality_report_readme.py",
        ):
            (scripts_directory / name).touch()

        with (
            patch.dict(
                "os.environ",
                {TASKS.BUILD_SYSTEM_ROOT_ENV: str(build_system_root)},
            ),
            patch.object(TASKS, "run_command", return_value=0) as run_command,
        ):
            self.assertEqual(TASKS.coverage(self.context), 0)
            self.assertEqual(TASKS.coverage_metrics(self.context), 0)
            self.assertEqual(TASKS.quality_readme(self.context), 0)

        rendered_commands = "\n".join(
            " ".join(call.args[1]) for call in run_command.call_args_list
        )
        for name in (
            "run_llvm_coverage.py",
            "coverage_metrics.py",
            "write_quality_report_readme.py",
        ):
            self.assertIn(
                str((scripts_directory / name).resolve()),
                rendered_commands,
            )

    def test_coverage_declares_header_only_package(self) -> None:
        build_system_root = (
            Path(self.temporary_directory.name) / "build-system"
        )
        scripts_directory = build_system_root / "scripts"
        scripts_directory.mkdir(parents=True)
        (scripts_directory / "run_llvm_coverage.py").touch()
        self.context.workspace.mkdir(parents=True)
        (self.context.workspace / "conanfile.py").write_text(
            "class Recipe:\n    package_type = \"header-library\"\n",
            encoding="utf-8",
        )

        with (
            patch.dict(
                "os.environ",
                {TASKS.BUILD_SYSTEM_ROOT_ENV: str(build_system_root)},
            ),
            patch.object(TASKS, "run_command", return_value=0) as run_command,
        ):
            self.assertEqual(TASKS.coverage(self.context), 0)

        command = run_command.call_args.args[1]
        self.assertIn("--header-only", command)

    def test_benchmark_task_uses_shared_runner(self) -> None:
        build_system_root = (
            Path(self.temporary_directory.name) / "build-system"
        )
        scripts_directory = build_system_root / "scripts"
        scripts_directory.mkdir(parents=True)
        (scripts_directory / "run_llvm_coverage.py").touch()
        benchmark_runner = scripts_directory / "run_benchmarks.sh"
        benchmark_runner.touch()

        with (
            patch.dict(
                "os.environ",
                {TASKS.BUILD_SYSTEM_ROOT_ENV: str(build_system_root)},
            ),
            patch.object(TASKS, "install_dependencies", return_value=0),
            patch.object(TASKS, "configure_cmake", return_value=0),
            patch.object(TASKS, "run_command", return_value=0) as run_command,
        ):
            result = TASKS.benchmark_all(self.context)

        self.assertEqual(result, 0)
        self.assertEqual(
            run_command.call_args_list[-1].args[1][0],
            str(benchmark_runner.resolve()),
        )

    def test_valgrind_task_uses_shared_driver_and_consumer_source(
        self,
    ) -> None:
        build_system_root = (
            Path(self.temporary_directory.name) / "build-system"
        )
        scripts_directory = build_system_root / "scripts"
        scripts_directory.mkdir(parents=True)
        (scripts_directory / "run_llvm_coverage.py").touch()
        valgrind_driver = scripts_directory / "run_valgrind_ctest.cmake"
        valgrind_driver.touch()

        with (
            patch.dict(
                "os.environ",
                {TASKS.BUILD_SYSTEM_ROOT_ENV: str(build_system_root)},
            ),
            patch.object(TASKS, "run_command", return_value=0) as run_command,
        ):
            result = TASKS.valgrind_tool(self.context, "memcheck")

        self.assertEqual(result, 0)
        command = run_command.call_args.args[1]
        self.assertIn(str(valgrind_driver.resolve()), command)
        self.assertIn(
            f"-DCTEST_SOURCE_DIRECTORY={self.context.workspace}",
            command,
        )


if __name__ == "__main__":
    unittest.main()
