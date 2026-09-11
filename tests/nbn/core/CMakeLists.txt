set(NBN_CORE_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Application.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationSignalInterrupt.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationTerminateHandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationThreadsManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationUncaughtException.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Color.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CommandLineParser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ConcurrentQueue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ConcurrentStack.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ElapsedTimer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FilesystemWatcher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FiniteStateMachine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/JsonMigratorBase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Object.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ObjectSync.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Serialization.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Signal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Slots.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Thread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Timer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UnitTests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp
)

option(NBN_TSAN_BUILD "Build the ThreadSanitizer-compatible test subset" OFF)
if(NBN_TSAN_BUILD)
    list(REMOVE_ITEM NBN_CORE_TEST_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationSignalInterrupt.cpp
    )
endif()

# UI-dependent migration tests and the standalone benchmark are intentionally
# separate from the core CTest target. They require nbn-ui or have different
# execution semantics and are registered explicitly below when appropriate.

add_compile_definitions(NBN_TEST_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}")

if(UNIX)
    set(PTHREAD_LIB pthread)
else()
    set(PTHREAD_LIB)
endif()

nbn_unit_tests_add(
    SOURCE_FILES ${NBN_CORE_TEST_SOURCES}
    LINK_LIBRARIES nbn-core ${PTHREAD_LIB} atomic
    COMPILE_DEFINITIONS NBN_CONCURRENT_QUEUE_TEST_HOOK NBN_LOGGER_TEST_HOOK
)

# The built-in --help action intentionally exits the process. Keep it outside
# the in-process unit-test runner so the exit path is covered by a real test.
add_executable(test_CommandLineParserHelp CommandLineParserHelp.cpp)
target_link_libraries(test_CommandLineParserHelp PRIVATE nbn-core ${PTHREAD_LIB} atomic)
add_test(NAME test_CommandLineParserHelp COMMAND test_CommandLineParserHelp)

# Fatal logging must preserve its throwing contract before Application::init()
# creates the logger used by the regular unit-test runner.
add_executable(test_ApplicationFatalBeforeInit ApplicationFatalBeforeInit.cpp)
target_link_libraries(test_ApplicationFatalBeforeInit PRIVATE nbn-core ${PTHREAD_LIB} atomic)
add_test(NAME test_ApplicationFatalBeforeInit COMMAND test_ApplicationFatalBeforeInit)

# The crash test must run before Application::init() starts Logger's worker
# thread. Otherwise a forked crash child inherits a live std::jthread control
# block that cannot be reclaimed on the _exit() signal-handler path.
if(NOT NBN_TSAN_BUILD)
    add_executable(test_ApplicationSegfault ApplicationSegfault.cpp)
    target_link_libraries(test_ApplicationSegfault PRIVATE nbn-core ${PTHREAD_LIB} atomic)
    add_test(NAME test_ApplicationSegfault COMMAND test_ApplicationSegfault)
    set_tests_properties(test_ApplicationSegfault PROPERTIES WILL_FAIL TRUE)

    add_executable(test_ApplicationTerminateProcess ApplicationTerminateProcess.cpp)
    target_link_libraries(test_ApplicationTerminateProcess PRIVATE nbn-core ${PTHREAD_LIB} atomic)
    add_test(NAME test_ApplicationTerminateProcess COMMAND test_ApplicationTerminateProcess)
endif()

set(NBN_TEST_TIMEOUT_SECONDS 30 CACHE STRING "Maximum runtime for an individual core test")
get_property(NBN_CORE_REGISTERED_TESTS DIRECTORY PROPERTY TESTS)
set_tests_properties(${NBN_CORE_REGISTERED_TESTS} PROPERTIES TIMEOUT ${NBN_TEST_TIMEOUT_SECONDS})

target_compile_definitions(nbn-core PRIVATE NBN_LOGGER_TEST_HOOK)
if(NBN_BUILD_TESTS)
    target_compile_definitions(nbn-core PRIVATE NBN_CONCURRENT_QUEUE_TEST_HOOK)
endif()

# `test_ApplicationSegfault` validates the crash handler in a child process and
# returns normally after checking the child's exit status.
set_tests_properties(test_Application PROPERTIES WILL_FAIL TRUE)

