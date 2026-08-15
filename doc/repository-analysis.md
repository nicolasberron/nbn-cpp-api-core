# nbn-cpp-api-core repository analysis

## Scope and summary

This report reviews the current working tree as a standalone C++23 core library. It is based on the source, tests, CMake/Conan files, README, formatting configuration, benchmark history, LLVM coverage, and Valgrind and sanitizer reports. The supported compiler policy is LLVM on Linux and Windows: Clang on Linux, and `clang-cl` on Windows with the MSVC-compatible ABI, MSVC headers/libraries, and linker integration required by CUDA. Performance observations from Callgrind remain hypotheses until confirmed with normal benchmark runs outside Valgrind.

**Status snapshot:** 2026-08-05. Validation claims in this report refer to the latest committed quality artifacts and the final Linux/Clang `coverage-all` run available at this snapshot; they are not a substitute for rerunning the workflows after subsequent source or toolchain changes.

## Measured validation snapshot

| Area | Result | Evidence | Interpretation |
| --- | --- | --- | --- |
| CTest / Valgrind Memcheck | 26 reports; no invalid accesses or definite/indirect leaks | [Valgrind report](quality-reports/valgrind/README.md) and [Memcheck findings](quality-reports/valgrind/memcheck.md) | No confirmed memory-safety defect in the analyzed runs. `still reachable` allocations are concentrated in intentional process-termination tests and remain informational. |
| Massif | Peak heap: 16.5 MiB in `test_Serialization` | [Massif observations](quality-reports/valgrind/massif.md) | A repeatable heap baseline exists; the result is an observation, not evidence of a leak. |
| Callgrind | 8,830,030,250 aggregate instructions | [Callgrind hotspots](quality-reports/valgrind/callgrind.md) | The largest costs are application lifecycle, thread/task execution, stack-trace formatting, and `std::function`; these are not wall-clock measurements. |
| Benchmarks | Four serialization workloads with 15 samples each; one accepted serialization baseline | [Benchmark report](quality-reports/benchmark/README.md) and [benchmark-history.csv](quality-reports/benchmark-history.csv) | The serialization path has a measured baseline. Queue, logging, signals, timers, tasks, and object-factory workloads are still missing. |
| AddressSanitizer | 27 of 27 CTest executables passed; 0 findings | [Sanitizer report](quality-reports/sanitizers/sanitizers.md) | No ASan diagnostic was found in the analyzed Linux/Clang run. Windows `clang-cl` sanitizer evidence must be reported separately by host and test subset. |
| ThreadSanitizer | 24 of 24 CTest executables passed; 0 findings | [Sanitizer report](quality-reports/sanitizers/sanitizers.md) | No TSan diagnostic was found in the analyzed Linux/Clang run. The signal-interrupt test and process-termination tests are excluded from the TSan subset; Windows support must be validated separately. |
| Adversarial concurrency stress | Queue and stack stress passed with 4 producers, 4 consumers, 10,000 unique items, and 3 rounds each; repeated thread and manager shutdown stress also passed | Focused CTest runs for `test_ConcurrentQueue`, `test_ConcurrentStack`, `test_Thread`, and `test_ApplicationThreadsManager` | Linux/Clang now exercises high-contention conservation, move-only ownership, cancellation, and join/lifetime paths. This remains host-specific evidence, not a proof for Windows schedules. |
| LLVM source coverage | 100.00% regions, 100.00% functions, and 100.00% lines | [LLVM coverage report](quality-reports/coverage/html/index.html) and [quality report](quality-reports/README.md) | The configured production coverage gate is satisfied. Branch coverage remains below 100% in several files, so 100% line/function/region coverage does not mean every decision outcome is tested. |

