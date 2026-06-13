#include "test_framework.hpp"

#include "forge/app/application.hpp"
#include "forge/app/application_config.hpp"
#include "forge/app/runtime.hpp"

FORGE_TEST_CASE("app.application_config_defaults")
{
    forge::ApplicationConfig config;
    config.apply_defaults();
    FORGE_CHECK_EQ(config.application_name, std::string("Forge3D"));
    FORGE_CHECK(!config.version.empty());
    FORGE_CHECK(config.headless);
}

FORGE_TEST_CASE("app.runtime_initializes_and_shuts_down")
{
    forge::Runtime runtime;
    FORGE_CHECK(!runtime.is_initialized());
    FORGE_REQUIRE(runtime.initialize().has_value());
    FORGE_CHECK(runtime.is_initialized());

    runtime.begin_frame();
    runtime.end_frame();
    runtime.begin_frame();
    FORGE_CHECK_EQ(runtime.frame_index(), static_cast<std::uint64_t>(1));

    runtime.request_quit();
    FORGE_CHECK(runtime.quit_requested());

    runtime.shutdown();
    FORGE_CHECK(!runtime.is_initialized());
    runtime.shutdown(); // idempotent
}

FORGE_TEST_CASE("app.application_initializes_with_defaults")
{
    forge::ApplicationConfig config;
    config.headless = true;
    forge::Application app(config);
    FORGE_REQUIRE(app.initialize().has_value());

    FORGE_CHECK(app.is_initialized());
    FORGE_CHECK(app.operators().size() >= static_cast<std::size_t>(16));
    FORGE_CHECK_EQ(app.keymap().size(), static_cast<std::size_t>(12));
    FORGE_CHECK(app.windows().has_windows());

    app.shutdown();
}

FORGE_TEST_CASE("app.application_runs_frames")
{
    forge::Application app;
    FORGE_REQUIRE(app.initialize().has_value());
    const int ran = app.run(3);
    FORGE_CHECK_EQ(ran, 3);
    app.shutdown();
}

FORGE_TEST_CASE("app.application_quit_stops_loop")
{
    forge::Application app;
    FORGE_REQUIRE(app.initialize().has_value());
    app.request_quit();
    FORGE_CHECK(app.quit_requested());
    const int ran = app.run(10);
    FORGE_CHECK(ran <= 1); // first frame observes the quit request
    app.shutdown();
}

FORGE_TEST_CASE("app.application_shortcut_dispatch_runs_operator")
{
    forge::Application app;
    FORGE_REQUIRE(app.initialize().has_value());

    // Ctrl+S resolves to file.save (a placeholder that returns Finished).
    const bool handled =
        app.dispatch_shortcut(forge::Shortcut(forge::Key::S, forge::ModifierKey::Ctrl));
    FORGE_CHECK(handled);

    // An unbound shortcut is not handled.
    FORGE_CHECK(!app.dispatch_shortcut(forge::Shortcut(forge::Key::F12)));
    app.shutdown();
}

FORGE_TEST_CASE("app.application_key_event_triggers_app_quit_operator")
{
    forge::Application app;
    FORGE_REQUIRE(app.initialize().has_value());
    // Bind a key to app.quit and feed a key-down event through the pipeline.
    app.keymap().bind(forge::Shortcut(forge::Key::Q), "app.quit");
    app.dispatch_event(forge::Event::key_down(forge::Key::Q));
    FORGE_CHECK(app.quit_requested());
    app.shutdown();
}
