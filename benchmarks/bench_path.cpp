#include <cstdint>

#include "bench_framework.hpp"
#include "forge/filesystem/path.hpp"

using forge::Path;
using forge::bench::do_not_optimize;

FORGE_BENCHMARK("filesystem.path_normalize")
{
    const Path messy("assets/textures/../materials/./wood/../stone/diffuse.png");
    for (std::uint64_t index = 0; index < state.iterations(); ++index)
    {
        do_not_optimize(messy.normalized());
    }
}

FORGE_BENCHMARK("filesystem.path_join")
{
    const Path root("projects/forge/scenes");
    const Path leaf("shot_010/lighting.forge");
    for (std::uint64_t index = 0; index < state.iterations(); ++index)
    {
        do_not_optimize(root / leaf);
    }
}

FORGE_BENCHMARK("filesystem.path_decompose")
{
    const Path path("projects/forge/scenes/shot_010/lighting_v002.forge");
    for (std::uint64_t index = 0; index < state.iterations(); ++index)
    {
        do_not_optimize(path.filename());
        do_not_optimize(path.extension());
        do_not_optimize(path.parent());
    }
}
