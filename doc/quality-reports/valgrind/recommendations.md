# Recommendations

> Prioritized actions generated from the analyzed Valgrind reports. Start with confirmed correctness defects; treat profiling observations as hypotheses until validated outside Valgrind.

## At a glance

| Area | Status | What it means | Next action |
|---|---|---|---|
| Memcheck | 🟢 **Clear** | No invalid accesses or definite/indirect leaks were detected | Continue monitoring possibly lost and still-reachable memory |
| Still reachable | 🟢 **None found** | 0 allocation record(s) remain reachable at process exit | Confirm ownership and intended lifetime |
| Valgrind warnings | 🟢 **None found** | 0 warning occurrence group(s) were recorded | Correct actionable warnings, then rerun Memcheck |
| Massif | ⚪ **Unavailable** | No Massif peak was available | Compare against a workload baseline before optimizing |
| Callgrind | ⚪ **Unavailable** | No Callgrind reports were available | Validate candidates with a normal benchmark |

## 1. Memcheck status: clear

No invalid-access errors or definite/indirect leaks were detected in the analyzed logs. Review `possibly lost` and `still reachable` allocations only if they grow across repeated runs or violate the intended lifetime model.

## Repeatable workflow

1. Keep raw reports beside the generated Markdown so every finding remains traceable.
2. Reproduce one affected executable with the same build and Valgrind options.
3. Apply one focused change, rerun the relevant profile, and compare the result.
4. Use normal benchmark timings—not Valgrind instruction counts—for performance decisions.
