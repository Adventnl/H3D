// forge_bench — installable benchmark runner.
//
// When benchmarks are built (FORGE_BUILD_BENCHMARKS=ON) this links the same
// benchmark object library as the in-tree forge_benchmarks runner. When they
// are disabled, the executable still builds and explains how to enable them.

#include <cstdio>

#include "forge/foundation/build_info.hpp"

#if !defined(FORGE_NO_BENCHMARKS)
    #include <cstdlib>
    #include <string_view>

    #include "bench_framework.hpp"
#endif

#if defined(FORGE_NO_BENCHMARKS)

int main()
{
    std::printf("%s\n\n", forge::build_info_string().c_str());
    std::printf("forge_bench was built without benchmarks.\n"
                "Reconfigure with -DFORGE_BUILD_BENCHMARKS=ON to enable them.\n");
    return 0;
}

#else

int main(int argc, char** argv)
{
    std::string_view filter;
    double min_seconds = 0.2;

    for (int index = 1; index < argc; ++index)
    {
        const std::string_view argument = argv[index];
        if (argument == "--filter" && index + 1 < argc)
        {
            filter = argv[++index];
        }
        else if (argument == "--min-time" && index + 1 < argc)
        {
            min_seconds = std::strtod(argv[++index], nullptr);
            if (min_seconds <= 0.0)
            {
                min_seconds = 0.2;
            }
        }
        else if (argument == "--help" || argument == "-h")
        {
            std::printf(
                "usage: forge_bench [--filter <substring>] [--min-time <seconds>]\n");
            return 0;
        }
        else
        {
            std::printf("unknown argument: %.*s\n", static_cast<int>(argument.size()),
                        argument.data());
            return 2;
        }
    }

    std::printf("%s\n\n", forge::build_info_string().c_str());
    return forge::bench::run_all_benchmarks(filter, min_seconds);
}

#endif
