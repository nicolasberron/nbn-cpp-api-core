project(nbn-core)

set(NBN_CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Application.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Color.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CommandLineParser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ConcurrentQueue.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ElapsedTimer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/filesystem/Watcher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FiniteStateMachine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/JsonMigratorBase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Object.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ObjectSync.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Timer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UnitTests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/detail/ObjectFeature.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/detail/serialization.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/private/Application.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/private/ApplicationMainThread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/private/ApplicationThreadsManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/private/Object.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/private/Thread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/semver.cpp
)

if(NBN_BUILD_SHARED_LIBS)
    add_library(${PROJECT_NAME} SHARED ${NBN_CORE_SOURCES})
    if(WIN32)
        target_compile_definitions(${PROJECT_NAME} PRIVATE NBN_EXPORT_DLL_core)
    endif()
else()
    add_library(${PROJECT_NAME} STATIC ${NBN_CORE_SOURCES})
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES EXPORT_NAME core)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

if(NBN_LLVM_COVERAGE_ENABLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE NBN_LLVM_COVERAGE_ENABLE)
endif()

# GCC's C++23 stacktrace implementation is provided by the experimental
# support library on current Linux toolchains. Link it transitively so
# consumers of nbn-core resolve std::stacktrace as well.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT MSVC))
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libstdc++exp.a
        OUTPUT_VARIABLE NBN_STDCXXEXP_LIBRARY
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(NOT EXISTS "${NBN_STDCXXEXP_LIBRARY}")
        find_library(NBN_STDCXXEXP_LIBRARY NAMES stdc++exp)
    endif()
    if(NBN_STDCXXEXP_LIBRARY)
        target_link_libraries(${PROJECT_NAME} PUBLIC stdc++exp)
    else()
        message(FATAL_ERROR "C++23 stacktrace support requires the stdc++exp library")
    endif()
endif()

if(NBN_ENABLE_WARNINGS)
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /W4)
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()

install(TARGETS ${PROJECT_NAME}
    EXPORT nbn-cpp-api-core-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(NBN_BUILD_STATIC_LIBS AND NBN_BUILD_SHARED_LIBS)
    add_library(${PROJECT_NAME}-static STATIC ${NBN_CORE_SOURCES})
    set_target_properties(${PROJECT_NAME}-static PROPERTIES OUTPUT_NAME ${PROJECT_NAME}-static EXPORT_NAME core-static)
    if(NBN_STDCXXEXP_LIBRARY)
        target_link_libraries(${PROJECT_NAME}-static PUBLIC stdc++exp)
    endif()
    install(TARGETS ${PROJECT_NAME}-static
        EXPORT nbn-cpp-api-core-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()

if(MSVC)
    target_link_libraries(${PROJECT_NAME} dbghelp)
    if(TARGET ${PROJECT_NAME}-static)
        target_link_libraries(${PROJECT_NAME}-static dbghelp)
    endif()
endif()

option(NBN_CORE_DOXYGEN_ENABLE "Generate nbn::core classes documentation with Doxygen" ON)

if(NBN_CORE_DOXYGEN_ENABLE)
    find_package(Doxygen QUIET)

    if(DOXYGEN_FOUND)
        set(NBN_CORE_PUBLIC_HEADERS
            ${CMAKE_CURRENT_SOURCE_DIR}/Application.h
            ${CMAKE_CURRENT_SOURCE_DIR}/CommandLineParser.h
            ${CMAKE_CURRENT_SOURCE_DIR}/ConcurrentQueue.h
            ${CMAKE_CURRENT_SOURCE_DIR}/ConcurrentStack.h
            ${CMAKE_CURRENT_SOURCE_DIR}/ElapsedTimer.h
            ${CMAKE_CURRENT_SOURCE_DIR}/FiniteStateMachine.h
            ${CMAKE_CURRENT_SOURCE_DIR}/filesystem/Watcher.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Color.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Logger.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Object.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Ring.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Task.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Timer.h
            ${CMAKE_CURRENT_SOURCE_DIR}/UnitTests.h
            ${CMAKE_CURRENT_SOURCE_DIR}/XRefUnorderedMap.h
            ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/Interface.h
            ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/concurrency.h
            ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/object.h
            ${CMAKE_CURRENT_SOURCE_DIR}/detail/serialization.h
        )

        set(DOXYGEN_PROJECT_NAME "nbn::core")
        set(DOXYGEN_PROJECT_BRIEF "nbn core API reference")
        set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/doc/core/doc")
        set(DOXYGEN_HTML_OUTPUT "classes")
        set(DOXYGEN_GENERATE_HTML YES)
        set(DOXYGEN_GENERATE_LATEX NO)
        set(DOXYGEN_EXTRACT_PRIVATE NO)
        set(DOXYGEN_EXTRACT_STATIC NO)
        set(DOXYGEN_WARN_IF_UNDOCUMENTED YES)
        set(DOXYGEN_RECURSIVE NO)

        doxygen_add_docs(nbn_core_classes_docs
            ${NBN_CORE_PUBLIC_HEADERS}
            COMMENT "Generate nbn::core class documentation in doc/core/doc/classes"
        )
    else()
        message(STATUS "NBN: Doxygen not found; target nbn_core_classes_docs is disabled")
    endif()
endif()
