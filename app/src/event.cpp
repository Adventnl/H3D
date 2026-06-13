#include "forge/app/event.hpp"

#include "forge/foundation/assert.hpp"

namespace forge
{

std::string_view event_type_name(EventType type) noexcept
{
    switch (type)
    {
    case EventType::None: return "None";
    case EventType::Quit: return "Quit";
    case EventType::WindowClose: return "WindowClose";
    case EventType::WindowResize: return "WindowResize";
    case EventType::WindowFocus: return "WindowFocus";
    case EventType::KeyDown: return "KeyDown";
    case EventType::KeyUp: return "KeyUp";
    case EventType::MouseMove: return "MouseMove";
    case EventType::MouseButtonDown: return "MouseButtonDown";
    case EventType::MouseButtonUp: return "MouseButtonUp";
    case EventType::MouseWheel: return "MouseWheel";
    case EventType::TextInput: return "TextInput";
    case EventType::CommandRequested: return "CommandRequested";
    case EventType::OperatorInvoked: return "OperatorInvoked";
    case EventType::FrameBegin: return "FrameBegin";
    case EventType::FrameEnd: return "FrameEnd";
    }
    return "Unknown";
}

Event Event::quit()
{
    return Event(EventType::Quit);
}

Event Event::frame_begin()
{
    return Event(EventType::FrameBegin);
}

Event Event::frame_end()
{
    return Event(EventType::FrameEnd);
}

Event Event::window_close(std::uint32_t window_id)
{
    return Event(EventType::WindowClose, WindowData{window_id});
}

Event Event::window_resize(std::uint32_t window_id, int width, int height)
{
    return Event(EventType::WindowResize, WindowResizeData{window_id, width, height});
}

Event Event::window_focus(std::uint32_t window_id, bool focused)
{
    return Event(EventType::WindowFocus, WindowFocusData{window_id, focused});
}

Event Event::key_down(Key key, ModifierKey modifiers, bool repeat)
{
    return Event(EventType::KeyDown, KeyEventData{key, modifiers, repeat});
}

Event Event::key_up(Key key, ModifierKey modifiers)
{
    return Event(EventType::KeyUp, KeyEventData{key, modifiers, false});
}

Event Event::mouse_move(float x, float y, float dx, float dy)
{
    return Event(EventType::MouseMove, MouseMoveData{x, y, dx, dy});
}

Event Event::mouse_button_down(MouseButton button, float x, float y, ModifierKey modifiers)
{
    return Event(EventType::MouseButtonDown, MouseButtonData{button, modifiers, x, y});
}

Event Event::mouse_button_up(MouseButton button, float x, float y, ModifierKey modifiers)
{
    return Event(EventType::MouseButtonUp, MouseButtonData{button, modifiers, x, y});
}

Event Event::mouse_wheel(float dx, float dy)
{
    return Event(EventType::MouseWheel, MouseWheelData{dx, dy});
}

Event Event::text_input(std::string text)
{
    return Event(EventType::TextInput, TextInputData{std::move(text)});
}

Event Event::command_requested(std::string command_id)
{
    return Event(EventType::CommandRequested, StringData{std::move(command_id)});
}

Event Event::operator_invoked(std::string operator_id)
{
    return Event(EventType::OperatorInvoked, StringData{std::move(operator_id)});
}

const KeyEventData& Event::key() const
{
    FORGE_ASSERT_MSG(std::holds_alternative<KeyEventData>(payload),
                     "Event::key() on a non-key event");
    return std::get<KeyEventData>(payload);
}

const MouseMoveData& Event::mouse_move_data() const
{
    FORGE_ASSERT_MSG(std::holds_alternative<MouseMoveData>(payload),
                     "Event::mouse_move_data() on a non-move event");
    return std::get<MouseMoveData>(payload);
}

const MouseButtonData& Event::mouse_button() const
{
    FORGE_ASSERT_MSG(std::holds_alternative<MouseButtonData>(payload),
                     "Event::mouse_button() on a non-button event");
    return std::get<MouseButtonData>(payload);
}

const MouseWheelData& Event::mouse_wheel_data() const
{
    FORGE_ASSERT_MSG(std::holds_alternative<MouseWheelData>(payload),
                     "Event::mouse_wheel_data() on a non-wheel event");
    return std::get<MouseWheelData>(payload);
}

const WindowResizeData& Event::window_resize_data() const
{
    FORGE_ASSERT_MSG(std::holds_alternative<WindowResizeData>(payload),
                     "Event::window_resize_data() on a non-resize event");
    return std::get<WindowResizeData>(payload);
}

const WindowFocusData& Event::window_focus_data() const
{
    FORGE_ASSERT_MSG(std::holds_alternative<WindowFocusData>(payload),
                     "Event::window_focus_data() on a non-focus event");
    return std::get<WindowFocusData>(payload);
}

const TextInputData& Event::text() const
{
    FORGE_ASSERT_MSG(std::holds_alternative<TextInputData>(payload),
                     "Event::text() on a non-text event");
    return std::get<TextInputData>(payload);
}

const StringData& Event::string_data() const
{
    FORGE_ASSERT_MSG(std::holds_alternative<StringData>(payload),
                     "Event::string_data() on an event without a string payload");
    return std::get<StringData>(payload);
}

} // namespace forge
