import tempfile
import unittest
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import patch

from conanfile import GENERATED_SCRIPT_EXCLUDES, NbnCppApiCoreConan
from scripts.install_vscode_assets import install_assets


class InstallVscodeAssetsTests(unittest.TestCase):

    def test_coverage_defaults_precede_build_system_initializer(self) -> None:
        cmake_file = Path(__file__).parents[2] / "CMakeLists.txt"
        cmake_contents = cmake_file.read_text(encoding="utf-8")

        defaults_position = cmake_contents.index("# Set coverage defaults before loading nbn-init.cmake.")
        initializer_position = cmake_contents.index("include(\"${NBN_CPP_API_CMAKE_DIR}/nbn-init.cmake\")")

        self.assertLess(
            defaults_position,
            initializer_position,
            "Coverage defaults must be established before nbn-init.cmake can apply its defaults",
        )

    def test_conan_package_accepts_string_paths(self) -> None:
        recipe = SimpleNamespace(
            source_folder="/tmp/exported-source",
            package_folder="/tmp/package",
        )

        with patch("conanfile.CMake") as cmake_class, patch("conanfile.copy") as copy:
            NbnCppApiCoreConan.package(recipe)

        cmake_class.return_value.install.assert_called_once_with()
        copy.assert_any_call(
            recipe,
            "*",
            src="/tmp/exported-source/scripts",
            dst="/tmp/package/scripts",
            excludes=GENERATED_SCRIPT_EXCLUDES,
        )

    def test_installs_assets_and_replaces_tokens(self) -> None:
        with tempfile.TemporaryDirectory() as temporary_directory:
            package_root = Path(temporary_directory) / "package"
            destination = Path(temporary_directory) / "consumer"
            settings = package_root / "vscode" / ".vscode" / "settings.json"
            agent = package_root / "vscode" / ".github" / "agents" / "agent.md"
            settings.parent.mkdir(parents=True)
            agent.parent.mkdir(parents=True)
            (package_root / "scripts").mkdir(parents=True)
            settings.write_text("${workspaceFolder}/@NBN_PROFILE@", encoding="utf-8")
            agent.write_text("profile: @NBN_PROFILE@", encoding="utf-8")

            installed = install_assets(
                package_root,
                destination,
                {"NBN_PROFILE": "conan-debug"},
            )

            self.assertEqual(len(installed), 2)
            self.assertEqual(
                (destination / ".vscode" / "settings.json").read_text(
                    encoding="utf-8"
                ),
                "${workspaceFolder}/conan-debug",
            )
            self.assertEqual(
                (destination / ".github" / "agents" / "agent.md").read_text(
                    encoding="utf-8"
                ),
                "profile: conan-debug",
            )

    def test_installs_packaged_scripts(self) -> None:
        with tempfile.TemporaryDirectory() as temporary_directory:
            package_root = Path(temporary_directory) / "package"
            destination = Path(temporary_directory) / "consumer"
            script = package_root / "scripts" / "run_nbn_task.py"
            script.parent.mkdir(parents=True)
            (package_root / "vscode" / ".vscode").mkdir(parents=True)
            (package_root / "vscode" / ".github").mkdir(parents=True)
            script.write_text("print('task')\n", encoding="utf-8")

            installed = install_assets(package_root, destination)

            self.assertIn(destination / "scripts" / "run_nbn_task.py", installed)
            self.assertEqual(
                (destination / "scripts" / "run_nbn_task.py").read_text(
                    encoding="utf-8"
                ),
                "print('task')\n",
            )

    def test_requires_force_for_existing_files(self) -> None:
        with tempfile.TemporaryDirectory() as temporary_directory:
            package_root = Path(temporary_directory) / "package"
            destination = Path(temporary_directory) / "consumer"
            source = package_root / "vscode" / ".vscode" / "settings.json"
            target = destination / ".vscode" / "settings.json"
            source.parent.mkdir(parents=True)
            (package_root / "vscode" / ".github").mkdir(parents=True)
            (package_root / "scripts").mkdir(parents=True)
            target.parent.mkdir(parents=True)
            source.write_text("new", encoding="utf-8")
            target.write_text("existing", encoding="utf-8")

            with self.assertRaises(FileExistsError):
                install_assets(package_root, destination)

            install_assets(package_root, destination, force=True)
            self.assertEqual(target.read_text(encoding="utf-8"), "new")


if __name__ == "__main__":
    unittest.main()