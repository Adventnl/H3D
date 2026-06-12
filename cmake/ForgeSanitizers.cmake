# Sanitizer configuration for Forge3D targets.
#
# Usage:
#   forge_enable_sanitizers(<target>)
#
# Honors FORGE_ENABLE_SANITIZERS. Address + undefined behavior sanitizers are
# enabled on GCC/Clang. MSVC users can opt into /fsanitize=address manually;
# it is not enabled by default because it conflicts with some debug runtimes.

function(forge_enable_sanitizers target)
    if(NOT FORGE_ENABLE_SANITIZERS)
        return()
    endif()

    if(MSVC)
        message(STATUS "Forge3D: FORGE_ENABLE_SANITIZERS is not applied to MSVC target '${target}'")
        return()
    endif()

    set(forge_sanitizer_flags -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_compile_options(${target} PRIVATE ${forge_sanitizer_flags})
    target_link_options(${target} PRIVATE ${forge_sanitizer_flags})
endfunction()
