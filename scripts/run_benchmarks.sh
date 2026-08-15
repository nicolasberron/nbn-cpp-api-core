#!/usr/bin/env bash
set -euo pipefail

usage() {
    printf '%s\n' 'Usage: run_benchmarks.sh --build-dir DIR --executable PATH --tool TOOL [--output-dir DIR] [--report-dir DIR] [-- ARG ...]'
    printf '%s\n' 'Tools: normal, perf, heaptrack, callgrind, massif, memcheck, asan, ubsan, fuzz'
}

build_dir=''
executable=''
tool='normal'
output_dir=''
report_dir='doc/quality-reports/benchmark'
program_args=()
while (($# > 0)); do
    case "$1" in
        --build-dir) build_dir=${2-}; shift 2 ;;
        --executable) executable=${2-}; shift 2 ;;
        --tool) tool=${2-}; shift 2 ;;
        --output-dir) output_dir=${2-}; shift 2 ;;
        --report-dir) report_dir=${2-}; shift 2 ;;
        --) shift; program_args=("$@"); break ;;
        -h|--help) usage; exit 0 ;;
        *) printf 'Unknown option: %s\n' "$1" >&2; usage >&2; exit 2 ;;
    esac
done

if [[ -z "$build_dir" || -z "$executable" ]]; then usage >&2; exit 2; fi
if [[ -z "$output_dir" ]]; then output_dir="$build_dir/benchmark-results"; fi
mkdir -p "$output_dir"
if [[ "$executable" != /* ]]; then executable="$build_dir/$executable"; fi
if [[ ! -x "$executable" ]]; then printf 'Executable not found or not executable: %s\n' "$executable" >&2; exit 1; fi

require_tool() { command -v "$1" >/dev/null 2>&1 || { printf 'Missing optional tool: %s\n' "$1" >&2; exit 1; }; }
timestamp=$(date --iso-8601=seconds)
run_type='diagnostic'
if [[ "$tool" == normal || "$tool" == perf ]]; then run_type='timing'; fi
printf 'run_type=%s\ntool=%s\nexecutable=%s\nstarted=%s\n' "$run_type" "$tool" "$executable" "$timestamp" > "$output_dir/metadata.txt"
readme_path="$output_dir/README.md"
cat > "$readme_path" <<EOF
# Benchmark report

- **Tool:** $tool
- **Executable:** \`$executable\`
- **Started:** $timestamp

The raw output for this run is stored in \`$output_dir\`. This page is the published entry point for the run.

[PASS]
EOF

case "$tool" in
    normal) "$executable" --json "$output_dir/timing.json" "${program_args[@]}" 2>&1 | tee "$output_dir/timing.txt" ;;
    perf) require_tool perf; perf stat -o "$output_dir/perf.txt" -- "$executable" "${program_args[@]}" ;;
    heaptrack) require_tool heaptrack; heaptrack --output "$output_dir/heaptrack" "$executable" "${program_args[@]}" 2>&1 | tee "$output_dir/heaptrack.txt" ;;
    callgrind) require_tool valgrind; valgrind --tool=callgrind --callgrind-out-file="$output_dir/callgrind.out" "$executable" "${program_args[@]}" 2>&1 | tee "$output_dir/callgrind.txt" ;;
    massif) require_tool valgrind; valgrind --tool=massif --massif-out-file="$output_dir/massif.out" "$executable" "${program_args[@]}" 2>&1 | tee "$output_dir/massif.txt" ;;
    memcheck) require_tool valgrind; valgrind --tool=memcheck --leak-check=full "$executable" "${program_args[@]}" 2>&1 | tee "$output_dir/memcheck.txt" ;;
    asan|ubsan) "$executable" "${program_args[@]}" 2>&1 | tee "$output_dir/$tool.txt" ;;
    fuzz) "$executable" "${program_args[@]}" 2>&1 | tee "$output_dir/fuzz.txt" ;;
    *) printf 'Unknown tool: %s\n' "$tool" >&2; usage >&2; exit 2 ;;
esac

if [[ "$tool" == normal ]]; then
    if [[ ! -f "$output_dir/timing.json" ]]; then
        printf 'Benchmark did not produce timing results: %s\n' "$output_dir/timing.json" >&2
        exit 1
    fi
    python3 - "$output_dir/timing.json" "$readme_path" <<'PY'
import json
import math
import sys
from pathlib import Path

timing_path = Path(sys.argv[1])
readme_path = Path(sys.argv[2])
try:
    document = json.loads(timing_path.read_text(encoding="utf-8"))
except (OSError, json.JSONDecodeError) as error:
    raise SystemExit(f"Unable to parse benchmark results: {error}")

results = document.get("results")
if not isinstance(results, list) or not results:
    raise SystemExit("Benchmark results must contain a non-empty results array")

required_fields = (
    "name", "iterations", "sample_runs", "median_latency_seconds",
    "p90_latency_seconds", "p95_latency_seconds",
    "median_throughput_ops_per_second",
)
for index, result in enumerate(results):
    if not isinstance(result, dict) or any(field not in result for field in required_fields):
        raise SystemExit(f"Benchmark result {index} is missing a required field")
    for field in required_fields[1:]:
        value = result[field]
        if not isinstance(value, (int, float)) or isinstance(value, bool) or not math.isfinite(value):
            raise SystemExit(f"Benchmark result {index} has a non-numeric {field}")


def markdown_text(value):
    return str(value).replace("\\", "\\\\").replace("|", "\\|").replace("\n", " ")

with readme_path.open("a", encoding="utf-8") as readme:
    readme.write("\n## Results\n\n")
    readme.write("| Benchmark | Iterations | Samples | Median latency (s) | P90 latency (s) | P95 latency (s) | Median throughput (ops/s) |\n")
    readme.write("| --- | ---: | ---: | ---: | ---: | ---: | ---: |\n")
    for result in results:
        readme.write(
            "| {name} | {iterations} | {sample_runs} | {median_latency:.6f} | {p90_latency:.6f} | {p95_latency:.6f} | {median_throughput:.2f} |\n".format(
                name=markdown_text(result["name"]),
                iterations=result["iterations"],
                sample_runs=result["sample_runs"],
                median_latency=result["median_latency_seconds"],
                p90_latency=result["p90_latency_seconds"],
                p95_latency=result["p95_latency_seconds"],
                median_throughput=result["median_throughput_ops_per_second"],
            )
        )
PY
fi

mkdir -p "$report_dir"
cp "$readme_path" "$report_dir/README.md"
