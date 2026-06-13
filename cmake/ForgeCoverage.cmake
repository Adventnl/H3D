# Coverage instrumentation for Forge3D targets.
#
# Usage:
#   forge_enable_coverage(<target>)
#
# Honors FORGE_ENABLE_COVERAGE. Uses gcov-style instrumentation on GCC/Clang.
# MSVC has no equivalent flag here and is skipped with a notice.

function(forge_enable_coverage target)
    if(NOT FORGE_ENABLE_COVERAGE)
        return()
    endif()

    if(MSVC)
        message(STATUS "Forge3D: FORGE_ENABLE_COVERAGE is not applied to MSVC target '${target}'")
        return()
    endif()

    set(forge_coverage_flags --coverage -fprofile-arcs -ftest-coverage)
    target_compile_options(${target} PRIVATE ${forge_coverage_flags})
    target_link_options(${target} PRIVATE --coverage)
endfunction()
