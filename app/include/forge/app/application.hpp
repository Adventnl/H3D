#pragma once

// Application: the top-level object that owns the core services and runs the
// frame loop. It wires everything an AppContext exposes, registers the default
// operators and keymap, and turns input into operator invocations:
//
//   Input -> Event -> Keymap -> Operator -> Command -> Undo/redo
//
// The ui layer attaches a WorkspaceService so workspace/screen operators work.

#include <memory>

#include "forge/app/app_context.hpp"
#include "forge/app/application_config.hpp"
#include "forge/app/command_stack.hpp"
#include "forge/app/event.hpp"
#include "forge/app/event_bus.hpp"
#include "forge/app/input.hpp"
#include "forge/app/keymap.hpp"
#include "forge/app/modal_tool.hpp"
#include "forge/app/operator_registry.hpp"
#include "forge/app/preferences.hpp"
#include "forge/app/runtime.hpp"
#include "forge/app/window.hpp"
#include "forge/foundation/result.hpp"

namespace forge
{

class Application
{
public:
    explicit Application(ApplicationConfig config = {});
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    /// Initialize logging, the runtime and the default content (operators,
    /// keymap). Safe to call once. Returns an error if a subsystem fails.
    [[nodiscard]] Result<void> initialize();

    /// Tear everything down in reverse order. Safe to call more than once.
    void shutdown();

    [[nodiscard]] bool is_initialized() const noexcept { return initialized_; }

    /// Run up to `frame_count` frames (or config.frame_limit when negative),
    /// stopping early on a quit request. Returns the number of frames run.
    int run(int frame_count);

    /// Run a single frame: dispatch window/queued events, begin/end frame,
    /// publish FrameBegin/FrameEnd. Returns false once a quit was requested.
    bool run_frame();

    /// Feed an event through the pipeline: modal tools first, then input
    /// state, then (for key presses) the keymap -> operator path, then the bus.
    void dispatch_event(const Event& event);

    /// Resolve a shortcut against the keymap and execute the bound operator.
    /// Returns true when a binding was found and executed.
    bool dispatch_shortcut(const Shortcut& shortcut);

    /// Execute an operator by id with the application context.
    [[nodiscard]] OperatorResult run_operator(std::string_view id, std::string argument = {});

    void request_quit() noexcept;
    [[nodiscard]] bool quit_requested() const noexcept;

    /// Attach the ui workspace service (called by the ui layer / desktop app).
    void set_workspace_service(WorkspaceService* service);

    // --- Service accessors ------------------------------------------------
    [[nodiscard]] const ApplicationConfig& config() const noexcept { return config_; }
    [[nodiscard]] AppContext& context() noexcept { return context_; }
    [[nodiscard]] Runtime& runtime() noexcept { return runtime_; }
    [[nodiscard]] EventBus& events() noexcept { return events_; }
    [[nodiscard]] WindowSystem& windows() noexcept { return windows_; }
    [[nodiscard]] OperatorRegistry& operators() noexcept { return operators_; }
    [[nodiscard]] CommandStack& commands() noexcept { return commands_; }
    [[nodiscard]] CommandStack& undo_stack() noexcept { return commands_; }
    [[nodiscard]] Keymap& keymap() noexcept { return keymap_; }
    [[nodiscard]] Preferences& preferences() noexcept { return preferences_; }
    [[nodiscard]] InputState& input() noexcept { return input_; }
    [[nodiscard]] ModalToolManager& modal_tools() noexcept { return modal_tools_; }

    [[nodiscard]] std::uint64_t frame_index() const noexcept { return runtime_.frame_index(); }

private:
    void build_context();

    ApplicationConfig config_;
    bool initialized_ = false;

    Runtime runtime_;
    EventBus events_;
    WindowSystem windows_;
    OperatorRegistry operators_;
    CommandStack commands_;
    Keymap keymap_;
    Preferences preferences_;
    InputState input_;
    ModalToolManager modal_tools_;
    AppContext context_;
};

} // namespace forge
