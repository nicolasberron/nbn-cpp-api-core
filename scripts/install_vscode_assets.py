#!/usr/bin/env python3
"""Install the VS Code and AgentX assets shipped in the Conan package."""

from __future__ import annotations

import argparse
import shutil
from pathlib import Path
from typing import Mapping, Sequence


ASSET_DIRECTORY_NAMES = ("scripts", ".vscode", ".github")
TEXT_SUFFIXES = {".json", ".md", ".py", ".ps1", ".sh", ".txt", ".yaml", ".yml"}
TOKEN_PREFIX = "@"
TOKEN_SUFFIX = "@"


def _replace_tokens(content: bytes, replacements: Mapping[str, str]) -> bytes:
    """Replace UTF-8 text tokens while leaving binary files unchanged."""
    if not replacements or b"\x00" in content:
        return content
    try:
        text = content.decode("utf-8")
    except UnicodeDecodeError:
        return content
    for name, value in replacements.items():
        text = text.replace(f"{TOKEN_PREFIX}{name}{TOKEN_SUFFIX}", value)
    return text.encode("utf-8")


def install_assets(
    package_root: Path,
    destination: Path,
    replacements: Mapping[str, str] | None = None,
    *,
    force: bool = False,
) -> list[Path]:
    """Install packaged VS Code assets into a downstream workspace.

    Args:
        package_root: Root of the Conan package containing the ``vscode`` folder.
        destination: Downstream workspace root.
        replacements: Optional values for ``@NBN_NAME@`` text tokens.
        force: Replace files already present in the destination.

    Returns:
        Paths of files written to the destination.

    Raises:
        FileNotFoundError: If a packaged asset directory is missing.
        FileExistsError: If a destination file exists without ``force``.
    """
    source_root = package_root / "vscode"
    resolved_replacements = replacements or {}
    installed: list[Path] = []
    source_files: list[tuple[Path, Path]] = []

    for asset_directory_name in ASSET_DIRECTORY_NAMES:
        source_directory = (
            package_root / asset_directory_name
            if asset_directory_name == "scripts"
            else source_root / asset_directory_name
        )
        if not source_directory.is_dir():
            raise FileNotFoundError(source_directory)
        for source_file in source_directory.rglob("*"):
            if not source_file.is_file():
                continue
            relative_path = source_file.relative_to(source_directory)
            destination_file = destination / asset_directory_name / relative_path
            source_files.append((source_file, destination_file))

    destination_root = destination.resolve()
    for source_file, destination_file in source_files:
        resolved_destination = destination_file.resolve(strict=False)
        if destination_file.is_symlink() or not resolved_destination.is_relative_to(destination_root):
            raise ValueError(f"destination path escapes workspace: {destination_file}")
        if destination_file.exists() and not force:
            if destination_file.read_bytes() == _replace_tokens(source_file.read_bytes(), resolved_replacements):
                continue
            raise FileExistsError(destination_file)

    for source_file, destination_file in source_files:
        destination_file.parent.mkdir(parents=True, exist_ok=True)
        if destination_file.exists() and not force:
            continue
        content = source_file.read_bytes()
        if source_file.suffix not in TEXT_SUFFIXES:
            content = source_file.read_bytes()
        else:
            content = _replace_tokens(content, resolved_replacements)
        destination_file.write_bytes(content)
        shutil.copystat(source_file, destination_file)
        installed.append(destination_file)
    return installed


def _parse_replacements(values: Sequence[str]) -> dict[str, str]:
    """Parse ``NAME=VALUE`` command-line replacement definitions."""
    replacements: dict[str, str] = {}
    for value in values:
        name, separator, replacement = value.partition("=")
        if not separator or not name.startswith("NBN_") or not replacement:
            raise ValueError(
                "definitions must use the form NBN_NAME=VALUE with a non-empty value"
            )
        replacements[name] = replacement
    return replacements


def main() -> int:
    """Run the installer command-line interface."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--destination",
        type=Path,
        default=Path.cwd(),
        help="downstream workspace root (default: current directory)",
    )
    parser.add_argument(
        "--package-root",
        type=Path,
        default=Path(__file__).resolve().parents[1],
        help="Conan package root (default: this package)",
    )
    parser.add_argument(
        "--define",
        action="append",
        default=[],
        metavar="NBN_NAME=VALUE",
        help="replace @NBN_NAME@ tokens in text files; may be repeated",
    )
    parser.add_argument(
        "--force",
        action="store_true",
        help="overwrite existing downstream files",
    )
    arguments = parser.parse_args()
    try:
        replacements = _parse_replacements(arguments.define)
        installed = install_assets(
            arguments.package_root,
            arguments.destination,
            replacements,
            force=arguments.force,
        )
    except (FileExistsError, FileNotFoundError, ValueError) as error:
        parser.error(str(error))
    print(f"Installed {len(installed)} VS Code asset files into {arguments.destination}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())