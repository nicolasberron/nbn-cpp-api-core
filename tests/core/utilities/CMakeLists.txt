set(NBN_BENCHMARK_PTHREAD_LIB)
if(UNIX)
    set(NBN_BENCHMARK_PTHREAD_LIB pthread)
endif()

option(NBN_BUILD_BENCHMARK_SANITIZERS "Build an AddressSanitizer/UBSan benchmark executable" OFF)
option(NBN_BUILD_BENCHMARK_FUZZER "Build the libFuzzer serialization target when supported" OFF)

add_executable(benchmark_serialization benchmark_serialization.cpp)
target_link_libraries(benchmark_serialization PRIVATE nbn-core ${NBN_BENCHMARK_PTHREAD_LIB} atomic)

if(NBN_BUILD_BENCHMARK_SANITIZERS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_executable(benchmark_serialization_sanitized benchmark_serialization.cpp)
        target_link_libraries(benchmark_serialization_sanitized PRIVATE nbn-core ${NBN_BENCHMARK_PTHREAD_LIB} atomic)
        target_compile_options(benchmark_serialization_sanitized PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(benchmark_serialization_sanitized PRIVATE -fsanitize=address,undefined)
    else()
        message(WARNING "NBN_BUILD_BENCHMARK_SANITIZERS requires GCC or Clang; sanitized benchmark will not be built")
    endif()
endif()

if(NBN_BUILD_BENCHMARK_FUZZER)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_executable(fuzz_serialization fuzz_serialization.cpp)
        target_link_libraries(fuzz_serialization PRIVATE nbn-core ${NBN_BENCHMARK_PTHREAD_LIB} atomic)
        target_compile_options(fuzz_serialization PRIVATE -fsanitize=fuzzer,address,undefined -fno-omit-frame-pointer)
        target_link_options(fuzz_serialization PRIVATE -fsanitize=fuzzer,address,undefined)

        add_executable(json_parser_probe json_parser_probe.cpp)
        target_link_libraries(json_parser_probe PRIVATE nbn-core ${NBN_BENCHMARK_PTHREAD_LIB} atomic)

        add_custom_target(fuzz_serialization_corpus ALL
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/fuzz_corpus
                ${CMAKE_CURRENT_BINARY_DIR}/fuzz_corpus
        )
    else()
        message(FATAL_ERROR "NBN_BUILD_BENCHMARK_FUZZER requires a Clang compiler with libFuzzer support; configure a separate Clang build directory")
    endif()
endif()
