# Changelog

All notable changes to Forge3D are recorded here. Versions follow the phased
roadmap in [README.md](README.md).

## [Unreleased] — Phase 1.5 hardening + Phase 2 application shell

### Phase 1.5 — hardening

- **Fixed a CI break**: a test ignored a `[[nodiscard]]` return value, which
  failed the warnings-as-errors build (`-Werror=unused-result`). The whole tree
  now compiles clean under `-Werror` / `/WX`.
- Added CMake options `FORGE_BUILD_APP`, `FORGE_BUILD_DOCS` and
  `FORGE_ENABLE_COVERAGE`; added a `cmake/ForgeCoverage.cmake` module and a
  `docs/` install target.
- Documented the supported toolchains (MSVC 19.36+, GCC 13+, Clang 16+) and
  reaffirmed portability invariants: portable aligned `new`/`delete`, VFS
  traversal rejection, serial `parallel_for` fallback.

### Phase 2 — application shell, UI and editor framework

- New module **`forge_app`** (`forge::app`): `Application`, `Runtime`,
  `AppContext`/`WorkspaceService`, `EventBus` + typed `Event`s, `InputState`
  (`Key`/`MouseButton`/`ModifierKey`), `Shortcut`/`Keymap`,
  `Command`/`CompositeCommand`/`CommandStack`/`UndoStack`,
  `Operator`/`OperatorRegistry` with command search, `ModalTool`/
  `ModalToolManager`, `Preferences`, and `Window`/`WindowSystem`/
  `NullWindowBackend`.
- New module **`forge_ui`** (`forge::ui`): `Workspace`/`WorkspaceRegistry` (16
  default workspaces), `Screen`/`Area`/`Region` with a binary docking tree,
  `Editor`/`EditorRegistry` with 19 placeholder editors, `Widget`/`Panel`/
  `Menu`/`Toolbar`/`StatusBar`, layout helpers, `Theme`/`ThemeRegistry`
  (Dark/Light).
- Default content: 16 operators, 12 keymap bindings (ForgeDefault), 16
  workspaces, 19 editor types, 2 themes.
- `forge_desktop` rewritten to run the Phase 2 `Application` (headless frame
  loop, `--frames`/`--headless`/`--windowed`/`--trace`/`--log-file`).
- `forge_cli` gained `app-info`, `list-workspaces`, `list-editors`,
  `list-operators`, `keymap`, `preferences-defaults`.
- Benchmarks: added `app`/`ui` micro-benchmarks and a `--quick` mode for CI.
- Tests grew from 97 to **198** cases across `tests/app` and `tests/ui`.
- CI builds the new modules and smoke-tests the new commands.

## [0.1.0] — Phase 1 foundation

- `forge_foundation` library: platform/compiler/config detection, version/build
  info, `Error`/`Result`/`Expected`, logging, assertions, `ScopeExit`,
  time utilities, math (`Vec`/`Mat`/`Quat`/`Transform`), memory allocators,
  filesystem + `VirtualFileSystem`, `ThreadPool`/`TaskGroup`/`JobSystem`,
  `Profiler` with Chrome-trace output.
- `forge_desktop` (skeleton), `forge_cli`, `forge_bench`, `forge_project_info`.
- Internal test and benchmark frameworks; CMake presets; CI (Linux/Windows/macOS).
