#pragma once

// Input primitives: keys, mouse buttons, modifier flags and the per-frame
// InputState that aggregates them.
//
// InputState is the canonical, queryable snapshot of devices for a frame. It
// is updated by feeding it events (apply()) and advanced once per frame with
// new_frame(), which clears the edge-triggered "just pressed/released" sets and
// the per-frame mouse/scroll deltas.

#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_set>

namespace forge
{

/// Physical keys. Values are stable but otherwise unspecified; do not persist.
enum class Key : std::uint16_t
{
    Unknown = 0,

    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

    Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,

    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

    Space, Enter, Tab, Escape, Backspace, Delete, Insert,
    Home, End, PageUp, PageDown,
    Left, Right, Up, Down,

    Minus, Equal, LeftBracket, RightBracket, Backslash,
    Semicolon, Apostrophe, Comma, Period, Slash, Grave,

    LeftShift, RightShift, LeftCtrl, RightCtrl,
    LeftAlt, RightAlt, LeftSuper, RightSuper,

    Count,
};

/// Mouse buttons.
enum class MouseButton : std::uint8_t
{
    Left = 0,
    Right,
    Middle,
    Extra1,
    Extra2,
    Count,
};

/// Keyboard modifier flags. Combinable with the bitwise operators below.
enum class ModifierKey : std::uint8_t
{
    None = 0,
    Ctrl = 1u << 0,
    Shift = 1u << 1,
    Alt = 1u << 2,
    Super = 1u << 3,
};

[[nodiscard]] constexpr ModifierKey operator|(ModifierKey a, ModifierKey b) noexcept
{
    return static_cast<ModifierKey>(static_cast<std::uint8_t>(a) | static_cast<std::uint8_t>(b));
}

[[nodiscard]] constexpr ModifierKey operator&(ModifierKey a, ModifierKey b) noexcept
{
    return static_cast<ModifierKey>(static_cast<std::uint8_t>(a) & static_cast<std::uint8_t>(b));
}

constexpr ModifierKey& operator|=(ModifierKey& a, ModifierKey b) noexcept
{
    a = a | b;
    return a;
}

/// True when `flags` contains every bit in `query` (query None is always true).
[[nodiscard]] constexpr bool has_modifier(ModifierKey flags, ModifierKey query) noexcept
{
    return (static_cast<std::uint8_t>(flags) & static_cast<std::uint8_t>(query)) ==
           static_cast<std::uint8_t>(query);
}

/// Stable display/identifier name for a key, e.g. Key::S -> "S", PageDown ->
/// "PageDown". Unknown keys return "Unknown".
[[nodiscard]] std::string_view key_name(Key key) noexcept;

/// Inverse of key_name(); returns Key::Unknown when no key matches.
[[nodiscard]] Key key_from_name(std::string_view name) noexcept;

/// True when the key is one of the modifier keys (shift/ctrl/alt/super).
[[nodiscard]] bool is_modifier_key(Key key) noexcept;

/// Modifier flag a modifier key contributes (None for non-modifier keys).
[[nodiscard]] ModifierKey modifier_for_key(Key key) noexcept;

struct Event; // forward declaration; defined in event.hpp

/// A queryable snapshot of input devices for the current frame.
class InputState
{
public:
    // --- Keyboard ---------------------------------------------------------
    void press_key(Key key);
    void release_key(Key key);

    [[nodiscard]] bool is_key_down(Key key) const;
    [[nodiscard]] bool was_key_pressed(Key key) const;  // edge: pressed this frame
    [[nodiscard]] bool was_key_released(Key key) const;  // edge: released this frame

    [[nodiscard]] ModifierKey modifiers() const noexcept { return modifiers_; }

    // --- Mouse ------------------------------------------------------------
    void set_mouse_position(float x, float y);
    void add_scroll(float dx, float dy);
    void press_mouse_button(MouseButton button);
    void release_mouse_button(MouseButton button);

    [[nodiscard]] float mouse_x() const noexcept { return mouse_x_; }
    [[nodiscard]] float mouse_y() const noexcept { return mouse_y_; }
    [[nodiscard]] float mouse_delta_x() const noexcept { return mouse_dx_; }
    [[nodiscard]] float mouse_delta_y() const noexcept { return mouse_dy_; }
    [[nodiscard]] float scroll_delta_x() const noexcept { return scroll_dx_; }
    [[nodiscard]] float scroll_delta_y() const noexcept { return scroll_dy_; }
    [[nodiscard]] bool is_mouse_button_down(MouseButton button) const;

    // --- Text -------------------------------------------------------------
    void append_text(std::string_view text);
    [[nodiscard]] const std::string& text_buffer() const noexcept { return text_buffer_; }
    void clear_text_buffer();

    // --- Frame lifecycle --------------------------------------------------
    /// Update state from a single event.
    void apply(const Event& event);

    /// Advance to a new frame: clears edge sets, per-frame deltas and the text
    /// buffer. Held keys/buttons and the cursor position persist.
    void new_frame();

    /// Clear everything to the initial state.
    void reset();

    [[nodiscard]] std::size_t pressed_key_count() const noexcept { return down_keys_.size(); }

private:
    void recompute_modifiers();

    std::unordered_set<Key> down_keys_;
    std::unordered_set<Key> pressed_keys_;   // edge-triggered, cleared each frame
    std::unordered_set<Key> released_keys_;  // edge-triggered, cleared each frame
    std::array<bool, static_cast<std::size_t>(MouseButton::Count)> mouse_buttons_{};
    ModifierKey modifiers_ = ModifierKey::None;

    float mouse_x_ = 0.0f;
    float mouse_y_ = 0.0f;
    float mouse_dx_ = 0.0f;
    float mouse_dy_ = 0.0f;
    float scroll_dx_ = 0.0f;
    float scroll_dy_ = 0.0f;
    bool mouse_position_known_ = false;  // first position set establishes, no delta
    std::string text_buffer_;
};

} // namespace forge
