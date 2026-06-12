# forge_foundation: the base library every Forge3D target depends on.

add_library(forge_foundation STATIC
    src/foundation/assert.cpp
    src/foundation/build_info.cpp
    src/foundation/error.cpp
    src/foundation/log.cpp
    src/foundation/platform.cpp
    src/foundation/time.cpp
    src/foundation/version.cpp
    src/math/mat3.cpp
    src/math/mat4.cpp
    src/math/quat.cpp
    src/math/transform.cpp
    src/memory/linear_allocator.cpp
    src/memory/memory_tracker.cpp
    src/memory/system_allocator.cpp
    src/filesystem/file.cpp
    src/filesystem/path.cpp
    src/filesystem/virtual_filesystem.cpp
    src/threading/job_system.cpp
    src/threading/task_group.cpp
    src/threading/thread_pool.cpp
    src/profiling/profiler.cpp
    src/profiling/trace_writer.cpp
)
add_library(forge::foundation ALIAS forge_foundation)

target_include_directories(forge_foundation PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(forge_foundation PUBLIC Threads::Threads)

# Feature toggles must be PUBLIC: headers (macros, config.hpp) change meaning
# with them, and every consumer has to agree with the library build.
target_compile_definitions(forge_foundation PUBLIC
    FORGE_PROFILING_ENABLED=$<BOOL:${FORGE_ENABLE_PROFILING}>
    FORGE_TRACING_ENABLED=$<BOOL:${FORGE_ENABLE_TRACING}>
)

# Version and commit are baked into specific translation units only, so a
# commit change does not rebuild the world.
set_source_files_properties(src/foundation/version.cpp PROPERTIES
    COMPILE_DEFINITIONS
    "FORGE_VERSION_MAJOR=${PROJECT_VERSION_MAJOR};FORGE_VERSION_MINOR=${PROJECT_VERSION_MINOR};FORGE_VERSION_PATCH=${PROJECT_VERSION_PATCH}")
set_source_files_properties(src/foundation/build_info.cpp PROPERTIES
    COMPILE_DEFINITIONS "FORGE_GIT_COMMIT=\"${FORGE_GIT_COMMIT}\"")

forge_apply_target_defaults(forge_foundation)
forge_install_library(forge_foundation)
