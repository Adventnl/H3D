# forge_bench: installable benchmark runner. Reuses the benchmark object
# library when benchmarks are enabled; otherwise builds a stub that explains
# how to enable them.

add_executable(forge_bench src/main.cpp)

if(TARGET forge_bench_core)
    target_link_libraries(forge_bench PRIVATE forge_bench_core)
else()
    target_link_libraries(forge_bench PRIVATE forge::foundation)
    target_compile_definitions(forge_bench PRIVATE FORGE_NO_BENCHMARKS=1)
endif()

forge_apply_target_defaults(forge_bench)
forge_install_executable(forge_bench)
