# Benchmark tools installation

This page installs the optional tools used by the dependency-free benchmark
framework. The fenced `bash` blocks are executable from VS Code with the
`nbn-vscode-interactive-markdown` extension.

The commands below target Debian/Ubuntu Linux. They install tools globally
through the system package manager; review each command before running it.

## Install all benchmark tools

```bash
sudo apt-get update && sudo apt-get install -y \
    clang \
    llvm \
    lldb \
    linux-perf \
    heaptrack \
    valgrind
```

## Verify the installations

```bash
for tool in clang clang++ lldb perf heaptrack valgrind; do
    if command -v "$tool" >/dev/null 2>&1; then
        printf '%-12s %s\\n' "$tool" "$(command -v "$tool")"
    else
        printf '%-12s NOT FOUND\\n' "$tool"
    fi
done
```

## Verify sanitizer support

AddressSanitizer and UndefinedBehaviorSanitizer are provided by Clang. This
only checks compiler support; select `linux-clang19-asan` or
`linux-clang19-ubsan` when generating the benchmark build.

```bash
printf 'clang: '
clang++ --version | head -n 1
printf 'ASan/UBSan compile check: '
printf 'int main(){}\n' | clang++ -x c++ - -fsanitize=address,undefined -o /tmp/nbn-sanitizer-check && rm -f /tmp/nbn-sanitizer-check
```

## Verify libFuzzer support

The fuzz target requires Clang's libFuzzer runtime.

```bash
printf 'extern "C" int LLVMFuzzerTestOneInput(const unsigned char*, unsigned long){return 0;}\n' \
    | clang++ -x c++ - -fsanitize=fuzzer,address -o /tmp/nbn-libfuzzer-check && \
    rm -f /tmp/nbn-libfuzzer-check
```

## Verify Valgrind tools

```bash
valgrind --tool=memcheck --version
valgrind --tool=callgrind --version
valgrind --tool=massif --version
```

## Verify performance tools

`perf` may require additional kernel permissions on some systems. The command
below checks availability without collecting a benchmark profile.

```bash
perf --version && heaptrack --version
```

## Build the optional benchmark targets

Run this from the repository root after Conan has generated the configured
build directory. The benchmark option is intentionally disabled by default.

```bash
cmake --preset conan-debug -DNBN_BUILD_BENCHMARKS=ON
cmake --build --preset conan-debug --target benchmark_serialization
```

Build the sanitizer target in a directory generated with the matching Conan
profile:

```bash
conan install . \
    -of /home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-asan \
    -pr linux-clang19-asan
source /home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-asan/conanbuildenv-debug-x86_64.sh
cmake -S . \
    -B /home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-asan \
    -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE=/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-asan/conan_toolchain.cmake \
    -DNBN_BUILD_BENCHMARKS=ON
cmake --build /home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-asan \
    --target benchmark_serialization_sanitized
```

Use `linux-clang19-ubsan` instead for a UBSan-only benchmark. The fuzz target
requires Clang's libFuzzer runtime and the dedicated fuzz profile:

```bash
conan install . \
    -of /home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-fuzz \
    -pr linux-clang19-fuzz
source /home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-fuzz/conanbuildenv-debug-x86_64.sh
cmake -S . -B /home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-fuzz \
    -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE=/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-fuzz/conan_toolchain.cmake \
    -DNBN_BUILD_BENCHMARKS=ON
cmake --build /home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-fuzz \
    --target fuzz_serialization
```

## Run the benchmark tools

The runner writes each tool's output below the selected result directory.
Replace the build directory if a different Conan profile is being used.

```bash
"$NBN_CPP_API_BUILD_SYSTEM_SCRIPTS_DIR/run_benchmarks.sh" \
    --build-dir /home/nbn/dev/builds/nbn-cpp-api-core/linux-gcc13-debug \
    --executable tests/core/utilities/benchmark_serialization \
    --tool normal \
    --output-dir benchmark-results/normal
```

```bash
"$NBN_CPP_API_BUILD_SYSTEM_SCRIPTS_DIR/run_benchmarks.sh" \
    --build-dir /home/nbn/dev/builds/nbn-cpp-api-core/linux-gcc13-debug \
    --executable tests/core/utilities/benchmark_serialization \
    --tool perf \
    --output-dir benchmark-results/perf
```

```bash
"$NBN_CPP_API_BUILD_SYSTEM_SCRIPTS_DIR/run_benchmarks.sh" \
    --build-dir /home/nbn/dev/builds/nbn-cpp-api-core/linux-gcc13-debug \
    --executable tests/core/utilities/benchmark_serialization \
    --tool heaptrack \
    --output-dir benchmark-results/heaptrack
```

Run Valgrind diagnostics with the same runner:

```bash
for tool in callgrind massif memcheck; do
    "$NBN_CPP_API_BUILD_SYSTEM_SCRIPTS_DIR/run_benchmarks.sh" \
        --build-dir /home/nbn/dev/builds/nbn-cpp-api-core/linux-gcc13-debug \
        --executable tests/core/utilities/benchmark_serialization \
        --tool "$tool" \
        --output-dir "benchmark-results/$tool"
done
```

Run sanitizer and fuzz diagnostics with their separately built executables:

```bash
"$NBN_CPP_API_BUILD_SYSTEM_SCRIPTS_DIR/run_benchmarks.sh" \
    --build-dir /home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-asan \
    --executable tests/core/utilities/benchmark_serialization_sanitized \
    --tool asan \
    --output-dir benchmark-results/asan
```

```bash
"$NBN_CPP_API_BUILD_SYSTEM_SCRIPTS_DIR/run_benchmarks.sh" \
    --build-dir /home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-ubsan \
    --executable tests/core/utilities/benchmark_serialization_sanitized \
    --tool ubsan \
    --output-dir benchmark-results/ubsan
```

```bash
"$NBN_CPP_API_BUILD_SYSTEM_SCRIPTS_DIR/run_benchmarks.sh" \
    --build-dir /home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-fuzz \
    --executable tests/core/utilities/fuzz_serialization \
    --tool fuzz \
    --output-dir benchmark-results/fuzz
```

## Notes

- Debian Trixie provides `perf` through the `linux-perf` package. The
    kernel-specific `linux-tools-common` and `linux-tools-$(uname -r)` package
    names are not available on all Debian installations and are intentionally
    not used here.
- If `apt-get update` reports a signature error for an unrelated third-party
    repository, repair or temporarily disable that repository and rerun the
    update. Do not bypass APT signature verification.
- `normal` and `perf` are timing runs.
- Heaptrack, Valgrind, sanitizers, and fuzzing are diagnostic runs and must
  not be compared with normal benchmark timings.
- The fuzz target may run indefinitely; stop it with `Ctrl+C` after collecting
  the desired number of executions.
- The benchmark framework itself has no third-party code dependency. These
  tools are optional developer-machine dependencies only.
