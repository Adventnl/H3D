// Entry point for the Forge3D unit-test runner.
//
//   forge_tests                 run everything
//   forge_tests --filter math.  run tests whose name contains "math."
//   forge_tests --list          print test names

#include <cstdio>
#include <cstring>
#include <string_view>

#include "test_framework.hpp"

int main(int argc, char** argv)
{
    std::string_view filter;

    for (int index = 1; index < argc; ++index)
    {
        const std::string_view argument = argv[index];
        if (argument == "--list")
        {
            forge::test::list_tests();
            return 0;
        }
        if (argument == "--filter" && index + 1 < argc)
        {
            filter = argv[++index];
        }
        else if (argument.starts_with("--filter="))
        {
            filter = argument.substr(std::strlen("--filter="));
        }
        else if (argument == "--help" || argument == "-h")
        {
            std::printf("usage: forge_tests [--filter <substring>] [--list]\n");
            return 0;
        }
        else
        {
            std::printf("unknown argument: %.*s\n", static_cast<int>(argument.size()),
                        argument.data());
            return 2;
        }
    }

    const int failed = forge::test::run_all_tests(filter);
    return failed == 0 ? 0 : 1;
}
