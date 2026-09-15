set(NBN_BENCHMARK_PTHREAD_LIB)
if(UNIX)
    set(NBN_BENCHMARK_PTHREAD_LIB pthread)
endif()

add_executable(benchmark_serialization benchmark_serialization.cpp)
target_link_libraries(benchmark_serialization PRIVATE nbn-core ${NBN_BENCHMARK_PTHREAD_LIB} atomic)

if(NBN_INSTRUMENTATION MATCHES "^(asan|ubsan)$")
    add_executable(benchmark_serialization_sanitized benchmark_serialization.cpp)
    target_link_libraries(benchmark_serialization_sanitized PRIVATE nbn-core ${NBN_BENCHMARK_PTHREAD_LIB} atomic)
endif()

if(NBN_INSTRUMENTATION STREQUAL "fuzz")
    add_executable(fuzz_serialization fuzz_serialization.cpp)
    target_link_libraries(fuzz_serialization PRIVATE nbn-core ${NBN_BENCHMARK_PTHREAD_LIB} atomic)
    nbn_enable_fuzzer(fuzz_serialization)

    add_executable(json_parser_probe json_parser_probe.cpp)
    target_link_libraries(json_parser_probe PRIVATE nbn-core ${NBN_BENCHMARK_PTHREAD_LIB} atomic)

    add_custom_target(fuzz_serialization_corpus ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/fuzz_corpus
            ${CMAKE_CURRENT_BINARY_DIR}/fuzz_corpus
    )
endif()
