# UI and editor framework

The `ui` module is a **logical** UI model: it computes bounds, owns the
workspace/screen/area/region/editor hierarchy and carries a theme palette. It
does not draw anything. A future renderer will consume these bounds; nothing in
`ui` depends on a renderer, scene, geometry or GPU.

## Geometry primitives

`Rect`, `Point`, `Size` (`rect.hpp`). `Rect` uses a top-left origin (y grows
down). It offers `contains`, `intersects`, edge splits (`split_left/right/top/
bottom`), `inset`, and accessors (`right`, `bottom`, `center`, `area`). The
`layout.hpp` helpers (`split_horizontal/vertical`, `layout_columns/rows`,
`layout_weighted_columns`, `grid_cell`) carve a rect into sub-rects for widget
placement.

## Widgets

`Widget` is the base UI node: an id, bounds, `visible`/`enabled` flags and child
widgets (`add_child`, `remove_child`, `find_child`). `layout()` positions
children (default: recurse). Concrete widgets:

- `Panel` — titled, collapsible container; `layout()` stacks visible children
  under a header.
- `Menu` — ordered `MenuItem`s, each bound to a command/operator id (or a
  separator).
- `Toolbar` — `ToolEntry`s plus one active tool id.
- `StatusBar` — a primary message, a bounded history and optional progress.

These are the building blocks editors will assemble; they store command ids, not
callbacks, so the operator system stays the single source of behavior.

## Screens, areas, regions and docking

A **Screen** is a Blender-style tiled layout backed by a binary **docking tree**
(`docking.hpp`):

```
LayoutNode
├── leaf  → owns one Area
└── split → direction (Vertical | Horizontal) + ratio + two child LayoutNodes
```

- `SplitDirection::Vertical` ⇒ a vertical divider ⇒ left | right children
  (the `screen.split_vertical` operator).
- `SplitDirection::Horizontal` ⇒ a horizontal divider ⇒ top / bottom children.

`Screen` owns the root node and performs the structural edits:

- `split_area(id, direction, ratio, editor?)` turns a leaf into a split; the
  original area keeps the first slot, a new area takes the second (it copies the
  editor type unless one is given). Returns the new area id.
- `close_area(id)` removes a leaf; its sibling expands to fill the freed space.
  The **last** area cannot be closed.
- `find_area`, `areas`, `area_count`, `active_area`, `set_active_area`.
- `compute_layout(bounds)` walks the tree assigning each area a rectangle, then
  each area tiles its regions.

An **Area** hosts one editor and owns **Regions** (`Header`, `Main`, `Toolbar`,
`Sidebar`, `Footer`, `Status`). `Area::layout_regions` carves fixed-thickness
strips off the edges and gives the `Main` region the remainder.

## Editors

An `Editor` drives an area: it has a `type`, a `display_name`/`description`,
lifecycle hooks (`on_open`/`on_close`), an `update` tick, an event handler and a
`status_text`. The `EditorRegistry` maps an `EditorType` to a factory
(`register_factory` / `create` / `is_registered` / `registered_types`).

Phase 2 ships an honest **placeholder editor** for all 19 editor types
(`ViewportEditor`, `OutlinerEditor`, `PropertiesEditor`, …). Each is a real,
distinct type derived from `PlaceholderEditor`; its `status_text` names the
future system it depends on, e.g.:

> *Viewport editor placeholder: renderer not implemented until a later phase.*
> *Outliner placeholder: scene database not implemented until Phase 3.*

`register_default_editors` registers every factory. No placeholder touches scene
data or draws — they exist so the registry, area↔editor binding and workspace
construction are real and tested today.

## Themes

`Theme` is a flat palette of `Color`s (8-bit RGBA) — `background`,
`panel_background`, `text`, `accent`, `selection`, `button`, … `ThemeRegistry`
holds the built-in `Dark` and `Light` themes (Dark active by default) and
supports lookup and `set_active`. Themes carry no rendering logic. See
[preferences_and_themes.md](preferences_and_themes.md).

## Future renderer integration

A later phase adds a GPU/render layer that walks this same hierarchy:
`Screen::compute_layout` already produces every area and region rectangle, so the
renderer's job is to draw the editor assigned to each area within its bounds and
hit-test input against the same rects. The logical model does not change; only a
drawing backend is added on top.
