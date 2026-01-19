#include <benchmark/benchmark.h>

#include <string>

#include "greeter/Greeter.h"

static void BM_Greeter_Greet(benchmark::State& states) {
    Greeter my_greeter;
    // This loop is the core of the benchmark
    for ([[maybe_unused]] auto state : states) {
        // This code gets timed
        std::string result = my_greeter.greet();
        // Prevent the result from being optimized away
        benchmark::DoNotOptimize(result);
    }
}
// Register the function as a benchmark
BENCHMARK(BM_Greeter_Greet);

static void BM_Greeter_Greet_Name(benchmark::State& states) {
    // state.range(0) is the first argument to the benchmark
    const auto  size = static_cast<std::string::size_type>(states.range(0));
    std::string name(size, 'x');
    Greeter     my_greeter(name);

    for ([[maybe_unused]] auto state : states) {
        std::string result = my_greeter.greet_name();
        benchmark::DoNotOptimize(result);
    }
}
// Register benchmarks with different arguments
BENCHMARK(BM_Greeter_Greet_Name)->Range(8, 32768);
BENCHMARK(BM_Greeter_Greet_Name)->Arg(262'144)->Complexity();

// // Run all benchmarks
BENCHMARK_MAIN();
