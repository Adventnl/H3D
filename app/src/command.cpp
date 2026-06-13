#include "forge/app/command.hpp"

#include <utility>

namespace forge
{

Command::Command(std::string name, std::string description)
    : name_(std::move(name)), description_(std::move(description))
{
}

Command::~Command() = default;

bool Command::can_merge(const Command&) const
{
    return false;
}

bool Command::merge(const Command&)
{
    return false;
}

CompositeCommand::CompositeCommand(std::string name, std::string description)
    : Command(std::move(name), std::move(description))
{
}

void CompositeCommand::add(std::unique_ptr<Command> command)
{
    if (command)
    {
        commands_.push_back(std::move(command));
    }
}

void CompositeCommand::execute()
{
    for (const auto& command : commands_)
    {
        command->execute();
    }
}

void CompositeCommand::undo()
{
    // Reverse order so dependencies unwind correctly.
    for (auto it = commands_.rbegin(); it != commands_.rend(); ++it)
    {
        (*it)->undo();
    }
}

void CompositeCommand::redo()
{
    for (const auto& command : commands_)
    {
        command->redo();
    }
}

CallbackCommand::CallbackCommand(std::string name, Action execute_fn, Action undo_fn,
                                 std::string description)
    : Command(std::move(name), std::move(description)),
      execute_(std::move(execute_fn)),
      undo_(std::move(undo_fn))
{
}

void CallbackCommand::execute()
{
    if (execute_)
    {
        execute_();
    }
}

void CallbackCommand::undo()
{
    if (undo_)
    {
        undo_();
    }
}

} // namespace forge
