# Benchmarks

Forge3D uses a small internal benchmark framework
(`benchmarks/bench_framework.hpp`). The numbers are a regression baseline,
not a rigorous microbenchmark suite — treat run-to-run noise accordingly.

## Running

```bash
./build/release/benchmarks/forge_benchmarks                 # in-tree runner
./build/release/apps/forge_bench/forge_bench                # same suites
./build/release/apps/forge_bench/forge_bench --filter math.
./build/release/apps/forge_bench/forge_bench --min-time 0.5
```

Always benchmark **release** builds; debug numbers are meaningless.

Output:

```txt
benchmark                                iterations     total ms          ns/op            ops/s
---------                                ----------     --------          -----            -----
math.vec3_normalize                        33554432      210.123           6.26        159744000
...
```

## How it works

Each benchmark is a function receiving a `State`; it loops
`state.iterations()` times over the measured operation. The runner grows the
iteration count until a run lasts at least `--min-time` seconds (default
0.2 s), then reports the final run.

`forge::bench::do_not_optimize(value)` keeps the optimizer from deleting the
measured work — wrap every computed result with it.

## Writing a benchmark

```cpp
#include "bench_framework.hpp"
#include "forge/math/vec3.hpp"

FORGE_BENCHMARK("math.vec3_length")
{
    forge::Vec3 value{1.0f, 2.0f, 3.0f};
    for (std::uint64_t i = 0; i < state.iterations(); ++i)
    {
        value.x = static_cast<float>(i % 31);   // defeat hoisting
        forge::bench::do_not_optimize(value.length());
    }
}
```

Add the file to `benchmarks/CMakeLists.txt` (the `forge_bench_core` object
library). Benchmarks never fail the build or CI unless they crash.

## Current suites

- `math.*` — Vec3 normalize/dot/cross, Mat4 multiply, Quat rotate
- `memory.*` — linear allocator allocate/reset, system allocator alloc/free
- `threading.*` — thread-pool tiny tasks, submit+future round trip
- `filesystem.*` — path normalize, join, decompose
