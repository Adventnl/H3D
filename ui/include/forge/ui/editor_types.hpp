#pragma once

// The set of editor kinds an area can host. Most are placeholders in Phase 2;
// their real implementations arrive in later phases (scene database, renderer,
// node systems, ...).

#include <cstdint>
#include <string_view>

namespace forge::ui
{

enum class EditorType : std::uint8_t
{
    Viewport3D = 0,
    Outliner,
    Properties,
    Timeline,
    DopeSheet,
    GraphEditor,
    NLAEditor,
    ShaderNodeEditor,
    GeometryNodeEditor,
    Compositor,
    UVEditor,
    ImageEditor,
    TextEditor,
    PythonConsole,
    AssetBrowser,
    Spreadsheet,
    VideoSequencer,
    MovieClipEditor,
    Preferences,

    Count,
};

/// Number of distinct editor types.
inline constexpr std::size_t kEditorTypeCount = static_cast<std::size_t>(EditorType::Count);

/// Human-readable name, e.g. EditorType::Viewport3D -> "Viewport 3D".
[[nodiscard]] std::string_view editor_type_name(EditorType type) noexcept;

/// Stable identifier, e.g. EditorType::Viewport3D -> "viewport_3d".
[[nodiscard]] std::string_view editor_type_id(EditorType type) noexcept;

/// Inverse of editor_type_id(); returns EditorType::Count on no match.
[[nodiscard]] EditorType editor_type_from_id(std::string_view id) noexcept;

} // namespace forge::ui
