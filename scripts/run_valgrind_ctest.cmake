cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED CTEST_BINARY_DIRECTORY OR NOT IS_DIRECTORY "${CTEST_BINARY_DIRECTORY}")
    message(FATAL_ERROR "CTEST_BINARY_DIRECTORY must point to a configured build directory")
endif()
if(NOT DEFINED VALGRIND_TOOL OR NOT VALGRIND_TOOL MATCHES "^(callgrind|massif|memcheck)$")
    message(FATAL_ERROR "VALGRIND_TOOL must be callgrind, massif, or memcheck")
endif()
if(NOT DEFINED VALGRIND_OUTPUT_DIR)
    message(FATAL_ERROR "VALGRIND_OUTPUT_DIR must be specified")
endif()

get_filename_component(CTEST_SOURCE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
find_program(CTEST_MEMORYCHECK_COMMAND valgrind REQUIRED)
# A profiling run is a fresh measurement. Remove reports from earlier runs so
# the analyzer cannot mistake historical output for results from this run.
file(REMOVE_RECURSE "${VALGRIND_OUTPUT_DIR}")
file(MAKE_DIRECTORY "${VALGRIND_OUTPUT_DIR}")

set(CTEST_MEMORYCHECK_COMMAND_OPTIONS "--tool=${VALGRIND_TOOL}")
if(VALGRIND_TOOL STREQUAL "callgrind")
    string(APPEND CTEST_MEMORYCHECK_COMMAND_OPTIONS " --callgrind-out-file=${VALGRIND_OUTPUT_DIR}/callgrind-%p.out")
elseif(VALGRIND_TOOL STREQUAL "massif")
    string(APPEND CTEST_MEMORYCHECK_COMMAND_OPTIONS " --massif-out-file=${VALGRIND_OUTPUT_DIR}/massif-%p.out")
else()
    string(APPEND CTEST_MEMORYCHECK_COMMAND_OPTIONS
           " --leak-check=full"
           " --show-leak-kinds=all"
           " --track-origins=yes"
           " --log-file=${VALGRIND_OUTPUT_DIR}/memcheck-%p.log")
endif()

set(CTEST_MEMORYCHECK_TYPE "Valgrind")
set(CTEST_TEST_TIMEOUT 300)
if(NOT DEFINED VALGRIND_EXCLUDE_TESTS)
    set(VALGRIND_EXCLUDE_TESTS "test_ConcurrentQueue|test_Logger")
endif()
ctest_start(Experimental)
ctest_memcheck(BUILD "${CTEST_BINARY_DIRECTORY}" EXCLUDE "${VALGRIND_EXCLUDE_TESTS}" RETURN_VALUE test_result)
if(test_result)
    message(FATAL_ERROR "Valgrind CTest run failed with exit code ${test_result}")
endif()
