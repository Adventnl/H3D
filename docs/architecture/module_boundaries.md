# Module Boundaries

Forge3D will eventually contain dozens of modules. The dependency direction
is fixed now, while there is only one library, because it is nearly
impossible to fix later.

## Current modules

```txt
foundation      the base library (namespace forge)
apps/*          executable entry points
tests           unit tests
benchmarks      benchmark suites
tools/*         developer utilities
```

## Allowed dependencies

| Module | May depend on | Must not depend on |
|---|---|---|
| `foundation` | C++ standard library, OS APIs behind abstraction files | anything else in the repo; any future module |
| `apps/*` | `foundation` | tests, benchmarks, each other |
| `tests` | `foundation` (and the internal test framework) | apps, benchmarks |
| `benchmarks` | `foundation` (and the internal bench framework) | apps, tests |
| `tools/*` | `foundation` | apps, tests, benchmarks |

`foundation` must stay free of any knowledge of scenes, meshes, rendering,
UI, animation, nodes, assets or scripting. If a future module needs shared
logic, that logic moves *down* into `foundation` (or a new low-level module)
— never sideways or upward.

## Future dependency direction

All later systems follow this strict downward-only direction:

```txt
foundation → app → ui → scene → geometry → modifiers → gpu → viewport →
render → nodes → animation → rigging → sculpt_paint → grease_pencil →
simulation → compositor → vfx → sequencer → assets → io → scripting
```

Rules that hold for every future module:

- **No circular dependencies.** A cycle between two modules means one of
  them owns code that belongs in a lower layer.
- **Public headers only.** A module consumes another module exclusively
  through its `include/forge/...` headers, never through `src/`.
- **Entry points stay thin.** `apps/*` initialize systems and delegate; real
  logic lives in libraries where it can be tested.
- **Platform-specific code hides behind foundation abstractions** (as
  `platform.hpp`/`platform.cpp` do today). No `#if FORGE_PLATFORM_WINDOWS`
  in high-level modules.

## Enforcement

Today enforcement is by review plus the CMake target graph (a target only
links what it may use — nothing links "everything"). When the number of
modules grows, an include-checker in CI will enforce the layering
mechanically.
