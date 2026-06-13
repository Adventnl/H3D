#include "test_framework.hpp"

#include <memory>

#include "forge/app/command.hpp"
#include "forge/app/undo_stack.hpp"

namespace
{

// A mergeable command: successive instances of the same kind fold together so
// the history stays compact (like a continuous drag).
class AccumulateCommand final : public forge::Command
{
public:
    AccumulateCommand(int* target, int delta)
        : forge::Command("Accumulate"), target_(target), delta_(delta)
    {
    }

    void execute() override { *target_ += delta_; }
    void undo() override { *target_ -= total_; }

    [[nodiscard]] bool can_merge(const forge::Command& other) const override
    {
        return dynamic_cast<const AccumulateCommand*>(&other) != nullptr;
    }

    bool merge(const forge::Command& other) override
    {
        const auto* typed = dynamic_cast<const AccumulateCommand*>(&other);
        if (typed == nullptr)
        {
            return false;
        }
        total_ += typed->delta_;
        return true;
    }

private:
    int* target_;
    int delta_;
    int total_ = delta_;
};

} // namespace

FORGE_TEST_CASE("app.undo_stack_callback_command")
{
    int value = 0;
    forge::UndoStack stack;
    stack.execute(std::make_unique<forge::CallbackCommand>(
        "Set", [&value] { value = 9; }, [&value] { value = 0; }));
    FORGE_CHECK_EQ(value, 9);
    FORGE_CHECK_EQ(stack.undo_name(), std::string("Set"));
    FORGE_CHECK(stack.undo());
    FORGE_CHECK_EQ(value, 0);
    FORGE_CHECK_EQ(stack.redo_name(), std::string("Set"));
}

FORGE_TEST_CASE("app.undo_stack_merges_compatible_commands")
{
    int value = 0;
    forge::UndoStack stack;
    stack.execute(std::make_unique<AccumulateCommand>(&value, 1));
    stack.execute(std::make_unique<AccumulateCommand>(&value, 1));
    stack.execute(std::make_unique<AccumulateCommand>(&value, 1));

    // All three merged into a single history entry.
    FORGE_CHECK_EQ(value, 3);
    FORGE_CHECK_EQ(stack.undo_count(), static_cast<std::size_t>(1));

    FORGE_CHECK(stack.undo());
    FORGE_CHECK_EQ(value, 0);
}

FORGE_TEST_CASE("app.undo_stack_clear")
{
    int value = 0;
    forge::UndoStack stack;
    stack.execute(std::make_unique<forge::CallbackCommand>(
        "Inc", [&value] { ++value; }, [&value] { --value; }));
    stack.clear();
    FORGE_CHECK(!stack.can_undo());
    FORGE_CHECK(!stack.can_redo());
    FORGE_CHECK_EQ(stack.undo_count(), static_cast<std::size_t>(0));
}
