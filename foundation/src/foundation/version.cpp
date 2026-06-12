#include "forge/foundation/version.hpp"

#include "forge/foundation/macros.hpp"

// The build system injects these; the fallbacks keep the file compilable
// standalone (e.g. in IDE syntax-only passes).
#if !defined(FORGE_VERSION_MAJOR)
    #define FORGE_VERSION_MAJOR 0
#endif
#if !defined(FORGE_VERSION_MINOR)
    #define FORGE_VERSION_MINOR 1
#endif
#if !defined(FORGE_VERSION_PATCH)
    #define FORGE_VERSION_PATCH 0
#endif

namespace forge
{

std::string_view project_name() noexcept
{
    return "Forge3D";
}

int version_major() noexcept
{
    return FORGE_VERSION_MAJOR;
}

int version_minor() noexcept
{
    return FORGE_VERSION_MINOR;
}

int version_patch() noexcept
{
    return FORGE_VERSION_PATCH;
}

std::string_view version_string() noexcept
{
    return FORGE_STRINGIFY(FORGE_VERSION_MAJOR) "." FORGE_STRINGIFY(
        FORGE_VERSION_MINOR) "." FORGE_STRINGIFY(FORGE_VERSION_PATCH);
}

} // namespace forge
