#pragma once

// Preferences: user-configurable application settings, grouped logically and
// persisted as a flat key=value text file (no external JSON dependency).
//
// Loading is lenient: unknown keys are ignored, malformed lines are skipped,
// and out-of-range values are clamped to a valid range. A missing file yields
// defaults().

#include <string>
#include <string_view>

#include "forge/filesystem/path.hpp"
#include "forge/foundation/result.hpp"

namespace forge
{

struct ApplicationPreferences
{
    std::string default_workspace = "Layout";
    bool autosave_enabled = true;
    int autosave_interval_seconds = 120;
    int recent_files_limit = 20;
};

struct InterfacePreferences
{
    std::string theme_name = "Dark";
    std::string language = "en";
    float ui_scale = 1.0f;
    bool enable_tooltips = true;
    bool enable_developer_extras = false;
};

struct InputPreferences
{
    std::string keymap_preset = "ForgeDefault";
};

struct SystemPreferences
{
    bool use_hardware_acceleration = false; // placeholder until the GPU phase
};

struct Preferences
{
    ApplicationPreferences application;
    InterfacePreferences interface;
    InputPreferences input;
    SystemPreferences system;

    /// The built-in default preferences.
    [[nodiscard]] static Preferences defaults();

    /// Reset all groups to defaults().
    void reset();

    /// Serialize to the flat key=value format.
    [[nodiscard]] std::string serialize() const;

    /// Parse the flat key=value format, starting from defaults() and applying
    /// recognized keys. Always succeeds (lenient parsing).
    [[nodiscard]] static Preferences parse(std::string_view text);

    /// Write to disk (creating parent directories as needed).
    [[nodiscard]] Result<void> save(const Path& path) const;

    /// Load from disk. A missing file returns defaults(); other I/O errors are
    /// reported.
    [[nodiscard]] static Result<Preferences> load(const Path& path);

    [[nodiscard]] bool operator==(const Preferences& other) const;
};

} // namespace forge
