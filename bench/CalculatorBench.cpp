#include <benchmark/benchmark.h>
#include "myproject/ModuleB.h"

using namespace myproject;

static void BM_Addition(benchmark::State& state) {
    Calculator calc;
    const double a = 42.0;
    const double b = 18.7;
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(calc.add(a, b));
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_Addition);

static void BM_Multiplication(benchmark::State& state) {
    Calculator calc;
    const double a = 42.0;
    const double b = 18.7;
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(calc.multiply(a, b));
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_Multiplication);

static void BM_Division(benchmark::State& state) {
    Calculator calc;
    const double a = 42.0;
    const double b = 2.5;
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(calc.divide(a, b));
        benchmark::ClobberMemory();
    }
}
BENCHMARK(BM_Division);

BENCHMARK_MAIN();