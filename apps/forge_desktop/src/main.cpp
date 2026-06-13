// Forge3D desktop shell — Phase 2.
//
// This drives the real Phase 2 application runtime: it builds an Application
// (events, input, operators, commands, keymap, preferences, a headless window),
// attaches the ui workspace/editor registries, then runs a frame loop. There is
// still no GPU or real viewport rendering — editors are logical placeholders.
//
//   forge_desktop [--frames N] [--headless] [--windowed]
//                 [--trace <file>] [--log-file <file>]

#include <cstdio>
#include <cstdlib>
#include <format>
#include <string>
#include <string_view>

#include "forge/app/application.hpp"
#include "forge/foundation/build_info.hpp"
#include "forge/foundation/log.hpp"
#include "forge/foundation/scope_exit.hpp"
#include "forge/foundation/version.hpp"
#include "forge/profiling/profiler.hpp"
#include "forge/profiling/profiler_scope.hpp"
#include "forge/profiling/trace_writer.hpp"
#include "forge/ui/editor.hpp"
#include "forge/ui/editor_registry.hpp"
#include "forge/ui/workspace_registry.hpp"

namespace
{

constexpr std::string_view kLogCategory = "desktop";

struct Options
{
    int frames = 5;
    bool headless = true;
    std::string trace_path;
    std::string log_file_path;
    bool tracing = false;
};

Options parse_arguments(int argc, char** argv)
{
    Options options;
    for (int index = 1; index < argc; ++index)
    {
        const std::string_view argument = argv[index];
        if (argument == "--frames" && index + 1 < argc)
        {
            options.frames = std::atoi(argv[++index]);
            if (options.frames < 0)
            {
                options.frames = 0;
            }
        }
        else if (argument == "--headless")
        {
            options.headless = true;
        }
        else if (argument == "--windowed")
        {
            options.headless = false;
        }
        else if (argument == "--trace" && index + 1 < argc)
        {
            options.trace_path = argv[++index];
            options.tracing = true;
        }
        else if (argument == "--log-file" && index + 1 < argc)
        {
            options.log_file_path = argv[++index];
        }
        else if (argument == "--help" || argument == "-h")
        {
            std::printf("usage: forge_desktop [--frames N] [--headless] [--windowed] "
                        "[--trace <file>] [--log-file <file>]\n");
            std::exit(0);
        }
    }
    if (options.tracing && options.trace_path.empty())
    {
        options.trace_path = "forge_trace.json";
    }
    return options;
}

} // namespace

int main(int argc, char** argv)
{
    const Options options = parse_arguments(argc, argv);

    forge::ApplicationConfig config;
    config.application_name = "Forge3D";
    config.headless = options.headless;
    config.enable_tracing = options.tracing;
    config.log_file_path = options.log_file_path;

    forge::Application app(config);
    if (auto ready = app.initialize(); !ready)
    {
        std::fprintf(stderr, "fatal: %s\n", ready.error().to_string().c_str());
        return 1;
    }
    FORGE_SCOPE_EXIT(app.shutdown());

    // Attach the ui layer: workspaces (also the WorkspaceService) and editors.
    forge::ui::WorkspaceRegistry workspaces;
    forge::ui::register_default_workspaces(workspaces);
    app.set_workspace_service(&workspaces);

    forge::ui::EditorRegistry editors;
    forge::ui::register_default_editors(editors);

    const forge::ui::Workspace* active = workspaces.active();
    std::printf("%s\n", forge::build_info_string().c_str());
    std::printf("\nForge3D Phase 2 application shell\n");
    std::printf("  active workspace : %s\n",
                active != nullptr ? active->display_name().c_str() : "(none)");
    std::printf("  workspaces       : %zu\n", workspaces.size());
    std::printf("  registered editors: %zu\n", editors.size());
    std::printf("  registered operators: %zu\n", app.operators().size());
    std::printf("  keymap bindings  : %zu\n", app.keymap().size());
    std::printf("  window backend   : %s (%s)\n", app.windows().backend().name().data(),
                options.headless ? "headless" : "windowed");

    // Prove the editor framework: instantiate the editor for each area of the
    // active workspace and report its placeholder status.
    if (active != nullptr)
    {
        forge::ui::EditorContext editor_context;
        editor_context.app = &app.context();
        for (const forge::ui::Area* area : active->screen().areas())
        {
            auto editor = editors.create(area->editor_type());
            if (editor)
            {
                editor->on_open(editor_context);
                FORGE_LOG_INFO(kLogCategory,
                               std::format("area {} -> {}", area->id(),
                                           editor->status_text()));
                editor->on_close(editor_context);
            }
        }
    }

    // Frame loop driven through the Application pipeline.
    int frames_run = 0;
    for (int i = 0; i < options.frames; ++i)
    {
        FORGE_PROFILE_SCOPE("frame");
        const bool keep_going = app.run_frame();
        ++frames_run;
        if (!keep_going)
        {
            break;
        }
    }
    std::printf("  frames run       : %d\n", frames_run);

    if (options.tracing)
    {
        const auto events = forge::Profiler::instance().events();
        const auto written =
            forge::write_chrome_trace_file(forge::Path(options.trace_path), events);
        if (written)
        {
            FORGE_LOG_INFO(kLogCategory,
                           std::format("wrote {} profile events to {}", events.size(),
                                       options.trace_path));
        }
        else
        {
            FORGE_LOG_WARNING(kLogCategory,
                              std::format("failed to write trace: {}",
                                          written.error().to_string()));
        }
    }

    std::printf("  shutdown         : clean\n");
    return 0;
}
