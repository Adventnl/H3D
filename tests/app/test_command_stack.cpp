#include "test_framework.hpp"

#include <memory>
#include <vector>

#include "forge/app/command_stack.hpp"
#include "forge/app/undo_stack.hpp"

namespace
{

// A command that adds `delta` to a shared accumulator on execute and removes it
// on undo, recording the order of operations for composite-order checks.
class AddCommand final : public forge::Command
{
public:
    AddCommand(int* target, int delta, std::vector<int>* trace = nullptr)
        : forge::Command("Add"), target_(target), delta_(delta), trace_(trace)
    {
    }

    void execute() override
    {
        *target_ += delta_;
        if (trace_ != nullptr)
        {
            trace_->push_back(delta_);
        }
    }

    void undo() override
    {
        *target_ -= delta_;
        if (trace_ != nullptr)
        {
            trace_->push_back(-delta_);
        }
    }

private:
    int* target_;
    int delta_;
    std::vector<int>* trace_;
};

} // namespace

FORGE_TEST_CASE("app.command_stack_execute_undo_redo")
{
    int value = 0;
    forge::CommandStack stack;

    stack.execute(std::make_unique<AddCommand>(&value, 5));
    FORGE_CHECK_EQ(value, 5);
    FORGE_CHECK(stack.can_undo());
    FORGE_CHECK(!stack.can_redo());
    FORGE_CHECK_EQ(stack.undo_count(), static_cast<std::size_t>(1));

    FORGE_CHECK(stack.undo());
    FORGE_CHECK_EQ(value, 0);
    FORGE_CHECK(!stack.can_undo());
    FORGE_CHECK(stack.can_redo());

    FORGE_CHECK(stack.redo());
    FORGE_CHECK_EQ(value, 5);
    FORGE_CHECK(stack.can_undo());
}

FORGE_TEST_CASE("app.command_stack_redo_cleared_after_new_command")
{
    int value = 0;
    forge::CommandStack stack;
    stack.execute(std::make_unique<AddCommand>(&value, 1));
    stack.execute(std::make_unique<AddCommand>(&value, 2));
    FORGE_CHECK(stack.undo());           // value == 1, one redoable
    FORGE_CHECK_EQ(stack.redo_count(), static_cast<std::size_t>(1));

    stack.execute(std::make_unique<AddCommand>(&value, 10)); // clears redo
    FORGE_CHECK_EQ(stack.redo_count(), static_cast<std::size_t>(0));
    FORGE_CHECK(!stack.can_redo());
    FORGE_CHECK_EQ(value, 11);
}

FORGE_TEST_CASE("app.command_stack_empty_is_safe")
{
    forge::CommandStack stack;
    FORGE_CHECK(!stack.undo());
    FORGE_CHECK(!stack.redo());
    FORGE_CHECK(!stack.can_undo());
    FORGE_CHECK(!stack.can_redo());
}

FORGE_TEST_CASE("app.composite_command_executes_all_and_undoes_in_reverse")
{
    int value = 0;
    std::vector<int> trace;

    auto composite = std::make_unique<forge::CompositeCommand>("Batch");
    composite->add(std::make_unique<AddCommand>(&value, 1, &trace));
    composite->add(std::make_unique<AddCommand>(&value, 2, &trace));
    composite->add(std::make_unique<AddCommand>(&value, 3, &trace));

    forge::CommandStack stack;
    stack.execute(std::move(composite));
    FORGE_CHECK_EQ(value, 6);
    // executed in order 1,2,3
    FORGE_REQUIRE(trace.size() == static_cast<std::size_t>(3));
    FORGE_CHECK_EQ(trace[0], 1);
    FORGE_CHECK_EQ(trace[1], 2);
    FORGE_CHECK_EQ(trace[2], 3);

    trace.clear();
    FORGE_CHECK(stack.undo());
    FORGE_CHECK_EQ(value, 0);
    // undone in reverse order: -3, -2, -1
    FORGE_REQUIRE(trace.size() == static_cast<std::size_t>(3));
    FORGE_CHECK_EQ(trace[0], -3);
    FORGE_CHECK_EQ(trace[1], -2);
    FORGE_CHECK_EQ(trace[2], -1);
}

FORGE_TEST_CASE("app.command_stack_max_history")
{
    int value = 0;
    forge::CommandStack stack;
    stack.set_max_history(2);
    for (int i = 0; i < 5; ++i)
    {
        stack.execute(std::make_unique<AddCommand>(&value, 1));
    }
    FORGE_CHECK_EQ(stack.undo_count(), static_cast<std::size_t>(2));
    FORGE_CHECK_EQ(value, 5);
}

FORGE_TEST_CASE("app.command_stack_transaction_is_single_undo_step")
{
    int value = 0;
    forge::CommandStack stack;

    stack.begin_transaction("Move");
    FORGE_CHECK(stack.in_transaction());
    stack.execute(std::make_unique<AddCommand>(&value, 4));
    stack.execute(std::make_unique<AddCommand>(&value, 6));
    stack.commit_transaction();

    FORGE_CHECK(!stack.in_transaction());
    FORGE_CHECK_EQ(value, 10);
    FORGE_CHECK_EQ(stack.undo_count(), static_cast<std::size_t>(1));

    FORGE_CHECK(stack.undo());
    FORGE_CHECK_EQ(value, 0);
}

FORGE_TEST_CASE("app.command_stack_cancel_transaction_reverts")
{
    int value = 0;
    forge::CommandStack stack;
    stack.begin_transaction("Drag");
    stack.execute(std::make_unique<AddCommand>(&value, 7));
    stack.execute(std::make_unique<AddCommand>(&value, 3));
    FORGE_CHECK_EQ(value, 10);
    stack.cancel_transaction();
    FORGE_CHECK_EQ(value, 0);
    FORGE_CHECK(!stack.can_undo());
    FORGE_CHECK_EQ(stack.undo_count(), static_cast<std::size_t>(0));
}

FORGE_TEST_CASE("app.undo_stack_alias_is_command_stack")
{
    forge::UndoStack stack; // alias for CommandStack
    int value = 0;
    stack.execute(std::make_unique<AddCommand>(&value, 42));
    FORGE_CHECK_EQ(value, 42);
    FORGE_CHECK(stack.undo());
    FORGE_CHECK_EQ(value, 0);
}