The current evidence is strong for the exercised Linux/Clang configuration: memory errors, sanitizer findings, and configured production coverage gaps were not observed, and all 27 registered CTest executables passed in the latest coverage run. The target portability matrix is Linux Clang plus Windows `clang-cl` using the MSVC ABI/toolchain for CUDA compatibility; GCC and standalone MSVC compiler validation are outside the supported scope. Shared and static Release Conan packages, a shared Debug package, and external CMake consumers using `nbn::core` have now been validated on Linux/Clang. The evidence does not by itself establish Windows behavior, complete branch behavior, resistance to adversarial schedules, or release-process quality.

### Current validation update

The latest `coverage-all` run completed successfully after focused serialization tests exercised top-level null objects, quoted strings, and quoted variants. A structural change in [serialization.cpp](../src/nbn/core/detail/serialization.cpp) removed an uncovered closing-brace mapping without using an LLVM coverage exclusion. The reported `test_ApplicationThreadsManager` failure was intermittent: it appeared as a test assertion during an earlier run, while repeated focused executions and the final 27-test coverage run passed. No production change was made to `ApplicationThreadsManager`; [Application.cpp](../src/nbn/core/private/Application.cpp) was only the exception-reporting path.

## Features

### Public runtime and application services

- Application lifecycle management, initialization, execution, shutdown, signal forwarding, command-line access, and termination handling in [Application.h](../src/nbn/core/Application.h) and [Application.cpp](../src/nbn/core/Application.cpp).
- Structured logging with debug/info/warning/error/fatal levels, source locations, asynchronous file output, platform-specific thread identifiers, and logger signals in [Logger.h](../src/nbn/core/Logger.h) and [Logger.cpp](../src/nbn/core/Logger.cpp).
- Tasks, worker threads, timers, elapsed-time measurement, and application thread management in [Task.h](../src/nbn/core/Task.h), [Timer.h](../src/nbn/core/Timer.h), and the private thread-management implementation.
- A built-in asynchronous unit-test runner and assertion helpers in [UnitTests.h](../src/nbn/core/UnitTests.h).

### Object model and event system

- `Object` instances with UUIDs, class names, factory registration/creation, serialization, and lifecycle signaling in [Object.h](../src/nbn/core/Object.h) and [Object.cpp](../src/nbn/core/Object.cpp).
- Typed signals and slots, named slot connections, sender tracking, synchronous emission, and asynchronous emission in [Signal.h](../src/nbn/core/detail/Signal.h).
- Typed serializable properties and enum properties with change signals, values, indexes, and JSON persistence in [Property.h](../src/nbn/core/detail/Property.h).

### Data, serialization, and utility facilities

- JSON serialization/deserialization for scalar types, strings, enums, vectors, maps, variants, serializable objects, control characters, Unicode escapes, and nested values in [serialization.h](../src/nbn/core/detail/serialization.h) and [serialization.cpp](../src/nbn/core/detail/serialization.cpp).
- JSON migration support through [JsonMigratorBase.h](../src/nbn/core/JsonMigratorBase.h).
- Concurrent multi-producer/multi-consumer queue and stack containers with documented CAS/link linearization points and shared-pointer node lifetime handling in [ConcurrentQueue.h](../src/nbn/core/ConcurrentQueue.h) and [ConcurrentStack.h](../src/nbn/core/ConcurrentStack.h).
- Finite-state machine, ring buffer, cross-reference unordered map, color utilities, semantic-version helpers, and string/type-name utilities.
- A formatting compatibility bridge for platforms without `std::format` in [format_bridge.h](../src/nbn/core/private/format_bridge.h).

### Build and distribution

- Shared or static library builds, optional tests, optional Doxygen generation, and installation of public headers in [CMakeLists.txt](../CMakeLists.txt) and [src/nbn/core/CMakeLists.txt](../src/nbn/core/CMakeLists.txt).
- Conan packaging with CMake toolchain/dependency generation in [conanfile.py](../conanfile.py).
- Core tests covering application behavior, concurrency, serialization, object/property/signal behavior, timers, tasks, logging, and failure handlers under [tests/nbn/core](../tests/nbn/core).

