#pragma once

// Platform and architecture detection.
//
// Exactly one FORGE_PLATFORM_* macro and one FORGE_ARCH_* macro is defined
// to 1; the rest are defined to 0 so they can be used with plain `#if`.

#include <cstdint>

// ---------------------------------------------------------------------------
// Operating system
// ---------------------------------------------------------------------------
#if defined(_WIN32)
    #define FORGE_PLATFORM_WINDOWS 1
#else
    #define FORGE_PLATFORM_WINDOWS 0
#endif

#if defined(__APPLE__)
    #define FORGE_PLATFORM_MACOS 1
#else
    #define FORGE_PLATFORM_MACOS 0
#endif

#if defined(__linux__)
    #define FORGE_PLATFORM_LINUX 1
#else
    #define FORGE_PLATFORM_LINUX 0
#endif

#if !(FORGE_PLATFORM_WINDOWS || FORGE_PLATFORM_MACOS || FORGE_PLATFORM_LINUX)
    #error "Forge3D: unsupported platform"
#endif

// ---------------------------------------------------------------------------
// CPU architecture
// ---------------------------------------------------------------------------
#if defined(_M_X64) || defined(__x86_64__)
    #define FORGE_ARCH_X64 1
#else
    #define FORGE_ARCH_X64 0
#endif

#if defined(_M_ARM64) || defined(__aarch64__)
    #define FORGE_ARCH_ARM64 1
#else
    #define FORGE_ARCH_ARM64 0
#endif

#if !(FORGE_ARCH_X64 || FORGE_ARCH_ARM64)
    #define FORGE_ARCH_UNKNOWN 1
#else
    #define FORGE_ARCH_UNKNOWN 0
#endif

namespace forge
{

/// Human-readable platform name, e.g. "Windows".
[[nodiscard]] const char* platform_name() noexcept;

/// Human-readable architecture name, e.g. "x64".
[[nodiscard]] const char* architecture_name() noexcept;

/// Stable identifier for the calling thread, suitable for logs and traces.
/// Derived from std::thread::id; unique per thread within a single run.
[[nodiscard]] std::uint32_t current_thread_id() noexcept;

} // namespace forge
