#include <benchmark/benchmark.h>

#include <string>

#include "greeter/Greeter.h"


static void BM_Greeter_Greet(benchmark::State& state) {
    Greeter g;
    // This loop is the core of the benchmark
    // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
    for (auto _ : state) {
        // This code gets timed
        std::string result = g.greet();
        // Prevent the result from being optimized away
        benchmark::DoNotOptimize(result);
    }
}
// Register the function as a benchmark
BENCHMARK(BM_Greeter_Greet);

static void BM_Greeter_Greet_Name(benchmark::State& state) {
    // state.range(0) is the first argument to the benchmark
    std::string name(state.range(0), 'x');
    Greeter g(name);

    // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
    for (auto _ : state) {
        std::string result = g.greet_name();
        benchmark::DoNotOptimize(result);
    }
}
// Register benchmarks with different arguments
BENCHMARK(BM_Greeter_Greet_Name)->Range(8, 32768);
BENCHMARK(BM_Greeter_Greet_Name)->Arg(32768 * 2)->Complexity();

// // Run all benchmarks
BENCHMARK_MAIN();
