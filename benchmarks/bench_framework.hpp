#pragma once

// Forge3D's internal micro-benchmark framework.
//
//   FORGE_BENCHMARK("math.vec3_normalize")
//   {
//       for (std::uint64_t i = 0; i < state.iterations(); ++i)
//       {
//           forge::bench::do_not_optimize(value.normalized());
//       }
//   }
//
// The runner calibrates the iteration count until each benchmark runs for at
// least a minimum wall time, then reports ns/op and ops/s. Numbers are a
// regression baseline, not a rigorous microbenchmark.

#include <atomic>
#include <cstdint>
#include <string>
#include <string_view>

#include "forge/foundation/compiler.hpp"
#include "forge/foundation/macros.hpp"

namespace forge::bench
{

class State
{
public:
    explicit State(std::uint64_t iterations) : iterations_(iterations) {}

    /// Number of times the measured operation should be executed.
    [[nodiscard]] std::uint64_t iterations() const noexcept { return iterations_; }

private:
    std::uint64_t iterations_;
};

using BenchFunction = void (*)(State&);

/// Register a benchmark; invoked via FORGE_BENCHMARK.
bool register_benchmark(std::string name, BenchFunction function);

/// Run benchmarks whose name contains `filter` (all when empty).
/// `min_seconds` is the calibration target per benchmark. Returns 0; the
/// process only fails when a benchmark crashes.
int run_all_benchmarks(std::string_view filter = {}, double min_seconds = 0.2);

/// Prevent the optimizer from discarding a computed value.
template <typename Value>
FORGE_FORCE_INLINE void do_not_optimize(const Value& value)
{
#if FORGE_COMPILER_MSVC
    // Touching the bytes through a volatile pointer defeats dead-code
    // elimination; the fence stops reordering around the measurement.
    [[maybe_unused]] const volatile char* sink =
        &reinterpret_cast<const volatile char&>(value);
    std::atomic_signal_fence(std::memory_order_acq_rel);
#else
    __asm__ __volatile__("" : : "g"(&value) : "memory");
#endif
}

} // namespace forge::bench

#define FORGE_BENCHMARK_IMPL(name, function_name, registrar_name)              \
    static void function_name(::forge::bench::State& state);                   \
    [[maybe_unused]] static const bool registrar_name =                        \
        ::forge::bench::register_benchmark(name, &function_name);              \
    static void function_name([[maybe_unused]] ::forge::bench::State& state)

#define FORGE_BENCHMARK_EXPAND(name, id) \
    FORGE_BENCHMARK_IMPL(name, FORGE_CONCAT(forge_bench_function_, id), \
                         FORGE_CONCAT(forge_bench_registrar_, id))

#define FORGE_BENCHMARK(name) FORGE_BENCHMARK_EXPAND(name, __COUNTER__)
