# Callgrind

Callgrind instruction counts are useful for locating hot functions. They are not wall-clock timings, and dynamic-loader/startup symbols should not be optimized as application code.

## ✅ Profiles collected

## `test_Application`

- Report: [callgrind-3464775.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-3464775.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Application (PID 3464775, part 1)`
- Program instructions: **851,527,329**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 843,320,905 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 1,673,419,976 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 843,444,755 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 843,344,120 | `???:0x00000000000e0e50` |
| 843,332,085 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 843,330,895 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 843,329,463 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 843,323,145 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 843,321,885 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_ApplicationSignalInterrupt`

- Report: [callgrind-3465046.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-3465046.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ApplicationSignalInterrupt (PID 3465046, part 1)`
- Program instructions: **8,188,892**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,276,032 | `???:0x000000000001d2c0` |
| 5,071,010 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 3,277,251 | `./elf/./elf/rtld.c:_dl_start` |
| 3,276,510 | `./elf/../sysdeps/unix/sysv/linux/dl-sysdep.c:_dl_sysdep_start` |
| 3,243,152 | `./elf/./elf/rtld.c:dl_main` |
| 3,128,240 | `./elf/./elf/dl-reloc.c:_dl_relocate_object` |
| 3,128,221 | `./elf/./elf/dl-reloc.c:_dl_relocate_object_no_relro` |
| 3,095,221 | `???:(below main)` |
| 3,095,210 | `./csu/../csu/libc-start.c:__libc_start_main@@GLIBC_2.34` |
| 3,095,043 | `./csu/../sysdeps/nptl/libc_start_call_main.h:(below main)` |

</details>

## `test_ApplicationTerminateHandler`

- Report: [callgrind-3465092.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-3465092.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ApplicationTerminateHandler (PID 3465092, part 1)`
- Program instructions: **8,244,654**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,498,104 | `???:0x000000000001d2c0` |
| 5,113,517 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 3,270,232 | `./elf/./elf/rtld.c:_dl_start` |
| 3,269,491 | `./elf/../sysdeps/unix/sysv/linux/dl-sysdep.c:_dl_sysdep_start` |
| 3,236,133 | `./elf/./elf/rtld.c:dl_main` |
| 3,184,212 | `???:(below main)` |
| 3,184,201 | `./csu/../csu/libc-start.c:__libc_start_main@@GLIBC_2.34` |
| 3,184,034 | `./csu/../sysdeps/nptl/libc_start_call_main.h:(below main)` |
| 3,124,180 | `./elf/./elf/dl-reloc.c:_dl_relocate_object` |
| 3,124,161 | `./elf/./elf/dl-reloc.c:_dl_relocate_object_no_relro` |

</details>

## `test_ApplicationThreadsManager`

- Report: [callgrind-3465135.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-3465135.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ApplicationThreadsManager (PID 3465135, part 1)`
- Program instructions: **26,982,923**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 19,488,501 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 28,461,975 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 19,760,567 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 19,531,471 | `???:0x00000000000e0e50` |
| 19,503,731 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 19,502,046 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 19,500,164 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 19,491,641 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 19,489,931 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_ApplicationUncaughtException`

- Report: [callgrind-3465269.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-3465269.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ApplicationUncaughtException (PID 3465269, part 1)`
- Program instructions: **707,361,463**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 699,206,166 | [src/nbn/core/Application.cpp:nbn::core::Application::exec() (line 105)](../../../src/nbn/core/Application.cpp#L105) |
| 699,205,337 | [src/nbn/core/private/Application.cpp:nbn::core::private_ns::Application::exec() (line 185)](../../../src/nbn/core/private/Application.cpp#L185) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 758,777,057 | `???:elf_zlib_inflate.cold'2` |
| 705,738,705 | `???:0x000000000001d2c0` |
| 701,426,746 | `???:(below main)` |
| 701,426,735 | `./csu/../csu/libc-start.c:__libc_start_main@@GLIBC_2.34` |
| 701,426,568 | `./csu/../sysdeps/nptl/libc_start_call_main.h:(below main)` |
| 701,426,518 | `tests/nbn/core/main_test_ApplicationUncaughtException.cpp:main` |

</details>

## `test_Color`

- Report: [callgrind-3465444.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-3465444.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Color (PID 3465444, part 1)`
- Program instructions: **18,319,981**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 10,999,681 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 20,362,495 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 11,363,615 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 11,048,884 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 11,016,353 | `???:0x00000000000e0e50` |
| 11,009,553 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 11,008,493 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 11,007,211 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 11,001,621 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 11,000,511 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_CommandLineParser`

- Report: [callgrind-3465494.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-3465494.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_CommandLineParser (PID 3465494, part 1)`
- Program instructions: **10,959,185**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,239,304 | `???:0x000000000001d2c0` |
| 5,740,779 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 5,727,920 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 4,036,142 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 3,719,860 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 3,710,582 | `./elf/../sysdeps/x86_64/dl-trampoline.h:_dl_runtime_resolve_xsave` |
| 3,700,450 | `???:0x00000000000e0e50` |
| 3,693,650 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 3,692,625 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 3,691,343 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |

</details>

## `test_ConcurrentStack`

- Report: [callgrind-3465539.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-3465539.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ConcurrentStack (PID 3465539, part 1)`
- Program instructions: **517,862,776**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 1,266,574,295 | [src/nbn/core/../../nbn/core/ConcurrentStack.h:nbn::core::ConcurrentStack<std::unique_ptr<int, std::default_delete<int> > >::Node::~Node()'2](../../../src/nbn/core/ConcurrentStack.h) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 1,272,623,513 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/shared_ptr_base.h:std::_Sp_counted_base<(__gnu_cxx::_Lock_policy)2>::_M_release()'2` |
| 1,266,836,063 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/shared_ptr_base.h:std::_Sp_counted_ptr_inplace<nbn::core::ConcurrentStack<std::unique_ptr<int, std::default_delete<int> > >::Node, std::allocator<void>, (__gnu_cxx::_Lock_policy)2>::_M_dispose()'2` |
| 1,266,644,771 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/alloc_traits.h:std::_Sp_counted_ptr_inplace<nbn::core::ConcurrentStack<std::unique_ptr<int, std::default_delete<int> > >::Node, std::allocator<void>, (__gnu_cxx::_Lock_policy)2>::_M_dispose()'2` |
| 1,266,634,703 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/stl_construct.h:void std::_Destroy<nbn::core::ConcurrentStack<std::unique_ptr<int, std::default_delete<int> > >::Node>(nbn::core::ConcurrentStack<std::unique_ptr<int, std::default_delete<int> > >::Node*)'2` |
| 1,266,604,499 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/stl_construct.h:void std::destroy_at<nbn::core::ConcurrentStack<std::unique_ptr<int, std::default_delete<int> > >::Node>(nbn::core::ConcurrentStack<std::unique_ptr<int, std::default_delete<int> > >::Node*)'2` |
| 1,266,336,019 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/atomic:std::atomic<std::shared_ptr<nbn::core::ConcurrentStack<std::unique_ptr<int, std::default_delete<int> > >::Node> >::~atomic()'2` |
| 1,266,305,815 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/shared_ptr_atomic.h:std::_Sp_atomic<std::shared_ptr<nbn::core::ConcurrentStack<std::unique_ptr<int, std::default_delete<int> > >::Node> >::~_Sp_atomic()'2` |
| 1,266,272,255 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/shared_ptr_atomic.h:std::_Sp_atomic<std::shared_ptr<nbn::core::ConcurrentStack<std::unique_ptr<int, std::default_delete<int> > >::Node> >::_Atomic_count::~_Atomic_count()'2` |

</details>

## `test_ElapsedTimer`

- Report: [callgrind-3465837.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-3465837.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ElapsedTimer (PID 3465837, part 1)`
- Program instructions: **8,604,973**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,199,592 | `???:0x000000000001d2c0` |
| 5,008,680 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 3,275,124 | `./elf/./elf/rtld.c:_dl_start` |
| 3,274,383 | `./elf/../sysdeps/unix/sysv/linux/dl-sysdep.c:_dl_sysdep_start` |
| 3,241,025 | `./elf/./elf/rtld.c:dl_main` |
| 3,127,437 | `./elf/./elf/dl-reloc.c:_dl_relocate_object` |
| 3,127,418 | `./elf/./elf/dl-reloc.c:_dl_relocate_object_no_relro` |
| 3,017,563 | `???:(below main)` |
| 3,017,552 | `./csu/../csu/libc-start.c:__libc_start_main@@GLIBC_2.34` |
| 3,017,385 | `./csu/../sysdeps/nptl/libc_start_call_main.h:(below main)` |

</details>

## `test_FilesystemWatcher`

- Report: [callgrind-3465897.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-3465897.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_FilesystemWatcher (PID 3465897, part 1)`
- Program instructions: **16,914,932**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 9,808,443 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 12,053,212 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 9,880,362 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 9,839,444 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 9,828,146 | `???:0x00000000000e0e50` |
| 9,818,903 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 9,817,801 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 9,816,449 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 9,810,523 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 9,809,343 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_FiniteStateMachine`

- Report: [callgrind-3465978.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-3465978.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_FiniteStateMachine (PID 3465978, part 1)`
- Program instructions: **1,612,885,505**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 1,605,656,783 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 2,710,864,209 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 1,605,720,086 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 1,605,675,211 | `???:0x00000000000e0e50` |
| 1,605,667,015 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 1,605,665,911 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 1,605,664,589 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 1,605,658,803 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 1,605,657,653 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_JsonMigratorBase`

- Report: [callgrind-3466576.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-3466576.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_JsonMigratorBase (PID 3466576, part 1)`
- Program instructions: **10,487,537**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,315,128 | `???:0x000000000001d2c0` |
| 5,527,153 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 4,913,316 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 3,500,244 | `./elf/../sysdeps/x86_64/dl-trampoline.h:_dl_runtime_resolve_xsave` |
| 3,486,476 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 3,383,284 | `./elf/./elf/dl-runtime.c:_dl_fixup` |
| 3,275,860 | `./elf/./elf/rtld.c:_dl_start` |
| 3,275,119 | `./elf/../sysdeps/unix/sysv/linux/dl-sysdep.c:_dl_sysdep_start` |
| 3,241,761 | `./elf/./elf/rtld.c:dl_main` |
| 3,172,388 | `./nptl/./nptl/pthread_create.c:start_thread` |

</details>

## `test_Object`

- Report: [callgrind-3466631.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-3466631.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Object (PID 3466631, part 1)`
- Program instructions: **1,023,558,734**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 1,016,143,644 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 2,029,680,057 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 1,016,488,670 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 1,016,178,683 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 1,016,160,317 | `???:0x00000000000e0e50` |
| 1,016,153,517 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 1,016,152,492 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 1,016,151,210 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 1,016,145,627 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 1,016,144,517 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_ObjectSync`

- Report: [callgrind-3466894.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-3466894.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ObjectSync (PID 3466894, part 1)`
- Program instructions: **12,006,227**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,310,306 | `???:0x000000000001d2c0` |
| 7,158,471 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 5,218,260 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 5,013,938 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 4,695,900 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 4,675,527 | `???:0x00000000000e0e50` |
| 4,668,727 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 4,667,667 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 4,666,385 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 4,660,795 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_Serialization`

- Report: [callgrind-3466941.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-3466941.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Serialization (PID 3466941, part 1)`
- Program instructions: **1,811,875,109**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 3,778,356,377 | [src/nbn/core/detail/serialization.cpp:nbn::core::serialization::json::skipValue(std::basic_string_view<char, std::char_traits<char> >, unsigned long&)'2](../../../src/nbn/core/detail/serialization.cpp) |
| 3,762,631,695 | [src/nbn/core/detail/serialization.cpp:nbn::core::serialization::json::skipArray(std::basic_string_view<char, std::char_traits<char> >, unsigned long&)'2](../../../src/nbn/core/detail/serialization.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 4,704,448,956 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 1,804,062,363 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 1,803,753,949 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 1,803,732,630 | `???:0x00000000000e0e50` |
| 1,803,725,830 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 1,803,724,805 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 1,803,723,523 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 1,803,717,940 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_Signal`

- Report: [callgrind-3467537.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-3467537.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Signal (PID 3467537, part 1)`
- Program instructions: **11,200,538**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,080,359 | `???:0x000000000001d2c0` |
| 5,695,029 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 4,950,086 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 4,165,906 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 4,120,088 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 4,075,276 | `???:0x00000000000e0e50` |
| 4,016,472 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 4,015,447 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 4,014,165 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 4,008,516 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_Slots`

- Report: [callgrind-3467621.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-3467621.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Slots (PID 3467621, part 1)`
- Program instructions: **20,687,857**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 13,403,387 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 18,807,046 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 13,576,367 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 13,445,729 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 13,420,456 | `???:0x00000000000e0e50` |
| 13,413,307 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 13,412,271 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 13,410,979 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 13,405,347 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 13,404,227 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_Task`

- Report: [callgrind-3467696.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-3467696.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Task (PID 3467696, part 1)`
- Program instructions: **831,776,007**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 1,646,687,599 | [src/nbn/core/Task.cpp:nbn::core::Task::run()'2](../../../src/nbn/core/Task.cpp) |
| 1,646,687,116 | [src/nbn/core/private/Task.h:nbn::core::Task::Impl::run()'2](../../../src/nbn/core/private/Task.h) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 2,469,626,456 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 824,917,150 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 824,598,525 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 824,581,413 | `???:0x00000000000e0e50` |
| 824,574,613 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 824,573,588 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |

</details>

## `test_Thread`

- Report: [callgrind-3467883.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-3467883.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Thread (PID 3467883, part 1)`
- Program instructions: **16,666,807**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 26,353,488 | `./nptl/./nptl/pthread_create.c:start_thread'2` |
| 24,744,217 | `./misc/../sysdeps/unix/sysv/linux/x86_64/clone.S:start_thread'2` |
| 13,383,596 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 9,503,109 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 9,400,875 | `???:0x00000000000e0e50` |
| 9,334,176 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 9,332,766 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 9,331,134 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 9,323,836 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 9,322,376 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_Timer`

- Report: [callgrind-3467977.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-3467977.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Timer (PID 3467977, part 1)`
- Program instructions: **861,877,754**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 854,663,846 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 2,552,892,740 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 854,727,657 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 854,683,980 | `???:0x00000000000e0e50` |
| 854,674,396 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 854,673,283 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 854,672,477 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 854,671,921 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 854,665,946 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 854,664,756 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_UnitTests`

- Report: [callgrind-3468215.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-3468215.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_UnitTests (PID 3468215, part 1)`
- Program instructions: **911,986,484**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 904,283,535 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 3,598,741,051 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 904,642,743 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 904,321,622 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 904,300,200 | `???:0x00000000000e0e50` |
| 904,293,400 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 904,292,375 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 904,291,093 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 904,285,475 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 904,284,365 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_utils`

- Report: [callgrind-3468475.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-3468475.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_utils (PID 3468475, part 1)`
- Program instructions: **866,768,251**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 859,396,635 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../../src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 2,575,327,393 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 859,743,637 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 859,427,701 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 859,413,265 | `???:0x00000000000e0e50` |
| 859,406,465 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 859,405,440 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 859,404,158 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 859,398,575 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 859,397,465 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_CommandLineParserHelp`

- Report: [callgrind-3468674.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-3468674.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_CommandLineParserHelp (PID 3468674, part 1)`
- Program instructions: **5,425,737**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 5,425,737 | `???:0x000000000001d2c0` |
| 4,049,718 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 3,259,509 | `./elf/./elf/rtld.c:_dl_start` |
| 3,258,768 | `./elf/../sysdeps/unix/sysv/linux/dl-sysdep.c:_dl_sysdep_start` |
| 3,225,410 | `./elf/./elf/rtld.c:dl_main` |
| 3,115,047 | `./elf/./elf/dl-reloc.c:_dl_relocate_object` |
| 3,115,028 | `./elf/./elf/dl-reloc.c:_dl_relocate_object_no_relro` |
| 2,081,364 | `./elf/../sysdeps/generic/dl-new-hash.h:_dl_lookup_symbol_x` |
| 1,806,435 | `./elf/../sysdeps/x86_64/dl-trampoline.h:_dl_runtime_resolve_xsave` |
| 1,750,355 | `./elf/./elf/dl-runtime.c:_dl_fixup` |

</details>

## `test_ApplicationSegfault`

- Report: [callgrind-3468718.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-3468718.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ApplicationSegfault (PID 3468718, part 1)`
- Program instructions: **4,540,519**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 664,711,558 | [src/nbn/core/private/Application.cpp:nbn::core::private_ns::hardwaretSignalHandler(int) (line 80)](../../../src/nbn/core/private/Application.cpp#L80) |
| 664,528,036 | [src/nbn/core/private/Application.cpp:nbn::core::private_ns::stackTraceMessage[abi:cxx11](std::basic_string_view<char, std::char_traits<char> >) (line 39)](../../../src/nbn/core/private/Application.cpp#L39) |

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

- Report: [callgrind-3468847.out](../../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-3468847.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ApplicationTerminateProcess (PID 3468847, part 1)`
- Program instructions: **4,818,011**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 4,818,011 | `???:0x000000000001d2c0` |
| 3,567,223 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 3,266,533 | `./elf/./elf/rtld.c:_dl_start` |
| 3,265,792 | `./elf/../sysdeps/unix/sysv/linux/dl-sysdep.c:_dl_sysdep_start` |
| 3,232,434 | `./elf/./elf/rtld.c:dl_main` |
| 3,121,566 | `./elf/./elf/dl-reloc.c:_dl_relocate_object` |
| 3,121,547 | `./elf/./elf/dl-reloc.c:_dl_relocate_object_no_relro` |
| 1,741,316 | `./elf/../sysdeps/generic/dl-new-hash.h:_dl_lookup_symbol_x` |
| 1,447,968 | `./elf/./elf/dl-lookup.c:do_lookup_x` |
| 1,266,106 | `./elf/../sysdeps/x86_64/dl-trampoline.h:_dl_runtime_resolve_xsave` |

</details>
