#include "forge/app/shortcut.hpp"

#include <string>

namespace forge
{
namespace
{

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

std::string Shortcut::to_string() const
{
    if (!is_valid())
    {
        return {};
    }
    std::string result;
    if (has_modifier(modifiers, ModifierKey::Ctrl))
    {
        result += "Ctrl+";
    }
    if (has_modifier(modifiers, ModifierKey::Shift))
    {
        result += "Shift+";
    }
    if (has_modifier(modifiers, ModifierKey::Alt))
    {
        result += "Alt+";
    }
    if (has_modifier(modifiers, ModifierKey::Super))
    {
        result += "Super+";
    }
    result += key_name(key);
    return result;
}

std::optional<Shortcut> Shortcut::parse(std::string_view text)
{
    Shortcut shortcut;
    std::size_t start = 0;
    bool have_key = false;

    while (start <= text.size())
    {
        std::size_t plus = text.find('+', start);
        if (plus == std::string_view::npos)
        {
            plus = text.size();
        }
        const std::string_view token = text.substr(start, plus - start);
        start = plus + 1;

        if (token.empty())
        {
            continue;
        }

        if (iequals(token, "Ctrl") || iequals(token, "Control") || iequals(token, "Cmd"))
        {
            shortcut.modifiers |= ModifierKey::Ctrl;
        }
        else if (iequals(token, "Shift"))
        {
            shortcut.modifiers |= ModifierKey::Shift;
        }
        else if (iequals(token, "Alt") || iequals(token, "Option"))
        {
            shortcut.modifiers |= ModifierKey::Alt;
        }
        else if (iequals(token, "Super") || iequals(token, "Win") || iequals(token, "Meta"))
        {
            shortcut.modifiers |= ModifierKey::Super;
        }
        else
        {
            const Key parsed = key_from_name(token);
            if (parsed == Key::Unknown || have_key)
            {
                return std::nullopt;
            }
            shortcut.key = parsed;
            have_key = true;
        }

        if (plus == text.size())
        {
            break;
        }
    }

    if (!have_key)
    {
        return std::nullopt;
    }
    return shortcut;
}

} // namespace forge
