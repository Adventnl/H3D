# Workspaces

A **workspace** is a named screen layout — a saved arrangement of editors tuned
for a task ("Layout", "Modeling", "Animation"). Switching workspaces swaps the
whole screen the user sees.

## Types

- `Workspace` — an id, a display name, a description and an owned `Screen`.
- `WorkspaceRegistry` — owns the workspaces, tracks the active one, and
  implements `forge::WorkspaceService` so app-layer operators can drive it.

## Building a workspace layout

`build_workspace_screen(primary, secondaries)` assembles a `Screen`: the primary
editor fills the main slot and the secondaries are stacked in a side column, each
taking an even slice. It is implemented purely with `Screen::split_area`, so the
resulting tree is a normal docking tree the user could have built by hand.

```cpp
Screen s = build_workspace_screen(EditorType::Viewport3D,
                                  {EditorType::Outliner, EditorType::Properties,
                                   EditorType::Timeline});
// → Viewport3D (main) | Outliner / Properties / Timeline (right column)
```

## Default workspaces

`register_default_workspaces` registers the 16 Blender-class workspaces and makes
**Layout** active:

| Workspace | Primary | Side column |
|---|---|---|
| Layout | Viewport 3D | Outliner, Properties, Timeline |
| Modeling | Viewport 3D | Outliner, Properties |
| Sculpting | Viewport 3D | Properties |
| UV Editing | Viewport 3D | UV Editor, Image Editor, Properties |
| Texture Paint | Viewport 3D | Image Editor, Properties |
| Shading | Viewport 3D | Shader Editor, Properties |
| Animation | Viewport 3D | Timeline, Dope Sheet, Graph Editor |
| Rendering | Viewport 3D | Properties, Compositor |
| Compositing | Compositor | Image Editor |
| Geometry Nodes | Viewport 3D | Geometry Nodes, Spreadsheet, Properties |
| Scripting | Text Editor | Python Console, Properties |
| VFX | Movie Clip Editor | Viewport 3D, Properties |
| Video Editing | Video Sequencer | Image Editor, Properties |
| Grease Pencil | Viewport 3D | Dope Sheet, Properties |
| Assets | Asset Browser | Viewport 3D, Properties |
| Simulation | Viewport 3D | Properties, Timeline |

Every workspace has at least one area, and each area's editor is one of the
registered placeholder editors.

## Active workspace behavior

- `active()` / `active_workspace_name()` / `active_index()`
- `next_workspace()` / `previous_workspace()` — wrap around; no-op with < 2
  workspaces.
- `activate_workspace(name)` — by id or display name.
- `set_active_index(i)`.

Through the `WorkspaceService` interface these map to the `workspace.next`,
`workspace.previous`, `screen.split_vertical/horizontal` and `screen.close_area`
operators, so `Ctrl+PageDown`/`Ctrl+PageUp` change workspace and the screen
operators edit the active workspace's screen. See
[command_operator_undo.md](command_operator_undo.md).

## Inspecting workspaces

```bash
forge_cli list-workspaces
```

prints each workspace, its area count and the editor type of each area.
