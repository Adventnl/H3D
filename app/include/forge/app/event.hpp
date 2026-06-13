#pragma once

// The Event type: a tagged value describing something that happened (input,
// window, command/operator lifecycle, frame boundaries).
//
// An Event carries a strongly-typed payload in a std::variant. Construct events
// with the named factory functions; read payloads with the typed accessors
// (which assert the variant holds the matching alternative in debug builds).

#include <cstdint>
#include <string>
#include <variant>

#include "forge/app/input.hpp"

namespace forge
{

enum class EventType : std::uint16_t
{
    None = 0,
    Quit,
    WindowClose,
    WindowResize,
    WindowFocus,
    KeyDown,
    KeyUp,
    MouseMove,
    MouseButtonDown,
    MouseButtonUp,
    MouseWheel,
    TextInput,
    CommandRequested,
    OperatorInvoked,
    FrameBegin,
    FrameEnd,
};

/// Stable name of an event type, e.g. "KeyDown".
[[nodiscard]] std::string_view event_type_name(EventType type) noexcept;

struct KeyEventData
{
    Key key = Key::Unknown;
    ModifierKey modifiers = ModifierKey::None;
    bool repeat = false;
};

struct MouseMoveData
{
    float x = 0.0f;
    float y = 0.0f;
    float dx = 0.0f;
    float dy = 0.0f;
};

struct MouseButtonData
{
    MouseButton button = MouseButton::Left;
    ModifierKey modifiers = ModifierKey::None;
    float x = 0.0f;
    float y = 0.0f;
};

struct MouseWheelData
{
    float dx = 0.0f;
    float dy = 0.0f;
};

struct WindowResizeData
{
    std::uint32_t window_id = 0;
    int width = 0;
    int height = 0;
};

struct WindowFocusData
{
    std::uint32_t window_id = 0;
    bool focused = true;
};

struct WindowData
{
    std::uint32_t window_id = 0;
};

struct TextInputData
{
    std::string text;
};

struct StringData
{
    std::string value; // command id / operator id
};

using EventPayload = std::variant<std::monostate, KeyEventData, MouseMoveData,
                                  MouseButtonData, MouseWheelData, WindowResizeData,
                                  WindowFocusData, WindowData, TextInputData, StringData>;

struct Event
{
    EventType type = EventType::None;
    EventPayload payload{};
    bool handled = false;

    Event() = default;
    explicit Event(EventType event_type) : type(event_type) {}
    Event(EventType event_type, EventPayload event_payload)
        : type(event_type), payload(std::move(event_payload))
    {
    }

    // --- Named constructors ----------------------------------------------
    [[nodiscard]] static Event quit();
    [[nodiscard]] static Event frame_begin();
    [[nodiscard]] static Event frame_end();
    [[nodiscard]] static Event window_close(std::uint32_t window_id);
    [[nodiscard]] static Event window_resize(std::uint32_t window_id, int width, int height);
    [[nodiscard]] static Event window_focus(std::uint32_t window_id, bool focused);
    [[nodiscard]] static Event key_down(Key key, ModifierKey modifiers = ModifierKey::None,
                                        bool repeat = false);
    [[nodiscard]] static Event key_up(Key key, ModifierKey modifiers = ModifierKey::None);
    [[nodiscard]] static Event mouse_move(float x, float y, float dx = 0.0f, float dy = 0.0f);
    [[nodiscard]] static Event mouse_button_down(MouseButton button, float x = 0.0f,
                                                 float y = 0.0f,
                                                 ModifierKey modifiers = ModifierKey::None);
    [[nodiscard]] static Event mouse_button_up(MouseButton button, float x = 0.0f,
                                               float y = 0.0f,
                                               ModifierKey modifiers = ModifierKey::None);
    [[nodiscard]] static Event mouse_wheel(float dx, float dy);
    [[nodiscard]] static Event text_input(std::string text);
    [[nodiscard]] static Event command_requested(std::string command_id);
    [[nodiscard]] static Event operator_invoked(std::string operator_id);

    // --- Typed payload access (assert on mismatch) ------------------------
    [[nodiscard]] const KeyEventData& key() const;
    [[nodiscard]] const MouseMoveData& mouse_move_data() const;
    [[nodiscard]] const MouseButtonData& mouse_button() const;
    [[nodiscard]] const MouseWheelData& mouse_wheel_data() const;
    [[nodiscard]] const WindowResizeData& window_resize_data() const;
    [[nodiscard]] const WindowFocusData& window_focus_data() const;
    [[nodiscard]] const TextInputData& text() const;
    [[nodiscard]] const StringData& string_data() const;
};

} // namespace forge
