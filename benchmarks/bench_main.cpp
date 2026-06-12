// Entry point for the Forge3D benchmark runner.
//
//   forge_benchmarks                     run everything
//   forge_benchmarks --filter math.      run matching benchmarks
//   forge_benchmarks --min-time 0.5      calibrate to 0.5 s per benchmark

#include <cstdio>
#include <cstdlib>
#include <string_view>

#include "bench_framework.hpp"
#include "forge/foundation/build_info.hpp"

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
            std::printf("usage: forge_benchmarks [--filter <substring>] "
                        "[--min-time <seconds>]\n");
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
