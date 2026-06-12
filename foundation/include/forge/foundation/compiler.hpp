#pragma once

// Compiler detection and compiler-specific helpers.
//
// Exactly one FORGE_COMPILER_* macro is defined to 1; the others are 0.
// Note: clang-cl defines _MSC_VER, so __clang__ is checked first.

#if defined(__clang__)
    #define FORGE_COMPILER_CLANG 1
#else
    #define FORGE_COMPILER_CLANG 0
#endif

#if defined(_MSC_VER) && !FORGE_COMPILER_CLANG
    #define FORGE_COMPILER_MSVC 1
#else
    #define FORGE_COMPILER_MSVC 0
#endif

#if defined(__GNUC__) && !FORGE_COMPILER_CLANG
    #define FORGE_COMPILER_GCC 1
#else
    #define FORGE_COMPILER_GCC 0
#endif

#if !(FORGE_COMPILER_MSVC || FORGE_COMPILER_CLANG || FORGE_COMPILER_GCC)
    #error "Forge3D: unsupported compiler"
#endif

// ---------------------------------------------------------------------------
// Attributes and intrinsics
// ---------------------------------------------------------------------------
#if FORGE_COMPILER_MSVC
    #define FORGE_FORCE_INLINE __forceinline
    #define FORGE_NO_INLINE __declspec(noinline)
    #define FORGE_DEBUG_BREAK() __debugbreak()
#else
    #define FORGE_FORCE_INLINE inline __attribute__((always_inline))
    #define FORGE_NO_INLINE __attribute__((noinline))
    #define FORGE_DEBUG_BREAK() __builtin_trap()
#endif

// Standard since C++11; usable in any function for diagnostics.
#define FORGE_FUNCTION_NAME __func__
