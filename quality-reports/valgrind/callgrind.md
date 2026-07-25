# Callgrind

Callgrind instruction counts are useful for locating hot functions. They are not wall-clock timings, and dynamic-loader/startup symbols should not be optimized as application code.

## ✅ Profiles collected

## `test_Application`

- Report: [callgrind-2612206.out](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-2612206.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Application (PID 2612206, part 1)`
- Program instructions: **847,694,412**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 839,384,578 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 1,665,705,010 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 839,513,576 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 839,407,796 | `???:0x00000000000e0e50` |
| 839,395,758 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 839,394,568 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 839,393,136 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 839,386,818 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 839,385,558 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_ApplicationSignalInterrupt`

- Report: [callgrind-2612604.out](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-2612604.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ApplicationSignalInterrupt (PID 2612604, part 1)`
- Program instructions: **8,166,947**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,371,600 | `???:0x000000000001d2c0` |
| 5,057,139 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 3,277,924 | `./elf/./elf/rtld.c:_dl_start` |
| 3,277,183 | `./elf/../sysdeps/unix/sysv/linux/dl-sysdep.c:_dl_sysdep_start` |
| 3,243,825 | `./elf/./elf/rtld.c:dl_main` |
| 3,189,937 | `???:(below main)` |
| 3,189,926 | `./csu/../csu/libc-start.c:__libc_start_main@@GLIBC_2.34` |
| 3,189,759 | `./csu/../sysdeps/nptl/libc_start_call_main.h:(below main)` |
| 3,128,233 | `./elf/./elf/dl-reloc.c:_dl_relocate_object` |
| 3,128,214 | `./elf/./elf/dl-reloc.c:_dl_relocate_object_no_relro` |

</details>

## `test_ApplicationTerminateHandler`

- Report: [callgrind-2612646.out](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-2612646.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ApplicationTerminateHandler (PID 2612646, part 1)`
- Program instructions: **8,215,010**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,516,494 | `???:0x000000000001d2c0` |
| 5,096,211 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 3,270,797 | `./elf/./elf/rtld.c:_dl_start` |
| 3,270,056 | `./elf/../sysdeps/unix/sysv/linux/dl-sysdep.c:_dl_sysdep_start` |
| 3,236,698 | `./elf/./elf/rtld.c:dl_main` |
| 3,201,824 | `???:(below main)` |
| 3,201,813 | `./csu/../csu/libc-start.c:__libc_start_main@@GLIBC_2.34` |
| 3,201,646 | `./csu/../sysdeps/nptl/libc_start_call_main.h:(below main)` |
| 3,124,028 | `./elf/./elf/dl-reloc.c:_dl_relocate_object` |
| 3,124,009 | `./elf/./elf/dl-reloc.c:_dl_relocate_object_no_relro` |

</details>

## `test_ApplicationThreadsManager`

- Report: [callgrind-2612705.out](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-2612705.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ApplicationThreadsManager (PID 2612705, part 1)`
- Program instructions: **1,022,619,555**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 1,014,030,896 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 1,989,964,680 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 1,014,309,222 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 1,014,073,869 | `???:0x00000000000e0e50` |
| 1,014,046,126 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 1,014,044,441 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 1,014,042,559 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 1,014,034,036 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 1,014,032,326 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_ApplicationUncaughtException`

- Report: [callgrind-2613288.out](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-2613288.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ApplicationUncaughtException (PID 2613288, part 1)`
- Program instructions: **703,399,475**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 695,234,472 | [src/nbn/core/Application.cpp:nbn::core::Application::exec() (line 105)](../../src/nbn/core/Application.cpp#L105) |
| 695,233,643 | [src/nbn/core/private/Application.cpp:nbn::core::private_ns::Application::exec() (line 181)](../../src/nbn/core/private/Application.cpp#L181) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 758,777,057 | `???:elf_zlib_inflate.cold'2` |
| 701,783,157 | `???:0x000000000001d2c0` |
| 697,470,747 | `???:(below main)` |
| 697,470,736 | `./csu/../csu/libc-start.c:__libc_start_main@@GLIBC_2.34` |
| 697,470,569 | `./csu/../sysdeps/nptl/libc_start_call_main.h:(below main)` |
| 697,470,519 | `tests/nbn/core/main_test_ApplicationUncaughtException.cpp:main` |

</details>

## `test_Color`

- Report: [callgrind-2613474.out](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-2613474.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Color (PID 2613474, part 1)`
- Program instructions: **18,341,641**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 11,038,216 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 20,340,229 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 11,390,353 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 11,088,163 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 11,054,891 | `???:0x00000000000e0e50` |
| 11,048,088 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 11,047,028 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 11,045,746 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 11,040,156 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 11,039,046 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_CommandLineParser`

- Report: [callgrind-2613554.out](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-2613554.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_CommandLineParser (PID 2613554, part 1)`
- Program instructions: **10,972,811**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,243,565 | `???:0x000000000001d2c0` |
| 5,743,405 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 5,710,951 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 4,044,661 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 3,729,225 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 3,713,894 | `./elf/../sysdeps/x86_64/dl-trampoline.h:_dl_runtime_resolve_xsave` |
| 3,709,176 | `???:0x00000000000e0e50` |
| 3,702,373 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 3,701,348 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 3,700,066 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |

</details>

## `test_ConcurrentStack`

- Report: [callgrind-2613584.out](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-2613584.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ConcurrentStack (PID 2613584, part 1)`
- Program instructions: **527,447,482**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 181,633,429 | [src/nbn/core/../../nbn/core/ConcurrentStack.h:nbn::core::ConcurrentStack<int>::push(int) (line 86)](../../src/nbn/core/ConcurrentStack.h#L86) |
| 127,715,045 | [src/nbn/core/../../nbn/core/ConcurrentStack.h:nbn::core::ConcurrentStack<int>::pop(int&) (line 35)](../../src/nbn/core/ConcurrentStack.h#L35) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 520,088,808 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 519,957,937 | `???:0x00000000000e0e50` |
| 490,836,181 | `./nptl/./nptl/pthread_create.c:start_thread'2` |
| 265,452,223 | `./misc/../sysdeps/unix/sysv/linux/x86_64/clone.S:start_thread` |
| 225,384,224 | `./misc/../sysdeps/unix/sysv/linux/x86_64/clone.S:start_thread'2` |
| 147,781,263 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/shared_ptr_atomic.h:std::atomic<std::shared_ptr<nbn::core::ConcurrentStack<int>::Node> >::compare_exchange_weak(std::shared_ptr<nbn::core::ConcurrentStack<int>::Node>&, std::shared_ptr<nbn::core::ConcurrentStack<int>::Node>, std::memory_order)` |
| 123,841,149 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/shared_ptr_atomic.h:std::atomic<std::shared_ptr<nbn::core::ConcurrentStack<int>::Node> >::compare_exchange_strong(std::shared_ptr<nbn::core::ConcurrentStack<int>::Node>&, std::shared_ptr<nbn::core::ConcurrentStack<int>::Node>, std::memory_order)` |

</details>

## `test_ElapsedTimer`

- Report: [callgrind-2613923.out](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-2613923.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ElapsedTimer (PID 2613923, part 1)`
- Program instructions: **8,579,447**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,212,350 | `???:0x000000000001d2c0` |
| 5,011,750 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 3,275,868 | `./elf/./elf/rtld.c:_dl_start` |
| 3,275,127 | `./elf/../sysdeps/unix/sysv/linux/dl-sysdep.c:_dl_sysdep_start` |
| 3,241,769 | `./elf/./elf/rtld.c:dl_main` |
| 3,127,436 | `./elf/./elf/dl-reloc.c:_dl_relocate_object` |
| 3,127,417 | `./elf/./elf/dl-reloc.c:_dl_relocate_object_no_relro` |
| 3,029,188 | `???:(below main)` |
| 3,029,177 | `./csu/../csu/libc-start.c:__libc_start_main@@GLIBC_2.34` |
| 3,029,010 | `./csu/../sysdeps/nptl/libc_start_call_main.h:(below main)` |

</details>

## `test_FilesystemWatcher`

- Report: [callgrind-2613991.out](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-2613991.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_FilesystemWatcher (PID 2613991, part 1)`
- Program instructions: **16,943,063**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 9,830,790 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 12,058,011 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 9,903,726 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 9,861,279 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 9,850,496 | `???:0x00000000000e0e50` |
| 9,841,250 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 9,840,148 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 9,838,796 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 9,832,870 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 9,831,690 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_FiniteStateMachine`

- Report: [callgrind-2614084.out](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-2614084.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_FiniteStateMachine (PID 2614084, part 1)`
- Program instructions: **1,258,640,105**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 1,251,451,035 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 2,699,220,171 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 1,251,515,706 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 1,251,469,466 | `???:0x00000000000e0e50` |
| 1,251,461,267 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 1,251,460,163 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 1,251,458,841 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 1,251,453,055 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 1,251,451,905 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_JsonMigratorBase`

- Report: [callgrind-2614526.out](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-2614526.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_JsonMigratorBase (PID 2614526, part 1)`
- Program instructions: **10,492,307**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,319,518 | `???:0x000000000001d2c0` |
| 5,529,902 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 4,913,688 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 3,503,684 | `./elf/../sysdeps/x86_64/dl-trampoline.h:_dl_runtime_resolve_xsave` |
| 3,487,137 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 3,386,604 | `./elf/./elf/dl-runtime.c:_dl_fixup` |
| 3,276,383 | `./elf/./elf/rtld.c:_dl_start` |
| 3,275,642 | `./elf/../sysdeps/unix/sysv/linux/dl-sysdep.c:_dl_sysdep_start` |
| 3,242,284 | `./elf/./elf/rtld.c:dl_main` |
| 3,172,768 | `./nptl/./nptl/pthread_create.c:start_thread` |

</details>

## `test_Object`

- Report: [callgrind-2614605.out](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-2614605.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Object (PID 2614605, part 1)`
- Program instructions: **1,020,181,405**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 1,012,761,308 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 2,022,907,191 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 1,013,107,127 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 1,012,796,350 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 1,012,777,984 | `???:0x00000000000e0e50` |
| 1,012,771,181 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 1,012,770,156 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 1,012,768,874 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 1,012,763,291 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 1,012,762,181 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_ObjectSync`

- Report: [callgrind-2614882.out](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-2614882.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ObjectSync (PID 2614882, part 1)`
- Program instructions: **12,021,065**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,278,533 | `???:0x000000000001d2c0` |
| 7,157,251 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 5,220,078 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 5,057,547 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 4,742,511 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 4,720,026 | `???:0x00000000000e0e50` |
| 4,713,223 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 4,712,163 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 4,710,881 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 4,705,291 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_Serialization`

- Report: [callgrind-2614912.out](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-2614912.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Serialization (PID 2614912, part 1)`
- Program instructions: **1,808,804,242**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 3,778,559,129 | [src/nbn/core/detail/serialization.cpp:nbn::core::serialization::json::skipValue(std::basic_string_view<char, std::char_traits<char> >, unsigned long&)'2](../../src/nbn/core/detail/serialization.cpp) |
| 3,762,833,655 | [src/nbn/core/detail/serialization.cpp:nbn::core::serialization::json::skipArray(std::basic_string_view<char, std::char_traits<char> >, unsigned long&)'2](../../src/nbn/core/detail/serialization.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 4,694,928,609 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 1,801,480,605 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 1,801,182,108 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 1,801,157,230 | `???:0x00000000000e0e50` |
| 1,801,150,427 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 1,801,149,402 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 1,801,148,120 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 1,801,142,537 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_Signal`

- Report: [callgrind-2615604.out](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-2615604.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Signal (PID 2615604, part 1)`
- Program instructions: **11,181,977**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 7,130,113 | `???:0x000000000001d2c0` |
| 5,693,541 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 4,952,423 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 4,091,137 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 4,051,773 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 4,010,231 | `???:0x00000000000e0e50` |
| 3,952,566 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 3,951,541 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 3,950,259 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 3,944,610 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_Slots`

- Report: [callgrind-2615712.out](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-2615712.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Slots (PID 2615712, part 1)`
- Program instructions: **20,610,773**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 13,274,138 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 18,823,374 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 13,464,184 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 13,315,429 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 13,291,210 | `???:0x00000000000e0e50` |
| 13,284,058 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 13,283,022 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 13,281,730 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 13,276,098 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 13,274,978 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_Task`

- Report: [callgrind-2615789.out](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-2615789.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Task (PID 2615789, part 1)`
- Program instructions: **827,906,570**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 1,638,914,456 | [src/nbn/core/Task.cpp:nbn::core::Task::run()'2](../../src/nbn/core/Task.cpp) |
| 1,638,913,973 | [src/nbn/core/private/Task.h:nbn::core::Task::Impl::run()'2](../../src/nbn/core/private/Task.h) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 2,457,972,495 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 821,033,913 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 820,714,867 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 820,697,834 | `???:0x00000000000e0e50` |
| 820,691,031 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 820,690,006 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |

</details>

## `test_Thread`

- Report: [callgrind-2616044.out](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-2616044.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Thread (PID 2616044, part 1)`
- Program instructions: **16,637,563**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 22,446,854 | `./nptl/./nptl/pthread_create.c:start_thread'2` |
| 20,720,463 | `./misc/../sysdeps/unix/sysv/linux/x86_64/clone.S:start_thread'2` |
| 13,297,038 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 9,484,768 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 9,390,346 | `???:0x00000000000e0e50` |
| 9,323,644 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 9,322,234 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 9,320,602 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 9,313,304 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 9,311,844 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_Timer`

- Report: [callgrind-2616165.out](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-2616165.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_Timer (PID 2616165, part 1)`
- Program instructions: **858,119,044**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 850,903,623 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 2,541,181,811 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 850,967,646 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 850,923,760 | `???:0x00000000000e0e50` |
| 850,914,173 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 850,913,060 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 850,911,698 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 850,908,433 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 850,905,723 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 850,904,533 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_UnitTests`

- Report: [callgrind-2616469.out](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-2616469.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_UnitTests (PID 2616469, part 1)`
- Program instructions: **908,091,978**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 900,411,568 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 3,583,232,023 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 900,758,747 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 900,449,785 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 900,428,236 | `???:0x00000000000e0e50` |
| 900,421,433 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 900,420,408 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 900,419,126 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 900,413,508 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 900,412,398 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_utils`

- Report: [callgrind-2616737.out](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-2616737.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_utils (PID 2616737, part 1)`
- Program instructions: **862,967,023**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 855,664,810 | [src/nbn/core/private/Thread.cpp:nbn::core::Thread::Impl::run()::{lambda()#2}::operator()()](../../src/nbn/core/private/Thread.cpp) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 2,563,742,248 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const'2` |
| 855,933,091 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_function.h:std::function<void ()>::operator()() const` |
| 855,695,428 | `./nptl/./nptl/pthread_create.c:start_thread` |
| 855,681,443 | `???:0x00000000000e0e50` |
| 855,674,640 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_State_impl<std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> > >::_M_run()` |
| 855,673,615 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::operator()()` |
| 855,672,333 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/std_thread.h:void std::thread::_Invoker<std::tuple<nbn::core::Thread::Impl::run()::{lambda()#2}> >::_M_invoke<0ul>(std::_Index_tuple<0ul>)` |
| 855,666,750 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:std::__invoke_result<nbn::core::Thread::Impl::run()::{lambda()#2}>::type std::__invoke<nbn::core::Thread::Impl::run()::{lambda()#2}>(nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |
| 855,665,640 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/bits/invoke.h:void std::__invoke_impl<void, nbn::core::Thread::Impl::run()::{lambda()#2}>(std::__invoke_other, nbn::core::Thread::Impl::run()::{lambda()#2}&&)` |

</details>

## `test_CommandLineParserHelp`

- Report: [callgrind-2616983.out](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-2616983.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_CommandLineParserHelp (PID 2616983, part 1)`
- Program instructions: **5,426,474**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 5,426,474 | `???:0x000000000001d2c0` |
| 4,049,562 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 3,260,121 | `./elf/./elf/rtld.c:_dl_start` |
| 3,259,380 | `./elf/../sysdeps/unix/sysv/linux/dl-sysdep.c:_dl_sysdep_start` |
| 3,226,022 | `./elf/./elf/rtld.c:dl_main` |
| 3,114,903 | `./elf/./elf/dl-reloc.c:_dl_relocate_object` |
| 3,114,884 | `./elf/./elf/dl-reloc.c:_dl_relocate_object_no_relro` |
| 2,081,364 | `./elf/../sysdeps/generic/dl-new-hash.h:_dl_lookup_symbol_x` |
| 1,806,468 | `./elf/../sysdeps/x86_64/dl-trampoline.h:_dl_runtime_resolve_xsave` |
| 1,750,388 | `./elf/./elf/dl-runtime.c:_dl_fixup` |

</details>

## `test_ApplicationSegfault`

- Report: [callgrind-2617018.out](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-2617018.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ApplicationSegfault (PID 2617018, part 1)`
- Program instructions: **4,541,064**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| 660,728,226 | [src/nbn/core/private/Application.cpp:nbn::core::private_ns::hardwaretSignalHandler(int) (line 80)](../../src/nbn/core/private/Application.cpp#L80) |
| 660,544,773 | [src/nbn/core/private/Application.cpp:nbn::core::private_ns::stackTraceMessage[abi:cxx11](std::basic_string_view<char, std::char_traits<char> >) (line 39)](../../src/nbn/core/private/Application.cpp#L39) |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 758,777,057 | `???:elf_zlib_inflate.cold'2` |
| 660,479,745 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/stacktrace:std::ostream& std::operator<< <std::allocator<std::stacktrace_entry> >(std::ostream&, std::basic_stacktrace<std::allocator<std::stacktrace_entry> > const&)` |
| 660,461,114 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/stacktrace:std::operator<<(std::ostream&, std::stacktrace_entry const&)` |
| 660,443,541 | `/usr/lib/gcc/x86_64-linux-gnu/14/../../../../include/c++/14/stacktrace:std::stacktrace_entry::_M_get_info(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >*, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >*, int*) const` |
| 660,440,408 | `???:std::stacktrace_entry::_Info::_M_populate(unsigned long)` |
| 660,436,040 | `???:__glibcxx_backtrace_pcinfo` |
| 606,514,452 | `???:fileline_initialize` |
| 606,510,754 | `???:__glibcxx_backtrace_initialize` |

</details>

## `test_ApplicationTerminateProcess`

- Report: [callgrind-2617168.out](../../../../../../dev/builds/nbn-cpp-api-core/linux-clang19-debug/valgrind-results/callgrind/callgrind-2617168.out)
- Command: `/home/nbn/dev/builds/nbn-cpp-api-core/linux-clang19-debug/tests/nbn/core/test_ApplicationTerminateProcess (PID 2617168, part 1)`
- Program instructions: **4,660,064**

<details>
<summary>Callgrind hotspots by ownership</summary>

### nbn API

| Instructions | Function / location |
|---:|---|
| _none_ | No nbn API functions were identified in the parsed rows |

### External and runtime code

| Instructions | Function / location |
|---:|---|
| 4,660,064 | `???:0x000000000001d2c0` |
| 3,731,818 | `./elf/./elf/dl-lookup.c:_dl_lookup_symbol_x` |
| 3,267,173 | `./elf/./elf/rtld.c:_dl_start` |
| 3,266,432 | `./elf/../sysdeps/unix/sysv/linux/dl-sysdep.c:_dl_sysdep_start` |
| 3,233,074 | `./elf/./elf/rtld.c:dl_main` |
| 3,121,422 | `./elf/./elf/dl-reloc.c:_dl_relocate_object` |
| 3,121,403 | `./elf/./elf/dl-reloc.c:_dl_relocate_object_no_relro` |
| 1,840,852 | `./elf/../sysdeps/generic/dl-new-hash.h:_dl_lookup_symbol_x` |
| 1,496,953 | `./elf/./elf/dl-lookup.c:do_lookup_x` |
| 1,453,873 | `./elf/../sysdeps/x86_64/dl-trampoline.h:_dl_runtime_resolve_xsave` |

</details>
