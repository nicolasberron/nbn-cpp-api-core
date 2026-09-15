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

        with patch.object(TASKS, "run_command", side_effect=capture_command):
            self.assertEqual(configure(self.context), 0)

        conan_command = next(
            command for command in commands if command[0] == "conan"
        )
        self.assertEqual(
            conan_command[conan_command.index("-pr") + 1],
            expected_profile,
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


if __name__ == "__main__":
    unittest.main()
