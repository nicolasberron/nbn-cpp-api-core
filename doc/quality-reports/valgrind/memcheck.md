# Memcheck

Memcheck is **OK** when there are no invalid-access errors and no definite or indirect leaks. `still reachable` memory is reported as informational.


## ✅ Clean

| Test | Errors | Invalid accesses | Still reachable | Report |
|---|---:|---:|---:|---|
| `test_Application` | 0 | 0 | 0 B | [memcheck-725956.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-725956.log) |
| `test_ApplicationSignalInterrupt` | 0 | 0 | 0 B | [memcheck-726095.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726095.log) |
| `test_ApplicationTerminateHandler` | 0 | 0 | 0 B | [memcheck-726133.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726133.log) |
| `test_ApplicationThreadsManager` | 0 | 0 | 0 B | [memcheck-726164.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726164.log) |
| `test_ApplicationUncaughtException` | 1 | 0 | 2.3 KiB | [memcheck-726293.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726293.log) |
| `test_Color` | 0 | 0 | 0 B | [memcheck-726390.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726390.log) |
| `test_CommandLineParser` | 0 | 0 | 0 B | [memcheck-726432.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726432.log) |
| `test_ConcurrentStack` | 0 | 0 | 0 B | [memcheck-726470.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726470.log) |
| `test_ElapsedTimer` | 0 | 0 | 0 B | [memcheck-726781.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726781.log) |
| `test_FilesystemWatcher` | 0 | 0 | 0 B | [memcheck-726809.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726809.log) |
| `test_FiniteStateMachine` | 0 | 0 | 0 B | [memcheck-726864.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726864.log) |
| `test_JsonMigratorBase` | 0 | 0 | 0 B | [memcheck-727043.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-727043.log) |
| `test_Object` | 0 | 0 | 0 B | [memcheck-727106.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-727106.log) |
| `test_ObjectSync` | 0 | 0 | 0 B | [memcheck-727248.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-727248.log) |
| `test_Serialization` | 0 | 0 | 0 B | [memcheck-727298.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-727298.log) |
| `test_Signal` | 0 | 0 | 0 B | [memcheck-727596.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-727596.log) |
| `test_Slots` | 0 | 0 | 0 B | [memcheck-727652.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-727652.log) |
| `test_Task` | 0 | 0 | 0 B | [memcheck-727715.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-727715.log) |
| `test_Thread` | 0 | 0 | 0 B | [memcheck-727821.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-727821.log) |
| `test_Timer` | 0 | 0 | 0 B | [memcheck-727907.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-727907.log) |
| `test_UnitTests` | 0 | 0 | 0 B | [memcheck-728043.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728043.log) |
| `test_utils` | 0 | 0 | 0 B | [memcheck-728179.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728179.log) |
| `test_CommandLineParserHelp` | 0 | 0 | 1.3 KiB | [memcheck-728311.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728311.log) |
| `test_ApplicationSegfault` | 0 | 0 | 1.5 KiB | [memcheck-728340.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728340.log) |
| `test_ApplicationTerminateProcess` | 0 | 0 | 0 B | [memcheck-728412.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728412.log) |
| `test_ApplicationTerminateProcess` | 0 | 0 | 0 B | [memcheck-728436.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728436.log) |
| `test_ApplicationTerminateProcess` | 0 | 0 | 0 B | [memcheck-728438.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728438.log) |

## ℹ️ Informational allocations and Valgrind warnings

Still-reachable memory is not a lost allocation, but each record is listed so its lifetime can be reviewed. Warnings describe instrumentation or platform behavior and should not be silently discarded.

### Still reachable

