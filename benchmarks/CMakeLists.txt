cmake_minimum_required(VERSION 3.23)

find_package(benchmark CONFIG REQUIRED)
add_executable(GreeterBenchmarks bench_main.cpp)
target_link_libraries(
    GreeterBenchmarks PRIVATE 
    GreeterLib
    benchmark::benchmark
)
