#include "bench_framework.hpp"

#include <algorithm>
#include <cstdio>
#include <vector>

#include "forge/foundation/time.hpp"

namespace forge::bench
{
namespace
{

struct Benchmark
{
    std::string name;
    BenchFunction function = nullptr;
};

std::vector<Benchmark>& registry()
{
    static std::vector<Benchmark> benchmarks;
    return benchmarks;
}

double run_once(const Benchmark& benchmark, std::uint64_t iterations)
{
    State state(iterations);
    const Stopwatch stopwatch = Stopwatch::start_new();
    benchmark.function(state);
    return stopwatch.elapsed().seconds();
}

} // namespace

bool register_benchmark(std::string name, BenchFunction function)
{
    registry().push_back(Benchmark{std::move(name), function});
    return true;
}

int run_all_benchmarks(std::string_view filter, double min_seconds)
{
    std::printf("%-36s %14s %12s %14s %16s\n", "benchmark", "iterations", "total ms",
                "ns/op", "ops/s");
    std::printf("%-36s %14s %12s %14s %16s\n", "---------", "----------", "--------",
                "-----", "-----");

    int executed = 0;
    for (const Benchmark& benchmark : registry())
    {
        if (!filter.empty() && benchmark.name.find(filter) == std::string::npos)
        {
            continue;
        }
        ++executed;

        // Calibrate: grow the iteration count until the run is long enough
        // for stable timing.
        std::uint64_t iterations = 1;
        double elapsed_seconds = run_once(benchmark, iterations);
        while (elapsed_seconds < min_seconds && iterations < (1ull << 40))
        {
            const double target = min_seconds * 1.4;
            double growth = (elapsed_seconds > 1e-9) ? target / elapsed_seconds : 1000.0;
            growth = std::clamp(growth, 2.0, 1000.0);
            iterations = static_cast<std::uint64_t>(static_cast<double>(iterations) * growth);
            elapsed_seconds = run_once(benchmark, iterations);
        }

        const double ns_per_op =
            elapsed_seconds * 1e9 / static_cast<double>(iterations);
        const double ops_per_second =
            (elapsed_seconds > 0.0) ? static_cast<double>(iterations) / elapsed_seconds
                                    : 0.0;

        std::printf("%-36s %14llu %12.3f %14.2f %16.0f\n", benchmark.name.c_str(),
                    static_cast<unsigned long long>(iterations),
                    elapsed_seconds * 1e3, ns_per_op, ops_per_second);
    }

    if (executed == 0)
    {
        std::printf("no benchmarks matched\n");
    }
    return 0;
}

} // namespace forge::bench
