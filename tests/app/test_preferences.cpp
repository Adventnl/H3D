#include "test_framework.hpp"

#include <filesystem>
#include <system_error>

#include "forge/app/preferences.hpp"
#include "forge/filesystem/path.hpp"

FORGE_TEST_CASE("app.preferences_defaults_are_valid")
{
    const forge::Preferences prefs = forge::Preferences::defaults();
    FORGE_CHECK_EQ(prefs.interface.theme_name, std::string("Dark"));
    FORGE_CHECK_EQ(prefs.application.default_workspace, std::string("Layout"));
    FORGE_CHECK_EQ(prefs.input.keymap_preset, std::string("ForgeDefault"));
    FORGE_CHECK(prefs.application.autosave_interval_seconds > 0);
    FORGE_CHECK_NEAR(prefs.interface.ui_scale, 1.0f, 1e-4f);
}

FORGE_TEST_CASE("app.preferences_serialize_parse_round_trip")
{
    forge::Preferences prefs = forge::Preferences::defaults();
    prefs.interface.theme_name = "Light";
    prefs.interface.ui_scale = 1.5f;
    prefs.application.autosave_enabled = false;
    prefs.application.recent_files_limit = 7;
    prefs.system.use_hardware_acceleration = true;

    const forge::Preferences parsed = forge::Preferences::parse(prefs.serialize());
    FORGE_CHECK(parsed == prefs);
}

FORGE_TEST_CASE("app.preferences_parse_is_lenient")
{
    const forge::Preferences prefs = forge::Preferences::parse(
        "# a comment\n"
        "theme_name=Light\n"
        "unknown_key=ignored\n"
        "ui_scale=99.0\n"          // out of range -> clamped
        "autosave_interval_seconds=-5\n"  // invalid -> clamped to >= 1
        "garbage line without equals\n"
        "enable_tooltips=false\n");

    FORGE_CHECK_EQ(prefs.interface.theme_name, std::string("Light"));
    FORGE_CHECK(prefs.interface.ui_scale <= 4.0f);
    FORGE_CHECK(prefs.application.autosave_interval_seconds >= 1);
    FORGE_CHECK(!prefs.interface.enable_tooltips);
}

FORGE_TEST_CASE("app.preferences_save_and_load")
{
    const forge::Path path = forge::Path::temp_directory() / "forge_test_prefs.cfg";

    forge::Preferences prefs = forge::Preferences::defaults();
    prefs.interface.theme_name = "Light";
    prefs.application.recent_files_limit = 13;

    const auto saved = prefs.save(path);
    FORGE_REQUIRE(saved.has_value());

    const auto loaded = forge::Preferences::load(path);
    FORGE_REQUIRE(loaded.has_value());
    FORGE_CHECK(*loaded == prefs);

    std::error_code ignored;
    std::filesystem::remove(path.raw(), ignored);
}

FORGE_TEST_CASE("app.preferences_missing_file_returns_defaults")
{
    const forge::Path path =
        forge::Path::temp_directory() / "forge_nonexistent_prefs_zzz.cfg";
    const auto loaded = forge::Preferences::load(path);
    FORGE_REQUIRE(loaded.has_value());
    FORGE_CHECK(*loaded == forge::Preferences::defaults());
}
