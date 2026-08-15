# nbn-cpp-api-core

Standalone `nbn::core` C++23 library providing JSON serialization, signals and
slots, lock-free concurrent containers, timers, tasks, and application crash
handling.

## Build

The project uses Conan and the shared build system package
`nbn-cpp-api-build-system/1.0.0` as a build requirement. Install dependencies
with a suitable profile, then configure and build with CMake using the Conan
generated files.

The public headers are installed below `include/nbn/core` and the library is
published as `nbn-core`.

## CMake installation and consumption

The project can build the library without Conan when tests are disabled:

```sh
cmake -S . -B build -G Ninja -DNBN_BUILD_TESTS=OFF -DNBN_CORE_DOXYGEN_ENABLE=OFF
cmake --build build --target nbn-core
cmake --install build --prefix /tmp/nbn-cpp-api-core
```

Consumers can then use the installed package:

```cmake
find_package(nbn-cpp-api-core CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE nbn::core)
```

The package requires a compatible C++23 compiler and standard library. The
supported compiler policy is Clang on Linux and `clang-cl` on Windows with the
MSVC-compatible ABI and toolchain.

## Conan package consumption

Create the package from the repository with Conan 2:

```sh
conan create . --build=missing -o shared=True
```

Consumers should require `nbn-cpp-api-core/<version>` and use Conan's
`CMakeToolchain` and `CMakeDeps` generators. The generated package exposes the
same `nbn::core` CMake target. Use `-o nbn-cpp-api-core/*:shared=False` for a
static package.

## Conan package VS Code assets

The Conan package also contains the repository's downstream development assets:

- `vscode/.vscode/`: VS Code settings, launch configurations, and tasks.
- `vscode/.github/`: the `nbn-cpp-api` agent and repository-specific skills.
- `scripts/`: downstream helper scripts used by the packaged VS Code tasks.
- `scripts/install_vscode_assets.py`: a standard-library installer for copying
	those directories and scripts into a downstream workspace.

The package keeps these assets outside the C++ install tree, so consuming the
library does not install editor files into system directories. A downstream
Conan recipe must explicitly install them during its `generate()` step by using
the dependency package folder. `conan install` does not copy arbitrary files
from a dependency package into the consumer source tree automatically:

```python
from pathlib import Path
import subprocess
import sys

def generate(self):
	package_root = Path(self.dependencies["nbn-cpp-api-core"].package_folder)
	installer = package_root / "scripts" / "install_vscode_assets.py"
	subprocess.run(
		[
			sys.executable,
			str(installer),
			"--destination",
			str(self.recipe_folder),
			"--define",
			"NBN_PROFILE=conan-debug",
		],
		check=True,
	)
```

For example, `nbn-cpp-api-ui` invokes this installer from its `generate()` method.
After recreating or refreshing the `nbn-cpp-api-core` package, running `conan
install` for `nbn-cpp-api-ui` installs the packaged `.vscode` and `.github` trees
in the UI workspace. The package must be recreated after changing the core
recipe; an existing Conan package revision is not modified in place.

The same command can be run manually with the `scripts/install_vscode_assets.py`
path from the Conan package. Installation is merge-safe by default: existing
files are rejected rather than silently overwritten. Pass `--force` when the
downstream workspace intentionally wants the package versions. Files can be
customized at install time with repeated `--define NBN_NAME=value` options;
these replace `@NBN_NAME@` tokens in UTF-8 text files and leave binary files
unchanged. Downstream projects can therefore adapt paths, profiles, or other
workspace-specific values without modifying the package contents.

Core tests are available under `tests/nbn/core` and are enabled by default.
The tests that exercise UI-specific types remain in the migrated source tree
but are excluded from this standalone build because this repository does not
provide `nbn-ui`.

The repository also provides standalone VS Code tasks in
`.vscode/tasks.json` for Conan setup, opening the generated build directory, and
coverage report generation.

Optional dependency-free benchmark infrastructure is documented in
[tests/core/utilities/README.md](tests/core/utilities/README.md). The reusable
header is installed as `include/nbn/benchmark/benchmark_support.h`, so
downstream Conan packages can use the same measurement and report helpers.
Benchmarks are disabled by default and are kept outside the core test targets.
