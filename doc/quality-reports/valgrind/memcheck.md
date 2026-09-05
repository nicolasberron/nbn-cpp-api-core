# Memcheck

Memcheck is **OK** when there are no invalid-access errors and no definite or indirect leaks. `still reachable` memory is reported as informational.


## ✅ Clean

| Test | Errors | Invalid accesses | Still reachable | Report |
|---|---:|---:|---:|---|
| `test_Application` | 0 | 0 | 0 B | [memcheck-3468902.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3468902.log) |
| `test_ApplicationSignalInterrupt` | 0 | 0 | 0 B | [memcheck-3469320.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469320.log) |
| `test_ApplicationTerminateHandler` | 0 | 0 | 0 B | [memcheck-3469401.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469401.log) |
| `test_ApplicationThreadsManager` | 0 | 0 | 0 B | [memcheck-3469489.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469489.log) |
| `test_ApplicationUncaughtException` | 1 | 0 | 2.3 KiB | [memcheck-3469651.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469651.log) |
| `test_Color` | 0 | 0 | 0 B | [memcheck-3469913.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469913.log) |
| `test_CommandLineParser` | 0 | 0 | 0 B | [memcheck-3470032.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3470032.log) |
| `test_ConcurrentStack` | 0 | 0 | 0 B | [memcheck-3470131.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3470131.log) |
| `test_ElapsedTimer` | 0 | 0 | 0 B | [memcheck-3470798.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3470798.log) |
| `test_FilesystemWatcher` | 0 | 0 | 0 B | [memcheck-3470894.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3470894.log) |
| `test_FiniteStateMachine` | 0 | 0 | 0 B | [memcheck-3471025.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3471025.log) |
| `test_JsonMigratorBase` | 0 | 0 | 0 B | [memcheck-3471479.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3471479.log) |
| `test_Object` | 0 | 0 | 0 B | [memcheck-3471602.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3471602.log) |
| `test_ObjectSync` | 0 | 0 | 0 B | [memcheck-3472011.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3472011.log) |
| `test_Serialization` | 0 | 0 | 0 B | [memcheck-3472109.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3472109.log) |
| `test_Signal` | 0 | 0 | 0 B | [memcheck-3472927.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3472927.log) |
| `test_Slots` | 0 | 0 | 0 B | [memcheck-3473080.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3473080.log) |
| `test_Task` | 0 | 0 | 0 B | [memcheck-3473207.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3473207.log) |
| `test_Thread` | 0 | 0 | 0 B | [memcheck-3473525.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3473525.log) |
| `test_Timer` | 0 | 0 | 0 B | [memcheck-3473663.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3473663.log) |
| `test_UnitTests` | 0 | 0 | 0 B | [memcheck-3474041.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474041.log) |
| `test_utils` | 0 | 0 | 0 B | [memcheck-3474380.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474380.log) |
| `test_CommandLineParserHelp` | 0 | 0 | 1.3 KiB | [memcheck-3474712.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474712.log) |
| `test_ApplicationSegfault` | 0 | 0 | 1.5 KiB | [memcheck-3474777.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474777.log) |
| `test_ApplicationTerminateProcess` | 0 | 0 | 0 B | [memcheck-3474994.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474994.log) |
| `test_ApplicationTerminateProcess` | 0 | 0 | 0 B | [memcheck-3475040.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3475040.log) |
| `test_ApplicationTerminateProcess` | 0 | 0 | 0 B | [memcheck-3475042.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3475042.log) |

## ℹ️ Informational allocations and Valgrind warnings

Still-reachable memory is not a lost allocation, but each record is listed so its lifetime can be reviewed. Warnings describe instrumentation or platform behavior and should not be silently discarded.

### Still reachable

