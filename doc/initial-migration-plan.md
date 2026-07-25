# Migration of `nbn::core` to a standalone repository

## What will be moved

The target repository currently contains only README.md and LICENSE. The migration will bring over the `nbn::core` implementation from the source repository:

- 58 tracked source and header files from src/nbn/core, including:
  - Application and crash handling
  - Logging
  - Tasks, timers, threads, and synchronization
  - Signals/slots and object properties
  - JSON serialization and migration support
  - Concurrent queues/stacks
  - Finite state machines
  - Color and command-line utilities
  - Semantic-version utilities
  - Public interfaces
  - Internal `detail/`, `private/`, and `utils/` implementation files
- The existing core-specific CMakeLists.txt

The UI, LVGL, Thrust, and general utilities modules will not be moved. UI-dependent tests remain in the migrated test tree for reference but are excluded from the standalone build.

## Additional files required for a standalone core repository

Although the requested source directory is `src/nbn/core`, the core currently depends on the generated `nbn/config.h` header. Therefore, the target repository will also need the version template from src/nbn/config.h.in, or an equivalent simplified configuration-header mechanism.

A new top-level build entry point will be needed to:

1. Initialize the shared CMake settings.
2. Define the project and version.
3. Generate `nbn/config.h`.
4. Add `src` and the generated build directory to the include path.
5. Add the core subdirectory.
6. Preserve the existing shared/static library options.
7. Enable the portable core unit tests through the shared `nbn-unit-tests.cmake` module.

## Build-system changes

The target Conan recipe will add the shared build system as a build requirement:

`nbn-cpp-api-build-system/1.0.0`

This should be expressed through Conan’s `build_requirements()` method using `self.tool_requires(...)`.

The target CMake files will stop depending on copied local build-system modules. Instead, they will load the modules supplied by the Conan package through `NBN_CPP_API_CMAKE_DIR`, which is exported by the build-system package.

Relevant modules include:

- nbn-init.cmake
- nbn-unit-tests.cmake for the portable core tests
- UI generator modules only if UI functionality is later added; they are not required for the core-only migration

The target Conan recipe will therefore export the core source, CMake files, and configuration template, while the shared build-system package supplies the common CMake behavior and profiles.

## Conan recipe cleanup

The migrated recipe will be simplified compared with the original conanfile.py:

- Remove the `enable_lvgl` option and LVGL requirement.
- Remove UI-specific CMake variables.
- Keep version extraction from the project’s CMakeLists.txt, unless a simpler explicit version is preferred.
- Add `nbn-cpp-api-build-system/1.0.0` as a build requirement.
- Continue using `CMakeToolchain`, `CMakeDeps`, and the Conan `CMake` helper.
- Package the resulting `nbn-core` library and public headers according to the standalone repository layout.

## Resulting scope

The new repository will become a standalone core library repository rather than a copy of the complete `nbn-cpp-api` project:

- `nbn::core` source and headers move into this repository.
- Shared build conventions come from `nbn-cpp-api-build-system/1.0.0`.
- UI, LVGL, and Thrust modules remain outside this migration; UI-dependent tests are not built here.
- Git history will not be preserved, as requested.

Created 3 todos