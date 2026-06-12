# Common compiler configuration applied to every Forge3D target.
#
# Usage:
#   forge_apply_target_defaults(<target>)
#
# Applies the C++23 language level, baseline compiler options, the warning
# set from ForgeWarnings.cmake and (optionally) sanitizers from
# ForgeSanitizers.cmake.

function(forge_set_common_options target)
    set_target_properties(${target} PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

    if(MSVC)
        target_compile_options(${target} PRIVATE
            /utf-8          # Source and execution character set are UTF-8.
            /permissive-    # Standards-conforming mode.
            /Zc:__cplusplus # Report the real __cplusplus value.
            /Zc:preprocessor
            /EHsc           # Standard C++ exception semantics.
            /bigobj)
    endif()

    if(WIN32)
        target_compile_definitions(${target} PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    endif()
endfunction()

function(forge_apply_target_defaults target)
    forge_set_common_options(${target})
    forge_enable_warnings(${target})
    forge_enable_sanitizers(${target})
endfunction()
