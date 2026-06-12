#include "forge/foundation/platform.hpp"

#include <functional>
#include <thread>

namespace forge
{

const char* platform_name() noexcept
{
#if FORGE_PLATFORM_WINDOWS
    return "Windows";
#elif FORGE_PLATFORM_MACOS
    return "macOS";
#elif FORGE_PLATFORM_LINUX
    return "Linux";
#endif
}

const char* architecture_name() noexcept
{
#if FORGE_ARCH_X64
    return "x64";
#elif FORGE_ARCH_ARM64
    return "ARM64";
#else
    return "unknown";
#endif
}

std::uint32_t current_thread_id() noexcept
{
    // Hashing std::thread::id avoids platform headers. The id is stable for
    // the lifetime of the thread and unique within a single process run.
    thread_local const std::uint32_t id = static_cast<std::uint32_t>(
        std::hash<std::thread::id>{}(std::this_thread::get_id()));
    return id;
}

} // namespace forge
