# Operators, commands and undo

Phase 2 separates **what the user asks for** from **what changes and can be
undone**. This separation is the backbone of the long-term architecture.

## Operator vs Command

- An **Operator** is a *user-facing action*: `transform.move`, `edit.undo`,
  `file.save`, `screen.split_vertical`. Operators are registered by id, can be
  searched, can be bound to shortcuts, may be unavailable in some contexts, and
  may or may not be undoable. They are the verbs of the application.
- A **Command** is a *low-level, undoable mutation*. Commands are the unit the
  undo stack stores. They know how to `execute()` and `undo()` themselves.

An operator typically *creates and submits commands*; the command is what the
undo stack remembers. Some operators (e.g. `command.search`, `workspace.next`)
produce no command at all — switching workspace is not an undoable scene edit.

```
Operator::execute(ctx)
    └── ctx.app->commands->execute(std::make_unique<MoveCommand>(...))
                                   └── pushed onto the undo stack
```

## OperatorResult

`invoke()`/`execute()` return one of:

| Result | Meaning |
|---|---|
| `Finished` | completed successfully (may have produced an undo step) |
| `Cancelled` | nothing happened / user backed out |
| `RunningModal` | started a modal interaction; more events expected |
| `Failed` | could not run (unknown id, precondition failed) |

## OperatorRegistry and command search

`OperatorRegistry` keys operators by id and offers:

- `register_operator` / `unregister` / `find` / `contains`
- `execute(id, ctx)` — looks up, checks `is_available`, invokes
- `list()` / `categories()`
- `search(query, max_results)` — case-insensitive substring match over id,
  display name, description and category, ranked by a simple relevance score
  (exact > prefix > word-boundary > substring; display name weighted highest).
  An empty query returns every operator. This powers `F3` command search.

The built-in operators are registered by `register_default_operators`. See the
[defaults table](#built-in-operators).

## CommandStack / UndoStack

`CommandStack` (aliased as `UndoStack` — they are the same type) is the history:

- `execute(command)` applies the command and pushes it; the redo stack is cleared.
- `undo()` / `redo()` walk the history.
- `can_undo` / `can_redo` / `undo_count` / `redo_count` / `undo_name` / `redo_name`.
- `set_max_history(n)` trims the oldest entries (0 = unlimited).
- Adjacent commands may **merge** (`can_merge`/`merge`) so a continuous drag
  collapses into one undo step.

### CompositeCommand

`CompositeCommand` groups several commands and is itself a `Command`: it executes
and redoes its children in order and undoes them in **reverse** order, so a batch
of edits is a single undo step.

### Transactions

```cpp
stack.begin_transaction("Move things");
stack.execute(...);   // collected, applied immediately
stack.execute(...);
stack.commit_transaction();   // one undo step (a CompositeCommand)
// or
stack.cancel_transaction();   // reverts everything collected so far
```

An empty transaction commits to nothing (no spurious undo step).

## Modal tools

A **modal tool** is an interactive state that captures input until it finishes or
cancels — the future home of click-drag transforms, knife, lasso select, etc.
`ModalToolManager` keeps at most one active tool; while active it sees every
event first and returns `Continue`, `PassThrough`, `Finished` or `Cancelled`.
Modal *operators* (those returning `RunningModal`) will start a modal tool; in
Phase 2 the manager and a test tool exist and are wired into
`Application::dispatch_event`, but no built-in operator goes modal yet.

## The app↔ui seam

Workspace/screen operators (`workspace.next`, `screen.split_vertical`, …) must
act on `ui` state, but `app` must not depend on `ui`. The seam is the abstract
`forge::WorkspaceService` declared in `app/app_context.hpp`:

```cpp
class WorkspaceService {
    virtual std::string active_workspace_name() const = 0;
    virtual bool next_workspace() = 0;
    virtual bool split_active_area_vertical() = 0;
    /* ... */
};
```

`ui::WorkspaceRegistry` implements it; the desktop app calls
`app.set_workspace_service(&registry)`. Operators reach it through
`ctx.app->workspaces` and degrade gracefully (return `Cancelled`) when no UI is
attached.

## Built-in operators

| id | category | behavior in Phase 2 |
|---|---|---|
| `app.quit` | Application | requests runtime + window quit |
| `command.search` | Application | placeholder (search UI is a later phase) |
| `edit.undo` / `edit.redo` | Edit | drives the `CommandStack` |
| `file.new` / `file.open` / `file.save` | File | honest placeholders (no project I/O yet) |
| `workspace.next` / `workspace.previous` | Workspace | switch active workspace via the service |
| `screen.split_vertical` / `screen.split_horizontal` / `screen.close_area` | Screen | edit the active screen layout |
| `transform.move` / `transform.rotate` / `transform.scale` | Transform | placeholders (no scene objects yet) |
| `mode.toggle` | Edit | placeholder (object/edit modes need the scene database) |

Placeholder operators are *honest*: they log which future system they need and
return `Finished`/`Cancelled` rather than faking work.

## Future integration with the scene database

In Phase 3, scene-editing operators will wrap their mutations in commands that
touch data-blocks and mark the dependency graph dirty:

```
Operator → Command(execute: mutate data-block; undo: restore)
         → CommandStack → (Phase 3) depsgraph dirty → evaluation → redraw
```

The `Operator`/`Command`/`CommandStack` contract defined here does not change;
only the commands' bodies gain real effects.
