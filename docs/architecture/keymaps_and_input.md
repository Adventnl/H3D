# Keymaps and input

Input flows from raw events to operator invocations:

```
Event → InputState (device snapshot) → Keymap (shortcut → action id) → Operator
```

## InputState

`InputState` is the queryable, per-frame snapshot of devices:

- Keyboard: `press_key` / `release_key`, `is_key_down`, `was_key_pressed` and
  `was_key_released` (edge-triggered), and the current `modifiers()`.
- Mouse: `set_mouse_position`, `mouse_x/y`, `mouse_delta_x/y`, scroll delta,
  and per-button `is_mouse_button_down`.
- Text: an accumulated `text_buffer`.

`apply(event)` updates it from one `Event`. `new_frame()` advances to a new
frame: it clears the edge sets, the per-frame mouse/scroll deltas and the text
buffer, while held keys and the cursor position persist. The first
`set_mouse_position` establishes the cursor without producing a delta.

## Keys, buttons and modifiers

`Key` is a stable enum of physical keys (letters, digits, F-keys, navigation,
punctuation, modifiers). `MouseButton` covers the usual five. `ModifierKey` is a
bit flag (`Ctrl`, `Shift`, `Alt`, `Super`) with `|`, `&` and `has_modifier`.
`key_name`/`key_from_name` convert keys to and from their stable names.

## Shortcut

A `Shortcut` is a `Key` plus modifier flags. It compares by value, renders as a
string (`"Ctrl+Shift+S"`) and parses back from one (`Shortcut::parse("Ctrl+S")`).
The canonical modifier order is Ctrl, Shift, Alt, Super.

## Keymap

`Keymap` maps a `Shortcut` to an **action id** (an operator/command id):

- `bind` / `unbind` / `resolve` / `shortcut_for` / `bindings`
- `default_keymap()` — the `ForgeDefault` preset.

`Application::dispatch_shortcut` resolves a shortcut and runs the bound operator;
`dispatch_event` builds a shortcut from a non-repeat, non-modifier `KeyDown` and
feeds it through this path (after modal tools and input state).

## Default shortcuts (ForgeDefault)

| Shortcut | Action |
|---|---|
| `Ctrl+S` | `file.save` |
| `Ctrl+O` | `file.open` |
| `Ctrl+N` | `file.new` |
| `Ctrl+Z` | `edit.undo` |
| `Ctrl+Shift+Z` | `edit.redo` |
| `F3` | `command.search` |
| `G` | `transform.move` |
| `R` | `transform.rotate` |
| `S` | `transform.scale` |
| `Tab` | `mode.toggle` |
| `Ctrl+PageDown` | `workspace.next` |
| `Ctrl+PageUp` | `workspace.previous` |

```bash
forge_cli keymap   # prints all bindings
```

The bound ids are real, registered operators; the actions they invoke are the
Phase 2 placeholders described in
[command_operator_undo.md](command_operator_undo.md).
