# Memcheck

Memcheck is **OK** when there are no invalid-access errors and no definite or indirect leaks. `still reachable` memory is reported as informational.


## ✅ Clean

| Test | Errors | Invalid accesses | Still reachable | Report |
|---|---:|---:|---:|---|
| `test_Application` | 0 | 0 | 0 B | [memcheck-502349.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502349.log) |
| `test_ApplicationSignalInterrupt` | 0 | 0 | 0 B | [memcheck-502500.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502500.log) |
| `test_ApplicationTerminateHandler` | 0 | 0 | 0 B | [memcheck-502531.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502531.log) |
| `test_ApplicationThreadsManager` | 0 | 0 | 0 B | [memcheck-502571.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502571.log) |
| `test_ApplicationUncaughtException` | 1 | 0 | 2.3 KiB | [memcheck-502680.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502680.log) |
| `test_Color` | 0 | 0 | 0 B | [memcheck-502797.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502797.log) |
| `test_CommandLineParser` | 0 | 0 | 0 B | [memcheck-502850.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502850.log) |
| `test_ConcurrentStack` | 0 | 0 | 0 B | [memcheck-502916.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502916.log) |
| `test_ElapsedTimer` | 0 | 0 | 0 B | [memcheck-503217.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-503217.log) |
| `test_FilesystemWatcher` | 0 | 0 | 0 B | [memcheck-503244.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-503244.log) |
| `test_FiniteStateMachine` | 0 | 0 | 0 B | [memcheck-503306.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-503306.log) |
| `test_JsonMigratorBase` | 0 | 0 | 0 B | [memcheck-503464.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-503464.log) |
| `test_Object` | 0 | 0 | 0 B | [memcheck-503501.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-503501.log) |
| `test_ObjectSync` | 0 | 0 | 0 B | [memcheck-503647.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-503647.log) |
| `test_Serialization` | 0 | 0 | 0 B | [memcheck-503694.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-503694.log) |
| `test_Signal` | 0 | 0 | 0 B | [memcheck-504037.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504037.log) |
| `test_Slots` | 0 | 0 | 0 B | [memcheck-504102.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504102.log) |
| `test_Task` | 0 | 0 | 0 B | [memcheck-504158.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504158.log) |
| `test_Thread` | 0 | 0 | 0 B | [memcheck-504281.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504281.log) |
| `test_Timer` | 0 | 0 | 0 B | [memcheck-504362.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504362.log) |
| `test_UnitTests` | 0 | 0 | 0 B | [memcheck-504507.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504507.log) |
| `test_utils` | 0 | 0 | 0 B | [memcheck-504622.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504622.log) |
| `test_CommandLineParserHelp` | 0 | 0 | 1.3 KiB | [memcheck-504748.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504748.log) |
| `test_ApplicationFatalBeforeInit` | 0 | 0 | 0 B | [memcheck-504775.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504775.log) |
| `test_ApplicationSegfault` | 0 | 0 | 1.5 KiB | [memcheck-504790.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504790.log) |
| `test_ApplicationTerminateProcess` | 0 | 0 | 0 B | [memcheck-504893.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504893.log) |
| `test_ApplicationTerminateProcess` | 0 | 0 | 0 B | [memcheck-504903.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504903.log) |
| `test_ApplicationTerminateProcess` | 0 | 0 | 0 B | [memcheck-504905.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504905.log) |

## ℹ️ Informational allocations and Valgrind warnings

Still-reachable memory is not a lost allocation, but each record is listed so its lifetime can be reviewed. Warnings describe instrumentation or platform behavior and should not be silently discarded.

### Still reachable

