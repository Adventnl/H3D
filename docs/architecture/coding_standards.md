# Coding Standards

These rules keep a codebase that is expected to reach millions of lines
readable and uniform. When in doubt, copy the style of the surrounding code.

## Language level

C++23, no compiler extensions (`CXX_EXTENSIONS OFF`). Prefer the standard
library; external dependencies require an architecture discussion first.

## Naming

| Entity | Style | Example |
|---|---|---|
| Namespaces | `lower_snake` (top level: `forge`) | `forge::detail` |
| Types | `PascalCase` | `ThreadPool`, `LinearAllocator` |
| Functions, variables | `lower_snake_case` | `worker_count`, `read_text_file` |
| Private members | trailing underscore | `queue_mutex_` |
| Compile-time constants | `kPascalCase` | `kEpsilon`, `kDefaultAlignment` |
| Macros | `FORGE_UPPER_SNAKE` | `FORGE_ASSERT`, `FORGE_LOG_INFO` |
| Files | `lower_snake_case` | `thread_pool.hpp`, `virtual_filesystem.cpp` |

Avoid abbreviations: `allocator`, not `alloc`; `length`, not `len`.

## Files and headers

- `#pragma once` in every header.
- Headers start with a short comment block stating what the file provides
  and any non-obvious contract (thread-safety, conventions).
- Public headers live in `<module>/include/forge/<area>/`; include them with
  the full path: `#include "forge/math/vec3.hpp"`.
- Keep implementation in `.cpp` files unless templates or trivial inline
  accessors require header placement.
- Include order: own header first (in `.cpp`), then standard library, then
  forge headers; alphabetical within groups.

## Error handling

- Recoverable failures return `forge::Result<T>`
  (= `Expected<T, forge::Error>`). Never ignore a `Result` — they are
  `[[nodiscard]]` by construction at call sites.
- Programmer errors are assertions: `FORGE_ASSERT` (debug only),
  `FORGE_VERIFY` (all builds), `FORGE_UNREACHABLE` for impossible paths.
- Do not throw exceptions as an API contract. User callables may throw
  (futures and `TaskGroup` handle that), but foundation interfaces report
  errors by value.
- File-not-found and similar user-environment conditions are `Result`
  errors, never assertions or crashes.

## Logging

- Use the `FORGE_LOG_*` macros with a short, stable category string
  (`"startup"`, `"filesystem"`, `"jobs"`). Categories make later filtering
  and routing possible.
- Build messages with `std::format` at the call site.
- No raw `std::cout`/`printf` in library code; entry-point executables may
  print directly for their command output.

## Concurrency

- Every shared mutable structure documents its synchronization in the header
  comment ("thread-safe", "not thread-safe — callers synchronize").
- Prefer `std::scoped_lock`/`std::unique_lock`; never manual lock/unlock.
- Atomics use explicit memory ordering with a comment when anything other
  than `relaxed` counters is involved.

## Tests

- Every module ships unit tests in `tests/<area>/`. New code lands with
  tests in the same change.
- Test names are `area.behavior_under_test` (`math.vec3_cross`,
  `filesystem.vfs_rejects_path_traversal`) so `--filter area.` selects a
  suite.
- Tests must be deterministic, offline, and fast (the full run is a CI
  gate). Filesystem tests work in a scratch directory under the system temp
  directory and clean up after themselves.

## Formatting

4-space indentation, braces on their own line (Allman), ~95-column soft
limit, one blank line between functions. Pointer/reference bind to the type:
`const Path& path`.
