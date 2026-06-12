// forge_project_info — prints project metadata, build information and the
// compile-time feature configuration of this binary. Useful for verifying
// what a given build actually contains (CI artifacts, bug reports).

#include <cstdio>

#include "forge/foundation/build_info.hpp"
#include "forge/foundation/config.hpp"
#include "forge/foundation/version.hpp"

int main()
{
    const forge::BuildInfo& info = forge::build_info();

    std::printf("Project\n");
    std::printf("=======\n");
    std::printf("Name:          %.*s\n", static_cast<int>(info.project_name.size()),
                info.project_name.data());
    std::printf("Version:       %d.%d.%d\n", forge::version_major(),
                forge::version_minor(), forge::version_patch());
    std::printf("Commit:        %.*s\n", static_cast<int>(info.git_commit.size()),
                info.git_commit.data());

    std::printf("\nBuild\n");
    std::printf("=====\n");
    std::printf("Type:          %.*s\n", static_cast<int>(info.build_type.size()),
                info.build_type.data());
    std::printf("Compiler:      %.*s\n", static_cast<int>(info.compiler.size()),
                info.compiler.data());
    std::printf("Platform:      %.*s\n", static_cast<int>(info.platform.size()),
                info.platform.data());
    std::printf("Architecture:  %.*s\n", static_cast<int>(info.architecture.size()),
                info.architecture.data());
    std::printf("C++ standard:  %ld\n", info.cpp_standard);

    std::printf("\nCompile-time options\n");
    std::printf("====================\n");
    std::printf("FORGE_BUILD_DEBUG:       %d\n", FORGE_BUILD_DEBUG);
    std::printf("FORGE_ASSERTS_ENABLED:   %d\n", FORGE_ASSERTS_ENABLED);
    std::printf("FORGE_PROFILING_ENABLED: %d\n", FORGE_PROFILING_ENABLED);
    std::printf("FORGE_TRACING_ENABLED:   %d\n", FORGE_TRACING_ENABLED);

    return 0;
}
