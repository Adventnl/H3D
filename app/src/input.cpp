#include "forge/app/input.hpp"

#include <array>
#include <utility>

#include "forge/app/event.hpp"

namespace forge
{
namespace
{

struct KeyName
{
    Key key;
    std::string_view name;
};

// Single source of truth for key <-> name in both directions.
constexpr std::array<KeyName, 82> kKeyNames{{
    {Key::A, "A"},   {Key::B, "B"},   {Key::C, "C"},   {Key::D, "D"},
    {Key::E, "E"},   {Key::F, "F"},   {Key::G, "G"},   {Key::H, "H"},
    {Key::I, "I"},   {Key::J, "J"},   {Key::K, "K"},   {Key::L, "L"},
    {Key::M, "M"},   {Key::N, "N"},   {Key::O, "O"},   {Key::P, "P"},
    {Key::Q, "Q"},   {Key::R, "R"},   {Key::S, "S"},   {Key::T, "T"},
    {Key::U, "U"},   {Key::V, "V"},   {Key::W, "W"},   {Key::X, "X"},
    {Key::Y, "Y"},   {Key::Z, "Z"},
    {Key::Num0, "0"}, {Key::Num1, "1"}, {Key::Num2, "2"}, {Key::Num3, "3"},
    {Key::Num4, "4"}, {Key::Num5, "5"}, {Key::Num6, "6"}, {Key::Num7, "7"},
    {Key::Num8, "8"}, {Key::Num9, "9"},
    {Key::F1, "F1"}, {Key::F2, "F2"}, {Key::F3, "F3"}, {Key::F4, "F4"},
    {Key::F5, "F5"}, {Key::F6, "F6"}, {Key::F7, "F7"}, {Key::F8, "F8"},
    {Key::F9, "F9"}, {Key::F10, "F10"}, {Key::F11, "F11"}, {Key::F12, "F12"},
    {Key::Space, "Space"}, {Key::Enter, "Enter"}, {Key::Tab, "Tab"},
    {Key::Escape, "Escape"}, {Key::Backspace, "Backspace"}, {Key::Delete, "Delete"},
    {Key::Insert, "Insert"}, {Key::Home, "Home"}, {Key::End, "End"},
    {Key::PageUp, "PageUp"}, {Key::PageDown, "PageDown"},
    {Key::Left, "Left"}, {Key::Right, "Right"}, {Key::Up, "Up"}, {Key::Down, "Down"},
    {Key::Minus, "Minus"}, {Key::Equal, "Equal"}, {Key::LeftBracket, "LeftBracket"},
    {Key::RightBracket, "RightBracket"}, {Key::Backslash, "Backslash"},
    {Key::Semicolon, "Semicolon"}, {Key::Apostrophe, "Apostrophe"},
    {Key::Comma, "Comma"}, {Key::Period, "Period"}, {Key::Slash, "Slash"},
    {Key::Grave, "Grave"},
    {Key::LeftShift, "LeftShift"}, {Key::RightShift, "RightShift"},
    {Key::LeftCtrl, "LeftCtrl"}, {Key::RightCtrl, "RightCtrl"},
    {Key::LeftAlt, "LeftAlt"}, {Key::RightAlt, "RightAlt"},
    {Key::LeftSuper, "LeftSuper"}, {Key::RightSuper, "RightSuper"},
}};

char ascii_lower(char c) noexcept
{
    return (c >= 'A' && c <= 'Z') ? static_cast<char>(c - 'A' + 'a') : c;
}

bool iequals(std::string_view a, std::string_view b) noexcept
{
    if (a.size() != b.size())
    {
        return false;
    }
    for (std::size_t i = 0; i < a.size(); ++i)
    {
        if (ascii_lower(a[i]) != ascii_lower(b[i]))
        {
            return false;
        }
    }
    return true;
}

} // namespace

std::string_view key_name(Key key) noexcept
{
    for (const KeyName& entry : kKeyNames)
    {
        if (entry.key == key)
        {
            return entry.name;
        }
    }
    return "Unknown";
}

Key key_from_name(std::string_view name) noexcept
{
    for (const KeyName& entry : kKeyNames)
    {
        if (iequals(entry.name, name))
        {
            return entry.key;
        }
    }
    return Key::Unknown;
}

bool is_modifier_key(Key key) noexcept
{
    switch (key)
    {
    case Key::LeftShift:
    case Key::RightShift:
    case Key::LeftCtrl:
    case Key::RightCtrl:
    case Key::LeftAlt:
    case Key::RightAlt:
    case Key::LeftSuper:
    case Key::RightSuper:
        return true;
    default:
        return false;
    }
}

ModifierKey modifier_for_key(Key key) noexcept
{
    switch (key)
    {
    case Key::LeftShift:
    case Key::RightShift:
        return ModifierKey::Shift;
    case Key::LeftCtrl:
    case Key::RightCtrl:
        return ModifierKey::Ctrl;
    case Key::LeftAlt:
    case Key::RightAlt:
        return ModifierKey::Alt;
    case Key::LeftSuper:
    case Key::RightSuper:
        return ModifierKey::Super;
    default:
        return ModifierKey::None;
    }
}

void InputState::press_key(Key key)
{
    if (key == Key::Unknown)
    {
        return;
    }
    const auto [iter, inserted] = down_keys_.insert(key);
    (void)iter;
    if (inserted)
    {
        pressed_keys_.insert(key);
    }
    if (is_modifier_key(key))
    {
        modifiers_ |= modifier_for_key(key);
    }
}

void InputState::release_key(Key key)
{
    if (key == Key::Unknown)
    {
        return;
    }
    if (down_keys_.erase(key) != 0)
    {
        released_keys_.insert(key);
    }
    if (is_modifier_key(key))
    {
        recompute_modifiers();
    }
}

void InputState::recompute_modifiers()
{
    modifiers_ = ModifierKey::None;
    for (const Key key : down_keys_)
    {
        if (is_modifier_key(key))
        {
            modifiers_ |= modifier_for_key(key);
        }
    }
}

bool InputState::is_key_down(Key key) const
{
    return down_keys_.contains(key);
}

bool InputState::was_key_pressed(Key key) const
{
    return pressed_keys_.contains(key);
}

bool InputState::was_key_released(Key key) const
{
    return released_keys_.contains(key);
}

void InputState::set_mouse_position(float x, float y)
{
    // The first position seen establishes the cursor without a delta; later
    // moves accumulate a per-frame delta (cleared by new_frame()).
    if (mouse_position_known_)
    {
        mouse_dx_ += x - mouse_x_;
        mouse_dy_ += y - mouse_y_;
    }
    mouse_x_ = x;
    mouse_y_ = y;
    mouse_position_known_ = true;
}

void InputState::add_scroll(float dx, float dy)
{
    scroll_dx_ += dx;
    scroll_dy_ += dy;
}

void InputState::press_mouse_button(MouseButton button)
{
    if (button != MouseButton::Count)
    {
        mouse_buttons_[static_cast<std::size_t>(button)] = true;
    }
}

void InputState::release_mouse_button(MouseButton button)
{
    if (button != MouseButton::Count)
    {
        mouse_buttons_[static_cast<std::size_t>(button)] = false;
    }
}

bool InputState::is_mouse_button_down(MouseButton button) const
{
    return button != MouseButton::Count && mouse_buttons_[static_cast<std::size_t>(button)];
}

void InputState::append_text(std::string_view text)
{
    text_buffer_.append(text);
}

void InputState::clear_text_buffer()
{
    text_buffer_.clear();
}

void InputState::apply(const Event& event)
{
    switch (event.type)
    {
    case EventType::KeyDown:
        press_key(event.key().key);
        break;
    case EventType::KeyUp:
        release_key(event.key().key);
        break;
    case EventType::MouseMove:
    {
        const MouseMoveData& data = event.mouse_move_data();
        set_mouse_position(data.x, data.y);
        break;
    }
    case EventType::MouseButtonDown:
        press_mouse_button(event.mouse_button().button);
        break;
    case EventType::MouseButtonUp:
        release_mouse_button(event.mouse_button().button);
        break;
    case EventType::MouseWheel:
    {
        const MouseWheelData& data = event.mouse_wheel_data();
        add_scroll(data.dx, data.dy);
        break;
    }
    case EventType::TextInput:
        append_text(event.text().text);
        break;
    default:
        break;
    }
}

void InputState::new_frame()
{
    pressed_keys_.clear();
    released_keys_.clear();
    mouse_dx_ = 0.0f;
    mouse_dy_ = 0.0f;
    scroll_dx_ = 0.0f;
    scroll_dy_ = 0.0f;
    text_buffer_.clear();
}

void InputState::reset()
{
    down_keys_.clear();
    pressed_keys_.clear();
    released_keys_.clear();
    mouse_buttons_.fill(false);
    modifiers_ = ModifierKey::None;
    mouse_x_ = mouse_y_ = 0.0f;
    mouse_dx_ = mouse_dy_ = 0.0f;
    scroll_dx_ = scroll_dy_ = 0.0f;
    mouse_position_known_ = false;
    text_buffer_.clear();
}

} // namespace forge
