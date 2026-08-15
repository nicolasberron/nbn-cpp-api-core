# Memcheck

Memcheck is **OK** when there are no invalid-access errors and no definite or indirect leaks. `still reachable` memory is reported as informational.


## ✅ Clean

| Test | Errors | Invalid accesses | Still reachable | Report |
|---|---:|---:|---:|---|
| _none_ |  |  |  | ℹ️ none |

## ℹ️ Informational allocations and Valgrind warnings

Still-reachable memory is not a lost allocation, but each record is listed so its lifetime can be reviewed. Warnings describe instrumentation or platform behavior and should not be silently discarded.

No still-reachable allocation records were found.

### Warnings

No Valgrind warnings were found.
