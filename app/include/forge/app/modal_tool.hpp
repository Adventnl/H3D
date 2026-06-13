#pragma once

// Modal tools: interactive states that capture input until they finish or
// cancel (e.g. a transform drag, a knife cut). At most one modal tool is active
// at a time. While active it receives every event first and decides whether to
// consume it.

#include <memory>
#include <string>

#include "forge/app/event.hpp"

namespace forge
{

class AppContext;

/// What a modal tool wants to happen after handling an event.
enum class ModalToolResult
{
    Continue,     // stay active, event consumed
    PassThrough,  // stay active, event NOT consumed (let others see it)
    Finished,     // commit and deactivate
    Cancelled,    // abort and deactivate
};

class ModalTool
{
public:
    explicit ModalTool(std::string id, std::string display_name);
    virtual ~ModalTool();

    ModalTool(const ModalTool&) = delete;
    ModalTool& operator=(const ModalTool&) = delete;

    [[nodiscard]] const std::string& id() const noexcept { return id_; }
    [[nodiscard]] const std::string& display_name() const noexcept { return display_name_; }

    virtual void on_start(AppContext& context);
    [[nodiscard]] virtual ModalToolResult handle_event(AppContext& context,
                                                       const Event& event) = 0;
    virtual void on_finish(AppContext& context);
    virtual void on_cancel(AppContext& context);

private:
    std::string id_;
    std::string display_name_;
};

class ModalToolManager
{
public:
    /// Start a modal tool, replacing any currently active one (which is
    /// cancelled first). Calls on_start().
    void start(std::unique_ptr<ModalTool> tool, AppContext& context);

    /// Forward an event to the active tool. Returns true when the event was
    /// consumed. Safe when no tool is active (returns false). A Finished or
    /// Cancelled result deactivates the tool.
    bool dispatch(AppContext& context, const Event& event);

    /// Finish the active tool (commit). No-op when none is active.
    void finish(AppContext& context);

    /// Cancel the active tool (abort). No-op when none is active.
    void cancel(AppContext& context);

    [[nodiscard]] bool has_active() const noexcept { return active_ != nullptr; }
    [[nodiscard]] ModalTool* active() noexcept { return active_.get(); }

private:
    std::unique_ptr<ModalTool> active_;
};

} // namespace forge
