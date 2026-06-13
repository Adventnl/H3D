#pragma once

// Logical theme data: colors and named themes. No renderer dependency — a Theme
// is just a palette the future UI renderer will consult.

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace forge::ui
{

/// An 8-bit-per-channel RGBA color.
struct Color
{
    std::uint8_t r = 0;
    std::uint8_t g = 0;
    std::uint8_t b = 0;
    std::uint8_t a = 255;

    constexpr Color() = default;
    constexpr Color(std::uint8_t red, std::uint8_t green, std::uint8_t blue,
                    std::uint8_t alpha = 255)
        : r(red), g(green), b(blue), a(alpha)
    {
    }

    [[nodiscard]] static constexpr Color rgb(std::uint8_t r, std::uint8_t g, std::uint8_t b)
    {
        return Color{r, g, b, 255};
    }
    [[nodiscard]] static constexpr Color rgba(std::uint8_t r, std::uint8_t g, std::uint8_t b,
                                              std::uint8_t a)
    {
        return Color{r, g, b, a};
    }

    [[nodiscard]] constexpr float red_f() const noexcept { return r / 255.0f; }
    [[nodiscard]] constexpr float green_f() const noexcept { return g / 255.0f; }
    [[nodiscard]] constexpr float blue_f() const noexcept { return b / 255.0f; }
    [[nodiscard]] constexpr float alpha_f() const noexcept { return a / 255.0f; }

    [[nodiscard]] bool operator==(const Color&) const noexcept = default;

    /// "#RRGGBBAA"
    [[nodiscard]] std::string to_hex() const;
};

struct Theme
{
    std::string name = "Dark";

    Color background;
    Color panel_background;
    Color panel_border;
    Color text;
    Color text_muted;
    Color accent;
    Color warning;
    Color error;
    Color success;
    Color selection;
    Color highlight;
    Color button;
    Color button_hover;
    Color button_active;
};

[[nodiscard]] Theme make_dark_theme();
[[nodiscard]] Theme make_light_theme();

class ThemeRegistry
{
public:
    /// Registers the built-in Dark and Light themes and makes Dark active.
    ThemeRegistry();

    void register_theme(Theme theme);
    [[nodiscard]] const Theme* find(std::string_view name) const;
    [[nodiscard]] bool contains(std::string_view name) const;

    /// Set the active theme by name. Returns false when the name is unknown.
    bool set_active(std::string_view name);
    [[nodiscard]] const Theme& active() const;
    [[nodiscard]] const std::string& active_name() const noexcept { return active_name_; }

    [[nodiscard]] std::vector<std::string> names() const;
    [[nodiscard]] std::size_t size() const noexcept { return themes_.size(); }

private:
    std::vector<Theme> themes_;
    std::string active_name_;
};

} // namespace forge::ui
