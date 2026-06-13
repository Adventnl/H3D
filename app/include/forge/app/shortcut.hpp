#pragma once

// Shortcut: a key plus a set of modifiers, with a human-readable rendering
// ("Ctrl+Shift+S") and round-trip parsing.

#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <string_view>

#include "forge/app/input.hpp"

namespace forge
{

struct Shortcut
{
    Key key = Key::Unknown;
    ModifierKey modifiers = ModifierKey::None;

    Shortcut() = default;
    Shortcut(Key shortcut_key, ModifierKey shortcut_modifiers = ModifierKey::None)
        : key(shortcut_key), modifiers(shortcut_modifiers)
    {
    }

    [[nodiscard]] bool operator==(const Shortcut& other) const noexcept
    {
        return key == other.key && modifiers == other.modifiers;
    }

    [[nodiscard]] bool is_valid() const noexcept { return key != Key::Unknown; }

    /// "Ctrl+Shift+S". Empty when the shortcut is invalid.
    [[nodiscard]] std::string to_string() const;

    /// Parse "Ctrl+S" / "ctrl+shift+z" / "F3". Returns nullopt on failure.
    [[nodiscard]] static std::optional<Shortcut> parse(std::string_view text);
};

} // namespace forge

template <>
struct std::hash<forge::Shortcut>
{
    [[nodiscard]] std::size_t operator()(const forge::Shortcut& shortcut) const noexcept
    {
        const std::size_t key_hash = std::hash<std::uint16_t>{}(
            static_cast<std::uint16_t>(shortcut.key));
        const std::size_t mod_hash = std::hash<std::uint8_t>{}(
            static_cast<std::uint8_t>(shortcut.modifiers));
        return key_hash ^ (mod_hash << 1);
    }
};
