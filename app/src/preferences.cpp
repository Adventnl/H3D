#include "forge/app/preferences.hpp"

#include <algorithm>
#include <charconv>
#include <format>

#include "forge/filesystem/file.hpp"

namespace forge
{
namespace
{

std::string_view trim(std::string_view text)
{
    const auto is_space = [](char c) {
        return c == ' ' || c == '\t' || c == '\r' || c == '\n';
    };
    while (!text.empty() && is_space(text.front()))
    {
        text.remove_prefix(1);
    }
    while (!text.empty() && is_space(text.back()))
    {
        text.remove_suffix(1);
    }
    return text;
}

bool parse_bool(std::string_view value, bool fallback)
{
    if (value == "true" || value == "1" || value == "yes" || value == "on")
    {
        return true;
    }
    if (value == "false" || value == "0" || value == "no" || value == "off")
    {
        return false;
    }
    return fallback;
}

int parse_int(std::string_view value, int fallback)
{
    int result = fallback;
    const char* begin = value.data();
    const char* end = begin + value.size();
    const auto [ptr, ec] = std::from_chars(begin, end, result);
    if (ec != std::errc{} || ptr != end)
    {
        return fallback;
    }
    return result;
}

float parse_float(std::string_view value, float fallback)
{
    // std::from_chars for float is not available on every libstdc++ shipped
    // with the supported compilers; std::stof via a temporary string is the
    // portable choice here.
    try
    {
        std::size_t consumed = 0;
        const std::string owned(value);
        const float result = std::stof(owned, &consumed);
        if (consumed != owned.size())
        {
            return fallback;
        }
        return result;
    }
    catch (...)
    {
        return fallback;
    }
}

const char* bool_text(bool value)
{
    return value ? "true" : "false";
}

} // namespace

Preferences Preferences::defaults()
{
    return Preferences{};
}

void Preferences::reset()
{
    *this = Preferences{};
}

std::string Preferences::serialize() const
{
    std::string out;
    out += std::format("theme_name={}\n", interface.theme_name);
    out += std::format("language={}\n", interface.language);
    out += std::format("ui_scale={:.3f}\n", interface.ui_scale);
    out += std::format("enable_tooltips={}\n", bool_text(interface.enable_tooltips));
    out += std::format("enable_developer_extras={}\n",
                       bool_text(interface.enable_developer_extras));
    out += std::format("default_workspace={}\n", application.default_workspace);
    out += std::format("autosave_enabled={}\n", bool_text(application.autosave_enabled));
    out += std::format("autosave_interval_seconds={}\n",
                       application.autosave_interval_seconds);
    out += std::format("recent_files_limit={}\n", application.recent_files_limit);
    out += std::format("keymap_preset={}\n", input.keymap_preset);
    out += std::format("use_hardware_acceleration={}\n",
                       bool_text(system.use_hardware_acceleration));
    return out;
}

Preferences Preferences::parse(std::string_view text)
{
    Preferences prefs = defaults();

    std::size_t line_start = 0;
    while (line_start <= text.size())
    {
        std::size_t newline = text.find('\n', line_start);
        if (newline == std::string_view::npos)
        {
            newline = text.size();
        }
        const std::string_view raw_line = text.substr(line_start, newline - line_start);
        line_start = newline + 1;

        const std::string_view line = trim(raw_line);
        if (line.empty() || line.front() == '#')
        {
            if (newline == text.size())
            {
                break;
            }
            continue;
        }

        const std::size_t eq = line.find('=');
        if (eq == std::string_view::npos)
        {
            if (newline == text.size())
            {
                break;
            }
            continue;
        }

        const std::string_view key = trim(line.substr(0, eq));
        const std::string_view value = trim(line.substr(eq + 1));

        if (key == "theme_name")
        {
            if (!value.empty())
            {
                prefs.interface.theme_name = std::string(value);
            }
        }
        else if (key == "language")
        {
            if (!value.empty())
            {
                prefs.interface.language = std::string(value);
            }
        }
        else if (key == "ui_scale")
        {
            prefs.interface.ui_scale =
                std::clamp(parse_float(value, prefs.interface.ui_scale), 0.5f, 4.0f);
        }
        else if (key == "enable_tooltips")
        {
            prefs.interface.enable_tooltips =
                parse_bool(value, prefs.interface.enable_tooltips);
        }
        else if (key == "enable_developer_extras")
        {
            prefs.interface.enable_developer_extras =
                parse_bool(value, prefs.interface.enable_developer_extras);
        }
        else if (key == "default_workspace")
        {
            if (!value.empty())
            {
                prefs.application.default_workspace = std::string(value);
            }
        }
        else if (key == "autosave_enabled")
        {
            prefs.application.autosave_enabled =
                parse_bool(value, prefs.application.autosave_enabled);
        }
        else if (key == "autosave_interval_seconds")
        {
            prefs.application.autosave_interval_seconds = std::max(
                1, parse_int(value, prefs.application.autosave_interval_seconds));
        }
        else if (key == "recent_files_limit")
        {
            prefs.application.recent_files_limit =
                std::max(0, parse_int(value, prefs.application.recent_files_limit));
        }
        else if (key == "keymap_preset")
        {
            if (!value.empty())
            {
                prefs.input.keymap_preset = std::string(value);
            }
        }
        else if (key == "use_hardware_acceleration")
        {
            prefs.system.use_hardware_acceleration =
                parse_bool(value, prefs.system.use_hardware_acceleration);
        }
        // Unknown keys are ignored.

        if (newline == text.size())
        {
            break;
        }
    }

    return prefs;
}

Result<void> Preferences::save(const Path& path) const
{
    return write_text_file(path, serialize());
}

Result<Preferences> Preferences::load(const Path& path)
{
    if (!path.exists())
    {
        return defaults();
    }
    auto text = read_text_file(path);
    if (!text)
    {
        return failure(std::move(text).error());
    }
    return parse(*text);
}

bool Preferences::operator==(const Preferences& other) const
{
    return interface.theme_name == other.interface.theme_name &&
           interface.language == other.interface.language &&
           interface.ui_scale == other.interface.ui_scale &&
           interface.enable_tooltips == other.interface.enable_tooltips &&
           interface.enable_developer_extras == other.interface.enable_developer_extras &&
           application.default_workspace == other.application.default_workspace &&
           application.autosave_enabled == other.application.autosave_enabled &&
           application.autosave_interval_seconds ==
               other.application.autosave_interval_seconds &&
           application.recent_files_limit == other.application.recent_files_limit &&
           input.keymap_preset == other.input.keymap_preset &&
           system.use_hardware_acceleration == other.system.use_hardware_acceleration;
}

} // namespace forge
