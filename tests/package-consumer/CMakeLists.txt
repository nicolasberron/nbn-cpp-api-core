cmake_minimum_required(VERSION 3.16.3)

project(nbn-cpp-api-core-package-consumer LANGUAGES CXX)

find_package(nbn-cpp-api-core CONFIG REQUIRED)

add_executable(nbn-cpp-api-core-package-consumer main.cpp)
target_link_libraries(nbn-cpp-api-core-package-consumer PRIVATE nbn::core nbn::benchmark)
target_compile_features(nbn-cpp-api-core-package-consumer PRIVATE cxx_std_23)