#pragma once

// AppContext: the bundle of core services an operator, command or editor needs.
// It owns nothing; it aggregates non-owning pointers to services the
// Application owns. Passing one AppContext around avoids global mutable state.
//
// WorkspaceService is the seam between the app and ui modules: the app defines
// the interface it needs (switch workspace, split the active area, ...) and the
// ui module's WorkspaceRegistry implements it. This keeps `app` free of any
// dependency on `ui`.

#include <string>

namespace forge
{

class Runtime;
class EventBus;
class WindowSystem;
class OperatorRegistry;
class CommandStack;
class Keymap;
class Preferences;
class InputState;
class ModalToolManager;
class Logger;

/// Workspace/screen operations the app layer needs without depending on ui.
/// Implemented by ui::WorkspaceRegistry. Methods return true when they changed
/// state. A null WorkspaceService means "no UI is attached yet".
class WorkspaceService
{
public:
    virtual ~WorkspaceService();

    [[nodiscard]] virtual std::string active_workspace_name() const = 0;
    virtual bool next_workspace() = 0;
    virtual bool previous_workspace() = 0;
    virtual bool activate_workspace(const std::string& name) = 0;

    virtual bool split_active_area_vertical() = 0;
    virtual bool split_active_area_horizontal() = 0;
    virtual bool close_active_area() = 0;
};

/// Aggregates the services operators/commands/editors use. Pointers are owned
/// by the Application and outlive the contexts that reference them.
struct AppContext
{
    Runtime* runtime = nullptr;
    EventBus* events = nullptr;
    WindowSystem* windows = nullptr;
    OperatorRegistry* operators = nullptr;
    CommandStack* commands = nullptr; // the undo/redo history
    Keymap* keymap = nullptr;
    Preferences* preferences = nullptr;
    InputState* input = nullptr;
    ModalToolManager* modal_tools = nullptr;
    Logger* logger = nullptr;

    /// Optional UI seam; null until a ui layer attaches one.
    WorkspaceService* workspaces = nullptr;

    [[nodiscard]] CommandStack* undo_stack() const noexcept { return commands; }
};

} // namespace forge