## Coding rules and conventions

The following rules are either explicit in the repository or consistently inferred from the implementation:

1. Use C++23 language/library facilities, namespaces such as `nbn::core`, RAII, smart pointers, and `std::string_view` for non-owning string inputs.
2. Format C++ with the Chromium-based configuration in [.clang-format](../.clang-format): four-space indentation and a 132-column limit.
3. Prefer the repository's trailing-return-type style, `[[nodiscard]]` for query functions, constrained templates/concepts, and documented public headers.
4. Use private implementation classes and interfaces to keep public APIs stable.
5. Keep thread-safe state protected by mutexes or atomics and document asynchronous behavior.
6. Use Doxygen comments for public classes and APIs; keep tests deterministic and named using the test-discovery convention.
7. Avoid magic numbers and preserve portability across the supported Linux Clang and Windows `clang-cl`/MSVC ABI environments, plus embedded/PlatformIO builds where applicable.

### Places where rules were not fully respected — resolved

The initial review identified the following violations. They were addressed in this pass:

- Build files now use explicit source lists and target-scoped include directories in [src/nbn/core/CMakeLists.txt](../src/nbn/core/CMakeLists.txt) and [tests/nbn/core/CMakeLists.txt](../tests/nbn/core/CMakeLists.txt).
- C++23 is explicitly required by [CMakeLists.txt](../CMakeLists.txt#L12-L15) and the library target publishes `cxx_std_23`.
- Local test constants in the reviewed concurrency and unit-test files now use lower camel case rather than macro-like uppercase names.
- Test selection is explicit; UI-dependent and benchmark sources are no longer silently removed by filename filters.
- A tracked [.clang-tidy](../.clang-tidy) policy and the shared build-system `NBN_CLANG_TIDY_ENABLE` contract were restored, including warnings-as-errors and header filtering.
- Application operations that can allocate, create threads, or invoke callbacks no longer claim `noexcept` in [Application.h](../src/nbn/core/Application.h) and [Application.cpp](../src/nbn/core/Application.cpp).
- Signal and enum-property callbacks are invoked after releasing internal mutexes, allowing safe reentrant handlers.
- The custom test runner continues to convert its failure count into the process exit code through `Application::quit()`, which is the documented CTest integration contract in [tests/nbn/core/CMakeLists.txt](../tests/nbn/core/CMakeLists.txt).

The current Valgrind, sanitizer, benchmark, and coverage artifacts provide a strong Linux/Clang baseline. Windows `clang-cl`/MSVC ABI validation, package-consumer validation, broader stress and fuzz testing, branch coverage, and automated CI enforcement remain open. Valgrind is a Linux-only supplemental diagnostic and is not a Windows gate.

## Code quality

### Strengths

- The public API is consistently namespaced and generally documented with Doxygen comments.
- Ownership is mostly explicit through `std::unique_ptr`, `std::shared_ptr`, and weak slot references.
- Concepts and type traits constrain serialization, property, and signal APIs at compile time.
- Serialization tests cover malformed syntax, Unicode, numeric edge cases, nested structures, and round trips; for example, [Serialization.cpp](../tests/nbn/core/Serialization.cpp#L200-L290).
- Concurrency tests cover FIFO/LIFO behavior, multiple producers/consumers, move-only values, and a stale-tail queue interleaving in [ConcurrentQueue.cpp](../tests/nbn/core/ConcurrentQueue.cpp#L1-L151).
- Platform-specific behavior is isolated with preprocessor branches rather than spread throughout the public interfaces.

### Risks and maintainability concerns

1. **Concurrency evidence is now stronger but remains host-specific.** Repeated high-contention queue and stack tests cover unique-value conservation and move-only ownership across four producers, four consumers, 10,000 items, and three rounds. Repeated thread and application-manager shutdown tests cover cooperative cancellation and join ownership. The queue and stack headers document linearization and lifetime guarantees, but Windows `clang-cl` execution and formal progress proofs remain open.
2. **The custom JSON parser remains security-sensitive.** Resource limits, strict trailing-data rejection, duplicate-key rejection, numeric overflow checks, malformed-input regression tests, a committed seed corpus, differential acceptance checks, and bounded libFuzzer/minimization scripts are now present. Long-running fuzz jobs and broader reference-parser coverage still depend on running the Clang fuzzer configuration in CI. The parser contract is documented in [JSON_PARSER_SPEC.md](../docs/artifacts/specs/JSON_PARSER_SPEC.md): duplicate keys and overflow are rejected, unknown object fields are ignored for forward compatibility, and trailing non-whitespace data is rejected.
3. **The public package now has relocatable CMake consumer integration.** The root project installs headers, generated configuration/version files, and the exported `nbn::core` target. The `tests/package-consumer` fixture validates direct CMake installation and Conan `CMakeDeps` consumption for shared and static Release packages. The remaining risk is platform-specific validation, especially Windows `clang-cl` and its runtime/link dependencies.
4. **Doxygen output is directed into the source tree.** [src/nbn/core/CMakeLists.txt](../src/nbn/core/CMakeLists.txt#L76-L90) uses `${CMAKE_SOURCE_DIR}/doc/core/doc`, which can create generated files in the working tree and complicate reproducible builds.
5. **The Conan recipe uses the shared version source and standard build settings.** [conanfile.py](../conanfile.py) reads [VERSION](../VERSION), leaves `build_type` as a Conan setting, and exports the stacktrace support library as transitive system metadata. Shared and static package creation and external consumer linking now pass on Linux/Clang.
6. **Coverage is not branch-complete.** The configured 100% regions/functions/lines gate is useful, but the current report records 88.17% branch coverage. Uncovered decision outcomes should be reviewed and either tested or explicitly justified.
7. **Test selection is explicit but not yet sufficiently adversarial.** The CTest inventory is clear, and package-consumer smoke tests now cover distribution wiring, but this does not replace stress, fuzz, fault-injection, or repeated-run testing.

## Performance

### Positive characteristics

- The queue and stack avoid a single mutex around the data structure. Their public headers document successful link/CAS linearization points, ordering limits under overlapping operations, and node lifetime guarantees without claiming a formal lock-free progress classification.
- `std::string_view`, move-aware APIs, reserved vector/string capacity, and `std::unique_ptr` are used in performance-sensitive paths.
- Logging is decoupled from callers through a queue and worker thread, preventing ordinary log writes from synchronously performing file I/O.
- Property reads of trivially copyable types use atomics rather than a mutex.

### Algorithmic complexity

The table describes asymptotic complexity for the dominant operations. Here, $n$ is the number of input bytes or collection elements, $h$ is the number of connected signal handlers, $m$ is the number of enum values, $d$ is JSON nesting depth, $p$ is the number of object properties, $e$ is the number of array elements, and $k$ is the serialized output size. Concurrency entries describe the algorithmic operation and exclude scheduler contention, cache effects, and memory-allocation costs.

| Operation | Time complexity | Additional space | Notes |
| --- | --- | --- | --- |
| `ConcurrentQueue::push()` / `pop()` | $O(1)$ amortized | $O(1)$ per queued node | Node allocation and atomic `shared_ptr` reference-count operations can dominate in practice. |
| `ConcurrentStack::push()` / `pop()` | $O(1)$ amortized | $O(1)$ per stacked node | Atomic shared-pointer operations and contention costs are implementation-dependent; no formal lock-free progress classification is claimed. |
| `Signal::connect()` | $O(1)$ amortized | $O(1)$ besides the new connection | Connections are appended to the handler list; vector growth can occasionally be $O(h)$. |
| `Signal::disconnect()` | $O(h)$ | $O(1)$ | The matching connection is located in the handler list. |
| `Signal::emit()` | $O(h)$ plus callback cost | $O(h)$ | Handlers are copied into a snapshot under the mutex and invoked after unlocking. |
| Property `get()` / `set()` | $O(1)$ plus notification cost | $O(1)$ | Trivially copyable property values use atomic access; notifications add signal-dispatch cost. |
| `EnumProperty::set()` | $O(m)$ plus notification cost | $O(1)$ | The current implementation linearly searches serialized enum values. |
| JSON parse/validation | $O(n)$ expected | $O(n + d)$ | The parser scans input once in the normal case; recursive call depth is bounded by the input-derived limit and private ceiling. |
| JSON whitespace and token scanning | $O(n)$ | $O(1)$ | `skipWhitespace()` and primitive token scanning advance through the input without backtracking. |
| JSON string serialization and escaping | $O(n + k)$ | $O(k)$ | Each source character is inspected once; escaped characters can expand the output. |
| JSON scalar serialization | $O(k)$ | $O(k)$ | Includes numeric, boolean, null, and formatted scalar output. |
| JSON value-range parsing | $O(n)$ | $O(d)$ | `parseValueView()` locates a complete scalar, array, or object range without copying the input value. |
| JSON array serialization | $O(e + k)$ | $O(k)$ | Each element is serialized once; output capacity is reserved conservatively, but element-level temporary strings may still be created. |
| JSON array deserialization | $O(n)$ expected | $O(e + n)$ | Commas are counted for capacity reservation, then each value is scanned and converted once. |
| JSON object serialization | $O(p + k)$ | $O(k)$ | Each property is emitted once; the exact cost of property lookup depends on the source object/container. |
| JSON object parsing | $O(n)$ expected | $O(p + n)$ | Properties are scanned once and stored in an unordered map; expected lookup/insertion is $O(1)$ per property. |
| JSON variant serialization/deserialization | $O(k)$ / $O(n)$ expected | $O(k)$ / $O(n)$ | Only the active alternative is processed, followed by its normal JSON algorithm. |
| JSON root-object deserialization | $O(n + p)$ expected | $O(p + n)$ | Parses root properties, ignores reserved metadata fields, finds the serialized object, and delegates payload parsing to the object instance. |

### Opportunities to reduce complexity

Several parts can be improved without changing the public API. Some changes reduce asymptotic complexity; others preserve the same Big-O bound but remove repeated scans, allocations, or lock contention.

| Area | Current behavior | Possible improvement | Expected result |
| --- | --- | --- | --- |
| `EnumProperty::set()` | Linear search through $m$ serialized enum values: $O(m)$. | Build an `unordered_map` from serialized value to index when enum values are registered. | Average $O(1)$ lookup instead of $O(m)$; registration becomes $O(m)$. |
| Signal disconnection | `disconnect()` scans the handler list: $O(h)$. | Store connection IDs or iterators in an indexed connection registry, while retaining the ordered dispatch list. | Average $O(1)$ direct disconnection, at the cost of extra bookkeeping. |
| Cross-signal disconnection | Matching handlers across two lists can scan both lists repeatedly. | Index handlers by connection identity or use stable connection tokens. | Avoids repeated searches and can reduce a worst-case $O(h_1 h_2)$ operation to expected $O(h_1 + h_2)$. |
| Signal emission | Copies all handlers before every emission: $O(h)$ time and $O(h)$ temporary space. | Use copy-on-write connection storage: readers share an immutable snapshot; writers copy only on mutation. | Emission remains $O(h)$ but avoids a per-emission copy and reduces allocation pressure. |
| JSON array deserialization | Previously counted commas before parsing, causing an unconditional preliminary scan. | Parse and grow the destination container incrementally without the preliminary comma-count pass. | Same $O(n)$ complexity with one fewer full-input scan and no potentially inaccurate pre-reservation. |
| JSON array serialization | Previously serialized each element into a temporary string before appending. | Use a shared output buffer for nested arrays, variants, and supported scalar values. | Same $O(e + k)$ complexity with fewer temporary strings and lower allocation pressure. Serializable object values still use their existing string-returning API. |
| JSON root-object deserialization | Previously materialized all root properties before selecting the serialized object. | Parse root properties incrementally and retain only the selected object name and payload; metadata is consumed but not stored. | Same expected $O(n)$ time with lower memory use and less copying for large roots. |
| JSON nesting | Recursive descent still uses call-stack space proportional to $d$. | Replace recursive container parsing with an explicit container stack for array/object state. | Keeps $O(n)$ parsing time while making stack usage independent of JSON nesting depth; this remains future work because the current parser preserves recursive behavior for compatibility. |

### Likely bottlenecks

| Component | Observation | Recommendation |
| --- | --- | --- |
| `Signal::emit()` | Serializes all handlers under one mutex and invokes arbitrary user code synchronously. | Snapshot handlers under the mutex, invoke after unlocking, and define ordering/reentrancy semantics. Consider a copy-on-write connection list for read-heavy signals. |
| `Signal::emitAsync()` | Uses `std::async(std::launch::async)` for every emission in [Signal.h](../src/nbn/core/detail/Signal.h#L128-L131). | A thread per emission is expensive and can exhaust resources. Use the existing task/thread scheduler or a bounded executor. |
| `ConcurrentQueue` | Each push allocates a node and performs atomic `shared_ptr` reference-count operations. `push()` is marked `noexcept` although `make_shared` can throw in [ConcurrentQueue.h](../src/nbn/core/ConcurrentQueue.h#L95-L124). | Benchmark against a queue using hazard pointers, epoch reclamation, or a bounded ring buffer. Correct the exception contract and measure contention, allocation rate, and tail latency. |
| JSON serialization | Array serialization now reserves a conservative output capacity, and parser paths use non-owning value ranges where possible; per-element serialization and object/value materialization still allocate. | Profile large payloads; pass one output buffer through recursive serializers and optimize object deserialization to avoid materializing unused properties. |
| Logger | Formatting, timestamp conversion, stream construction, and string allocation occur for each message before enqueueing. | Add level checks before formatting, use a reusable formatter/buffer, batch writes, and benchmark disabled/enabled logging separately. |
| Enum properties | `set()` linearly searches serialized enum values and allocates a target string on every update in [Property.h](../src/nbn/core/detail/Property.h#L144-L153). | Cache enum-to-index mappings during `setValues()` and perform O(1) lookup where update frequency is high. |

## Recommendations

### Priority 0 - correctness and safety

1. Add adversarial stress suites for `ConcurrentQueue`, `ConcurrentStack`, `Signal`, `Property`, `Task`, and logger shutdown. Run them repeatedly under ASan and TSan with varied worker counts, forced yields, cancellation, shutdown races, and long-duration schedules. The existing queue, stack, thread, and application-manager stress tests are useful focused coverage but remain host-specific.
2. Run the committed JSON fuzz corpus continuously with the Clang libFuzzer target, retain minimized reproducers, and publish differential-parser results from [differential_json.py](../scripts/differential_json.py). Keep the 16 MiB input, timeout, RSS, and nesting limits active in fuzz jobs.
3. Review every public `noexcept` declaration and every callback boundary. Keep `noexcept` only where the complete reachable operation is non-throwing, and document what happens when user callbacks throw or block.
4. Review the 63 Valgrind `still reachable` records after lifecycle changes. They are currently associated with intentional process-termination tests, but the report should remain a regression baseline.

### Priority 1 - packaging and portability

1. Validate the installed exported CMake target and package configuration on Windows `clang-cl`, including public include directories, compile features, platform link dependencies, version metadata, and both shared/static variants.
2. Extend the Conan package matrix: the Linux/Clang shared and static Release consumers and shared Debug package now pass; add Windows `clang-cl` consumers and static Debug validation when that host is available.
3. Move generated Doxygen output to the build directory and add a clean documentation target.
4. Validate the supported LLVM matrix: Linux Clang and Windows `clang-cl` with the MSVC ABI/toolchain required by CUDA, on both Debug and Release, with the supported C++23 standard library combinations. Do not add GCC or standalone MSVC compiler jobs unless the support policy changes.

### Priority 2 - automated quality gates

1. Add CI for Linux Clang and Windows `clang-cl` with the MSVC ABI/toolchain, Debug and Release, shared and static builds, CTest, formatting, clang-tidy/compiler warnings, LLVM ASan/TSan where supported, Linux Valgrind, coverage, benchmarks, and package-consumer smoke tests.
2. Keep the existing 100% regions/functions/lines gate, but review the 88.17% branch coverage and define a policy for uncovered branches.
3. Add repeated-run and flaky-test detection, test timeouts, failure artifact collection, and deterministic random seeds for concurrency and fuzz tests.
4. Add API/ABI compatibility checks, a supported-platform/compiler matrix, a changelog, semantic versioning policy, and release provenance including dependency and toolchain versions.

### Priority 3 - API, operations, and documentation

1. Document thread-safety guarantees, linearization points, callback ordering, reentrancy, asynchronous execution, cancellation, shutdown behavior, and ownership semantics for every concurrent or asynchronous public type.
2. Document the supported JSON grammar and policy for unknown fields, duplicate keys, numeric overflow, trailing data, maximum input size, and nesting depth.
3. Add examples that build against the installed Conan/CMake package, not only examples embedded in Doxygen comments.
4. Define operational behavior for logger backpressure, queue exhaustion, task failure propagation, thread startup failure, and process termination.
5. Add security review, dependency scanning, fuzz corpus review, license tracking, and a documented disclosure process before treating the library as production-ready for external consumers.

## Conclusion

The repository has a strong functional foundation and a useful standalone core API. The current evidence is encouraging: Memcheck found no invalid accesses or definite/indirect leaks, ASan and TSan completed cleanly for their registered Linux/Clang subsets, the serialization benchmarks provide a normal-run baseline, all 27 registered CTest executables pass in the latest coverage run, and LLVM production regions/functions/lines are at 100%. Shared and static Release Conan packages, a shared Debug package, and external CMake consumers now validate the published `nbn::core` target on Linux/Clang. The supported portability target is LLVM on Linux and Windows, with Windows using `clang-cl` and the MSVC ABI/toolchain required by CUDA; that Windows matrix is not yet demonstrated by the current reports. The remaining risks are framework-level rather than an identified memory defect: branch coverage is incomplete, concurrency and parser stress/fuzz workloads are limited, Windows package validation is open, and the quality gates are not yet enforced continuously in CI.

## What to do next

1. **Strengthen adversarial testing:** extend the existing concurrency and shutdown stress coverage with fault injection and JSON fuzzing, then run these suites under ASan and TSan.
2. **Review branch coverage:** use the LLVM report to decide whether the 88.17% uncovered branch outcomes need tests or documented justification.
3. **Broaden package consumption:** validate the existing clean external Conan/CMake consumer on Windows `clang-cl`, add static Debug coverage, and include package smoke tests in CI.
4. **Broaden performance evidence:** add queue, logging, signal, timer, task, and object-factory benchmarks with repeatable baselines and regression thresholds.
5. **Automate and broaden the matrix:** enforce all quality checks in CI across Linux Clang and Windows `clang-cl`/MSVC ABI toolchain configurations, build types, and dependency configurations. Keep Linux-only Valgrind explicitly separate from the Windows gates.
6. **Specify the public contract:** document concurrency, callback, JSON, shutdown, error, ABI, and portability guarantees before calling the framework production-ready.
