#include "test_framework.hpp"

#include "forge/app/event.hpp"
#include "forge/app/input.hpp"

FORGE_TEST_CASE("app.input_key_press_release")
{
    forge::InputState input;
    FORGE_CHECK(!input.is_key_down(forge::Key::A));

    input.press_key(forge::Key::A);
    FORGE_CHECK(input.is_key_down(forge::Key::A));
    FORGE_CHECK(input.was_key_pressed(forge::Key::A));

    input.release_key(forge::Key::A);
    FORGE_CHECK(!input.is_key_down(forge::Key::A));
    FORGE_CHECK(input.was_key_released(forge::Key::A));
}

FORGE_TEST_CASE("app.input_new_frame_clears_edges")
{
    forge::InputState input;
    input.press_key(forge::Key::B);
    FORGE_CHECK(input.was_key_pressed(forge::Key::B));

    input.new_frame();
    // Held state persists; edge flags reset.
    FORGE_CHECK(input.is_key_down(forge::Key::B));
    FORGE_CHECK(!input.was_key_pressed(forge::Key::B));
}

FORGE_TEST_CASE("app.input_tracks_modifiers")
{
    forge::InputState input;
    input.press_key(forge::Key::LeftCtrl);
    FORGE_CHECK(forge::has_modifier(input.modifiers(), forge::ModifierKey::Ctrl));
    input.press_key(forge::Key::LeftShift);
    FORGE_CHECK(forge::has_modifier(input.modifiers(), forge::ModifierKey::Shift));
    input.release_key(forge::Key::LeftCtrl);
    FORGE_CHECK(!forge::has_modifier(input.modifiers(), forge::ModifierKey::Ctrl));
    FORGE_CHECK(forge::has_modifier(input.modifiers(), forge::ModifierKey::Shift));
}

FORGE_TEST_CASE("app.input_mouse_position_and_delta")
{
    forge::InputState input;
    input.set_mouse_position(10.0f, 20.0f);
    input.set_mouse_position(13.0f, 24.0f);
    FORGE_CHECK_NEAR(input.mouse_x(), 13.0f, 1e-4f);
    FORGE_CHECK_NEAR(input.mouse_y(), 24.0f, 1e-4f);
    FORGE_CHECK_NEAR(input.mouse_delta_x(), 3.0f, 1e-4f);
    FORGE_CHECK_NEAR(input.mouse_delta_y(), 4.0f, 1e-4f);

    input.new_frame();
    FORGE_CHECK_NEAR(input.mouse_delta_x(), 0.0f, 1e-4f);
}

FORGE_TEST_CASE("app.input_apply_event")
{
    forge::InputState input;
    input.apply(forge::Event::key_down(forge::Key::G));
    FORGE_CHECK(input.is_key_down(forge::Key::G));
    input.apply(forge::Event::mouse_button_down(forge::MouseButton::Left));
    FORGE_CHECK(input.is_mouse_button_down(forge::MouseButton::Left));
    input.apply(forge::Event::mouse_wheel(0.0f, 2.0f));
    FORGE_CHECK_NEAR(input.scroll_delta_y(), 2.0f, 1e-4f);
    input.apply(forge::Event::text_input("hi"));
    FORGE_CHECK_EQ(input.text_buffer(), std::string("hi"));

    input.apply(forge::Event::key_up(forge::Key::G));
    FORGE_CHECK(!input.is_key_down(forge::Key::G));
}

FORGE_TEST_CASE("app.input_reset")
{
    forge::InputState input;
    input.press_key(forge::Key::A);
    input.set_mouse_position(5.0f, 5.0f);
    input.reset();
    FORGE_CHECK(!input.is_key_down(forge::Key::A));
    FORGE_CHECK_NEAR(input.mouse_x(), 0.0f, 1e-4f);
    FORGE_CHECK_EQ(input.pressed_key_count(), static_cast<std::size_t>(0));
}
