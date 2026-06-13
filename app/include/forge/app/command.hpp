#pragma once

// Command: a low-level, undoable mutation. Commands are the unit the undo
// stack stores. Operators (the user-facing actions) create and submit commands.
//
// A command must be able to apply (execute / redo) and reverse (undo) its
// effect. redo() defaults to execute(); override it when re-application differs.

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace forge
{

class Command
{
public:
    explicit Command(std::string name, std::string description = {});
    virtual ~Command();

    Command(const Command&) = delete;
    Command& operator=(const Command&) = delete;

    /// Apply the command's effect for the first time.
    virtual void execute() = 0;

    /// Reverse the effect applied by execute()/redo().
    virtual void undo() = 0;

    /// Re-apply after an undo. Defaults to execute().
    virtual void redo() { execute(); }

    /// Whether `other` (a newer command) can be folded into this one to keep
    /// the undo history compact (e.g. successive drags). Default: no.
    [[nodiscard]] virtual bool can_merge(const Command& other) const;

    /// Fold `other` into this command. Only called when can_merge() is true.
    /// Returns true on success. Default: no-op returning false.
    virtual bool merge(const Command& other);

    [[nodiscard]] const std::string& name() const noexcept { return name_; }
    [[nodiscard]] const std::string& description() const noexcept { return description_; }

private:
    std::string name_;
    std::string description_;
};

/// A command composed of several sub-commands executed as a single unit.
/// Executes/redoes children in order; undoes them in reverse order.
class CompositeCommand final : public Command
{
public:
    explicit CompositeCommand(std::string name, std::string description = {});

    /// Add a command. It is NOT executed by add(); the composite is executed
    /// as a whole by the command stack.
    void add(std::unique_ptr<Command> command);

    [[nodiscard]] std::size_t size() const noexcept { return commands_.size(); }
    [[nodiscard]] bool empty() const noexcept { return commands_.empty(); }

    void execute() override;
    void undo() override;
    void redo() override;

private:
    std::vector<std::unique_ptr<Command>> commands_;
};

/// A lightweight command built from callbacks; convenient for simple actions
/// and tests without defining a dedicated subclass.
class CallbackCommand final : public Command
{
public:
    using Action = std::function<void()>;

    CallbackCommand(std::string name, Action execute_fn, Action undo_fn,
                    std::string description = {});

    void execute() override;
    void undo() override;

private:
    Action execute_;
    Action undo_;
};

} // namespace forge
