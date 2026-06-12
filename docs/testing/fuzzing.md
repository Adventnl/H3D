# Fuzzing

Phase 1 does not ship fuzz targets, but the code is structured so they can
be added without refactoring. This document records the plan.

## Why the foundation is fuzz-ready

- Parsing-adjacent entry points are pure functions over byte/string inputs
  with `Result` outputs — no global state, no exceptions as control flow:
  - `VirtualFileSystem::resolve(std::string_view)` (logical path parsing and
    traversal rejection)
  - `Path` operations (`normalized`, `joined`, decomposition)
  - `read_text_file` / `read_binary_file` (arbitrary file contents)
  - `write_chrome_trace_json` (arbitrary event names incl. control bytes)
- Recoverable failures return errors instead of crashing, so any abort,
  sanitizer report or hang found by a fuzzer is a real bug by definition.

## Planned structure (Phase 2+)

```txt
fuzz/
├── CMakeLists.txt              built only with -DFORGE_BUILD_FUZZERS=ON
├── fuzz_vfs_resolve.cpp
├── fuzz_path_normalize.cpp
└── fuzz_trace_writer.cpp
```

Each target uses the libFuzzer entry point and builds with
`-fsanitize=fuzzer,address,undefined` (Clang):

```cpp
#include <cstddef>
#include <cstdint>
#include <string_view>

#include "forge/filesystem/virtual_filesystem.hpp"

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size)
{
    static forge::VirtualFileSystem vfs = [] {
        forge::VirtualFileSystem instance;
        (void)instance.mount("assets", forge::Path::temp_directory());
        return instance;
    }();

    const std::string_view input(reinterpret_cast<const char*>(data), size);
    (void)vfs.resolve(input); // must never crash, hang, or escape the mount
    return 0;
}
```

## Rules once fuzzing lands

- Every new parser (Phase 3 file format, Phase 10 importers) ships with a
  fuzz target in the same change.
- Crash corpora are checked in under `fuzz/corpus/<target>/` and replayed as
  regression tests in CI.
- CI runs short fuzz sessions (minutes) per pull request; long sessions run
  on a schedule.
