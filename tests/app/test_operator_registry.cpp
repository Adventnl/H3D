#include "test_framework.hpp"

#include <memory>

#include "forge/app/app_context.hpp"
#include "forge/app/command.hpp"
#include "forge/app/command_stack.hpp"
#include "forge/app/operator_registry.hpp"

namespace
{

std::unique_ptr<forge::CallbackOperator> make_op(
    std::string id, forge::CallbackOperator::ExecuteFn exec,
    forge::CallbackOperator::AvailableFn avail = {})
{
    forge::OperatorInfo info;
    info.id = std::move(id);
    info.display_name = "Test Op";
    info.category = "Test";
    return std::make_unique<forge::CallbackOperator>(std::move(info), std::move(exec),
                                                     std::move(avail));
}

class IncCommand final : public forge::Command
{
public:
    explicit IncCommand(int* target) : forge::Command("Inc"), target_(target) {}
    void execute() override { ++*target_; }
    void undo() override { --*target_; }

private:
    int* target_;
};

} // namespace

FORGE_TEST_CASE("app.operator_registry_register_find_execute")
{
    forge::OperatorRegistry registry;
    int calls = 0;
    auto result = registry.register_operator(make_op("test.run", [&calls](forge::OperatorContext&) {
        ++calls;
        return forge::OperatorResult::Finished;
    }));
    FORGE_CHECK(result.has_value());
    FORGE_CHECK(registry.contains("test.run"));
    FORGE_REQUIRE(registry.find("test.run") != nullptr);

    forge::OperatorContext ctx;
    FORGE_CHECK(registry.execute("test.run", ctx) == forge::OperatorResult::Finished);
    FORGE_CHECK_EQ(calls, 1);
}

FORGE_TEST_CASE("app.operator_registry_duplicate_rejected")
{
    forge::OperatorRegistry registry;
    FORGE_CHECK(registry
                    .register_operator(make_op("dup", [](forge::OperatorContext&) {
                        return forge::OperatorResult::Finished;
                    }))
                    .has_value());
    const auto second = registry.register_operator(make_op("dup", [](forge::OperatorContext&) {
        return forge::OperatorResult::Finished;
    }));
    FORGE_CHECK(!second.has_value());
    FORGE_CHECK(second.error().code == forge::ErrorCode::AlreadyExists);
}

FORGE_TEST_CASE("app.operator_registry_unknown_and_unavailable")
{
    forge::OperatorRegistry registry;
    forge::OperatorContext ctx;
    FORGE_CHECK(registry.execute("missing", ctx) == forge::OperatorResult::Failed);

    registry.register_operator(make_op(
        "blocked",
        [](forge::OperatorContext&) { return forge::OperatorResult::Finished; },
        [](const forge::OperatorContext&) { return false; }));
    FORGE_CHECK(registry.execute("blocked", ctx) == forge::OperatorResult::Failed);
}

FORGE_TEST_CASE("app.operator_registry_unregister")
{
    forge::OperatorRegistry registry;
    registry.register_operator(make_op("a", [](forge::OperatorContext&) {
        return forge::OperatorResult::Finished;
    }));
    registry.register_operator(make_op("b", [](forge::OperatorContext&) {
        return forge::OperatorResult::Finished;
    }));
    FORGE_CHECK(registry.unregister("a"));
    FORGE_CHECK(!registry.contains("a"));
    FORGE_CHECK(registry.contains("b"));
    FORGE_CHECK(!registry.unregister("a"));
}

FORGE_TEST_CASE("app.operator_search")
{
    forge::OperatorRegistry registry;
    forge::register_default_operators(registry);

    const auto by_id = registry.search("transform.move");
    FORGE_REQUIRE(!by_id.empty());
    FORGE_CHECK_EQ(by_id.front().op->id(), std::string("transform.move"));

    // Case-insensitive by display name.
    const auto by_name = registry.search("UNDO");
    bool found_undo = false;
    for (const auto& hit : by_name)
    {
        if (hit.op->id() == "edit.undo")
        {
            found_undo = true;
        }
    }
    FORGE_CHECK(found_undo);

    FORGE_CHECK(registry.search("definitely-not-an-operator").empty());

    const auto limited = registry.search("e", 3); // common letter, cap to 3
    FORGE_CHECK(limited.size() <= static_cast<std::size_t>(3));

    // Empty query returns every operator.
    FORGE_CHECK_EQ(registry.search("").size(), registry.size());
}

FORGE_TEST_CASE("app.operator_undo_redo_uses_stack")
{
    forge::OperatorRegistry registry;
    forge::register_default_operators(registry);

    forge::CommandStack stack;
    int value = 0;
    stack.execute(std::make_unique<IncCommand>(&value));
    FORGE_CHECK_EQ(value, 1);

    forge::AppContext app;
    app.commands = &stack;
    forge::OperatorContext ctx;
    ctx.app = &app;

    FORGE_CHECK(registry.execute("edit.undo", ctx) == forge::OperatorResult::Finished);
    FORGE_CHECK_EQ(value, 0);
    FORGE_CHECK(registry.execute("edit.redo", ctx) == forge::OperatorResult::Finished);
    FORGE_CHECK_EQ(value, 1);

    // Nothing left to redo -> Cancelled.
    FORGE_CHECK(registry.execute("edit.redo", ctx) == forge::OperatorResult::Cancelled);
}

FORGE_TEST_CASE("app.operator_categories")
{
    forge::OperatorRegistry registry;
    forge::register_default_operators(registry);
    const auto categories = registry.categories();
    FORGE_CHECK(!categories.empty());
}
