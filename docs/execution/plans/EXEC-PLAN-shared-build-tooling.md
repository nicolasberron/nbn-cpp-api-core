# Execution Plan: Shared Build Tooling

**Author**: GitHub Copilot
**Date**: 2026-09-12
**Status**: Complete with manual build validation pending

## Purpose / Big Picture

Remove duplicated CMake policy and Python task tooling from the core and UI
repositories. The build-system Conan package will own reusable CMake functions,
and the UI task runner will extend the core runner obtained from either the Conan
environment or the sibling development checkout.

## Progress

- [x] Initial plan drafted
- [x] Dependencies reviewed
- [x] Validation path defined
- [x] Work started
- [x] Acceptance evidence recorded

## Surprises & Discoveries

- Observation: Core already packages its scripts but does not advertise their
  location to Conan consumers.
- Evidence: `conanfile.py` copies `scripts/*` while `package_info()` exports no
  script-directory environment variable.
- Observation: UI directly calls only three copied Python helpers and one copied
  CTest script, all from its copied task runner.
- Evidence: Workspace reference search found no other UI call sites.

## Decision Log

| Decision | Options considered | Chosen | Rationale | Date / Author |
|---|---|---|---|---|
| Shared CMake owner | Core, UI, build-system | Build-system | It is already the Conan `build-scripts` package used by both projects. | 2026-09-12 / GitHub Copilot |
| Shared Python owner | Build-system, core, copied UI scripts | Core | The scripts encode core quality workflows and core already packages them. | 2026-09-12 / GitHub Copilot |
| UI customization | Copy and patch, flags, subclass | Subclass with narrow hooks | It removes copied orchestration while retaining UI-specific dependency installation and library discovery. | 2026-09-12 / GitHub Copilot |

## Alternatives Considered

Moving every Python script into the build-system package would centralize more
files, but it would make that generic package own core-specific coverage,
benchmark, and report semantics. Thin wrappers for every UI helper would preserve
old paths but retain unnecessary files and indirection. A core-owned task runner
with a small UI subclass matches the existing dependency direction and removes
the duplicated implementation.

## Context and Orientation

The build-system recipe exports `NBN_CPP_API_CMAKE_DIR`. Core packages its
`scripts/` directory. UI depends on core and the build-system package, but its
development workflow also supports sibling source checkouts before Conan has
created an environment.

## Pre-Conditions

- [x] Issue is optional in local mode
- [x] Dependencies checked
- [x] Required skills identified
- [x] Task complexity warrants a plan

## Plan of Work

```mermaid
flowchart LR
    Discover[Research] --> Brainstorm[Brainstorm]
    Brainstorm --> Plan[Plan]
    Plan --> Design[Design]
    Design --> Deliver[Deliver slices]
    Deliver --> Validate[Validate and review]
    Validate --> Capture[Capture outcomes]
```

First centralize project setup and package-install helpers in CMake modules.
Then expose core scripts through Conan and add an importable task-runner base.
Finally replace UI copies with a subclass and validate imports, task listings,
dry-run command construction, Conan metadata, and CMake script syntax.

## Steps

| # | Step | Owner | Status | Notes |
|---|---|---|---|---|
| 1 | Add shared CMake modules and consume them | GitHub Copilot | Complete | Project-specific coverage defaults are preserved. |
| 2 | Expose and encapsulate core task scripts | GitHub Copilot | Complete | Core CLI compatibility entry points remain. |
| 3 | Replace UI copies with inherited behavior | GitHub Copilot | Complete | Conan and sibling-checkout lookup paths are supported. |
| 4 | Run static and dry-run validation | GitHub Copilot | Complete | No C++ compilation was run. |
| 5 | Run scrub, documentation drift, and review | GitHub Copilot | Complete | Review approved; drift found only pre-existing broken report links. |

## Concrete Steps

| Validation activity | Working area | Evidence |
|---|---|---|
| Python compile and import checks | Core and UI scripts | Successful process exit |
| CLI listing and dry-run checks | Core and UI task runners | Expected tasks and UI-specific command options |
| Conan recipe inspection | Three recipes | Exported paths and matching version constraints |
| CMake script-mode checks | Shared modules | Successful parse or expected project-only guard |

## Blockers

| Blocker | Impact | Resolution | Status |
|---|---|---|---|
| Full builds are user-operated | No compile-level evidence in this session | Use static and dry-run checks; report manual build requirement. | Deferred to user |
| Documentation drift reports 23 broken links | Repository gate does not pass | No stale or missing claims were found; links are in pre-existing analysis and generated reports outside this refactor. | Known limitation |

## Validation and Acceptance

- [x] Documentation impact reviewed; no stale or missing claims were found
- [x] Required independent review has zero High/Medium findings
- [x] Final evidence matches the reviewed code, configuration and documents
- [x] Core and UI load shared CMake behavior from the build-system package
- [x] UI task commands are provided by an inherited core runner
- [x] UI no longer contains copied shared helper implementations

## Idempotence and Recovery

Each repository remains independently usable. The UI loader checks an explicit
Conan-provided path before the sibling source checkout and reports a direct error
if neither exists. Failed edits can be repaired per repository without resetting
unrelated working-tree changes.

**Shared loop owner / reference**: GitHub Copilot, active loop in
`nbn-cpp-api-core`.

## Rollback Plan

Revert only the shared-module consumers, Conan environment export, task-runner
base, UI subclass, and deleted duplicate scripts. Do not alter unrelated UI or
build-system worktree changes.

## Artifacts and Notes

- Existing UI `CMakeLists.txt` and `scripts/run_nbn_task.py` changes are treated
  as user-owned input and preserved semantically.

## Outcomes & Retrospective

Reusable CMake setup now belongs to the build-system Conan package. Core owns
the shared task implementation and exports its script directory through Conan;
UI keeps only a subclass for dependency preparation, coverage inputs, and
project capabilities. Focused Python, CMake, Conan, dry-run, report-generation,
reference, ASCII, and diff checks pass. Independent functional review approved
the final implementation with zero findings. Real core and UI configure/build,
link, install, and package-consumer validation remains for the user's manual
build workflow.