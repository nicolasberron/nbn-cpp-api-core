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

Install the Conan dependencies and activate the generated build environment
before configuring. The environment supplies the shared CMake modules through
`NBN_CPP_API_CMAKE_DIR`.

```sh
conan install . -of build -pr linux-clang19-debug
source build/conanbuildenv-debug-x86_64.sh
cmake -S . -B build -G Ninja \
	-DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake \
	-DNBN_BUILD_TESTS=OFF -DNBN_CORE_DOXYGEN_ENABLE=OFF
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

Core tests are available under `tests/nbn/core` and are enabled by default.
The tests that exercise UI-specific types remain in the migrated source tree
but are excluded from this standalone build because this repository does not
provide `nbn-ui`.

The repository also provides standalone VS Code tasks in
`.vscode/tasks.json` for Conan setup, opening the generated build directory, and
coverage report generation. Its packaged `scripts/` directory is exposed to
Conan consumers as `NBN_CPP_API_CORE_SCRIPTS_DIR`.

Optional dependency-free benchmark infrastructure is documented in
[tests/core/utilities/README.md](tests/core/utilities/README.md). The reusable
header is installed as `include/nbn/benchmark/benchmark_support.h`, so
downstream Conan packages can use the same measurement and report helpers.
Benchmarks are disabled by default and are kept outside the core test targets.
