// forge_cli — command-line utility for Forge3D.
//
//   forge_cli --help
//   forge_cli --version
//   forge_cli doctor
//   forge_cli paths
//   forge_cli benchmark-info

#include <cstdio>
#include <filesystem>
#include <string_view>
#include <thread>

#include "forge/filesystem/file.hpp"
#include "forge/filesystem/path.hpp"
#include "forge/foundation/build_info.hpp"
#include "forge/foundation/version.hpp"

namespace
{

int print_help()
{
    std::printf(
        "Forge3D command-line utility\n"
        "\n"
        "usage: forge_cli <command>\n"
        "\n"
        "commands:\n"
        "  --help, -h        show this help\n"
        "  --version, -v     print the version and build information\n"
        "  doctor            print runtime diagnostics\n"
        "  paths             print important directories\n"
        "  benchmark-info    explain how to run the benchmark suite\n");
    return 0;
}

int print_version()
{
    std::printf("%s\n", forge::build_info_string().c_str());
    return 0;
}

int run_doctor()
{
    std::printf("Forge3D doctor\n");
    std::printf("==============\n\n");
    std::printf("%s\n\n", forge::build_info_string().c_str());

    const unsigned int threads = std::thread::hardware_concurrency();
    std::printf("Hardware threads:  %u\n", threads);

    const forge::Path current = forge::Path::current_directory();
    std::printf("Working directory: %s\n",
                current.empty() ? "(unavailable)" : current.native_string().c_str());

    const forge::Path temp = forge::Path::temp_directory();
    std::printf("Temp directory:    %s\n",
                temp.empty() ? "(unavailable)" : temp.native_string().c_str());

    // Probe that temporary files can actually be written and read.
    bool temp_writable = false;
    if (!temp.empty())
    {
        const forge::Path probe = temp / "forge_doctor_probe.txt";
        if (forge::write_text_file(probe, "probe").has_value())
        {
            const auto read_back = forge::read_text_file(probe);
            temp_writable = read_back.has_value() && *read_back == "probe";
            std::error_code ignored;
            std::filesystem::remove(probe.raw(), ignored);
        }
    }
    std::printf("Temp writable:     %s\n", temp_writable ? "yes" : "NO");

    std::printf("\nDiagnostics %s\n", temp_writable ? "passed" : "found problems");
    return temp_writable ? 0 : 1;
}

int run_paths()
{
    const forge::Path current = forge::Path::current_directory();
    const forge::Path temp = forge::Path::temp_directory();

    std::printf("working directory: %s\n",
                current.empty() ? "(unavailable)" : current.native_string().c_str());
    std::printf("temp directory:    %s\n",
                temp.empty() ? "(unavailable)" : temp.native_string().c_str());
    return 0;
}

int run_benchmark_info()
{
    std::printf(
        "Forge3D benchmark suite\n"
        "\n"
        "The benchmarks live in a separate executable. From the build "
        "directory run:\n"
        "\n"
        "  forge_bench                      run every benchmark\n"
        "  forge_bench --filter math.       run benchmarks matching a name\n"
        "  forge_bench --min-time 0.5       calibrate for longer, steadier runs\n"
        "\n"
        "Current suites: math (Vec3/Mat4/Quat), memory (linear and system\n"
        "allocators), threading (thread pool), filesystem (path operations).\n"
        "Hardware threads available on this machine: %u\n",
        std::thread::hardware_concurrency());
    return 0;
}

} // namespace

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        return print_help();
    }

    const std::string_view command = argv[1];
    if (command == "--help" || command == "-h" || command == "help")
    {
        return print_help();
    }
    if (command == "--version" || command == "-v" || command == "version")
    {
        return print_version();
    }
    if (command == "doctor")
    {
        return run_doctor();
    }
    if (command == "paths")
    {
        return run_paths();
    }
    if (command == "benchmark-info")
    {
        return run_benchmark_info();
    }

    std::printf("unknown command: %.*s\n\n", static_cast<int>(command.size()),
                command.data());
    print_help();
    return 2;
}
