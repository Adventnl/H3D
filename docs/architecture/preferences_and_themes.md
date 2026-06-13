# Preferences and themes

## Preferences

`Preferences` holds user-configurable settings grouped into
`application`, `interface`, `input` and `system`. It is persisted as a flat
`key=value` text file — no external JSON dependency.

### Fields and defaults

```ini
theme_name=Dark
language=en
ui_scale=1.000
enable_tooltips=true
enable_developer_extras=false
default_workspace=Layout
autosave_enabled=true
autosave_interval_seconds=120
recent_files_limit=20
keymap_preset=ForgeDefault
use_hardware_acceleration=false
```

(`use_hardware_acceleration` is a placeholder until the GPU phase.)

### Loading and saving

- `Preferences::defaults()` — the built-in defaults.
- `serialize()` / `parse(text)` — round-trip the key=value format.
- `save(path)` — writes the file, creating parent directories.
- `load(path)` — reads the file; a **missing file returns defaults()**, other
  I/O errors are reported through `Result`.
- `reset()` — restore defaults.

Parsing is intentionally **lenient**: blank lines and `#` comments are skipped,
unknown keys are ignored, malformed lines are dropped, and out-of-range values
are clamped (`ui_scale` to `[0.5, 4.0]`, `autosave_interval_seconds` to `>= 1`,
`recent_files_limit` to `>= 0`). This keeps a hand-edited or version-skewed file
from bricking startup.

```bash
forge_cli preferences-defaults   # prints the default file
```

## Themes

`Theme` is a flat palette of `Color`s consumed by the future UI renderer; it has
**no rendering logic** and lives entirely in the `ui` module. A `Color` is 8-bit
RGBA with `rgb`/`rgba` constructors, float accessors (`red_f`, …) and `to_hex`.

A `Theme` names every UI surface color: `background`, `panel_background`,
`panel_border`, `text`, `text_muted`, `accent`, `warning`, `error`, `success`,
`selection`, `highlight`, `button`, `button_hover`, `button_active`.

`ThemeRegistry` ships two built-in themes:

- **Dark** (active by default) — dark grey surfaces, light text, blue accent.
- **Light** — light surfaces, dark text.

API: `find(name)`, `contains(name)`, `set_active(name)`, `active()`,
`active_name()`, `names()`, `register_theme(theme)` (re-registering a name
replaces it). The active theme always exists.

The `interface.theme_name` preference selects which theme a future UI will make
active at startup; in Phase 2 the value is stored and round-tripped but not yet
applied to any pixels.
