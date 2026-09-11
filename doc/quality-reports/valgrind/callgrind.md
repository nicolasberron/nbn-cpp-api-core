# Callgrind

Callgrind instruction counts are useful for locating hot functions. They are not wall-clock timings, and dynamic-loader/startup symbols should not be optimized as application code.

## ✅ Profiles collected

## `test_Application`

- Report: [callgrind-724070.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-724070.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Application (PID 724070, part 1)`
- Program instructions: **851,423,947**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 843,133,811 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 1,673,418,582 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 843,255,926 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 843,157,026 | `???:0x00000000000e0e50` |
| 843,144,991 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 843,143,801 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 843,142,369 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 843,136,051 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 843,134,791 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_ApplicationSignalInterrupt`

- Report: [callgrind-724181.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-724181.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ApplicationSignalInterrupt (PID 724181, part 1)`
- Program instructions: **8,177,603**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,387,906 | `???:0x000000000001d2c0` |
| 5,071,030 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 3,276,892 | `./elf/./elf/rtld.c:_dl_start` |
| 3,276,151 | `./elf/../sysdeps/unix/sysv/linux/dl-sysdep.c:_dl_sysdep_start` |
| 3,243,181 | `./elf/./elf/rtld.c:dl_main` |
| 3,207,462 | `???:(below main)` |
| 3,207,451 | `./csu/../csu/libc-start.c:__libc_start_main@@GLIBC_2.34` |
| 3,207,284 | `./csu/../sysdeps/nptl/libc_start_call_main.h:(below main)` |
| 3,128,188 | `./elf/./elf/dl-reloc.c:_dl_relocate_object` |
| 3,128,169 | `./elf/./elf/dl-reloc.c:_dl_relocate_object_no_relro` |

</details>

## `test_ApplicationTerminateHandler`

- Report: [callgrind-724214.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-724214.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ApplicationTerminateHandler (PID 724214, part 1)`
- Program instructions: **8,216,328**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,510,192 | `???:0x000000000001d2c0` |
| 5,093,390 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 3,269,873 | `./elf/./elf/rtld.c:_dl_start` |
| 3,269,132 | `./elf/../sysdeps/unix/sysv/linux/dl-sysdep.c:_dl_sysdep_start` |
| 3,236,162 | `./elf/./elf/rtld.c:dl_main` |
| 3,196,739 | `???:(below main)` |
| 3,196,728 | `./csu/../csu/libc-start.c:__libc_start_main@@GLIBC_2.34` |
| 3,196,561 | `./csu/../sysdeps/nptl/libc_start_call_main.h:(below main)` |
| 3,124,128 | `./elf/./elf/dl-reloc.c:_dl_relocate_object` |
| 3,124,109 | `./elf/./elf/dl-reloc.c:_dl_relocate_object_no_relro` |

</details>

## `test_ApplicationThreadsManager`

- Report: [callgrind-724226.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-724226.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ApplicationThreadsManager (PID 724226, part 1)`
- Program instructions: **26,883,913**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 19,394,030 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 28,520,097 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 19,682,485 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 19,437,000 | `???:0x00000000000e0e50` |
| 19,409,260 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 19,407,575 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 19,405,693 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 19,397,170 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 19,395,460 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_ApplicationUncaughtException`

- Report: [callgrind-724313.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-724313.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ApplicationUncaughtException (PID 724313, part 1)`
- Program instructions: **707,398,536**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 699,210,998 | [src/nbn/core/Application.cpp:nbn::core::Application::exec()](../../../../nbn-main-vscode-workspace/src/nbn/core/Application.cpp) |
| 699,210,169 | [src/nbn/core/private/Application.cpp:nbn::core::private_ns::Application::exec()](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Application.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 758,777,057 | `???:elf_zlib_inflate.cold'2` |
| 705,755,736 | `???:0x000000000001d2c0` |
| 701,444,190 | `???:(below main)` |
| 701,444,179 | `./csu/../csu/libc-start.c:__libc_start_main@@GLIBC_2.34` |
| 701,444,012 | `./csu/../sysdeps/nptl/libc_start_call_main.h:(below main)` |
| 701,443,962 | `tests/nbn/core/main_test_ApplicationUncaughtException.cpp:main` |

</details>

## `test_Color`

- Report: [callgrind-724389.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-724389.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Color (PID 724389, part 1)`
- Program instructions: **18,330,696**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 10,973,748 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 20,358,599 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 11,325,701 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 11,018,119 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 10,990,420 | `???:0x00000000000e0e50` |
| 10,983,620 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 10,982,560 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 10,981,278 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 10,975,688 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 10,974,578 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_CommandLineParser`

- Report: [callgrind-724417.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-724417.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_CommandLineParser (PID 724417, part 1)`
- Program instructions: **10,953,684**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,238,871 | `???:0x000000000001d2c0` |
| 5,740,669 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 5,727,834 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 4,030,697 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 3,714,792 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 3,710,524 | `./elf/../sysdeps/x86_64/dl-trampoline.h:_dl_runtime_resolve_xsave` |
| 3,694,775 | `???:0x00000000000e0e50` |
| 3,687,975 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 3,686,950 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 3,685,668 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |

</details>

## `test_ConcurrentStack`

- Report: [callgrind-724424.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-724424.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ConcurrentStack (PID 724424, part 1)`
- Program instructions: **520,252,194**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 156,697,545 | [src/nbn/core/../../nbn/core/ConcurrentStack.h:nbn::core::ConcurrentStack<int>::push(int)](../../../../nbn-main-vscode-workspace/src/nbn/core/ConcurrentStack.h) |
| 146,748,060 | [src/nbn/core/../../nbn/core/ConcurrentStack.h:nbn::core::ConcurrentStack<int>::pop(int&)](../../../../nbn-main-vscode-workspace/src/nbn/core/ConcurrentStack.h) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 512,884,266 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 512,754,842 | `???:0x00000000000e0e50` |
| 510,686,190 | `./nptl/./nptl/pthread_create.c:start_thread'2` |
| 269,036,144 | `./misc/../sysdeps/unix/sysv/linux/x86_64/clone.S:start_thread` |
| 241,650,326 | `./misc/../sysdeps/unix/sysv/linux/x86_64/clone.S:start_thread'2` |
| 102,654,266 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/shared_ptr_base.h:std::__shared_count<(__gnu_cxx::_Lock_policy)2>::~__shared_count()` |
| 96,215,275 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<(anonymous namespace)::runStackStress<std::unique_ptr<int, std::default_delete<int> >, test_concurrent_stack_high_contention_preserves_unique_move_only_values()::$_0, test_concurrent_stack_high_contention_preserves_unique_move_only_values()::$_1>(test_concurrent_stack_high_contention_preserves_unique_move_only_values()::$_0, test_concurrent_stack_high_contention_preserves_unique_move_only_values()::$_1)::{lambda()#2}> > >::_M_run()` |

</details>

## `test_ElapsedTimer`

- Report: [callgrind-724581.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-724581.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ElapsedTimer (PID 724581, part 1)`
- Program instructions: **8,543,804**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,211,628 | `???:0x000000000001d2c0` |
| 5,008,538 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 3,274,779 | `./elf/./elf/rtld.c:_dl_start` |
| 3,274,038 | `./elf/../sysdeps/unix/sysv/linux/dl-sysdep.c:_dl_sysdep_start` |
| 3,241,068 | `./elf/./elf/rtld.c:dl_main` |
| 3,127,385 | `./elf/./elf/dl-reloc.c:_dl_relocate_object` |
| 3,127,366 | `./elf/./elf/dl-reloc.c:_dl_relocate_object_no_relro` |
| 3,029,986 | `???:(below main)` |
| 3,029,975 | `./csu/../csu/libc-start.c:__libc_start_main@@GLIBC_2.34` |
| 3,029,808 | `./csu/../sysdeps/nptl/libc_start_call_main.h:(below main)` |

</details>

## `test_FilesystemWatcher`

- Report: [callgrind-724612.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-724612.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_FilesystemWatcher (PID 724612, part 1)`
- Program instructions: **16,923,250**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 9,899,189 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 12,049,976 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 9,971,822 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 9,918,892 | `???:0x00000000000e0e50` |
| 9,909,649 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 9,908,547 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 9,907,195 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 9,901,269 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 9,900,089 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_FiniteStateMachine`

- Report: [callgrind-724637.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-724637.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_FiniteStateMachine (PID 724637, part 1)`
- Program instructions: **2,574,288,664**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 2,567,103,052 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 2,710,885,356 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 2,567,168,370 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 2,567,121,480 | `???:0x00000000000e0e50` |
| 2,567,113,284 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 2,567,112,180 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 2,567,110,858 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 2,567,105,072 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 2,567,103,922 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_JsonMigratorBase`

- Report: [callgrind-725029.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-725029.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_JsonMigratorBase (PID 725029, part 1)`
- Program instructions: **10,487,425**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,327,623 | `???:0x000000000001d2c0` |
| 5,527,052 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 4,893,637 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 3,500,195 | `./elf/../sysdeps/x86_64/dl-trampoline.h:_dl_runtime_resolve_xsave` |
| 3,486,498 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 3,383,235 | `./elf/./elf/dl-runtime.c:_dl_fixup` |
| 3,275,515 | `./elf/./elf/rtld.c:_dl_start` |
| 3,274,774 | `./elf/../sysdeps/unix/sysv/linux/dl-sysdep.c:_dl_sysdep_start` |
| 3,241,804 | `./elf/./elf/rtld.c:dl_main` |
| 3,167,101 | `???:(below main)` |

</details>

## `test_Object`

- Report: [callgrind-725046.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-725046.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Object (PID 725046, part 1)`
- Program instructions: **1,023,539,860**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 1,016,097,331 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 2,029,679,045 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 1,016,456,492 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 1,016,132,112 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 1,016,114,004 | `???:0x00000000000e0e50` |
| 1,016,107,204 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 1,016,106,179 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 1,016,104,897 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 1,016,099,314 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 1,016,098,204 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_ObjectSync`

- Report: [callgrind-725154.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-725154.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ObjectSync (PID 725154, part 1)`
- Program instructions: **12,010,124**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,291,404 | `???:0x000000000001d2c0` |
| 7,157,728 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 5,218,106 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 5,046,606 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 4,718,699 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 4,696,411 | `???:0x00000000000e0e50` |
| 4,689,611 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 4,688,551 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 4,687,269 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 4,681,679 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_Serialization`

- Report: [callgrind-725187.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-725187.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Serialization (PID 725187, part 1)`
- Program instructions: **1,811,850,134**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 3,778,334,617 | [src/nbn/core/detail/serialization.cpp:nbn::core::serialization::json::skipValue(std::basic_string_view<char, std::char_traits<char> >, unsigned long&)'2](../../../../nbn-main-vscode-workspace/src/nbn/core/detail/serialization.cpp) |
| 3,762,610,020 | [src/nbn/core/detail/serialization.cpp:nbn::core::serialization::json::skipArray(std::basic_string_view<char, std::char_traits<char> >, unsigned long&)'2](../../../../nbn-main-vscode-workspace/src/nbn/core/detail/serialization.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 4,704,421,880 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 1,804,049,916 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 1,803,728,065 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 1,803,706,793 | `???:0x00000000000e0e50` |
| 1,803,699,993 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 1,803,698,968 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 1,803,697,686 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 1,803,692,103 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_Signal`

- Report: [callgrind-725386.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-725386.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Signal (PID 725386, part 1)`
- Program instructions: **11,197,595**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,080,441 | `???:0x000000000001d2c0` |
| 5,694,024 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 4,950,436 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 4,163,383 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 4,117,063 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 4,072,610 | `???:0x00000000000e0e50` |
| 4,012,730 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 4,011,705 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 4,010,423 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 4,004,774 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_Slots`

- Report: [callgrind-725412.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-725412.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Slots (PID 725412, part 1)`
- Program instructions: **20,696,676**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 13,396,159 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 18,804,360 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 13,582,095 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 13,438,049 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 13,413,228 | `???:0x00000000000e0e50` |
| 13,406,079 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 13,405,043 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 13,403,751 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 13,398,119 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 13,396,999 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_Task`

- Report: [callgrind-725447.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-725447.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Task (PID 725447, part 1)`
- Program instructions: **831,758,588**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 1,646,687,631 | [src/nbn/core/Task.cpp:nbn::core::Task::run()'2](../../../../nbn-main-vscode-workspace/src/nbn/core/Task.cpp) |
| 1,646,687,148 | [src/nbn/core/private/Task.h:nbn::core::Task::Impl::run()'2](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Task.h) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 2,469,626,562 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 824,900,782 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 824,568,976 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 824,551,906 | `???:0x00000000000e0e50` |
| 824,545,106 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 824,544,081 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |

</details>

## `test_Thread`

- Report: [callgrind-725529.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-725529.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Thread (PID 725529, part 1)`
- Program instructions: **16,640,835**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 28,366,050 | `./nptl/./nptl/pthread_create.c:start_thread'2` |
| 26,545,611 | `./misc/../sysdeps/unix/sysv/linux/x86_64/clone.S:start_thread'2` |
| 13,357,401 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 9,477,732 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 9,376,008 | `???:0x00000000000e0e50` |
| 9,309,309 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 9,307,899 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 9,306,267 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 9,298,969 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 9,297,509 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_Timer`

- Report: [callgrind-725583.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-725583.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Timer (PID 725583, part 1)`
- Program instructions: **861,832,502**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 854,604,885 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 2,552,891,088 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 854,670,286 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 854,627,288 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 854,625,019 | `???:0x00000000000e0e50` |
| 854,615,435 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 854,614,322 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 854,612,960 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 854,606,985 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 854,605,795 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_UnitTests`

- Report: [callgrind-725682.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-725682.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_UnitTests (PID 725682, part 1)`
- Program instructions: **911,940,046**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 904,238,271 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 3,598,746,415 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 904,599,144 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 904,276,219 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 904,254,936 | `???:0x00000000000e0e50` |
| 904,248,136 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 904,247,111 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 904,245,829 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 904,240,211 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 904,239,101 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_utils`

- Report: [callgrind-725781.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-725781.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_utils (PID 725781, part 1)`
- Program instructions: **866,745,223**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 859,361,318 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 2,575,314,108 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 859,721,720 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 859,392,342 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 859,377,948 | `???:0x00000000000e0e50` |
| 859,371,148 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 859,370,123 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 859,368,841 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 859,363,258 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 859,362,148 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_CommandLineParserHelp`

- Report: [callgrind-725851.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-725851.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_CommandLineParserHelp (PID 725851, part 1)`
- Program instructions: **5,425,298**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 5,425,298 | `???:0x000000000001d2c0` |
| 4,049,601 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 3,259,136 | `./elf/./elf/rtld.c:_dl_start` |
| 3,258,395 | `./elf/../sysdeps/unix/sysv/linux/dl-sysdep.c:_dl_sysdep_start` |
| 3,225,425 | `./elf/./elf/rtld.c:dl_main` |
| 3,114,995 | `./elf/./elf/dl-reloc.c:_dl_relocate_object` |
| 3,114,976 | `./elf/./elf/dl-reloc.c:_dl_relocate_object_no_relro` |
| 2,081,364 | `./elf/../sysdeps/generic/dl-new-hash.h:_dl_lookup_symbol_x` |
| 1,806,370 | `./elf/../sysdeps/x86_64/dl-trampoline.h:_dl_runtime_resolve_xsave` |
| 1,750,290 | `./elf/./elf/dl-runtime.c:_dl_fixup` |

</details>

## `test_ApplicationSegfault`

- Report: [callgrind-725884.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-725884.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ApplicationSegfault (PID 725884, part 1)`
- Program instructions: **4,540,081**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 664,711,563 | [src/nbn/core/private/Application.cpp:nbn::core::private_ns::hardwaretSignalHandler(int)](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Application.cpp) |
| 664,528,057 | [src/nbn/core/private/Application.cpp:nbn::core::private_ns::stackTraceMessage[abi:cxx11](std::basic_string_view<char, std::char_traits<char> >)](../../../../nbn-main-vscode-workspace/src/nbn/core/private/Application.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 758,777,057 | `???:elf_zlib_inflate.cold'2` |
| 664,463,030 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/stacktrace:std::ostream& std::operator<< <std::allocator<std::stacktrace_entry> >(std::ostream&, std::basic_stacktrace<std::allocator<std::stacktrace_entry> > const&)` |
| 664,444,399 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/stacktrace:std::operator<<(std::ostream&, std::stacktrace_entry const&)` |
| 664,426,826 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/stacktrace:std::stacktrace_entry::_M_get_info(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >*, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >*, int*) const` |
| 664,423,693 | `???:std::stacktrace_entry::_Info::_M_populate(unsigned long)` |
| 664,419,325 | `???:__glibcxx_backtrace_pcinfo` |
| 610,639,459 | `???:fileline_initialize` |
| 610,635,761 | `???:__glibcxx_backtrace_initialize` |

</details>

## `test_ApplicationTerminateProcess`

- Report: [callgrind-725915.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-725915.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ApplicationTerminateProcess (PID 725915, part 1)`
- Program instructions: **4,817,581**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 4,817,581 | `???:0x000000000001d2c0` |
| 3,567,122 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 3,266,174 | `./elf/./elf/rtld.c:_dl_start` |
| 3,265,433 | `./elf/../sysdeps/unix/sysv/linux/dl-sysdep.c:_dl_sysdep_start` |
| 3,232,463 | `./elf/./elf/rtld.c:dl_main` |
| 3,121,514 | `./elf/./elf/dl-reloc.c:_dl_relocate_object` |
| 3,121,495 | `./elf/./elf/dl-reloc.c:_dl_relocate_object_no_relro` |
| 1,741,316 | `./elf/../sysdeps/generic/dl-new-hash.h:_dl_lookup_symbol_x` |
| 1,447,867 | `./elf/./elf/dl-lookup.c:do_lookup_x` |
| 1,266,057 | `./elf/../sysdeps/x86_64/dl-trampoline.h:_dl_runtime_resolve_xsave` |

</details>
