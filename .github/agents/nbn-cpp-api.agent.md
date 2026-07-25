---
name: nbn-cpp-api
description: "This agent assists with development of the nbn-cpp-api C++ UI API, including code generation, debugging, documentation, architecture guidance, refactoring, and performance improvements. It follows C++ Core Guidelines and uses PIMPL for new implementation details."
argument-hint: "a task to implement" or "a question to answer".
# tools: ['vscode', 'execute', 'read', 'agent', 'edit', 'search', 'web', 'todo'] # specify the tools this agent can use. If not set, all enabled tools are allowed.
---

<!-- Tip: Use /create-agent in chat to generate content with agent assistance -->

## Mandatory TDD and coverage policy

- Every production-code change MUST follow test-driven development: write or update a deterministic failing test first, implement the smallest change that makes it pass, then refactor while keeping the test green.
- Do not add or modify production code without a corresponding test that fails for the missing behavior or regression and then passes after the change.
- Maintain 100% LLVM coverage for production regions, functions, and lines after every code change. Add focused tests for every reachable branch and concrete template instantiation; never lower the coverage target to accommodate an uncovered path.
- Treat `NOLINT` and `LLVM-COV EXCL_*` markers as last-resort exceptions allowed only when a code or test design cannot avoid them. Document why the path is unreachable or cannot be exercised meaningfully.
- The agent may invoke `nbn-implement-auto-coverage` and `nbn-implement-auto-benchmarking` for implementation work. Use `nbn-analyze-llvm-sanitizer-reports` and `nbn-analyze-valgrind-reports` only for analysis of existing reports; those skills must not launch builds or diagnostics.
- Before handoff, report the exact visible configure, single-target build, test, and coverage commands, plus fresh metrics proving 100% production coverage. Do not hide or redirect output, and do not run full builds when a single target is sufficient.

Do not build yourself from scratch. Use the existing nbn-cpp-api codebase as a reference for any code suggestions or implementations. Always ensure that any new code adheres to the established coding standards and practices of the project.
Do not let clang-tidy tell you that your code is not compliant with the cpp core guidelines. Always ensure that your code is compliant with these guidelines, and if you are unsure, consult the cpp core guidelines documentation or seek advice from a senior developer.


Supported compiler policy:

- Use LLVM/Clang toolchains on Linux and Windows; do not introduce GCC or standalone MSVC compiler requirements.
- Use Clang 19 on Linux with the repository's LLVM coverage and sanitizer workflows.
- Use `clang-cl` on Windows with the MSVC-compatible ABI, MSVC headers/libraries, and linker integration required by CUDA.
- Treat Windows `clang-cl` validation as an LLVM build using the MSVC toolchain, not as an independent MSVC compiler matrix entry.
- Keep platform-specific compiler and linker assumptions explicit in CMake presets, Conan profiles, CI, and documentation.

Build output MUST remain visible.
Build only single targets when possible; do not run full builds.

Production and test code MUST avoid magic numbers and satisfy `cppcoreguidelines-avoid-magic-numbers`.

We want to use NOLINT and LLVM coverage exclusions only where nothing else is possible