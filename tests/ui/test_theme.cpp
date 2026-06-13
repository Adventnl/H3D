#include "test_framework.hpp"

#include "forge/ui/theme.hpp"

FORGE_TEST_CASE("ui.theme_default_dark_and_light_exist")
{
    forge::ui::ThemeRegistry registry;
    FORGE_CHECK(registry.contains("Dark"));
    FORGE_CHECK(registry.contains("Light"));
    FORGE_CHECK_EQ(registry.active_name(), std::string("Dark"));
    FORGE_CHECK(registry.size() >= static_cast<std::size_t>(2));
}

FORGE_TEST_CASE("ui.theme_lookup_and_set_active")
{
    forge::ui::ThemeRegistry registry;
    const forge::ui::Theme* dark = registry.find("Dark");
    FORGE_REQUIRE(dark != nullptr);
    FORGE_CHECK_EQ(dark->name, std::string("Dark"));

    FORGE_CHECK(registry.set_active("Light"));
    FORGE_CHECK_EQ(registry.active().name, std::string("Light"));
    FORGE_CHECK(!registry.set_active("Nonexistent"));
    FORGE_CHECK_EQ(registry.active().name, std::string("Light")); // unchanged
}

FORGE_TEST_CASE("ui.theme_colors_are_valid")
{
    const forge::ui::Theme dark = forge::ui::make_dark_theme();
    const forge::ui::Theme light = forge::ui::make_light_theme();

    // Alpha defaults to opaque; channels are within byte range by construction.
    FORGE_CHECK_EQ(dark.text.a, static_cast<std::uint8_t>(255));
    FORGE_CHECK_EQ(light.background.a, static_cast<std::uint8_t>(255));

    // Float accessors normalize to [0, 1].
    FORGE_CHECK_NEAR(forge::ui::Color::rgb(255, 0, 0).red_f(), 1.0f, 1e-4f);
    FORGE_CHECK_NEAR(forge::ui::Color::rgb(0, 0, 0).blue_f(), 0.0f, 1e-4f);

    FORGE_CHECK_EQ(forge::ui::Color::rgb(16, 32, 48).to_hex(), std::string("#102030FF"));
}

FORGE_TEST_CASE("ui.theme_register_custom")
{
    forge::ui::ThemeRegistry registry;
    forge::ui::Theme custom = forge::ui::make_dark_theme();
    custom.name = "HighContrast";
    registry.register_theme(custom);
    FORGE_CHECK(registry.contains("HighContrast"));
    FORGE_CHECK(registry.set_active("HighContrast"));
}
