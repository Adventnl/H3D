# Phase 1 — Foundation

Phase 1 builds the engineering base of Forge3D. It is deliberately invisible
to artists: the output is a build system, a core library, runnable shells,
tests, benchmarks and documentation. Everything later — scene database,
viewport, modeling, rendering, animation — sits on top of this code.

## What Phase 1 builds

### Build system

- CMake ≥ 3.24 with presets (`debug`, `release`, `relwithdebinfo`), Ninja as
  the recommended generator.
- Shared CMake modules in `cmake/` apply one consistent set of compiler
  options, warnings and (optional) sanitizers to every target via
  `forge_apply_target_defaults()`.
- Feature toggles are cache options (`FORGE_BUILD_TESTS`,
  `FORGE_ENABLE_PROFILING`, ...) that map to compile definitions consumed by
  `forge/foundation/config.hpp`.

### The foundation library (`forge_foundation`)

One static library, namespace `forge`, six sub-areas:

| Area | Contents |
|---|---|
| `foundation/` | platform/compiler/config detection, version + build info, `Error`/`Result`/`Expected`, logging, assertions, `ScopeExit`, time utilities |
| `math/` | `Vec2/3/4`, `Mat3`, `Mat4` (column-major, right-handed), `Quat`, `Transform`, constants and angle helpers |
| `memory/` | `Allocator` interface, `SystemAllocator`, `LinearAllocator`, `MemoryTracker` |
| `filesystem/` | `Path`, whole-file read/write helpers, `VirtualFileSystem` with traversal protection |
| `threading/` | `ThreadPool` (futures, graceful drain-on-shutdown), `TaskGroup`, `JobSystem::parallel_for` |
| `profiling/` | `Profiler`, `ProfilerScope` macros, Chrome-trace writer |

Key design decisions:

- **Own `Expected<T, E>`** instead of `std::expected`: identical behavior on
  every supported standard library, and an easy later migration since the API
  mirrors the standard type.
- **Column-major, right-handed math with OpenGL-style clip space**, fixed now
  so the Phase 4 viewport and Phase 6 renderers cannot disagree.
- **Recoverable errors return `Result`; programmer errors assert.** Nothing
  in the foundation throws as API contract (the thread pool transports
  user-task exceptions through futures, which is the standard mechanism).
- **No work stealing yet.** The job system is a plain pool + chunked
  `parallel_for`. It is good enough for Phase 1 workloads and trivially
  replaceable because callers only see `submit`/`parallel_for`.

### Executables

- `forge_desktop` — the future application entry point. Today: initialize
  logging, print build info, bring up the job system, run a few profiled
  frames of placeholder work, shut down cleanly, optionally write a Chrome
  trace. The Phase 2 window manager grows out of this skeleton.
- `forge_cli` — `--help`, `--version`, `doctor` (build info, hardware
  threads, working/temp directory, temp write probe), `paths`,
  `benchmark-info`.
- `forge_bench` / `forge_benchmarks` — the benchmark runner (one installable,
  one in-tree; both link the same benchmark object library).
- `forge_project_info` (tools) — dumps project/build/feature configuration.

### Test and benchmark infrastructure

Internal frameworks (no GoogleTest/Catch2/Google Benchmark):
self-registering `FORGE_TEST_CASE` / `FORGE_BENCHMARK` functions, a runner
with name filtering, failure locations and non-zero exit on failure. CTest
wraps the test runner.

## What Phase 1 intentionally avoids

GPU APIs, windows/UI, scene data, meshes, file formats, import/export,
materials, nodes, animation, simulation, Python, asset systems — and all
external dependencies. Building those now would freeze interfaces before the
foundation has proven itself.

## How later phases depend on this code

- Phase 2 (shell/UI) consumes logging, time, jobs, profiling and builds the
  application object out of the `forge_desktop` runtime skeleton.
- Phase 3 (scene) builds data-blocks on the allocator interfaces and reports
  errors through `Result`.
- Phase 4+ (viewport/render) rely on the fixed math conventions and the
  profiler for frame analysis.
- Every phase adds suites to the existing test/benchmark frameworks and new
  modules under the dependency rules in
  [module_boundaries.md](module_boundaries.md).
