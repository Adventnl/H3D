# Forge3D

Forge3D is a from-scratch, production-scale 3D digital content creation
application targeting long-term, Blender-class capability: modeling,
sculpting, UV editing, materials, shader nodes, geometry nodes, animation,
rigging, 2D/3D drawing, simulation, rendering, compositing, motion tracking,
video editing, asset libraries, Python scripting, add-ons and studio pipeline
workflows.

This repository currently contains **Phase 1: the engineering foundation** —
the build system, core runtime library, test/benchmark infrastructure and
documentation that every later system depends on. There is intentionally no
viewport, renderer, scene database or UI yet.

## Status

| Component | State |
|---|---|
| CMake build (C++23, presets, CI) | done |
| `forge_foundation` library | done |
| `forge_desktop` runtime shell | done (startup/frame-loop/shutdown skeleton, no window) |
| `forge_cli` (`--version`, `doctor`, `paths`, `benchmark-info`) | done |
| `forge_bench` / `forge_benchmarks` | done |
| Unit tests (internal framework) | done |
| Everything artist-facing | later phases |

## The 10-phase roadmap

1. **Foundation** — build system, logging, assertions, math, memory,
   filesystem/VFS, threading and job system, profiling, tests, benchmarks.
2. **Application shell** — window manager, input, dockable UI, editors,
   workspaces, operators, undo/redo.
3. **Scene database** — data-blocks, IDs, collections, dependency graph,
   native file format, autosave/crash recovery.
4. **Viewport** — GPU abstraction, render graph, navigation, selection,
   transforms, gizmos, primitives.
5. **Geometry and modeling** — mesh structure, edit mode, modeling tools,
   modifiers, UV editing.
6. **Materials and rendering** — shader nodes, real-time renderer, path
   tracer, baking.
7. **Animation and rigging** — keyframes, f-curves, NLA, armatures, IK/FK,
   skinning.
8. **Geometry nodes, sculpt/paint, Grease Pencil** — node execution engine,
   fields, brush engine, 2D/3D drawing.
9. **Simulation, compositor, VFX, sequencer** — physics, compositing,
   tracking, video editing.
10. **Assets, I/O, Python, hardening** — asset browser, import/export,
    scripting, add-ons, LTS process.

Details: [docs/architecture/future_phases.md](docs/architecture/future_phases.md).

## Phase 1 goal

Make the repository durable enough that the systems above can grow without
collapsing. Concretely, Phase 1 ships a foundation library
(`forge::` namespace) with:

- platform/compiler/configuration detection
- version and build information
- `Error`, `Result<T>`, `Expected<T, E>` error handling
- thread-safe category logging with console and file sinks
- assertion macros (`FORGE_ASSERT`, `FORGE_VERIFY`, `FORGE_UNREACHABLE`)
- `ScopeExit`, `NonCopyable`, time utilities (`Instant`, `Duration`,
  `Stopwatch`, `FrameTimer`)
- a math library (`Vec2/3/4`, `Mat3`, `Mat4`, `Quat`, `Transform`)
- memory architecture (`Allocator`, `SystemAllocator`, `LinearAllocator`,
  `MemoryTracker`)
- file/path abstraction and a traversal-safe `VirtualFileSystem`
- `ThreadPool`, `TaskGroup`, `JobSystem` with `parallel_for`
- an instrumentation `Profiler` with Chrome-trace output
- internal unit-test and benchmark frameworks (no external dependencies)

## Building

Requirements: CMake ≥ 3.24, Ninja (for the presets), and a C++23 compiler
(MSVC 19.36+/VS 2022, GCC 13+, or Clang 16+ / Xcode 15.3+).

```bash
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

Without presets (any generator):

```bash
cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release
cmake --build build/release --parallel
ctest --test-dir build/release --output-on-failure
```

Platform-specific instructions: [docs/build/windows.md](docs/build/windows.md),
[docs/build/macos.md](docs/build/macos.md), [docs/build/linux.md](docs/build/linux.md).

### Running the executables

```bash
./build/debug/apps/forge_cli/forge_cli --version
./build/debug/apps/forge_cli/forge_cli doctor
./build/debug/apps/forge_desktop/forge_desktop --frames 5
./build/debug/apps/forge_bench/forge_bench
./build/debug/tools/forge_project_info/forge_project_info
```

### Tests and benchmarks

```bash
ctest --preset debug                                 # everything via CTest
./build/debug/tests/forge_tests --filter math.       # direct, filtered
./build/debug/benchmarks/forge_benchmarks --filter memory.
```

See [docs/testing/unit_tests.md](docs/testing/unit_tests.md) and
[docs/testing/benchmarks.md](docs/testing/benchmarks.md).

### CMake options

| Option | Default | Meaning |
|---|---|---|
| `FORGE_BUILD_TESTS` | `ON` | build the unit-test runner |
| `FORGE_BUILD_BENCHMARKS` | `ON` | build the benchmark suites |
| `FORGE_BUILD_TOOLS` | `ON` | build developer tools |
| `FORGE_ENABLE_WARNINGS_AS_ERRORS` | `OFF` (ON in CI) | `-Werror` / `/WX` |
| `FORGE_ENABLE_SANITIZERS` | `OFF` | ASan+UBSan on GCC/Clang |
| `FORGE_ENABLE_PROFILING` | `ON` | compile `FORGE_PROFILE_*` macros in |
| `FORGE_ENABLE_TRACING` | `OFF` (ON in debug presets) | desktop shell writes a Chrome trace |

## Repository structure

```txt
forge3d/
├── apps/             executable entry points (desktop shell, CLI, bench)
├── benchmarks/       benchmark framework + suites
├── cmake/            shared CMake modules (options, warnings, sanitizers, ...)
├── docs/             architecture, build and testing documentation
├── foundation/       the forge_foundation library (the only library so far)
│   ├── include/forge/{foundation,math,memory,filesystem,threading,profiling}/
│   └── src/...
├── tests/            internal test framework + unit tests
└── tools/            developer tools (forge_project_info)
```

Module dependency rules: [docs/architecture/module_boundaries.md](docs/architecture/module_boundaries.md).
Coding conventions: [docs/architecture/coding_standards.md](docs/architecture/coding_standards.md).

## Non-goals for Phase 1

No GPU APIs (Vulkan/Metal/DirectX/OpenGL), no window manager or UI, no mesh
data structures, no scene graph or dependency graph, no file format, no
import/export, no materials/nodes/animation/simulation, no Python. No
external dependencies — only the C++ standard library and OS APIs — and no
Blender code anywhere.

## Phase 1 acceptance criteria

- CMake configures and builds on at least one major platform (CI covers
  Linux, Windows, macOS).
- The foundation library, desktop shell, CLI, benchmark runner and tools all
  build; tests build, run and pass.
- `forge_cli --version` and `forge_cli doctor` work; the desktop shell starts
  and exits cleanly; the benchmark runner prints timings.
- Documentation exists for architecture, builds and testing.
- No premature DCC systems, no external dependencies, clear module
  boundaries.

## License

MIT — see [LICENSE](LICENSE).
