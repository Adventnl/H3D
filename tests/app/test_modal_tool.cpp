#include "test_framework.hpp"

#include <memory>

#include "forge/app/app_context.hpp"
#include "forge/app/event.hpp"
#include "forge/app/modal_tool.hpp"

namespace
{

// A modal tool that counts events and finishes/cancels on specific keys.
class CountingTool final : public forge::ModalTool
{
public:
    CountingTool(int* events, int* starts, int* finishes, int* cancels)
        : forge::ModalTool("test.counting", "Counting Tool"),
          events_(events),
          starts_(starts),
          finishes_(finishes),
          cancels_(cancels)
    {
    }

    void on_start(forge::AppContext&) override { ++*starts_; }
    void on_finish(forge::AppContext&) override { ++*finishes_; }
    void on_cancel(forge::AppContext&) override { ++*cancels_; }

    forge::ModalToolResult handle_event(forge::AppContext&, const forge::Event& event) override
    {
        ++*events_;
        if (event.type == forge::EventType::KeyDown)
        {
            const forge::Key key = event.key().key;
            if (key == forge::Key::Enter)
            {
                return forge::ModalToolResult::Finished;
            }
            if (key == forge::Key::Escape)
            {
                return forge::ModalToolResult::Cancelled;
            }
        }
        return forge::ModalToolResult::Continue;
    }

private:
    int* events_;
    int* starts_;
    int* finishes_;
    int* cancels_;
};

} // namespace

FORGE_TEST_CASE("app.modal_tool_no_active_is_safe")
{
    forge::ModalToolManager manager;
    forge::AppContext ctx;
    FORGE_CHECK(!manager.has_active());
    FORGE_CHECK(!manager.dispatch(ctx, forge::Event::key_down(forge::Key::A)));
    manager.finish(ctx); // no-op
    manager.cancel(ctx); // no-op
}

FORGE_TEST_CASE("app.modal_tool_start_event_finish")
{
    forge::ModalToolManager manager;
    forge::AppContext ctx;
    int events = 0, starts = 0, finishes = 0, cancels = 0;

    manager.start(std::make_unique<CountingTool>(&events, &starts, &finishes, &cancels), ctx);
    FORGE_CHECK(manager.has_active());
    FORGE_CHECK_EQ(starts, 1);

    FORGE_CHECK(manager.dispatch(ctx, forge::Event::key_down(forge::Key::A)));
    FORGE_CHECK_EQ(events, 1);
    FORGE_CHECK(manager.has_active());

    // Enter -> Finished deactivates the tool.
    FORGE_CHECK(manager.dispatch(ctx, forge::Event::key_down(forge::Key::Enter)));
    FORGE_CHECK_EQ(finishes, 1);
    FORGE_CHECK(!manager.has_active());
}

FORGE_TEST_CASE("app.modal_tool_cancel_via_event")
{
    forge::ModalToolManager manager;
    forge::AppContext ctx;
    int events = 0, starts = 0, finishes = 0, cancels = 0;
    manager.start(std::make_unique<CountingTool>(&events, &starts, &finishes, &cancels), ctx);
    FORGE_CHECK(manager.dispatch(ctx, forge::Event::key_down(forge::Key::Escape)));
    FORGE_CHECK_EQ(cancels, 1);
    FORGE_CHECK(!manager.has_active());
}

FORGE_TEST_CASE("app.modal_tool_manager_cancel_clears_active")
{
    forge::ModalToolManager manager;
    forge::AppContext ctx;
    int events = 0, starts = 0, finishes = 0, cancels = 0;
    manager.start(std::make_unique<CountingTool>(&events, &starts, &finishes, &cancels), ctx);
    manager.cancel(ctx);
    FORGE_CHECK_EQ(cancels, 1);
    FORGE_CHECK(!manager.has_active());
}
