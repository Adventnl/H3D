# Forge3D

Forge3D is a from-scratch, production-scale 3D digital content creation
application targeting long-term, Blender-class capability: modeling,
sculpting, UV editing, materials, shader nodes, geometry nodes, animation,
rigging, 2D/3D drawing, simulation, rendering, compositing, motion tracking,
video editing, asset libraries, Python scripting, add-ons and studio pipeline
workflows.

This repository contains **Phase 1 (engineering foundation)**, **Phase 1.5
(hardening)** and **Phase 2 (application shell, UI and editor framework)**. The
operator/command/undo spine and the logical workspace/screen/editor model are
real, tested and documented. There is intentionally still no viewport, renderer,
scene database, mesh data, materials, nodes or Python yet — Phase 2 is logic, not
pixels.

## Status

| Component | State |
|---|---|
| CMake build (C++23, presets, CI) | done |
| `forge_foundation` library | done |
| Phase 1.5 hardening (CI `-Werror` fix, options, coverage, more tests) | done |
| `forge_app` — runtime, events, input, operators, commands/undo, modal tools, preferences, windows | done |
| `forge_ui` — workspaces, screens, areas, regions, editors, widgets, themes | done |
| `forge_desktop` — Phase 2 application shell (headless frame loop) | done |
| `forge_cli` (`--version`, `doctor`, `app-info`, `list-*`, `keymap`, …) | done |
| `forge_bench` / `forge_benchmarks` (with `--quick`) | done |
| Unit tests (internal framework, 198 cases) | done |
| GPU/renderer, scene database, meshes, materials, nodes, Python | later phases |

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

## Phase 1.5 — hardening

Phase 1.5 strengthened the foundation before building on it:

- fixed a real CI break: the warnings-as-errors build failed on a `[[nodiscard]]`
  result being ignored in a test;
- the **entire** codebase (foundation + app + ui + apps + tests + benchmarks)
  compiles clean under `-Werror` / `/WX`;
- added CMake options `FORGE_BUILD_APP`, `FORGE_BUILD_DOCS`,
  `FORGE_ENABLE_COVERAGE` and the `app`/`ui` modules to the dependency graph;
- the supported toolchains are **MSVC 19.36+ (VS 2022), GCC 13+, Clang 16+**;
  aligned allocation uses portable C++17 aligned `new`/`delete`, the VFS rejects
  `..`/absolute/drive-letter traversal, and the job system's `parallel_for`
  falls back to serial execution when uninitialized.

## Phase 2 — application shell, UI and editor framework

Phase 2 builds the logical application architecture every artist-facing system
will plug into, implementing the first half of the long-term pipeline:

```
Input → Event → Operator → Command → Undo/redo            (Phase 2)
      → Scene mutation → Dependency graph → Redraw          (Phase 3+)
```

It adds two modules under the strict dependency order
`foundation → app → ui → apps`:

- **`forge::app`** — `Application`/`Runtime`/`AppContext`, the `EventBus` and
  typed `Event`s, `InputState`, `Shortcut`/`Keymap`, `Command`/`CompositeCommand`
  and the `CommandStack`/`UndoStack`, `Operator`/`OperatorRegistry` with command
  search, `ModalTool`/`ModalToolManager`, `Preferences`, and a headless-safe
  `Window`/`WindowSystem`/`NullWindowBackend`.
- **`forge::ui`** — `Workspace`/`WorkspaceRegistry` (16 Blender-class
  workspaces), `Screen`/`Area`/`Region` with a binary docking tree,
  `Editor`/`EditorRegistry` with 19 honest placeholder editors, `Widget`/`Panel`/
  `Menu`/`Toolbar`/`StatusBar`, layout helpers, and `Theme`/`ThemeRegistry`
  (Dark/Light).

The `app` module never depends on `ui`; the one cross-cutting need (workspace and
screen operators) is bridged by the abstract `forge::WorkspaceService` that
`ui::WorkspaceRegistry` implements.

**Phase 2 intentionally does *not* include**: any GPU API or renderer, real
viewport drawing, a scene graph or dependency graph, meshes/UVs, materials or
node execution, animation, simulation, an asset system, Python, or any external
UI toolkit. The window backend is headless by default so CI runs the whole shell.

Design docs:
[phase 2 overview](docs/architecture/phase_02_application_shell.md) ·
[operators/commands/undo](docs/architecture/command_operator_undo.md) ·
[UI/editor framework](docs/architecture/ui_editor_framework.md) ·
[workspaces](docs/architecture/workspaces.md) ·
[keymaps & input](docs/architecture/keymaps_and_input.md) ·
[preferences & themes](docs/architecture/preferences_and_themes.md).

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
# diagnostics (Phase 1)
./build/debug/apps/forge_cli/forge_cli --version
./build/debug/apps/forge_cli/forge_cli doctor

# Phase 2 application introspection
./build/debug/apps/forge_cli/forge_cli app-info
./build/debug/apps/forge_cli/forge_cli list-workspaces
./build/debug/apps/forge_cli/forge_cli list-editors
./build/debug/apps/forge_cli/forge_cli list-operators
./build/debug/apps/forge_cli/forge_cli keymap
./build/debug/apps/forge_cli/forge_cli preferences-defaults

# the Phase 2 desktop shell (headless frame loop)
./build/debug/apps/forge_desktop/forge_desktop --frames 5 --headless
./build/debug/apps/forge_desktop/forge_desktop --frames 2 --headless --trace trace.json

./build/debug/apps/forge_bench/forge_bench --quick
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
| `FORGE_BUILD_APP` | `ON` | build the `app`/`ui` modules and the desktop shell |
| `FORGE_BUILD_TESTS` | `ON` | build the unit-test runner |
| `FORGE_BUILD_BENCHMARKS` | `ON` | build the benchmark suites |
| `FORGE_BUILD_TOOLS` | `ON` | build developer tools |
| `FORGE_BUILD_DOCS` | `OFF` | install the Markdown docs (and a Doxygen target if found) |
| `FORGE_ENABLE_WARNINGS_AS_ERRORS` | `OFF` (ON in CI) | `-Werror` / `/WX` |
| `FORGE_ENABLE_SANITIZERS` | `OFF` | ASan+UBSan on GCC/Clang |
| `FORGE_ENABLE_COVERAGE` | `OFF` | gcov-style coverage on GCC/Clang |
| `FORGE_ENABLE_PROFILING` | `ON` | compile `FORGE_PROFILE_*` macros in |
| `FORGE_ENABLE_TRACING` | `OFF` (ON in debug presets) | desktop shell writes a Chrome trace |

With `-DFORGE_BUILD_APP=OFF` the `app`/`ui` modules and `forge_desktop` are
skipped, `forge_cli` builds with only its Phase 1 commands, and the test runner
builds only the foundation suites — useful for a minimal foundation-only build.

## Repository structure

```txt
forge3d/
├── apps/             executable entry points (desktop shell, CLI, bench)
├── app/              forge_app: runtime, events, input, operators, commands, windows
│   └── include/forge/app/  src/
├── ui/               forge_ui: workspaces, screens, editors, widgets, themes
│   └── include/forge/ui/   src/
├── benchmarks/       benchmark framework + suites (foundation + app/ui)
├── cmake/            shared CMake modules (options, warnings, sanitizers, coverage, ...)
├── docs/             architecture, build and testing documentation
├── foundation/       forge_foundation: the Phase 1 core library
│   ├── include/forge/{foundation,math,memory,filesystem,threading,profiling}/
│   └── src/...
├── tests/            internal test framework + unit tests (foundation, app, ui)
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