| Test | Allocation | Report |
|---|---|---|
| `test_ApplicationUncaughtException` | 29 bytes in 1 block(s); first project frame `src/nbn/core/utils.cpp:86` | [memcheck-3469651.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469651.log) |
| `test_ApplicationUncaughtException` | 32 bytes in 1 block(s); first project frame `src/nbn/core/../../nbn/core/detail/Signal.h:59` | [memcheck-3469651.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469651.log) |
| `test_ApplicationUncaughtException` | 32 bytes in 1 block(s); first project frame `src/nbn/core/detail/Signal.h:118` | [memcheck-3469651.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469651.log) |
| `test_ApplicationUncaughtException` | 37 bytes in 1 block(s); first project frame `src/nbn/core/utils.cpp:146` | [memcheck-3469651.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469651.log) |
| `test_ApplicationUncaughtException` | 37 bytes in 1 block(s); first project frame `src/nbn/core/utils.cpp:146` | [memcheck-3469651.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469651.log) |
| `test_ApplicationUncaughtException` | 40 bytes in 1 block(s); first project frame `src/nbn/core/detail/ObjectFeature.cpp:9` | [memcheck-3469651.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469651.log) |
| `test_ApplicationUncaughtException` | 40 bytes in 1 block(s); first project frame `src/nbn/core/detail/ObjectFeature.cpp:9` | [memcheck-3469651.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469651.log) |
| `test_ApplicationUncaughtException` | 40 bytes in 1 block(s); first project frame `src/nbn/core/detail/ObjectFeature.cpp:9` | [memcheck-3469651.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469651.log) |
| `test_ApplicationUncaughtException` | 48 bytes in 1 block(s); first project frame `tests/nbn/core/ApplicationUncaughtException.cpp:12` | [memcheck-3469651.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469651.log) |
| `test_ApplicationUncaughtException` | 60 bytes in 1 block(s); first project frame `src/nbn/core/UnitTests.cpp:51` | [memcheck-3469651.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469651.log) |
| `test_ApplicationUncaughtException` | 60 bytes in 1 block(s); first project frame `src/nbn/core/UnitTests.cpp:48` | [memcheck-3469651.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469651.log) |
| `test_ApplicationUncaughtException` | 64 bytes in 1 block(s); first project frame `src/nbn/core/private/Object.cpp:49` | [memcheck-3469651.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469651.log) |
| `test_ApplicationUncaughtException` | 64 bytes in 1 block(s) | [memcheck-3469651.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469651.log) |
| `test_ApplicationUncaughtException` | 64 bytes in 1 block(s); first project frame `src/nbn/core/UnitTests.cpp:17` | [memcheck-3469651.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469651.log) |
| `test_ApplicationUncaughtException` | 64 bytes in 1 block(s); first project frame `src/nbn/core/private/Object.cpp:49` | [memcheck-3469651.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469651.log) |
| `test_ApplicationUncaughtException` | 92 bytes in 2 block(s); first project frame `src/nbn/core/utils.cpp:78` | [memcheck-3469651.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469651.log) |
| `test_ApplicationUncaughtException` | 96 bytes in 1 block(s); first project frame `src/nbn/core/UnitTests.cpp:62` | [memcheck-3469651.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469651.log) |
| `test_ApplicationUncaughtException` | 104 bytes in 1 block(s); first project frame `src/nbn/core/private/Object.cpp:49` | [memcheck-3469651.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469651.log) |
| `test_ApplicationUncaughtException` | 112 bytes in 1 block(s); first project frame `src/nbn/core/private/ApplicationThreadsManager.cpp:142` | [memcheck-3469651.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469651.log) |
| `test_ApplicationUncaughtException` | 112 bytes in 1 block(s); first project frame `tests/nbn/core/ApplicationUncaughtException.cpp:18` | [memcheck-3469651.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469651.log) |
| `test_ApplicationUncaughtException` | 120 bytes in 1 block(s); first project frame `src/nbn/core/detail/Object.h:12` | [memcheck-3469651.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469651.log) |
| `test_ApplicationUncaughtException` | 120 bytes in 1 block(s); first project frame `src/nbn/core/detail/Object.h:12` | [memcheck-3469651.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469651.log) |
| `test_ApplicationUncaughtException` | 120 bytes in 1 block(s); first project frame `src/nbn/core/detail/Object.h:12` | [memcheck-3469651.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469651.log) |
| `test_ApplicationUncaughtException` | 208 bytes in 1 block(s); first project frame `src/nbn/core/Task.cpp:15` | [memcheck-3469651.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469651.log) |
| `test_ApplicationUncaughtException` | 304 bytes in 1 block(s); first project frame `src/nbn/core/Object.cpp:11` | [memcheck-3469651.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469651.log) |
| `test_ApplicationUncaughtException` | 304 bytes in 1 block(s); first project frame `src/nbn/core/Object.cpp:11` | [memcheck-3469651.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3469651.log) |
| `test_CommandLineParserHelp` | 24 bytes in 1 block(s) | [memcheck-3474712.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474712.log) |
| `test_CommandLineParserHelp` | 25 bytes in 1 block(s) | [memcheck-3474712.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474712.log) |
| `test_CommandLineParserHelp` | 25 bytes in 1 block(s) | [memcheck-3474712.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474712.log) |
| `test_CommandLineParserHelp` | 32 bytes in 1 block(s); first project frame `src/nbn/core/CommandLineParser.cpp:17` | [memcheck-3474712.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474712.log) |
| `test_CommandLineParserHelp` | 37 bytes in 1 block(s); first project frame `src/nbn/core/utils.cpp:146` | [memcheck-3474712.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474712.log) |
| `test_CommandLineParserHelp` | 64 bytes in 1 block(s); first project frame `tests/nbn/core/CommandLineParserHelp.cpp:7` | [memcheck-3474712.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474712.log) |
| `test_CommandLineParserHelp` | 64 bytes in 1 block(s); first project frame `src/nbn/core/CommandLineParser.cpp:16` | [memcheck-3474712.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474712.log) |
| `test_CommandLineParserHelp` | 80 bytes in 1 block(s); first project frame `src/nbn/core/CommandLineParser.cpp:173` | [memcheck-3474712.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474712.log) |
| `test_CommandLineParserHelp` | 80 bytes in 1 block(s); first project frame `src/nbn/core/CommandLineParser.cpp:174` | [memcheck-3474712.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474712.log) |
| `test_CommandLineParserHelp` | 104 bytes in 1 block(s); first project frame `src/nbn/core/CommandLineParser.cpp:173` | [memcheck-3474712.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474712.log) |
| `test_CommandLineParserHelp` | 104 bytes in 1 block(s); first project frame `src/nbn/core/CommandLineParser.cpp:174` | [memcheck-3474712.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474712.log) |
| `test_CommandLineParserHelp` | 304 bytes in 1 block(s); first project frame `src/nbn/core/Object.cpp:11` | [memcheck-3474712.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474712.log) |
| `test_CommandLineParserHelp` | 352 bytes in 1 block(s); first project frame `src/nbn/core/CommandLineParser.cpp:265` | [memcheck-3474712.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474712.log) |
| `test_ApplicationSegfault` | 17 bytes in 1 block(s); first project frame `src/nbn/core/private/Application.cpp:236` | [memcheck-3474777.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474777.log) |
| `test_ApplicationSegfault` | 17 bytes in 1 block(s); first project frame `src/nbn/core/private/Application.cpp:236` | [memcheck-3474777.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474777.log) |
| `test_ApplicationSegfault` | 18 bytes in 1 block(s); first project frame `src/nbn/core/private/Application.cpp:236` | [memcheck-3474777.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474777.log) |
| `test_ApplicationSegfault` | 18 bytes in 1 block(s); first project frame `src/nbn/core/private/Application.cpp:236` | [memcheck-3474777.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474777.log) |
| `test_ApplicationSegfault` | 18 bytes in 1 block(s); first project frame `src/nbn/core/private/Application.cpp:236` | [memcheck-3474777.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474777.log) |
| `test_ApplicationSegfault` | 18 bytes in 1 block(s); first project frame `src/nbn/core/utils.cpp:86` | [memcheck-3474777.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474777.log) |
| `test_ApplicationSegfault` | 18 bytes in 1 block(s) | [memcheck-3474777.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474777.log) |
| `test_ApplicationSegfault` | 24 bytes in 1 block(s); first project frame `src/nbn/core/private/Application.cpp:236` | [memcheck-3474777.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474777.log) |
| `test_ApplicationSegfault` | 31 bytes in 1 block(s); first project frame `src/nbn/core/Color.cpp:107` | [memcheck-3474777.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474777.log) |
| `test_ApplicationSegfault` | 31 bytes in 1 block(s); first project frame `src/nbn/core/Color.cpp:107` | [memcheck-3474777.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474777.log) |
| `test_ApplicationSegfault` | 37 bytes in 1 block(s); first project frame `src/nbn/core/utils.cpp:146` | [memcheck-3474777.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474777.log) |
| `test_ApplicationSegfault` | 40 bytes in 1 block(s); first project frame `src/nbn/core/detail/ObjectFeature.cpp:9` | [memcheck-3474777.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474777.log) |
| `test_ApplicationSegfault` | 64 bytes in 1 block(s); first project frame `src/nbn/core/private/Object.cpp:49` | [memcheck-3474777.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474777.log) |
| `test_ApplicationSegfault` | 80 bytes in 1 block(s); first project frame `src/nbn/core/Color.cpp:107` | [memcheck-3474777.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474777.log) |
| `test_ApplicationSegfault` | 80 bytes in 1 block(s); first project frame `src/nbn/core/ElapsedTimer.cpp:9` | [memcheck-3474777.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474777.log) |
| `test_ApplicationSegfault` | 80 bytes in 1 block(s); first project frame `src/nbn/core/Logger.cpp:66` | [memcheck-3474777.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474777.log) |
| `test_ApplicationSegfault` | 80 bytes in 1 block(s); first project frame `src/nbn/core/Object.cpp:9` | [memcheck-3474777.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474777.log) |
| `test_ApplicationSegfault` | 80 bytes in 1 block(s); first project frame `src/nbn/core/Task.cpp:11` | [memcheck-3474777.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474777.log) |
| `test_ApplicationSegfault` | 80 bytes in 1 block(s); first project frame `src/nbn/core/Timer.cpp:35` | [memcheck-3474777.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474777.log) |
| `test_ApplicationSegfault` | 80 bytes in 1 block(s); first project frame `src/nbn/core/private/Thread.cpp:33` | [memcheck-3474777.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474777.log) |
| `test_ApplicationSegfault` | 104 bytes in 1 block(s); first project frame `src/nbn/core/private/Application.cpp:236` | [memcheck-3474777.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474777.log) |
| `test_ApplicationSegfault` | 104 bytes in 1 block(s); first project frame `src/nbn/core/private/Object.cpp:49` | [memcheck-3474777.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474777.log) |
| `test_ApplicationSegfault` | 120 bytes in 1 block(s); first project frame `src/nbn/core/detail/Object.h:12` | [memcheck-3474777.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474777.log) |
| `test_ApplicationSegfault` | 304 bytes in 1 block(s); first project frame `src/nbn/core/Color.cpp:107` | [memcheck-3474777.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-3474777.log) |

### Warnings

No Valgrind warnings were found.
