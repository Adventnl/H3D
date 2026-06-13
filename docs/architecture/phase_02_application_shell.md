# Phase 2 — Application shell, UI and editor framework

Phase 2 builds the **logical** application shell that every artist-facing system
will plug into: the runtime, events, input, operators, commands/undo, modal
tools, preferences, a window abstraction, and a logical UI model of workspaces,
screens, areas, regions, editors, widgets and themes.

It deliberately stops short of pixels. There is no GPU, no renderer, no real
viewport, no scene database, no meshes, no materials, no nodes, no Python. The
goal is to make the **architecture** real, tested and documented so later phases
extend it instead of inventing it.

## Modules and dependency direction

```
foundation  →  app  →  ui  →  apps
```

- **`foundation`** — unchanged Phase 1 core (math, memory, filesystem, threading,
  logging, profiling). Depends on nothing above it.
- **`app`** (`forge::app`) — runtime, events, input, shortcuts, keymap, commands,
  undo stack, operators, modal tools, preferences, the window abstraction and the
  `Application` object. Depends only on `foundation`. **Never** depends on `ui`.
- **`ui`** (`forge::ui`) — workspaces, screens, areas, regions, the docking tree,
  editors, widgets, panels, menus, toolbars, status bars, layout helpers and
  themes. Depends on `foundation` and `app`.
- **`apps`** — `forge_desktop` (the shell) and `forge_cli` wire `app` + `ui`
  together.

The one place `app` needs something from `ui` (switch workspace, split the active
area) is solved with a seam, not a back-dependency: `app` defines the abstract
`forge::WorkspaceService`; `ui::WorkspaceRegistry` implements it; the desktop app
attaches the implementation at runtime. See
[command_operator_undo.md](command_operator_undo.md).

## The input → command pipeline

The long-term Blender-class pipeline is:

```
User input → Event → Operator → Command → Undo transaction
           → Scene mutation → Dependency graph → Evaluation → Viewport redraw
```

Phase 2 implements the **first half**:

```
Input → Event → Keymap → Operator → Command → Undo/redo
```

Scene mutation and everything to its right arrive in Phase 3+.

`Application::dispatch_event` runs an event through: modal tools (first refusal) →
`InputState` → keymap → operator → event bus.

## What each subsystem is

| Subsystem | Type(s) | Role |
|---|---|---|
| Runtime | `Runtime` | subsystem lifecycle, frame clock, quit request |
| Application | `Application`, `ApplicationConfig` | owns services, runs the frame loop |
| Context | `AppContext`, `WorkspaceService` | service bundle handed to operators/editors |
| Events | `Event`, `EventBus` | typed events, immediate + queued dispatch |
| Input | `InputState`, `Key`, `MouseButton`, `ModifierKey` | per-frame device snapshot |
| Shortcuts | `Shortcut`, `Keymap` | key+modifiers → action id |
| Commands | `Command`, `CompositeCommand`, `CommandStack`/`UndoStack` | undoable mutations + history |
| Operators | `Operator`, `OperatorRegistry`, `OperatorResult` | user-facing actions + search |
| Modal tools | `ModalTool`, `ModalToolManager` | input-capturing interactive states |
| Preferences | `Preferences` | persisted settings (key=value) |
| Window | `Window`, `WindowSystem`, `NullWindowBackend` | headless-safe window model |
| Workspaces | `Workspace`, `WorkspaceRegistry` | named screen layouts |
| Screens | `Screen`, `LayoutNode`, `Area`, `Region` | tiled docking tree |
| Editors | `Editor`, `EditorRegistry`, placeholder editors | per-area logic |
| Widgets/Theme | `Widget`, `Panel`, `Menu`, `Toolbar`, `StatusBar`, `Theme` | logical UI + palette |

Each has dedicated tests under `tests/app` and `tests/ui`.

## Why rendering / scene / mesh are not here

A Blender-class app lives or dies on the cleanliness of the layer below the UI.
Building the operator/command/undo spine and the workspace/editor model *before*
any renderer means:

- operators are written against a stable contract from day one;
- the UI computes bounds and never reaches into scene data directly;
- the window backend is abstract, so CI runs the whole shell headlessly;
- Phase 3 can add a scene database behind the existing `Command`/`Operator`
  contract without reworking the UI.

## Non-goals (still)

No GPU/Vulkan/Metal/DirectX, no renderer or viewport drawing, no scene graph or
dependency graph, no meshes/UVs, no materials or node execution, no animation,
no simulation, no asset system, no Python, and no external UI toolkit. The
window backend defaults to `NullWindowBackend`; a native backend can come later
behind a build option without breaking headless CI.
