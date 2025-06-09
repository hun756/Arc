#include <benchmark/benchmark.h>
#include "myproject/ModuleA.h"

using namespace myproject;

static void BM_StringToUpper(benchmark::State& state) {
    const std::string test = "Hello World! This is a benchmark test.";
    StringProcessor sp;
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(sp.toUpper(test));
        benchmark::ClobberMemory();
    }
    
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_StringToUpper);

static void BM_StringToUpperScaled(benchmark::State& state) {
    const int length = state.range(0);
    std::string test(length, 'a');
    StringProcessor sp;
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(sp.toUpper(test));
        benchmark::ClobberMemory();
    }
    
    state.SetItemsProcessed(state.iterations() * length);
    state.SetBytesProcessed(state.iterations() * length);
}
BENCHMARK(BM_StringToUpperScaled)->Range(8, 8<<10);

static void BM_RemoveSpaces(benchmark::State& state) {
    const std::string test = "Hello   World!  This  is  a   benchmark   test.";
    StringProcessor sp;
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(sp.removeSpaces(test));
        benchmark::ClobberMemory();
    }
    
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_RemoveSpaces);

BENCHMARK_MAIN();