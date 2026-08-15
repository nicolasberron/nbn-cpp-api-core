# Recommendations

## Confirmed

- **ASan:** no AddressSanitizer finding marker was detected in the analyzed logs.
- **TSan:** no ThreadSanitizer finding marker was detected in the analyzed logs.

## Informational

- The consolidated status is based on sanitizer diagnostics, explicit CTest pass/fail markers, and sanitizer build-directory context.
- CTest's `LastTestsFailed.log` entries are reconciled with explicit `Test Passed.` and `Test Failed.` markers in `LastTest.log` so stale entries do not remain failures.
- Repeated sanitizer findings are deduplicated separately.
- No timeout or expected-signal classification was inferred unless it was present in the supplied logs.

## Exact next action

## Reconciled CTest entries

The following entries were present in `LastTestsFailed.log` but had an explicit `Test Passed.` marker in `LastTest.log`. They are reported here for traceability and are not counted as unresolved failures.

- **stale failure entry** `test_ApplicationThreadsManager`: explicit CTest pass reconciled the failure; failure list: [LastTestsFailed.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-asan/Testing/Temporary/LastTestsFailed.log); full failure entry: [failures.md](failures.md).
- **stale failure entry** `test_Logger`: explicit CTest pass reconciled the failure; failure list: [LastTestsFailed.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-tsan/Testing/Temporary/LastTestsFailed.log); full failure entry: [failures.md](failures.md).

No sanitizer findings or unresolved test failures were detected in the analyzed logs.
