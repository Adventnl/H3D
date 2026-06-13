#include "forge/app/application.hpp"

#include <algorithm>
#include <format>

#include "forge/foundation/build_info.hpp"
#include "forge/foundation/log.hpp"

namespace forge
{
namespace
{
constexpr std::string_view kLogCategory = "app";
}

Application::Application(ApplicationConfig config) : config_(std::move(config))
{
    config_.apply_defaults();
}

Application::~Application()
{
    shutdown();
}

void Application::build_context()
{
    context_.runtime = &runtime_;
    context_.events = &events_;
    context_.windows = &windows_;
    context_.operators = &operators_;
    context_.commands = &commands_;
    context_.keymap = &keymap_;
    context_.preferences = &preferences_;
    context_.input = &input_;
    context_.modal_tools = &modal_tools_;
    context_.logger = &default_logger();
    // context_.workspaces stays null until the ui layer attaches one.
}

Result<void> Application::initialize()
{
    if (initialized_)
    {
        return success();
    }

    config_.apply_defaults();

    if (!config_.log_file_path.empty() &&
        !default_logger().open_file(config_.log_file_path))
    {
        FORGE_LOG_WARNING(kLogCategory,
                          std::format("could not open log file '{}'", config_.log_file_path));
    }

    FORGE_LOG_INFO(kLogCategory,
                   std::format("starting {} {}", config_.application_name, config_.version));

    if (auto runtime_ready = runtime_.initialize(); !runtime_ready)
    {
        return runtime_ready;
    }

    keymap_ = Keymap::default_keymap();
    register_default_operators(operators_);
    preferences_ = Preferences::defaults();
    build_context();

    // A logical primary window for the shell (headless when configured).
    WindowConfig window_config;
    window_config.title = config_.application_name;
    window_config.headless = config_.headless;
    windows_.create_window(window_config);

    initialized_ = true;
    FORGE_LOG_INFO(kLogCategory,
                   std::format("ready: {} operators, {} keymap bindings",
                               operators_.size(), keymap_.size()));
    return success();
}

void Application::shutdown()
{
    if (!initialized_)
    {
        return;
    }
    FORGE_LOG_INFO(kLogCategory, "shutting down");
    modal_tools_.cancel(context_);
    events_.clear();
    runtime_.shutdown();
    initialized_ = false;
}

void Application::set_workspace_service(WorkspaceService* service)
{
    context_.workspaces = service;
}

bool Application::run_frame()
{
    if (runtime_.quit_requested())
    {
        return false;
    }

    input_.new_frame();
    runtime_.begin_frame();
    events_.publish(Event::frame_begin());

    // Pump window events and route each through the input pipeline.
    windows_.poll_events();
    if (Window* window = windows_.primary_window())
    {
        Event event;
        while (window->poll_event(event))
        {
            dispatch_event(event);
        }
    }
    events_.dispatch_queued();

    if (windows_.quit_requested())
    {
        runtime_.request_quit();
    }

    events_.publish(Event::frame_end());
    runtime_.end_frame();

    return !runtime_.quit_requested();
}

int Application::run(int frame_count)
{
    if (!initialized_)
    {
        if (auto ready = initialize(); !ready)
        {
            FORGE_LOG_FATAL(kLogCategory,
                            std::format("initialization failed: {}",
                                        ready.error().to_string()));
            return 0;
        }
    }

    int limit = frame_count;
    if (config_.frame_limit.has_value())
    {
        limit = (limit < 0) ? *config_.frame_limit : std::min(limit, *config_.frame_limit);
    }
    if (limit < 0)
    {
        limit = 0;
    }

    int frames_run = 0;
    for (int i = 0; i < limit; ++i)
    {
        const bool keep_going = run_frame();
        ++frames_run;
        if (!keep_going)
        {
            break;
        }
    }
    return frames_run;
}

void Application::dispatch_event(const Event& event)
{
    // Modal tools get first refusal on every event.
    if (modal_tools_.dispatch(context_, event))
    {
        return;
    }

    input_.apply(event);

    if (event.type == EventType::KeyDown)
    {
        const KeyEventData& data = event.key();
        if (!data.repeat && !is_modifier_key(data.key))
        {
            dispatch_shortcut(Shortcut(data.key, data.modifiers));
        }
    }
    else if (event.type == EventType::Quit)
    {
        request_quit();
    }

    events_.publish(event);
}

bool Application::dispatch_shortcut(const Shortcut& shortcut)
{
    auto action = keymap_.resolve(shortcut);
    if (!action)
    {
        return false;
    }
    const OperatorResult result = run_operator(*action);
    FORGE_LOG_DEBUG(kLogCategory,
                    std::format("shortcut {} -> {} ({})", shortcut.to_string(), *action,
                                operator_result_name(result)));
    return true;
}

OperatorResult Application::run_operator(std::string_view id, std::string argument)
{
    OperatorContext ctx;
    ctx.app = &context_;
    ctx.argument = std::move(argument);
    return operators_.execute(id, ctx);
}

void Application::request_quit() noexcept
{
    runtime_.request_quit();
    windows_.request_quit();
}

bool Application::quit_requested() const noexcept
{
    return runtime_.quit_requested();
}

} // namespace forge
