# Benchmark framework and benchmark suites.
#
# forge_bench_core is an OBJECT library so its self-registering benchmark
# translation units are always linked into consumers (a STATIC library would
# let the linker drop them). apps/forge_bench reuses it for the installable
# benchmark executable.

set(FORGE_BENCH_SOURCES
    bench_framework.cpp
    bench_math.cpp
    bench_allocator.cpp
    bench_thread_pool.cpp
    bench_path.cpp
)

# Benchmark the app/ui layers when they are part of this build.
if(TARGET forge::app AND TARGET forge::ui)
    list(APPEND FORGE_BENCH_SOURCES bench_app_ui.cpp)
endif()

add_library(forge_bench_core OBJECT ${FORGE_BENCH_SOURCES})
target_include_directories(forge_bench_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(forge_bench_core PUBLIC forge::foundation)
if(TARGET forge::app AND TARGET forge::ui)
    target_link_libraries(forge_bench_core PUBLIC forge::app forge::ui)
endif()
forge_apply_target_defaults(forge_bench_core)

# Developer-facing runner inside the build tree.
add_executable(forge_benchmarks bench_main.cpp)
target_link_libraries(forge_benchmarks PRIVATE forge_bench_core)
forge_apply_target_defaults(forge_benchmarks)
