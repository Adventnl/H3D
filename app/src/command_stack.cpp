#include "forge/app/command_stack.hpp"

#include <utility>

namespace forge
{

void CommandStack::execute(std::unique_ptr<Command> command)
{
    if (!command)
    {
        return;
    }
    command->execute();

    if (transaction_)
    {
        transaction_->add(std::move(command));
        return;
    }

    redo_.clear();
    push_undo(std::move(command));
}

void CommandStack::push_undo(std::unique_ptr<Command> command)
{
    // Try to merge into the previous command to keep history compact.
    if (!undo_.empty() && undo_.back()->can_merge(*command))
    {
        if (undo_.back()->merge(*command))
        {
            return;
        }
    }
    undo_.push_back(std::move(command));
    trim_history();
}

void CommandStack::trim_history()
{
    if (max_history_ == 0)
    {
        return;
    }
    while (undo_.size() > max_history_)
    {
        undo_.erase(undo_.begin());
    }
}

bool CommandStack::undo()
{
    if (transaction_ || undo_.empty())
    {
        return false;
    }
    std::unique_ptr<Command> command = std::move(undo_.back());
    undo_.pop_back();
    command->undo();
    redo_.push_back(std::move(command));
    return true;
}

bool CommandStack::redo()
{
    if (transaction_ || redo_.empty())
    {
        return false;
    }
    std::unique_ptr<Command> command = std::move(redo_.back());
    redo_.pop_back();
    command->redo();
    undo_.push_back(std::move(command));
    return true;
}

bool CommandStack::can_undo() const noexcept
{
    return !transaction_ && !undo_.empty();
}

bool CommandStack::can_redo() const noexcept
{
    return !transaction_ && !redo_.empty();
}

std::string CommandStack::undo_name() const
{
    return undo_.empty() ? std::string{} : undo_.back()->name();
}

std::string CommandStack::redo_name() const
{
    return redo_.empty() ? std::string{} : redo_.back()->name();
}

void CommandStack::clear()
{
    undo_.clear();
    redo_.clear();
    transaction_.reset();
}

void CommandStack::set_max_history(std::size_t max_entries)
{
    max_history_ = max_entries;
    trim_history();
}

void CommandStack::begin_transaction(std::string name)
{
    if (transaction_)
    {
        // Nested begin: keep the outermost transaction.
        return;
    }
    transaction_ = std::make_unique<CompositeCommand>(std::move(name));
}

void CommandStack::commit_transaction()
{
    if (!transaction_)
    {
        return;
    }
    std::unique_ptr<CompositeCommand> committed = std::move(transaction_);
    transaction_.reset();
    if (committed->empty())
    {
        return; // nothing collected; no undo step
    }
    redo_.clear();
    push_undo(std::move(committed));
}

void CommandStack::cancel_transaction()
{
    if (!transaction_)
    {
        return;
    }
    // Undo whatever was applied during the transaction, in reverse order.
    transaction_->undo();
    transaction_.reset();
}

} // namespace forge
