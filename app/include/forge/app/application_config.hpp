#pragma once

// ApplicationConfig: the inputs that shape an Application instance. Kept as a
// plain value type so callers (CLI flags, tests) can build one directly.

#include <optional>
#include <string>

namespace forge
{

struct ApplicationConfig
{
    std::string application_name = "Forge3D";
    std::string organization_name = "Forge3D";
    std::string version; // empty -> filled from forge::version_string()

    bool headless = true;            // Phase 2 defaults to headless (no native GUI)
    bool enable_profiling = true;
    bool enable_tracing = false;

    /// Optional overrides; empty/nullopt means "use the built-in default".
    std::string log_file_path;
    std::string user_config_directory;
    std::optional<int> frame_limit;       // run loop stops after this many frames
    std::optional<double> fixed_delta_time; // deterministic stepping when set

    /// Fill empty fields with sensible defaults (e.g. version string).
    void apply_defaults();
};

} // namespace forge
