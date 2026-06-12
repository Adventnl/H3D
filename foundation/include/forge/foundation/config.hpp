#pragma once

// Build configuration switches.
//
// FORGE_BUILD_DEBUG / FORGE_BUILD_RELEASE follow NDEBUG.
// Feature toggles (asserts, profiling, tracing) may be overridden by the
// build system via compile definitions; sensible defaults apply otherwise.

#if defined(NDEBUG)
    #define FORGE_BUILD_DEBUG 0
    #define FORGE_BUILD_RELEASE 1
#else
    #define FORGE_BUILD_DEBUG 1
    #define FORGE_BUILD_RELEASE 0
#endif

// FORGE_ASSERT compiles to nothing when disabled. Defaults to on in debug.
#if !defined(FORGE_ASSERTS_ENABLED)
    #define FORGE_ASSERTS_ENABLED FORGE_BUILD_DEBUG
#endif

// FORGE_PROFILE_* macros compile to nothing when disabled.
// The Profiler object itself is always available.
#if !defined(FORGE_PROFILING_ENABLED)
    #define FORGE_PROFILING_ENABLED 1
#endif

// When enabled, the desktop shell writes a Chrome trace on shutdown.
#if !defined(FORGE_TRACING_ENABLED)
    #define FORGE_TRACING_ENABLED 0
#endif

namespace forge
{

/// "Debug" or "Release" depending on how this translation unit was built.
[[nodiscard]] constexpr const char* build_type_name() noexcept
{
#if FORGE_BUILD_DEBUG
    return "Debug";
#else
    return "Release";
#endif
}

} // namespace forge
