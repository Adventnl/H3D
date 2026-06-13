#include "forge/ui/theme.hpp"

#include <algorithm>
#include <array>
#include <cstdio>

#include "forge/foundation/assert.hpp"

namespace forge::ui
{

std::string Color::to_hex() const
{
    std::array<char, 10> buffer{};
    std::snprintf(buffer.data(), buffer.size(), "#%02X%02X%02X%02X", r, g, b, a);
    return std::string(buffer.data());
}

Theme make_dark_theme()
{
    Theme theme;
    theme.name = "Dark";
    theme.background = Color::rgb(33, 33, 33);
    theme.panel_background = Color::rgb(48, 48, 48);
    theme.panel_border = Color::rgb(24, 24, 24);
    theme.text = Color::rgb(220, 220, 220);
    theme.text_muted = Color::rgb(150, 150, 150);
    theme.accent = Color::rgb(76, 142, 220);
    theme.warning = Color::rgb(220, 170, 60);
    theme.error = Color::rgb(220, 80, 70);
    theme.success = Color::rgb(90, 190, 110);
    theme.selection = Color::rgb(60, 110, 180);
    theme.highlight = Color::rgba(255, 255, 255, 30);
    theme.button = Color::rgb(64, 64, 64);
    theme.button_hover = Color::rgb(82, 82, 82);
    theme.button_active = Color::rgb(96, 96, 96);
    return theme;
}

Theme make_light_theme()
{
    Theme theme;
    theme.name = "Light";
    theme.background = Color::rgb(235, 235, 235);
    theme.panel_background = Color::rgb(220, 220, 220);
    theme.panel_border = Color::rgb(180, 180, 180);
    theme.text = Color::rgb(30, 30, 30);
    theme.text_muted = Color::rgb(110, 110, 110);
    theme.accent = Color::rgb(40, 110, 200);
    theme.warning = Color::rgb(190, 130, 20);
    theme.error = Color::rgb(200, 50, 45);
    theme.success = Color::rgb(40, 150, 70);
    theme.selection = Color::rgb(150, 190, 240);
    theme.highlight = Color::rgba(0, 0, 0, 20);
    theme.button = Color::rgb(205, 205, 205);
    theme.button_hover = Color::rgb(190, 190, 190);
    theme.button_active = Color::rgb(175, 175, 175);
    return theme;
}

ThemeRegistry::ThemeRegistry()
{
    register_theme(make_dark_theme());
    register_theme(make_light_theme());
    active_name_ = "Dark";
}

void ThemeRegistry::register_theme(Theme theme)
{
    for (Theme& existing : themes_)
    {
        if (existing.name == theme.name)
        {
            existing = std::move(theme);
            return;
        }
    }
    themes_.push_back(std::move(theme));
    if (active_name_.empty())
    {
        active_name_ = themes_.back().name;
    }
}

const Theme* ThemeRegistry::find(std::string_view name) const
{
    for (const Theme& theme : themes_)
    {
        if (theme.name == name)
        {
            return &theme;
        }
    }
    return nullptr;
}

bool ThemeRegistry::contains(std::string_view name) const
{
    return find(name) != nullptr;
}

bool ThemeRegistry::set_active(std::string_view name)
{
    if (!contains(name))
    {
        return false;
    }
    active_name_ = std::string(name);
    return true;
}

const Theme& ThemeRegistry::active() const
{
    const Theme* theme = find(active_name_);
    FORGE_ASSERT_MSG(theme != nullptr, "active theme must always exist");
    return *theme;
}

std::vector<std::string> ThemeRegistry::names() const
{
    std::vector<std::string> result;
    result.reserve(themes_.size());
    for (const Theme& theme : themes_)
    {
        result.push_back(theme.name);
    }
    std::sort(result.begin(), result.end());
    return result;
}

} // namespace forge::ui
