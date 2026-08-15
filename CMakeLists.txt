cmake_minimum_required(VERSION 3.16.3)

set(NBN_VERSION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/VERSION")
if(NOT EXISTS "${NBN_VERSION_FILE}")
    message(FATAL_ERROR "Missing project version file: ${NBN_VERSION_FILE}")
endif()
file(STRINGS "${NBN_VERSION_FILE}" NBN_PROJECT_VERSION LIMIT_COUNT 1)
string(STRIP "${NBN_PROJECT_VERSION}" NBN_PROJECT_VERSION)
if(NOT NBN_PROJECT_VERSION MATCHES "^[0-9]+\\.[0-9]+\\.[0-9]+([.-][0-9A-Za-z.-]+)?$")
    message(FATAL_ERROR "Invalid project version in ${NBN_VERSION_FILE}: '${NBN_PROJECT_VERSION}'")
endif()

# Set coverage defaults before loading nbn-init.cmake. The build-system package
# selects gcov by default unless it can see that LLVM coverage was requested.
# Keep ordinary builds free of coverage instrumentation while preserving
# explicit -D overrides from coverage tasks or callers.
if(NOT DEFINED NBN_LLVM_COVERAGE_ENABLE)
    set(NBN_LLVM_COVERAGE_ENABLE OFF)
endif()
if(NOT DEFINED NBN_GCOV_ENABLE)
    set(NBN_GCOV_ENABLE OFF)
endif()

if(DEFINED ENV{NBN_CPP_API_CMAKE_DIR} AND EXISTS "$ENV{NBN_CPP_API_CMAKE_DIR}/nbn-init.cmake")
    set(NBN_CPP_API_CMAKE_DIR "$ENV{NBN_CPP_API_CMAKE_DIR}")
    include("${NBN_CPP_API_CMAKE_DIR}/nbn-init.cmake")
elseif(DEFINED ENV{NBN_CPP_API_CMAKE_DIR})
    message(WARNING "NBN_CPP_API_CMAKE_DIR does not contain nbn-init.cmake; using standalone CMake defaults")
endif()

project(nbn-cpp-api-core VERSION "${NBN_PROJECT_VERSION}" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nbn/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/nbn/config.h
)

option(NBN_BUILD_SHARED_LIBS "Build shared libraries" ON)
option(NBN_BUILD_STATIC_LIBS "Build static libraries" OFF)
option(NBN_BUILD_TESTS "Build core test targets" ON)
option(NBN_BUILD_BENCHMARKS "Build standalone benchmark targets" OFF)
option(NBN_LLVM_COVERAGE_ENABLE "Enable LLVM source-based coverage instrumentation" OFF)
option(NBN_CORE_DOXYGEN_ENABLE "Generate nbn::core classes documentation with Doxygen" ON)
option(NBN_ENABLE_WARNINGS "Enable the project's compiler warning policy" ON)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(nbn-benchmark INTERFACE)
set_target_properties(nbn-benchmark PROPERTIES EXPORT_NAME benchmark)
target_include_directories(nbn-benchmark INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tests/core/utilities>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# nbn-init.cmake normally defines and configures this option. Keep the same
# contract in the standalone project so clang-tidy is not lost when the core
# repository is configured with a compatible build-system package.
if(NOT DEFINED NBN_CLANG_TIDY_ENABLE)
    option(NBN_CLANG_TIDY_ENABLE "Use clang-tidy" ON)
endif()

if(NBN_CLANG_TIDY_ENABLE AND NOT CMAKE_CXX_CLANG_TIDY)
    find_program(NBN_CLANG_TIDY_EXECUTABLE clang-tidy)
    if(NBN_CLANG_TIDY_EXECUTABLE)
        set(NBN_CLANG_TIDY_CHECKS
            "-*,bugprone-*,cppcoreguidelines-*,-cppcoreguidelines-special-member-functions"
        )
        set(CMAKE_CXX_CLANG_TIDY
            "${NBN_CLANG_TIDY_EXECUTABLE};-checks=${NBN_CLANG_TIDY_CHECKS};-warnings-as-errors=*;-header-filter=.*"
        )
    else()
        message(WARNING "NBN_CLANG_TIDY_ENABLE is ON but clang-tidy was not found")
    endif()
endif()

if(NOT NBN_BUILD_SHARED_LIBS AND NOT NBN_BUILD_STATIC_LIBS)
    message(FATAL_ERROR "At least one of NBN_BUILD_SHARED_LIBS or NBN_BUILD_STATIC_LIBS must be ON")
endif()

if(NBN_LLVM_COVERAGE_ENABLE)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(FATAL_ERROR "NBN_LLVM_COVERAGE_ENABLE requires a Clang compiler")
    endif()
    add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
    add_link_options(-fprofile-instr-generate)
endif()

add_subdirectory(src)

if(NBN_BUILD_TESTS)
    if(NOT DEFINED NBN_CPP_API_CMAKE_DIR)
        message(FATAL_ERROR "NBN_BUILD_TESTS requires NBN_CPP_API_CMAKE_DIR and the nbn-unit-tests.cmake helper")
    endif()
    include("${NBN_CPP_API_CMAKE_DIR}/nbn-unit-tests.cmake")
    add_subdirectory(tests)
    enable_testing()
endif()

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

set(NBN_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/nbn-cpp-api-core")
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/nbn-cpp-api-core-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/nbn-cpp-api-core-config.cmake
    INSTALL_DESTINATION ${NBN_INSTALL_CMAKEDIR}
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/nbn-cpp-api-core-config-version.cmake
    COMPATIBILITY SameMajorVersion
)
install(EXPORT nbn-cpp-api-core-targets
    FILE nbn-cpp-api-core-targets.cmake
    NAMESPACE nbn::
    DESTINATION ${NBN_INSTALL_CMAKEDIR}
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/nbn-cpp-api-core-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/nbn-cpp-api-core-config-version.cmake
    DESTINATION ${NBN_INSTALL_CMAKEDIR}
)
