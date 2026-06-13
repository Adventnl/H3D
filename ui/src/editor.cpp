#include "forge/ui/editor.hpp"

#include <array>
#include <utility>

namespace forge::ui
{
namespace
{

struct EditorTypeNames
{
    EditorType type;
    std::string_view name;
    std::string_view id;
};

constexpr std::array<EditorTypeNames, kEditorTypeCount> kEditorNames{{
    {EditorType::Viewport3D, "Viewport 3D", "viewport_3d"},
    {EditorType::Outliner, "Outliner", "outliner"},
    {EditorType::Properties, "Properties", "properties"},
    {EditorType::Timeline, "Timeline", "timeline"},
    {EditorType::DopeSheet, "Dope Sheet", "dope_sheet"},
    {EditorType::GraphEditor, "Graph Editor", "graph_editor"},
    {EditorType::NLAEditor, "Nonlinear Animation", "nla_editor"},
    {EditorType::ShaderNodeEditor, "Shader Editor", "shader_node_editor"},
    {EditorType::GeometryNodeEditor, "Geometry Nodes", "geometry_node_editor"},
    {EditorType::Compositor, "Compositor", "compositor"},
    {EditorType::UVEditor, "UV Editor", "uv_editor"},
    {EditorType::ImageEditor, "Image Editor", "image_editor"},
    {EditorType::TextEditor, "Text Editor", "text_editor"},
    {EditorType::PythonConsole, "Python Console", "python_console"},
    {EditorType::AssetBrowser, "Asset Browser", "asset_browser"},
    {EditorType::Spreadsheet, "Spreadsheet", "spreadsheet"},
    {EditorType::VideoSequencer, "Video Sequencer", "video_sequencer"},
    {EditorType::MovieClipEditor, "Movie Clip Editor", "movie_clip_editor"},
    {EditorType::Preferences, "Preferences", "preferences"},
}};

} // namespace

std::string_view editor_type_name(EditorType type) noexcept
{
    const auto index = static_cast<std::size_t>(type);
    if (index < kEditorNames.size())
    {
        return kEditorNames[index].name;
    }
    return "Unknown";
}

std::string_view editor_type_id(EditorType type) noexcept
{
    const auto index = static_cast<std::size_t>(type);
    if (index < kEditorNames.size())
    {
        return kEditorNames[index].id;
    }
    return "unknown";
}

EditorType editor_type_from_id(std::string_view id) noexcept
{
    for (const EditorTypeNames& entry : kEditorNames)
    {
        if (entry.id == id)
        {
            return entry.type;
        }
    }
    return EditorType::Count;
}

Editor::~Editor() = default;

std::string_view Editor::description() const noexcept
{
    return {};
}

std::string Editor::status_text() const
{
    return std::string(display_name()) + " editor.";
}

void Editor::on_open(EditorContext&) {}
void Editor::on_close(EditorContext&) {}
void Editor::update(EditorContext&) {}

bool Editor::handle_event(EditorContext&, const forge::Event&)
{
    return false;
}

PlaceholderEditor::PlaceholderEditor(EditorType type, std::string_view display_name,
                                     std::string_view description, std::string_view status)
    : type_(type),
      display_name_(display_name),
      description_(description),
      status_(status)
{
}

} // namespace forge::ui
