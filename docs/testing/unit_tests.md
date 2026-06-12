# Unit Tests

Forge3D uses a small internal test framework (`tests/test_framework.hpp`) —
no external test dependencies in Phase 1.

## Running

Via CTest (recommended; this is what CI runs):

```bash
ctest --preset debug
# or, without presets:
ctest --test-dir build/debug --output-on-failure
```

Directly, with name filtering:

```bash
./build/debug/tests/forge_tests                    # everything
./build/debug/tests/forge_tests --filter math.     # one area
./build/debug/tests/forge_tests --filter vfs       # any substring
./build/debug/tests/forge_tests --list             # print test names
```

The runner prints `[ PASS ]` / `[ FAIL ]` per test, each failed check with
its expression, file and line, plus a summary, and exits non-zero when any
test fails.

## Writing tests

```cpp
#include "forge/math/vec3.hpp"
#include "test_framework.hpp"

FORGE_TEST_CASE("math.vec3_cross")
{
    const forge::Vec3 result = cross(forge::Vec3::unit_x(), forge::Vec3::unit_y());
    FORGE_CHECK(near_equal(result, forge::Vec3::unit_z()));   // record and continue
    FORGE_REQUIRE(result.length() > 0.0f);                    // abort test on failure
    FORGE_CHECK_EQ(1 + 1, 2);                                 // prints both values on failure
    FORGE_CHECK_NEAR(result.length(), 1.0f, 1e-6f);           // float comparison
}
```

Rules:

- Name tests `area.behavior` (`filesystem.path_join`) so `--filter area.`
  selects a suite; add the file to `tests/CMakeLists.txt`.
- Tests must be deterministic and offline. No sleeps as synchronization, no
  network, no reliance on test order.
- Filesystem tests create a scratch directory under the system temp
  directory and remove it (see `tests/filesystem/test_file.cpp` for the
  pattern).

## Test coverage in Phase 1

- foundation: `Expected`, `Result`, `ScopeExit`, time utilities, logger
- math: Vec2/3/4, Mat3, Mat4 (including projections/look-at), Quat
  (including slerp), Transform
- memory: linear allocator, system allocator, memory tracker
- filesystem: path operations, file I/O, directory creation, VFS mounting,
  resolution and traversal rejection
- threading: thread pool execution/futures/shutdown-drain, task groups,
  job-system `parallel_for`
- profiling: event recording, nesting, enable/disable, trace JSON and file
  output