| Test | Allocation | Report |
|---|---|---|
| `test_ApplicationUncaughtException` | 29 bytes in 1 block(s); first project frame `src/nbn/core/utils.cpp:86` | [memcheck-502680.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502680.log) |
| `test_ApplicationUncaughtException` | 32 bytes in 1 block(s); first project frame `src/nbn/core/../../nbn/core/detail/Signal.h:59` | [memcheck-502680.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502680.log) |
| `test_ApplicationUncaughtException` | 32 bytes in 1 block(s); first project frame `src/nbn/core/filesystem/../detail/Signal.h:118` | [memcheck-502680.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502680.log) |
| `test_ApplicationUncaughtException` | 37 bytes in 1 block(s); first project frame `src/nbn/core/utils.cpp:146` | [memcheck-502680.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502680.log) |
| `test_ApplicationUncaughtException` | 37 bytes in 1 block(s); first project frame `src/nbn/core/utils.cpp:146` | [memcheck-502680.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502680.log) |
| `test_ApplicationUncaughtException` | 40 bytes in 1 block(s); first project frame `src/nbn/core/detail/ObjectFeature.cpp:9` | [memcheck-502680.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502680.log) |
| `test_ApplicationUncaughtException` | 40 bytes in 1 block(s); first project frame `src/nbn/core/detail/ObjectFeature.cpp:9` | [memcheck-502680.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502680.log) |
| `test_ApplicationUncaughtException` | 40 bytes in 1 block(s); first project frame `src/nbn/core/detail/ObjectFeature.cpp:9` | [memcheck-502680.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502680.log) |
| `test_ApplicationUncaughtException` | 48 bytes in 1 block(s); first project frame `tests/nbn/core/ApplicationUncaughtException.cpp:12` | [memcheck-502680.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502680.log) |
| `test_ApplicationUncaughtException` | 60 bytes in 1 block(s); first project frame `src/nbn/core/UnitTests.cpp:51` | [memcheck-502680.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502680.log) |
| `test_ApplicationUncaughtException` | 60 bytes in 1 block(s); first project frame `src/nbn/core/UnitTests.cpp:48` | [memcheck-502680.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502680.log) |
| `test_ApplicationUncaughtException` | 64 bytes in 1 block(s); first project frame `src/nbn/core/private/Object.cpp:49` | [memcheck-502680.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502680.log) |
| `test_ApplicationUncaughtException` | 64 bytes in 1 block(s) | [memcheck-502680.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502680.log) |
| `test_ApplicationUncaughtException` | 64 bytes in 1 block(s); first project frame `src/nbn/core/UnitTests.cpp:17` | [memcheck-502680.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502680.log) |
| `test_ApplicationUncaughtException` | 64 bytes in 1 block(s); first project frame `src/nbn/core/private/Object.cpp:49` | [memcheck-502680.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502680.log) |
| `test_ApplicationUncaughtException` | 92 bytes in 2 block(s); first project frame `src/nbn/core/utils.cpp:78` | [memcheck-502680.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502680.log) |
| `test_ApplicationUncaughtException` | 96 bytes in 1 block(s); first project frame `src/nbn/core/UnitTests.cpp:62` | [memcheck-502680.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502680.log) |
| `test_ApplicationUncaughtException` | 104 bytes in 1 block(s); first project frame `src/nbn/core/private/Object.cpp:49` | [memcheck-502680.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502680.log) |
| `test_ApplicationUncaughtException` | 112 bytes in 1 block(s); first project frame `src/nbn/core/private/ApplicationThreadsManager.cpp:142` | [memcheck-502680.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502680.log) |
| `test_ApplicationUncaughtException` | 112 bytes in 1 block(s); first project frame `tests/nbn/core/ApplicationUncaughtException.cpp:18` | [memcheck-502680.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502680.log) |
| `test_ApplicationUncaughtException` | 120 bytes in 1 block(s); first project frame `src/nbn/core/detail/Object.h:12` | [memcheck-502680.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502680.log) |
| `test_ApplicationUncaughtException` | 120 bytes in 1 block(s); first project frame `src/nbn/core/detail/Object.h:12` | [memcheck-502680.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502680.log) |
| `test_ApplicationUncaughtException` | 120 bytes in 1 block(s); first project frame `src/nbn/core/detail/Object.h:12` | [memcheck-502680.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502680.log) |
| `test_ApplicationUncaughtException` | 208 bytes in 1 block(s); first project frame `src/nbn/core/Task.cpp:15` | [memcheck-502680.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502680.log) |
| `test_ApplicationUncaughtException` | 304 bytes in 1 block(s); first project frame `src/nbn/core/Object.cpp:11` | [memcheck-502680.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502680.log) |
| `test_ApplicationUncaughtException` | 304 bytes in 1 block(s); first project frame `src/nbn/core/Object.cpp:11` | [memcheck-502680.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-502680.log) |
| `test_CommandLineParserHelp` | 24 bytes in 1 block(s) | [memcheck-504748.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504748.log) |
| `test_CommandLineParserHelp` | 25 bytes in 1 block(s) | [memcheck-504748.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504748.log) |
| `test_CommandLineParserHelp` | 25 bytes in 1 block(s) | [memcheck-504748.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504748.log) |
| `test_CommandLineParserHelp` | 32 bytes in 1 block(s); first project frame `src/nbn/core/CommandLineParser.cpp:17` | [memcheck-504748.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504748.log) |
| `test_CommandLineParserHelp` | 37 bytes in 1 block(s); first project frame `src/nbn/core/utils.cpp:146` | [memcheck-504748.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504748.log) |
| `test_CommandLineParserHelp` | 64 bytes in 1 block(s); first project frame `tests/nbn/core/CommandLineParserHelp.cpp:7` | [memcheck-504748.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504748.log) |
| `test_CommandLineParserHelp` | 64 bytes in 1 block(s); first project frame `src/nbn/core/CommandLineParser.cpp:16` | [memcheck-504748.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504748.log) |
| `test_CommandLineParserHelp` | 80 bytes in 1 block(s); first project frame `src/nbn/core/CommandLineParser.cpp:171` | [memcheck-504748.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504748.log) |
| `test_CommandLineParserHelp` | 80 bytes in 1 block(s); first project frame `src/nbn/core/CommandLineParser.cpp:172` | [memcheck-504748.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504748.log) |
| `test_CommandLineParserHelp` | 104 bytes in 1 block(s); first project frame `src/nbn/core/CommandLineParser.cpp:171` | [memcheck-504748.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504748.log) |
| `test_CommandLineParserHelp` | 104 bytes in 1 block(s); first project frame `src/nbn/core/CommandLineParser.cpp:172` | [memcheck-504748.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504748.log) |
| `test_CommandLineParserHelp` | 304 bytes in 1 block(s); first project frame `src/nbn/core/Object.cpp:11` | [memcheck-504748.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504748.log) |
| `test_CommandLineParserHelp` | 352 bytes in 1 block(s); first project frame `src/nbn/core/CommandLineParser.cpp:260` | [memcheck-504748.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504748.log) |
| `test_ApplicationSegfault` | 17 bytes in 1 block(s); first project frame `src/nbn/core/private/Application.cpp:207` | [memcheck-504790.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504790.log) |
| `test_ApplicationSegfault` | 17 bytes in 1 block(s); first project frame `src/nbn/core/private/Application.cpp:207` | [memcheck-504790.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504790.log) |
| `test_ApplicationSegfault` | 18 bytes in 1 block(s); first project frame `src/nbn/core/private/Application.cpp:207` | [memcheck-504790.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504790.log) |
| `test_ApplicationSegfault` | 18 bytes in 1 block(s); first project frame `src/nbn/core/private/Application.cpp:207` | [memcheck-504790.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504790.log) |
| `test_ApplicationSegfault` | 18 bytes in 1 block(s); first project frame `src/nbn/core/private/Application.cpp:207` | [memcheck-504790.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504790.log) |
| `test_ApplicationSegfault` | 18 bytes in 1 block(s); first project frame `src/nbn/core/utils.cpp:86` | [memcheck-504790.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504790.log) |
| `test_ApplicationSegfault` | 18 bytes in 1 block(s) | [memcheck-504790.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504790.log) |
| `test_ApplicationSegfault` | 24 bytes in 1 block(s); first project frame `src/nbn/core/private/Application.cpp:207` | [memcheck-504790.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504790.log) |
| `test_ApplicationSegfault` | 31 bytes in 1 block(s); first project frame `src/nbn/core/Color.cpp:109` | [memcheck-504790.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504790.log) |
| `test_ApplicationSegfault` | 31 bytes in 1 block(s); first project frame `src/nbn/core/Color.cpp:109` | [memcheck-504790.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504790.log) |
| `test_ApplicationSegfault` | 37 bytes in 1 block(s); first project frame `src/nbn/core/utils.cpp:146` | [memcheck-504790.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504790.log) |
| `test_ApplicationSegfault` | 40 bytes in 1 block(s); first project frame `src/nbn/core/detail/ObjectFeature.cpp:9` | [memcheck-504790.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504790.log) |
| `test_ApplicationSegfault` | 64 bytes in 1 block(s); first project frame `src/nbn/core/private/Object.cpp:49` | [memcheck-504790.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504790.log) |
| `test_ApplicationSegfault` | 80 bytes in 1 block(s); first project frame `src/nbn/core/Color.cpp:109` | [memcheck-504790.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504790.log) |
| `test_ApplicationSegfault` | 80 bytes in 1 block(s); first project frame `src/nbn/core/ElapsedTimer.cpp:9` | [memcheck-504790.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504790.log) |
| `test_ApplicationSegfault` | 80 bytes in 1 block(s); first project frame `src/nbn/core/Logger.cpp:66` | [memcheck-504790.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504790.log) |
| `test_ApplicationSegfault` | 80 bytes in 1 block(s); first project frame `src/nbn/core/Object.cpp:9` | [memcheck-504790.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504790.log) |
| `test_ApplicationSegfault` | 80 bytes in 1 block(s); first project frame `src/nbn/core/Task.cpp:11` | [memcheck-504790.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504790.log) |
| `test_ApplicationSegfault` | 80 bytes in 1 block(s); first project frame `src/nbn/core/Timer.cpp:35` | [memcheck-504790.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504790.log) |
| `test_ApplicationSegfault` | 80 bytes in 1 block(s); first project frame `src/nbn/core/private/Thread.cpp:33` | [memcheck-504790.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504790.log) |
| `test_ApplicationSegfault` | 104 bytes in 1 block(s); first project frame `src/nbn/core/private/Application.cpp:207` | [memcheck-504790.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504790.log) |
| `test_ApplicationSegfault` | 104 bytes in 1 block(s); first project frame `src/nbn/core/private/Object.cpp:49` | [memcheck-504790.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504790.log) |
| `test_ApplicationSegfault` | 120 bytes in 1 block(s); first project frame `src/nbn/core/detail/Object.h:12` | [memcheck-504790.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504790.log) |
| `test_ApplicationSegfault` | 304 bytes in 1 block(s); first project frame `src/nbn/core/Color.cpp:109` | [memcheck-504790.log](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/memcheck/memcheck-504790.log) |

### Warnings

No Valgrind warnings were found.
