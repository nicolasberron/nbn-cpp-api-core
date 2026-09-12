# ASan and TSan report

This is the consolidated sanitizer report. A sanitizer normally produces no console output when the instrumented process completes without findings; silence alone is not treated as proof of instrumentation.

- Reports analyzed: 4
- Sanitizer findings: 0
- Unresolved failed tests: 0

## AddressSanitizer (ASan)

**Status:** clean; no sanitizer diagnostic was emitted.

A clean sanitizer run is normally silent. This status is based on the absence of sanitizer diagnostics in the supplied logs, the recorded CTest results, and the sanitizer build-directory context.

- Reports classified as AddressSanitizer (ASan): 2
- Recorded passed CTest executables: 28
- Unresolved failed tests: 0
- Expected clean process status: 0; the supplied Markdown logs do not independently record the shell exit code.

## ThreadSanitizer (TSan)

**Status:** clean; no sanitizer diagnostic was emitted.

A clean sanitizer run is normally silent. This status is based on the absence of sanitizer diagnostics in the supplied logs, the recorded CTest results, and the sanitizer build-directory context.

- Reports classified as ThreadSanitizer (TSan): 2
- Recorded passed CTest executables: 25
- Unresolved failed tests: 0
- Expected clean process status: 0; the supplied Markdown logs do not independently record the shell exit code.
