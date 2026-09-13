cmake_minimum_required(VERSION 3.16.3)

if(NOT DEFINED ENV{NBN_CPP_API_CMAKE_DIR})
    message(FATAL_ERROR "NBN_CPP_API_CMAKE_DIR is not set; install the Conan build requirements first")
endif()
include("$ENV{NBN_CPP_API_CMAKE_DIR}/nbn-project.cmake")
nbn_project_initialize(
    VERSION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/VERSION"
    VERSION_VAR NBN_PROJECT_VERSION
    LLVM_COVERAGE_DEFAULT OFF
)

project(nbn-cpp-api-core VERSION "${NBN_PROJECT_VERSION}" LANGUAGES CXX)
nbn_project_configure_defaults()

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nbn/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/nbn/config.h
)

option(NBN_BUILD_BENCHMARKS "Build standalone benchmark targets" OFF)
option(NBN_CORE_DOXYGEN_ENABLE "Generate nbn::core classes documentation with Doxygen" ON)

add_library(nbn-benchmark INTERFACE)
set_target_properties(nbn-benchmark PROPERTIES EXPORT_NAME benchmark)
target_include_directories(nbn-benchmark INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tests/core/utilities>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

add_subdirectory(src)

nbn_project_enable_tests()

if(NBN_BUILD_BENCHMARKS)
    add_subdirectory(tests/core/utilities)
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/nbn/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nbn
    FILES_MATCHING PATTERN "*.h"
    PATTERN "CMakeFiles" EXCLUDE
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/nbn/config.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nbn)
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/core/utilities/benchmark_support.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nbn/benchmark
)
install(TARGETS nbn-benchmark
    EXPORT nbn-cpp-api-core-targets
)

nbn_install_cmake_package(
    PACKAGE_NAME nbn-cpp-api-core
    CONFIG_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/nbn-cpp-api-core-config.cmake.in"
)