| Test | Allocation | Report |
|---|---|---|
| `test_ApplicationUncaughtException` | 29 bytes in 1 block(s); first project frame `src/nbn/core/utils.cpp:86` | [memcheck-726293.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726293.log) |
| `test_ApplicationUncaughtException` | 32 bytes in 1 block(s); first project frame `src/nbn/core/../../nbn/core/detail/Signal.h:59` | [memcheck-726293.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726293.log) |
| `test_ApplicationUncaughtException` | 32 bytes in 1 block(s); first project frame `src/nbn/core/detail/Signal.h:118` | [memcheck-726293.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726293.log) |
| `test_ApplicationUncaughtException` | 37 bytes in 1 block(s); first project frame `src/nbn/core/utils.cpp:146` | [memcheck-726293.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726293.log) |
| `test_ApplicationUncaughtException` | 37 bytes in 1 block(s); first project frame `src/nbn/core/utils.cpp:146` | [memcheck-726293.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726293.log) |
| `test_ApplicationUncaughtException` | 40 bytes in 1 block(s); first project frame `src/nbn/core/detail/ObjectFeature.cpp:9` | [memcheck-726293.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726293.log) |
| `test_ApplicationUncaughtException` | 40 bytes in 1 block(s); first project frame `src/nbn/core/detail/ObjectFeature.cpp:9` | [memcheck-726293.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726293.log) |
| `test_ApplicationUncaughtException` | 40 bytes in 1 block(s); first project frame `src/nbn/core/detail/ObjectFeature.cpp:9` | [memcheck-726293.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726293.log) |
| `test_ApplicationUncaughtException` | 48 bytes in 1 block(s); first project frame `tests/nbn/core/ApplicationUncaughtException.cpp:12` | [memcheck-726293.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726293.log) |
| `test_ApplicationUncaughtException` | 60 bytes in 1 block(s); first project frame `src/nbn/core/UnitTests.cpp:51` | [memcheck-726293.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726293.log) |
| `test_ApplicationUncaughtException` | 60 bytes in 1 block(s); first project frame `src/nbn/core/UnitTests.cpp:48` | [memcheck-726293.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726293.log) |
| `test_ApplicationUncaughtException` | 64 bytes in 1 block(s) | [memcheck-726293.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726293.log) |
| `test_ApplicationUncaughtException` | 64 bytes in 1 block(s); first project frame `src/nbn/core/UnitTests.cpp:17` | [memcheck-726293.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726293.log) |
| `test_ApplicationUncaughtException` | 64 bytes in 1 block(s); first project frame `src/nbn/core/private/Object.cpp:49` | [memcheck-726293.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726293.log) |
| `test_ApplicationUncaughtException` | 64 bytes in 1 block(s); first project frame `src/nbn/core/private/Object.cpp:49` | [memcheck-726293.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726293.log) |
| `test_ApplicationUncaughtException` | 92 bytes in 2 block(s); first project frame `src/nbn/core/utils.cpp:78` | [memcheck-726293.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726293.log) |
| `test_ApplicationUncaughtException` | 96 bytes in 1 block(s); first project frame `src/nbn/core/UnitTests.cpp:62` | [memcheck-726293.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726293.log) |
| `test_ApplicationUncaughtException` | 104 bytes in 1 block(s); first project frame `src/nbn/core/private/Object.cpp:49` | [memcheck-726293.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726293.log) |
| `test_ApplicationUncaughtException` | 112 bytes in 1 block(s); first project frame `src/nbn/core/private/ApplicationThreadsManager.cpp:142` | [memcheck-726293.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726293.log) |
| `test_ApplicationUncaughtException` | 112 bytes in 1 block(s); first project frame `tests/nbn/core/ApplicationUncaughtException.cpp:18` | [memcheck-726293.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726293.log) |
| `test_ApplicationUncaughtException` | 120 bytes in 1 block(s); first project frame `src/nbn/core/detail/Object.h:12` | [memcheck-726293.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726293.log) |
| `test_ApplicationUncaughtException` | 120 bytes in 1 block(s); first project frame `src/nbn/core/detail/Object.h:12` | [memcheck-726293.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726293.log) |
| `test_ApplicationUncaughtException` | 120 bytes in 1 block(s); first project frame `src/nbn/core/detail/Object.h:12` | [memcheck-726293.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726293.log) |
| `test_ApplicationUncaughtException` | 208 bytes in 1 block(s); first project frame `src/nbn/core/Task.cpp:15` | [memcheck-726293.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726293.log) |
| `test_ApplicationUncaughtException` | 304 bytes in 1 block(s); first project frame `src/nbn/core/Object.cpp:11` | [memcheck-726293.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726293.log) |
| `test_ApplicationUncaughtException` | 304 bytes in 1 block(s); first project frame `src/nbn/core/Object.cpp:11` | [memcheck-726293.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-726293.log) |
| `test_CommandLineParserHelp` | 24 bytes in 1 block(s) | [memcheck-728311.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728311.log) |
| `test_CommandLineParserHelp` | 25 bytes in 1 block(s) | [memcheck-728311.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728311.log) |
| `test_CommandLineParserHelp` | 25 bytes in 1 block(s) | [memcheck-728311.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728311.log) |
| `test_CommandLineParserHelp` | 32 bytes in 1 block(s); first project frame `src/nbn/core/CommandLineParser.cpp:17` | [memcheck-728311.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728311.log) |
| `test_CommandLineParserHelp` | 37 bytes in 1 block(s); first project frame `src/nbn/core/utils.cpp:146` | [memcheck-728311.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728311.log) |
| `test_CommandLineParserHelp` | 64 bytes in 1 block(s); first project frame `tests/nbn/core/CommandLineParserHelp.cpp:7` | [memcheck-728311.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728311.log) |
| `test_CommandLineParserHelp` | 64 bytes in 1 block(s); first project frame `src/nbn/core/CommandLineParser.cpp:16` | [memcheck-728311.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728311.log) |
| `test_CommandLineParserHelp` | 80 bytes in 1 block(s); first project frame `src/nbn/core/CommandLineParser.cpp:173` | [memcheck-728311.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728311.log) |
| `test_CommandLineParserHelp` | 80 bytes in 1 block(s); first project frame `src/nbn/core/CommandLineParser.cpp:174` | [memcheck-728311.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728311.log) |
| `test_CommandLineParserHelp` | 104 bytes in 1 block(s); first project frame `src/nbn/core/CommandLineParser.cpp:173` | [memcheck-728311.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728311.log) |
| `test_CommandLineParserHelp` | 104 bytes in 1 block(s); first project frame `src/nbn/core/CommandLineParser.cpp:174` | [memcheck-728311.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728311.log) |
| `test_CommandLineParserHelp` | 304 bytes in 1 block(s); first project frame `src/nbn/core/Object.cpp:11` | [memcheck-728311.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728311.log) |
| `test_CommandLineParserHelp` | 352 bytes in 1 block(s); first project frame `src/nbn/core/CommandLineParser.cpp:265` | [memcheck-728311.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728311.log) |
| `test_ApplicationSegfault` | 17 bytes in 1 block(s); first project frame `src/nbn/core/private/Application.cpp:236` | [memcheck-728340.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728340.log) |
| `test_ApplicationSegfault` | 17 bytes in 1 block(s); first project frame `src/nbn/core/private/Application.cpp:236` | [memcheck-728340.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728340.log) |
| `test_ApplicationSegfault` | 18 bytes in 1 block(s); first project frame `src/nbn/core/private/Application.cpp:236` | [memcheck-728340.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728340.log) |
| `test_ApplicationSegfault` | 18 bytes in 1 block(s); first project frame `src/nbn/core/private/Application.cpp:236` | [memcheck-728340.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728340.log) |
| `test_ApplicationSegfault` | 18 bytes in 1 block(s); first project frame `src/nbn/core/private/Application.cpp:236` | [memcheck-728340.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728340.log) |
| `test_ApplicationSegfault` | 18 bytes in 1 block(s); first project frame `src/nbn/core/utils.cpp:86` | [memcheck-728340.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728340.log) |
| `test_ApplicationSegfault` | 18 bytes in 1 block(s) | [memcheck-728340.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728340.log) |
| `test_ApplicationSegfault` | 24 bytes in 1 block(s); first project frame `src/nbn/core/private/Application.cpp:236` | [memcheck-728340.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728340.log) |
| `test_ApplicationSegfault` | 31 bytes in 1 block(s); first project frame `src/nbn/core/Color.cpp:107` | [memcheck-728340.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728340.log) |
| `test_ApplicationSegfault` | 31 bytes in 1 block(s); first project frame `src/nbn/core/Color.cpp:107` | [memcheck-728340.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728340.log) |
| `test_ApplicationSegfault` | 37 bytes in 1 block(s); first project frame `src/nbn/core/utils.cpp:146` | [memcheck-728340.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728340.log) |
| `test_ApplicationSegfault` | 40 bytes in 1 block(s); first project frame `src/nbn/core/detail/ObjectFeature.cpp:9` | [memcheck-728340.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728340.log) |
| `test_ApplicationSegfault` | 64 bytes in 1 block(s); first project frame `src/nbn/core/private/Object.cpp:49` | [memcheck-728340.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728340.log) |
| `test_ApplicationSegfault` | 80 bytes in 1 block(s); first project frame `src/nbn/core/Color.cpp:107` | [memcheck-728340.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728340.log) |
| `test_ApplicationSegfault` | 80 bytes in 1 block(s); first project frame `src/nbn/core/ElapsedTimer.cpp:9` | [memcheck-728340.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728340.log) |
| `test_ApplicationSegfault` | 80 bytes in 1 block(s); first project frame `src/nbn/core/Logger.cpp:66` | [memcheck-728340.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728340.log) |
| `test_ApplicationSegfault` | 80 bytes in 1 block(s); first project frame `src/nbn/core/Object.cpp:9` | [memcheck-728340.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728340.log) |
| `test_ApplicationSegfault` | 80 bytes in 1 block(s); first project frame `src/nbn/core/Task.cpp:11` | [memcheck-728340.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728340.log) |
| `test_ApplicationSegfault` | 80 bytes in 1 block(s); first project frame `src/nbn/core/Timer.cpp:35` | [memcheck-728340.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728340.log) |
| `test_ApplicationSegfault` | 80 bytes in 1 block(s); first project frame `src/nbn/core/private/Thread.cpp:33` | [memcheck-728340.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728340.log) |
| `test_ApplicationSegfault` | 104 bytes in 1 block(s); first project frame `src/nbn/core/private/Application.cpp:236` | [memcheck-728340.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728340.log) |
| `test_ApplicationSegfault` | 104 bytes in 1 block(s); first project frame `src/nbn/core/private/Object.cpp:49` | [memcheck-728340.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728340.log) |
| `test_ApplicationSegfault` | 120 bytes in 1 block(s); first project frame `src/nbn/core/detail/Object.h:12` | [memcheck-728340.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728340.log) |
| `test_ApplicationSegfault` | 304 bytes in 1 block(s); first project frame `src/nbn/core/Color.cpp:107` | [memcheck-728340.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-728340.log) |

### Warnings

No Valgrind warnings were found.
