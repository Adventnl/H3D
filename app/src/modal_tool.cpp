#include "forge/app/modal_tool.hpp"

#include <utility>

namespace forge
{

ModalTool::ModalTool(std::string id, std::string display_name)
    : id_(std::move(id)), display_name_(std::move(display_name))
{
}

ModalTool::~ModalTool() = default;

void ModalTool::on_start(AppContext&) {}
void ModalTool::on_finish(AppContext&) {}
void ModalTool::on_cancel(AppContext&) {}

void ModalToolManager::start(std::unique_ptr<ModalTool> tool, AppContext& context)
{
    if (!tool)
    {
        return;
    }
    if (active_)
    {
        active_->on_cancel(context);
        active_.reset();
    }
    active_ = std::move(tool);
    active_->on_start(context);
}

bool ModalToolManager::dispatch(AppContext& context, const Event& event)
{
    if (!active_)
    {
        return false;
    }
    // Hold a raw pointer: the tool must stay alive across handle_event even if
    // it deactivates itself.
    ModalTool* tool = active_.get();
    const ModalToolResult result = tool->handle_event(context, event);
    switch (result)
    {
    case ModalToolResult::Continue:
        return true;
    case ModalToolResult::PassThrough:
        return false;
    case ModalToolResult::Finished:
        tool->on_finish(context);
        active_.reset();
        return true;
    case ModalToolResult::Cancelled:
        tool->on_cancel(context);
        active_.reset();
        return true;
    }
    return false;
}

void ModalToolManager::finish(AppContext& context)
{
    if (active_)
    {
        active_->on_finish(context);
        active_.reset();
    }
}

void ModalToolManager::cancel(AppContext& context)
{
    if (active_)
    {
        active_->on_cancel(context);
        active_.reset();
    }
}

} // namespace forge
