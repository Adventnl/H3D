#include "test_framework.hpp"

#include "forge/app/event.hpp"
#include "forge/app/window.hpp"

FORGE_TEST_CASE("app.window_system_create_and_destroy")
{
    forge::WindowSystem system; // defaults to NullWindowBackend
    FORGE_CHECK_EQ(system.backend().name(), std::string_view("null"));
    FORGE_CHECK(!system.has_windows());

    forge::WindowConfig config;
    config.title = "Main";
    config.headless = true;
    forge::Window* window = system.create_window(config);
    FORGE_REQUIRE(window != nullptr);
    FORGE_CHECK(system.has_windows());
    FORGE_CHECK_EQ(system.window_count(), static_cast<std::size_t>(1));
    FORGE_CHECK(system.primary_window() == window);

    const forge::WindowId id = window->id();
    FORGE_CHECK(system.destroy_window(id));
    FORGE_CHECK(!system.has_windows());
    FORGE_CHECK(!system.destroy_window(id)); // already gone
}

FORGE_TEST_CASE("app.window_event_queue")
{
    forge::WindowSystem system;
    forge::Window* window = system.create_window(forge::WindowConfig{});
    FORGE_REQUIRE(window != nullptr);

    window->queue_event(forge::Event::window_resize(window->id(), 1024, 768));
    FORGE_CHECK_EQ(window->pending_event_count(), static_cast<std::size_t>(1));

    forge::Event out;
    FORGE_REQUIRE(window->poll_event(out));
    FORGE_CHECK(out.type == forge::EventType::WindowResize);
    FORGE_CHECK_EQ(out.window_resize_data().width, 1024);
    FORGE_CHECK(!window->poll_event(out)); // empty now
}

FORGE_TEST_CASE("app.window_resize_updates_size")
{
    forge::Window window(1, forge::WindowConfig{});
    window.set_size(640, 480);
    FORGE_CHECK_EQ(window.width(), 640);
    FORGE_CHECK_EQ(window.height(), 480);
}

FORGE_TEST_CASE("app.window_close_request_drops_window_on_poll")
{
    forge::WindowSystem system;
    forge::Window* window = system.create_window(forge::WindowConfig{});
    FORGE_REQUIRE(window != nullptr);
    window->request_close();
    FORGE_CHECK(window->should_close());
    system.poll_events(); // drops closed windows
    FORGE_CHECK(!system.has_windows());
}

FORGE_TEST_CASE("app.window_system_poll_is_safe_when_empty")
{
    forge::WindowSystem system;
    FORGE_CHECK_EQ(system.poll_events(), static_cast<std::size_t>(0));
    FORGE_CHECK(system.primary_window() == nullptr);
}
