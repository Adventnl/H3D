// forge_cli — command-line utility for Forge3D.
//
//   forge_cli --help
//   forge_cli --version
//   forge_cli doctor
//   forge_cli paths
//   forge_cli benchmark-info
//
// Phase 2 commands (available when the app/ui modules are built):
//   forge_cli app-info
//   forge_cli list-workspaces
//   forge_cli list-editors
//   forge_cli list-operators
//   forge_cli keymap
//   forge_cli preferences-defaults

#include <cstdio>
#include <filesystem>
#include <string_view>
#include <thread>

#include "forge/filesystem/file.hpp"
#include "forge/filesystem/path.hpp"
#include "forge/foundation/build_info.hpp"
#include "forge/foundation/version.hpp"

#if FORGE_CLI_HAS_APP
    #include "forge/app/keymap.hpp"
    #include "forge/app/operator_registry.hpp"
    #include "forge/app/preferences.hpp"
    #include "forge/ui/editor.hpp"
    #include "forge/ui/editor_registry.hpp"
    #include "forge/ui/workspace_registry.hpp"
#endif

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
        "  --help, -h            show this help\n"
        "  --version, -v         print the version and build information\n"
        "  doctor                print runtime diagnostics\n"
        "  paths                 print important directories\n"
        "  benchmark-info        explain how to run the benchmark suite\n"
#if FORGE_CLI_HAS_APP
        "  app-info              summarize the Phase 2 application shell\n"
        "  list-workspaces       list the default workspaces\n"
        "  list-editors          list the registered editor types\n"
        "  list-operators        list the registered operators\n"
        "  keymap                list the default keymap bindings\n"
        "  preferences-defaults  print the default preferences file\n"
#endif
    );
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
        "  forge_bench --quick              fast pass (CI)\n"
        "  forge_bench --min-time 0.5       calibrate for longer, steadier runs\n"
        "\n"
        "Hardware threads available on this machine: %u\n",
        std::thread::hardware_concurrency());
    return 0;
}

#if FORGE_CLI_HAS_APP

int run_app_info()
{
    forge::OperatorRegistry operators;
    forge::register_default_operators(operators);
    forge::ui::WorkspaceRegistry workspaces;
    forge::ui::register_default_workspaces(workspaces);
    forge::ui::EditorRegistry editors;
    forge::ui::register_default_editors(editors);
    const forge::Keymap keymap = forge::Keymap::default_keymap();

    std::printf("Forge3D %s — Phase 2 application shell\n",
                std::string(forge::version_string()).c_str());
    std::printf("  workspaces : %zu (active: %s)\n", workspaces.size(),
                workspaces.active_workspace_name().c_str());
    std::printf("  editors    : %zu\n", editors.size());
    std::printf("  operators  : %zu\n", operators.size());
    std::printf("  keymap     : %zu bindings\n", keymap.size());
    std::printf("\nThe shell runs headlessly (NullWindowBackend). No GPU/scene yet.\n");
    return 0;
}

int run_list_workspaces()
{
    forge::ui::WorkspaceRegistry workspaces;
    forge::ui::register_default_workspaces(workspaces);
    std::printf("Workspaces (%zu):\n", workspaces.size());
    for (const forge::ui::Workspace* workspace : workspaces.list())
    {
        std::printf("  %-16s  %zu area(s):", workspace->id().c_str(),
                    workspace->screen().area_count());
        for (const forge::ui::Area* area : workspace->screen().areas())
        {
            std::printf(" %s", forge::ui::editor_type_name(area->editor_type()).data());
        }
        std::printf("\n");
    }
    return 0;
}

int run_list_editors()
{
    forge::ui::EditorRegistry editors;
    forge::ui::register_default_editors(editors);
    std::printf("Editors (%zu):\n", editors.size());
    for (const forge::ui::EditorType type : editors.registered_types())
    {
        auto editor = editors.create(type);
        std::printf("  %-22s %s\n",
                    forge::ui::editor_type_id(type).data(),
                    editor ? std::string(editor->display_name()).c_str() : "(failed)");
    }
    return 0;
}

int run_list_operators()
{
    forge::OperatorRegistry operators;
    forge::register_default_operators(operators);
    std::printf("Operators (%zu):\n", operators.size());
    for (const forge::Operator* op : operators.list())
    {
        std::printf("  %-24s [%-11s] %s\n", op->id().c_str(), op->category().c_str(),
                    op->display_name().c_str());
    }
    return 0;
}

int run_keymap()
{
    const forge::Keymap keymap = forge::Keymap::default_keymap();
    std::printf("Keymap (ForgeDefault, %zu bindings):\n", keymap.size());
    for (const forge::Keymap::Binding& binding : keymap.bindings())
    {
        std::printf("  %-16s -> %s\n", binding.shortcut.to_string().c_str(),
                    binding.action_id.c_str());
    }
    return 0;
}

int run_preferences_defaults()
{
    const forge::Preferences prefs = forge::Preferences::defaults();
    std::printf("%s", prefs.serialize().c_str());
    return 0;
}

#endif // FORGE_CLI_HAS_APP

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
#if FORGE_CLI_HAS_APP
    if (command == "app-info")
    {
        return run_app_info();
    }
    if (command == "list-workspaces")
    {
        return run_list_workspaces();
    }
    if (command == "list-editors")
    {
        return run_list_editors();
    }
    if (command == "list-operators")
    {
        return run_list_operators();
    }
    if (command == "keymap")
    {
        return run_keymap();
    }
    if (command == "preferences-defaults")
    {
        return run_preferences_defaults();
    }
#endif

    std::printf("unknown command: %.*s\n\n", static_cast<int>(command.size()),
                command.data());
    print_help();
    return 2;
}
