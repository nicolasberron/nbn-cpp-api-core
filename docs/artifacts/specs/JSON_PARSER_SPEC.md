# JSON parser security contract

## Scope

This contract applies to the public serialization entry points and the JSON helper parsers in `src/nbn/core/detail/serialization.*`.

## Input limits

- Input is limited to 16 MiB.
- Nesting is limited to 256 levels and is also bounded by the input size.
- Collection element and object-property counts are bounded by the input-size-derived budget.
- Decoded strings are bounded by the input-size-derived expansion budget.
- Exceeding a limit is a fatal parse error; callers must treat the input as rejected.

## Syntax and policy

- Duplicate object keys are rejected. The parser does not use last-key-wins behavior.
- Unknown object fields are accepted and ignored when deserializing an object envelope. This preserves forward compatibility; application schema validation remains the responsibility of the object type.
- Numeric syntax is validated before conversion. Values outside the destination type range are rejected; signed and unsigned overflow must not wrap.
- Floating-point values outside the supported range are rejected rather than converted to infinity.
- A complete JSON value must consume the entire input after optional JSON whitespace. Any trailing non-whitespace data is rejected.
- Trailing commas are rejected in arrays and objects.
- Malformed escapes, invalid Unicode surrogate sequences, invalid constants, and invalid number grammar are rejected.

## Differential and fuzz validation

The committed corpus under `tests/core/utilities/fuzz_corpus/` contains valid, malformed, duplicate-key, overflow, Unicode, nesting, and trailing-data seeds. When the Clang libFuzzer option is enabled:

- `fuzz_serialization` runs the variant deserializer with ASan and UBSan.
- `json_parser_probe` provides a file-oriented acceptance probe.
- `scripts/differential_json.py` compares acceptance against Python's strict decoder with duplicate-key rejection enabled.
- `scripts/run_json_fuzz.py` runs bounded fuzz jobs and libFuzzer corpus minimization. The default job is 300 seconds, has a 10-second per-input timeout, a 2 GiB RSS limit, and retains the 16 MiB parser input ceiling.

Fuzz findings must be minimized, added to the corpus, and reproduced by a deterministic serialization regression test before they are considered resolved.

## Non-goals

The parser is not a schema validator. It does not reject unknown fields in generic object maps, and it does not guarantee semantic equivalence with every JSON implementation for implementation-specific number precision or object ordering.
