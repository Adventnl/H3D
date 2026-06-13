#pragma once

// CommandStack: the undo/redo history. Executing a command applies it and
// pushes it onto the undo stack, discarding any redoable commands. undo()/redo()
// walk the history.
//
// Transactions batch several commands into one CompositeCommand undo step:
//   stack.begin_transaction("Move things");
//   stack.execute(...); stack.execute(...);
//   stack.commit_transaction();   // one undo step
//
// An optional history limit drops the oldest commands once exceeded.

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "forge/app/command.hpp"

namespace forge
{

class CommandStack
{
public:
    CommandStack() = default;

    /// Execute `command` and push it onto the undo stack. Clears the redo
    /// stack. During a transaction the command is collected instead.
    void execute(std::unique_ptr<Command> command);

    /// Undo the most recent command. Returns false when there is nothing to
    /// undo (or a transaction is open).
    bool undo();

    /// Redo the most recently undone command. Returns false when there is
    /// nothing to redo (or a transaction is open).
    bool redo();

    [[nodiscard]] bool can_undo() const noexcept;
    [[nodiscard]] bool can_redo() const noexcept;
    [[nodiscard]] std::size_t undo_count() const noexcept { return undo_.size(); }
    [[nodiscard]] std::size_t redo_count() const noexcept { return redo_.size(); }

    /// Name of the command that undo()/redo() would act on (empty if none).
    [[nodiscard]] std::string undo_name() const;
    [[nodiscard]] std::string redo_name() const;

    /// Drop all history.
    void clear();

    /// 0 means unlimited. Lowering the limit trims the oldest undo entries.
    void set_max_history(std::size_t max_entries);
    [[nodiscard]] std::size_t max_history() const noexcept { return max_history_; }

    // --- Transactions -----------------------------------------------------
    void begin_transaction(std::string name);
    [[nodiscard]] bool in_transaction() const noexcept { return transaction_ != nullptr; }

    /// Commit the open transaction as a single undo step. A transaction with no
    /// commands is discarded (no undo entry created).
    void commit_transaction();

    /// Undo every command collected so far and discard the transaction.
    void cancel_transaction();

private:
    void push_undo(std::unique_ptr<Command> command);
    void trim_history();

    std::vector<std::unique_ptr<Command>> undo_;
    std::vector<std::unique_ptr<Command>> redo_;
    std::unique_ptr<CompositeCommand> transaction_;
    std::size_t max_history_ = 0;
};

} // namespace forge
