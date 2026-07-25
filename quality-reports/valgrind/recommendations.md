# Recommendations

> Prioritized actions generated from the analyzed Valgrind reports. Start with confirmed correctness defects; treat profiling observations as hypotheses until validated outside Valgrind.

## At a glance

| Area | Status | What it means | Next action |
|---|---|---|---|
| Memcheck | 🟢 **Clear** | No invalid accesses or definite/indirect leaks were detected | Continue monitoring possibly lost and still-reachable memory |
| Still reachable | 🟡 **Review** | 63 allocation record(s) remain reachable at process exit | Confirm ownership and intended lifetime |
| Valgrind warnings | 🟢 **None found** | 0 warning occurrence group(s) were recorded | Correct actionable warnings, then rerun Memcheck |
| Massif | 🔵 **Observation** | Peak heap: 16.7 MiB | Compare against a workload baseline before optimizing |
| Callgrind | 🔵 **Observation** | Instruction hotspots were collected | Validate candidates with a normal benchmark |

## 1. Memcheck status: clear

No invalid-access errors or definite/indirect leaks were detected in the analyzed logs. Review `possibly lost` and `still reachable` allocations only if they grow across repeated runs or violate the intended lifetime model.

## 2. Still-reachable allocations

63 record(s) were found in 3 report(s). `still reachable` means Valgrind can still find a pointer at process exit; it is not proof of a leak.

| Classification | Records | Interpretation | Action |
|---|---:|---|---|
| 🟢 Expected process lifetime | 63 | Intentional exit tests: `test_ApplicationSegfault`, `test_ApplicationUncaughtException`, `test_CommandLineParserHelp` | Do not add artificial production cleanup; preserve the exit semantics |

<details>
<summary>Representative records and raw reports</summary>

| Test | Allocation record | Report |
|---|---|---|
| `test_ApplicationUncaughtException` | 29 bytes in 1 block(s); first project frame `src/nbn/core/utils.cpp:86` | [memcheck-2618900.log](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-2618900.log) |
| `test_ApplicationUncaughtException` | 32 bytes in 1 block(s); first project frame `src/nbn/core/../../nbn/core/detail/Signal.h:59` | [memcheck-2618900.log](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-2618900.log) |
| `test_ApplicationUncaughtException` | 32 bytes in 1 block(s); first project frame `src/nbn/core/detail/Signal.h:118` | [memcheck-2618900.log](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-2618900.log) |
| `test_ApplicationUncaughtException` | 37 bytes in 1 block(s); first project frame `src/nbn/core/utils.cpp:146` | [memcheck-2618900.log](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-2618900.log) |
| `test_ApplicationUncaughtException` | 37 bytes in 1 block(s); first project frame `src/nbn/core/utils.cpp:146` | [memcheck-2618900.log](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-2618900.log) |
| `test_ApplicationUncaughtException` | 40 bytes in 1 block(s); first project frame `src/nbn/core/detail/ObjectFeature.cpp:9` | [memcheck-2618900.log](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-2618900.log) |
| `test_ApplicationUncaughtException` | 40 bytes in 1 block(s); first project frame `src/nbn/core/detail/ObjectFeature.cpp:9` | [memcheck-2618900.log](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-2618900.log) |
| `test_ApplicationUncaughtException` | 40 bytes in 1 block(s); first project frame `src/nbn/core/detail/ObjectFeature.cpp:9` | [memcheck-2618900.log](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-2618900.log) |
| `test_ApplicationUncaughtException` | 48 bytes in 1 block(s); first project frame `tests/nbn/core/ApplicationUncaughtException.cpp:12` | [memcheck-2618900.log](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-2618900.log) |
| `test_ApplicationUncaughtException` | 60 bytes in 1 block(s); first project frame `src/nbn/core/UnitTests.cpp:51` | [memcheck-2618900.log](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-2618900.log) |

All affected reports: [memcheck-2618900.log](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-2618900.log), [memcheck-2624720.log](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-2624720.log), [memcheck-2624780.log](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-2624780.log).

</details>

## 4. Massif peak

| Test | Peak heap | Report |
|---|---:|---|
| `test_Serialization` | **16.7 MiB** | [massif-2626490.out](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/massif/massif-2626490.out) |

This is a measured peak, not proof of a leak. Inspect the peak snapshot’s allocation tree, identify the owning container or buffer, and compare the same test against a baseline before changing allocation behavior.

## 5. Callgrind hotspots

Instruction counts locate hot functions but are not wall-clock timings. Validate any optimization with a normal benchmark.

| Instructions | Project-owned hotspot |
|---:|---|
| 6,758,750,962 | `/home/nbn/src/github.com/nicolasberron/nbn-cpp-api-core/src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()` |
| 4,694,928,609 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2 [/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Serialization]` |
| 3,583,232,023 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2 [/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_UnitTests]` |
| 2,699,220,171 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2 [/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_FiniteStateMachine]` |
| 2,563,742,248 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2 [/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_utils]` |

**Required next step:** add a focused benchmark and compare wall-clock timings outside Valgrind before changing code.

## Repeatable workflow

1. Keep raw reports beside the generated Markdown so every finding remains traceable.
2. Reproduce one affected executable with the same build and Valgrind options.
3. Apply one focused change, rerun the relevant profile, and compare the result.
4. Use normal benchmark timings—not Valgrind instruction counts—for performance decisions.
