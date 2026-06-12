#include "forge/foundation/build_info.hpp"

#include <format>

#include "forge/foundation/compiler.hpp"
#include "forge/foundation/config.hpp"
#include "forge/foundation/platform.hpp"
#include "forge/foundation/version.hpp"

namespace forge
{
namespace
{

std::string make_compiler_string()
{
#if FORGE_COMPILER_MSVC
    return std::format("MSVC {}.{}", _MSC_VER / 100, _MSC_VER % 100);
#elif FORGE_COMPILER_CLANG
    return std::format("Clang {}.{}", __clang_major__, __clang_minor__);
#elif FORGE_COMPILER_GCC
    return std::format("GCC {}.{}", __GNUC__, __GNUC_MINOR__);
#endif
}

const char* git_commit_string() noexcept
{
#if defined(FORGE_GIT_COMMIT)
    return FORGE_GIT_COMMIT;
#else
    return "unknown";
#endif
}

} // namespace

const BuildInfo& build_info() noexcept
{
    static const std::string compiler = make_compiler_string();
    static const BuildInfo info{
        .project_name = project_name(),
        .version = version_string(),
        .build_type = build_type_name(),
        .compiler = compiler,
        .platform = platform_name(),
        .architecture = architecture_name(),
        .git_commit = git_commit_string(),
        .cpp_standard = __cplusplus,
    };
    return info;
}

std::string build_info_string()
{
    const BuildInfo& info = build_info();
    return std::format(
        "{} {}\n"
        "Build:        {}\n"
        "Compiler:     {}\n"
        "Platform:     {} {}\n"
        "C++ standard: {}\n"
        "Commit:       {}",
        info.project_name, info.version, info.build_type, info.compiler,
        info.platform, info.architecture, info.cpp_standard, info.git_commit);
}

} // namespace forge
