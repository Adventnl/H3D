# Warning configuration for Forge3D targets.
#
# Usage:
#   forge_enable_warnings(<target>)
#
# Honors FORGE_ENABLE_WARNINGS_AS_ERRORS.

function(forge_enable_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE
            /W4
            /w14062  # enumerator in switch of enum is not handled
            /w14254  # operator: conversion, possible loss of data
            /w14296  # expression is always true/false
            /w14545  # expression before comma evaluates to a missing function
            /w14905  # wide string literal cast
            /w14906  # string literal cast
        )
        if(FORGE_ENABLE_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE /WX)
        endif()
    else()
        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wnon-virtual-dtor
            -Wcast-align
            -Woverloaded-virtual
            -Wdouble-promotion
        )
        if(FORGE_ENABLE_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    endif()
endfunction()
