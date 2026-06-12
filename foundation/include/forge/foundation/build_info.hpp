#pragma once

// Static information about how this binary was built.

#include <string>
#include <string_view>

namespace forge
{

struct BuildInfo
{
    std::string_view project_name;
    std::string_view version;
    std::string_view build_type;   // "Debug" or "Release"
    std::string_view compiler;     // e.g. "MSVC 19.40", "GCC 13.2", "Clang 17.0"
    std::string_view platform;     // "Windows", "macOS", "Linux"
    std::string_view architecture; // "x64", "ARM64", "unknown"
    std::string_view git_commit;   // short hash or "unknown"
    long cpp_standard = 0;         // value of __cplusplus
};

/// Build information for the running binary. The returned reference is valid
/// for the lifetime of the process.
[[nodiscard]] const BuildInfo& build_info() noexcept;

/// Multi-line, human-readable rendering of build_info().
[[nodiscard]] std::string build_info_string();

} // namespace forge
