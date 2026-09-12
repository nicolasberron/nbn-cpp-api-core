# Callgrind

Callgrind instruction counts are useful for locating hot functions. They are not wall-clock timings, and dynamic-loader/startup symbols should not be optimized as application code.

## ✅ Profiles collected

## `test_Application`

- Report: [callgrind-500327.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-500327.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Application (PID 500327, part 1)`
- Program instructions: **849,132,876**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 840,861,853 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 1,668,641,600 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 840,979,998 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 840,885,068 | `???:0x00000000000e0e50` |
| 840,873,033 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 840,871,843 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 840,870,411 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 840,864,093 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 840,862,833 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_ApplicationSignalInterrupt`

- Report: [callgrind-500441.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-500441.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ApplicationSignalInterrupt (PID 500441, part 1)`
- Program instructions: **8,187,998**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,359,328 | `???:0x000000000001d2c0` |
| 5,071,986 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 3,275,294 | `./elf/./elf/rtld.c:_dl_start` |
| 3,274,553 | `./elf/../sysdeps/unix/sysv/linux/dl-sysdep.c:_dl_sysdep_start` |
| 3,243,774 | `./elf/./elf/rtld.c:dl_main` |
| 3,180,244 | `???:(below main)` |
| 3,180,233 | `./csu/../csu/libc-start.c:__libc_start_main@@GLIBC_2.34` |
| 3,180,066 | `./csu/../sysdeps/nptl/libc_start_call_main.h:(below main)` |
| 3,127,658 | `./elf/./elf/dl-reloc.c:_dl_relocate_object` |
| 3,127,639 | `./elf/./elf/dl-reloc.c:_dl_relocate_object_no_relro` |

</details>

## `test_ApplicationTerminateHandler`

- Report: [callgrind-500467.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-500467.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ApplicationTerminateHandler (PID 500467, part 1)`
- Program instructions: **8,215,406**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,496,678 | `???:0x000000000001d2c0` |
| 5,094,471 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 3,268,130 | `./elf/./elf/rtld.c:_dl_start` |
| 3,267,389 | `./elf/../sysdeps/unix/sysv/linux/dl-sysdep.c:_dl_sysdep_start` |
| 3,236,610 | `./elf/./elf/rtld.c:dl_main` |
| 3,184,538 | `???:(below main)` |
| 3,184,527 | `./csu/../csu/libc-start.c:__libc_start_main@@GLIBC_2.34` |
| 3,184,360 | `./csu/../sysdeps/nptl/libc_start_call_main.h:(below main)` |
| 3,123,453 | `./elf/./elf/dl-reloc.c:_dl_relocate_object` |
| 3,123,434 | `./elf/./elf/dl-reloc.c:_dl_relocate_object_no_relro` |

</details>

## `test_ApplicationThreadsManager`

- Report: [callgrind-500471.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-500471.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ApplicationThreadsManager (PID 500471, part 1)`
- Program instructions: **26,975,357**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 19,557,413 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 28,465,428 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 19,844,772 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 19,600,383 | `???:0x00000000000e0e50` |
| 19,572,643 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 19,570,958 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 19,569,076 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 19,560,553 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 19,558,843 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_ApplicationUncaughtException`

- Report: [callgrind-500565.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-500565.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ApplicationUncaughtException (PID 500565, part 1)`
- Program instructions: **704,857,305**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 696,703,752 | [src/nbn/core/Application.cpp:nbn::core::Application::exec()](../../../../nbn-main-vscode-workspace/src/nbn/core/Application.cpp) |
| 696,702,923 | [src/nbn/core/private/Application.cpp:nbn::core::private_ns::Application::exec()](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Application.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 758,777,057 | `???:elf_zlib_inflate.cold'2` |
| 703,245,996 | `???:0x000000000001d2c0` |
| 698,936,177 | `???:(below main)` |
| 698,936,166 | `./csu/../csu/libc-start.c:__libc_start_main@@GLIBC_2.34` |
| 698,935,999 | `./csu/../sysdeps/nptl/libc_start_call_main.h:(below main)` |
| 698,935,949 | `tests/nbn/core/main_test_ApplicationUncaughtException.cpp:main` |

</details>

## `test_Color`

- Report: [callgrind-500639.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-500639.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Color (PID 500639, part 1)`
- Program instructions: **18,309,392**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 10,991,950 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 20,348,555 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 11,356,018 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 11,040,542 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 11,008,622 | `???:0x00000000000e0e50` |
| 11,001,822 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 11,000,762 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 10,999,480 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 10,993,890 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 10,992,780 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_CommandLineParser`

- Report: [callgrind-500670.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-500670.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_CommandLineParser (PID 500670, part 1)`
- Program instructions: **10,955,486**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,235,713 | `???:0x000000000001d2c0` |
| 5,741,572 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 5,727,312 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 4,034,884 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 3,719,752 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 3,711,763 | `./elf/../sysdeps/x86_64/dl-trampoline.h:_dl_runtime_resolve_xsave` |
| 3,699,735 | `???:0x00000000000e0e50` |
| 3,692,935 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 3,691,910 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 3,690,628 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |

</details>

## `test_ConcurrentStack`

- Report: [callgrind-500677.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-500677.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ConcurrentStack (PID 500677, part 1)`
- Program instructions: **520,644,303**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 157,004,639 | [src/nbn/core/../../nbn/core/ConcurrentStack.h:nbn::core::ConcurrentStack<int>::push(int)](../../../../nbn-main-vscode-workspace/src/nbn/core/ConcurrentStack.h) |
| 148,709,366 | [src/nbn/core/../../nbn/core/ConcurrentStack.h:nbn::core::ConcurrentStack<int>::pop(int&)](../../../../nbn-main-vscode-workspace/src/nbn/core/ConcurrentStack.h) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 513,367,486 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 513,234,290 | `???:0x00000000000e0e50` |
| 505,626,387 | `./nptl/./nptl/pthread_create.c:start_thread'2` |
| 267,983,839 | `./misc/../sysdeps/unix/sysv/linux/x86_64/clone.S:start_thread` |
| 237,642,828 | `./misc/../sysdeps/unix/sysv/linux/x86_64/clone.S:start_thread'2` |
| 102,656,920 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/shared_ptr_base.h:std::__shared_count<(__gnu_cxx::_Lock_policy)2>::~__shared_count()` |
| 97,082,851 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<(anonymous namespace)::runStackStress<std::unique_ptr<int, std::default_delete<int> >, test_concurrent_stack_high_contention_preserves_unique_move_only_values()::$_0, test_concurrent_stack_high_contention_preserves_unique_move_only_values()::$_1>(test_concurrent_stack_high_contention_preserves_unique_move_only_values()::$_0, test_concurrent_stack_high_contention_preserves_unique_move_only_values()::$_1)::{lambda()#2}> > >::_M_run()` |

</details>

## `test_ElapsedTimer`

- Report: [callgrind-500818.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-500818.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ElapsedTimer (PID 500818, part 1)`
- Program instructions: **8,568,278**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,197,613 | `???:0x000000000001d2c0` |
| 5,009,836 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 3,273,187 | `./elf/./elf/rtld.c:_dl_start` |
| 3,272,446 | `./elf/../sysdeps/unix/sysv/linux/dl-sysdep.c:_dl_sysdep_start` |
| 3,241,667 | `./elf/./elf/rtld.c:dl_main` |
| 3,126,861 | `./elf/./elf/dl-reloc.c:_dl_relocate_object` |
| 3,126,842 | `./elf/./elf/dl-reloc.c:_dl_relocate_object_no_relro` |
| 3,016,976 | `???:(below main)` |
| 3,016,965 | `./csu/../csu/libc-start.c:__libc_start_main@@GLIBC_2.34` |
| 3,016,798 | `./csu/../sysdeps/nptl/libc_start_call_main.h:(below main)` |

</details>

## `test_FilesystemWatcher`

- Report: [callgrind-500840.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-500840.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_FilesystemWatcher (PID 500840, part 1)`
- Program instructions: **16,969,799**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 9,876,064 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 12,045,416 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 9,948,851 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 9,895,767 | `???:0x00000000000e0e50` |
| 9,892,724 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 9,886,524 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 9,885,422 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 9,884,070 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 9,878,144 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 9,876,964 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_FiniteStateMachine`

- Report: [callgrind-500886.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-500886.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_FiniteStateMachine (PID 500886, part 1)`
- Program instructions: **3,086,525,737**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 3,079,338,163 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Thread.cpp) |
| 3,079,181,057 | [src/nbn/core/Task.cpp:nbn::core::Task::run()](../../../../nbn-main-vscode-workspace/src/nbn/core/Task.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 3,079,402,721 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 3,079,356,591 | `???:0x00000000000e0e50` |
| 3,079,348,395 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 3,079,347,291 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 3,079,345,969 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 3,079,340,183 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 3,079,339,033 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_JsonMigratorBase`

- Report: [callgrind-501358.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-501358.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_JsonMigratorBase (PID 501358, part 1)`
- Program instructions: **10,483,307**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,313,974 | `???:0x000000000001d2c0` |
| 5,526,339 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 4,887,139 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 3,499,531 | `./elf/../sysdeps/x86_64/dl-trampoline.h:_dl_runtime_resolve_xsave` |
| 3,483,640 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 3,382,651 | `./elf/./elf/dl-runtime.c:_dl_fixup` |
| 3,273,710 | `./elf/./elf/rtld.c:_dl_start` |
| 3,272,969 | `./elf/../sysdeps/unix/sysv/linux/dl-sysdep.c:_dl_sysdep_start` |
| 3,242,190 | `./elf/./elf/rtld.c:dl_main` |
| 3,169,312 | `./nptl/./nptl/pthread_create.c:start_thread` |

</details>

## `test_Object`

- Report: [callgrind-501389.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-501389.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Object (PID 501389, part 1)`
- Program instructions: **1,043,503,657**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 1,036,087,562 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 2,069,576,269 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 1,036,445,111 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 1,036,122,777 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 1,036,104,235 | `???:0x00000000000e0e50` |
| 1,036,097,435 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 1,036,096,410 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 1,036,095,128 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 1,036,089,545 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 1,036,088,435 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_ObjectSync`

- Report: [callgrind-501488.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-501488.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ObjectSync (PID 501488, part 1)`
- Program instructions: **11,957,922**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,350,389 | `???:0x000000000001d2c0` |
| 7,138,543 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 5,197,953 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 4,941,615 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 4,607,512 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 4,589,633 | `???:0x00000000000e0e50` |
| 4,582,833 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 4,581,773 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 4,580,491 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 4,574,901 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_Serialization`

- Report: [callgrind-501519.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-501519.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Serialization (PID 501519, part 1)`
- Program instructions: **1,831,317,494**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 3,892,613,533 | [src/nbn/core/detail/serialization.cpp:nbn::core::serialization::json::skipValue(std::basic_string_view<char, std::char_traits<char> >, unsigned long&, unsigned long)'2](../../../../nbn-main-vscode-workspace/src/nbn/core/detail/serialization.cpp) |
| 3,877,610,685 | [src/nbn/core/detail/serialization.cpp:nbn::core::serialization::json::skipArray(std::basic_string_view<char, std::char_traits<char> >, unsigned long&, unsigned long)'2](../../../../nbn-main-vscode-workspace/src/nbn/core/detail/serialization.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 4,762,683,415 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 1,823,934,852 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 1,823,621,865 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 1,823,597,089 | `???:0x00000000000e0e50` |
| 1,823,590,289 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 1,823,589,264 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 1,823,587,982 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 1,823,582,399 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_Signal`

- Report: [callgrind-501761.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-501761.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Signal (PID 501761, part 1)`
- Program instructions: **11,200,038**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,079,768 | `???:0x000000000001d2c0` |
| 5,694,647 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 4,951,257 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 4,155,625 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 4,120,172 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 4,076,382 | `???:0x00000000000e0e50` |
| 4,017,637 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 4,016,612 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 4,015,330 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 4,009,681 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_Slots`

- Report: [callgrind-501809.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-501809.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Slots (PID 501809, part 1)`
- Program instructions: **20,641,543**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 13,368,261 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 18,806,837 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 13,554,573 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 13,407,971 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 13,385,330 | `???:0x00000000000e0e50` |
| 13,378,181 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 13,377,145 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 13,375,853 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 13,370,221 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 13,369,101 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_Task`

- Report: [callgrind-501844.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-501844.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Task (PID 501844, part 1)`
- Program instructions: **829,250,283**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 1,641,639,473 | [src/nbn/core/Task.cpp:nbn::core::Task::run()'2](../../../../nbn-main-vscode-workspace/src/nbn/core/Task.cpp) |
| 1,641,638,990 | [src/nbn/core/private/Task.h:nbn::core::Task::Impl::run()'2](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Task.h) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 2,462,054,483 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 822,393,061 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 822,062,520 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 822,045,450 | `???:0x00000000000e0e50` |
| 822,038,650 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 822,037,625 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |

</details>

## `test_Thread`

- Report: [callgrind-501911.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-501911.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Thread (PID 501911, part 1)`
- Program instructions: **16,765,743**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 22,167,290 | `./nptl/./nptl/pthread_create.c:start_thread'2` |
| 20,381,483 | `./misc/../sysdeps/unix/sysv/linux/x86_64/clone.S:start_thread'2` |
| 13,384,833 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 9,616,642 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 9,522,803 | `???:0x00000000000e0e50` |
| 9,456,104 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 9,454,694 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 9,453,062 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 9,445,764 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 9,444,304 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_Timer`

- Report: [callgrind-501973.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-501973.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Timer (PID 501973, part 1)`
- Program instructions: **858,992,689**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 851,782,641 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 2,546,076,914 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 851,848,402 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 851,809,862 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 851,802,775 | `???:0x00000000000e0e50` |
| 851,793,191 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 851,792,078 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 851,790,716 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 851,784,741 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 851,783,551 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_UnitTests`

- Report: [callgrind-502076.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-502076.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_UnitTests (PID 502076, part 1)`
- Program instructions: **909,771,187**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 902,071,268 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 3,588,786,500 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 902,419,857 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 902,109,216 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 902,087,933 | `???:0x00000000000e0e50` |
| 902,081,133 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 902,080,108 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 902,078,826 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 902,073,208 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 902,072,098 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_utils`

- Report: [callgrind-502180.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-502180.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_utils (PID 502180, part 1)`
- Program instructions: **864,539,596**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 857,233,215 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 2,568,708,584 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 857,504,451 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 857,263,232 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 857,249,845 | `???:0x00000000000e0e50` |
| 857,243,045 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 857,242,020 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 857,240,738 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 857,235,155 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 857,234,045 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_CommandLineParserHelp`

- Report: [callgrind-502264.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-502264.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_CommandLineParserHelp (PID 502264, part 1)`
- Program instructions: **5,424,175**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 5,424,175 | `???:0x000000000001d2c0` |
| 4,049,985 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 3,257,401 | `./elf/./elf/rtld.c:_dl_start` |
| 3,256,660 | `./elf/../sysdeps/unix/sysv/linux/dl-sysdep.c:_dl_sysdep_start` |
| 3,225,881 | `./elf/./elf/rtld.c:dl_main` |
| 3,114,328 | `./elf/./elf/dl-reloc.c:_dl_relocate_object` |
| 3,114,309 | `./elf/./elf/dl-reloc.c:_dl_relocate_object_no_relro` |
| 2,081,086 | `./elf/../sysdeps/generic/dl-new-hash.h:_dl_lookup_symbol_x` |
| 1,807,024 | `./elf/../sysdeps/x86_64/dl-trampoline.h:_dl_runtime_resolve_xsave` |
| 1,750,944 | `./elf/./elf/dl-runtime.c:_dl_fixup` |

</details>

## `test_ApplicationFatalBeforeInit`

- Report: [callgrind-502265.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-502265.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ApplicationFatalBeforeInit (PID 502265, part 1)`
- Program instructions: **4,881,142**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 4,881,142 | `???:0x000000000001d2c0` |
| 3,601,978 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 3,261,137 | `./elf/./elf/rtld.c:_dl_start` |
| 3,260,396 | `./elf/../sysdeps/unix/sysv/linux/dl-sysdep.c:_dl_sysdep_start` |
| 3,229,617 | `./elf/./elf/rtld.c:dl_main` |
| 3,119,354 | `./elf/./elf/dl-reloc.c:_dl_relocate_object` |
| 3,119,335 | `./elf/./elf/dl-reloc.c:_dl_relocate_object_no_relro` |
| 1,756,112 | `./elf/../sysdeps/generic/dl-new-hash.h:_dl_lookup_symbol_x` |
| 1,463,792 | `./elf/./elf/dl-lookup.c:do_lookup_x` |
| 1,308,602 | `./elf/../sysdeps/x86_64/dl-trampoline.h:_dl_runtime_resolve_xsave` |

</details>

## `test_ApplicationSegfault`

- Report: [callgrind-502266.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-502266.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ApplicationSegfault (PID 502266, part 1)`
- Program instructions: **4,538,717**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 662,141,911 | [src/nbn/core/private/Application.cpp:nbn::core::private_ns::hardwaretSignalHandler(int)](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Application.cpp) |
| 661,959,224 | [src/nbn/core/private/Application.cpp:nbn::core::private_ns::stackTraceMessage[abi:cxx11](std::basic_string_view<char, std::char_traits<char> >)](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Application.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 758,777,057 | `???:elf_zlib_inflate.cold'2` |
| 661,894,162 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/stacktrace:std::ostream& std::operator<< <std::allocator<std::stacktrace_entry> >(std::ostream&, std::basic_stacktrace<std::allocator<std::stacktrace_entry> > const&)` |
| 661,874,081 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/stacktrace:std::operator<<(std::ostream&, std::stacktrace_entry const&)` |
| 661,857,963 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/stacktrace:std::stacktrace_entry::_M_get_info(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >*, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >*, int*) const` |
| 661,854,830 | `???:std::stacktrace_entry::_Info::_M_populate(unsigned long)` |
| 661,850,462 | `???:__glibcxx_backtrace_pcinfo` |
| 607,856,427 | `???:fileline_initialize` |
| 607,852,614 | `???:__glibcxx_backtrace_initialize` |

</details>

## `test_ApplicationTerminateProcess`

- Report: [callgrind-502330.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-502330.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ApplicationTerminateProcess (PID 502330, part 1)`
- Program instructions: **4,816,348**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 4,816,348 | `???:0x000000000001d2c0` |
| 3,567,412 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 3,264,439 | `./elf/./elf/rtld.c:_dl_start` |
| 3,263,698 | `./elf/../sysdeps/unix/sysv/linux/dl-sysdep.c:_dl_sysdep_start` |
| 3,232,919 | `./elf/./elf/rtld.c:dl_main` |
| 3,120,847 | `./elf/./elf/dl-reloc.c:_dl_relocate_object` |
| 3,120,828 | `./elf/./elf/dl-reloc.c:_dl_relocate_object_no_relro` |
| 1,741,038 | `./elf/../sysdeps/generic/dl-new-hash.h:_dl_lookup_symbol_x` |
| 1,448,533 | `./elf/./elf/dl-lookup.c:do_lookup_x` |
| 1,266,617 | `./elf/../sysdeps/x86_64/dl-trampoline.h:_dl_runtime_resolve_xsave` |

</details>
