#pragma once

// Editor framework. An Editor is the logic that drives an area: it has a type
// and display name, lifecycle hooks (on_open/on_close), an update() tick and an
// event handler. Phase 2 ships honest placeholder editors that describe which
// future system they depend on; none of them render or touch scene data.

#include <string>
#include <string_view>

#include "forge/ui/editor_types.hpp"

namespace forge
{
struct AppContext;
struct Event;
}

namespace forge::ui
{

/// Services an editor may use. `app` is null in pure-UI tests.
struct EditorContext
{
    forge::AppContext* app = nullptr;
};

class Editor
{
public:
    virtual ~Editor();

    [[nodiscard]] virtual EditorType type() const noexcept = 0;
    [[nodiscard]] virtual std::string_view display_name() const noexcept = 0;
    [[nodiscard]] virtual std::string_view description() const noexcept;

    /// One-line status describing the editor's current capability. Placeholder
    /// editors use this to say which phase brings them to life.
    [[nodiscard]] virtual std::string status_text() const;

    virtual void on_open(EditorContext& context);
    virtual void on_close(EditorContext& context);
    virtual void update(EditorContext& context);

    /// Handle an event. Returns true when consumed. Default: ignore.
    [[nodiscard]] virtual bool handle_event(EditorContext& context, const forge::Event& event);
};

/// Base for the Phase 2 placeholder editors: stores its identity and a status
/// message and implements all the no-op behavior.
class PlaceholderEditor : public Editor
{
public:
    PlaceholderEditor(EditorType type, std::string_view display_name,
                      std::string_view description, std::string_view status);

    [[nodiscard]] EditorType type() const noexcept final { return type_; }
    [[nodiscard]] std::string_view display_name() const noexcept final { return display_name_; }
    [[nodiscard]] std::string_view description() const noexcept final { return description_; }
    [[nodiscard]] std::string status_text() const final { return status_; }

private:
    EditorType type_;
    std::string display_name_;
    std::string description_;
    std::string status_;
};

// Each editor is a real, distinct type so later phases can specialize them.
#define FORGE_PLACEHOLDER_EDITOR(ClassName, TypeEnum, Display, Desc, Status)         \
    class ClassName final : public PlaceholderEditor                                 \
    {                                                                                \
    public:                                                                          \
        ClassName() : PlaceholderEditor(TypeEnum, Display, Desc, Status) {}          \
    }

FORGE_PLACEHOLDER_EDITOR(ViewportEditor, EditorType::Viewport3D, "Viewport 3D",
                         "The 3D viewport.",
                         "Viewport editor placeholder: renderer not implemented until a later phase.");
FORGE_PLACEHOLDER_EDITOR(OutlinerEditor, EditorType::Outliner, "Outliner",
                         "Hierarchical scene browser.",
                         "Outliner placeholder: scene database not implemented until Phase 3.");
FORGE_PLACEHOLDER_EDITOR(PropertiesEditor, EditorType::Properties, "Properties",
                         "Object, data and tool properties.",
                         "Properties placeholder: data blocks not implemented until Phase 3.");
FORGE_PLACEHOLDER_EDITOR(TimelineEditor, EditorType::Timeline, "Timeline",
                         "Playback and keyframe timeline.",
                         "Timeline placeholder: animation system not implemented yet.");
FORGE_PLACEHOLDER_EDITOR(DopeSheetEditor, EditorType::DopeSheet, "Dope Sheet",
                         "Keyframe summary and editing.",
                         "Dope Sheet placeholder: animation system not implemented yet.");
FORGE_PLACEHOLDER_EDITOR(GraphEditor, EditorType::GraphEditor, "Graph Editor",
                         "Animation F-curve editor.",
                         "Graph Editor placeholder: animation system not implemented yet.");
FORGE_PLACEHOLDER_EDITOR(NLAEditor, EditorType::NLAEditor, "Nonlinear Animation",
                         "Action strip editor.",
                         "NLA placeholder: animation system not implemented yet.");
FORGE_PLACEHOLDER_EDITOR(ShaderNodeEditor, EditorType::ShaderNodeEditor, "Shader Editor",
                         "Material shader node graph.",
                         "Shader Editor placeholder: node system not implemented yet.");
FORGE_PLACEHOLDER_EDITOR(GeometryNodeEditor, EditorType::GeometryNodeEditor,
                         "Geometry Nodes", "Procedural geometry node graph.",
                         "Geometry Nodes placeholder: node system not implemented yet.");
FORGE_PLACEHOLDER_EDITOR(CompositorEditor, EditorType::Compositor, "Compositor",
                         "Image compositing node graph.",
                         "Compositor placeholder: compositing not implemented yet.");
FORGE_PLACEHOLDER_EDITOR(UVEditor, EditorType::UVEditor, "UV Editor",
                         "UV unwrap and editing.",
                         "UV Editor placeholder: mesh/UV system not implemented yet.");
FORGE_PLACEHOLDER_EDITOR(ImageEditor, EditorType::ImageEditor, "Image Editor",
                         "Image viewing and painting.",
                         "Image Editor placeholder: image system not implemented yet.");
FORGE_PLACEHOLDER_EDITOR(TextEditor, EditorType::TextEditor, "Text Editor",
                         "Script and text editing.",
                         "Text Editor placeholder: text data blocks not implemented yet.");
FORGE_PLACEHOLDER_EDITOR(PythonConsoleEditor, EditorType::PythonConsole, "Python Console",
                         "Interactive scripting console.",
                         "Python Console placeholder: scripting not implemented yet.");
FORGE_PLACEHOLDER_EDITOR(AssetBrowserEditor, EditorType::AssetBrowser, "Asset Browser",
                         "Asset library browser.",
                         "Asset Browser placeholder: asset system not implemented yet.");
FORGE_PLACEHOLDER_EDITOR(SpreadsheetEditor, EditorType::Spreadsheet, "Spreadsheet",
                         "Tabular data inspector.",
                         "Spreadsheet placeholder: geometry data not implemented yet.");
FORGE_PLACEHOLDER_EDITOR(VideoSequencerEditor, EditorType::VideoSequencer,
                         "Video Sequencer", "Video editing timeline.",
                         "Video Sequencer placeholder: sequencer not implemented yet.");
FORGE_PLACEHOLDER_EDITOR(MovieClipEditor, EditorType::MovieClipEditor, "Movie Clip Editor",
                         "Motion tracking editor.",
                         "Movie Clip Editor placeholder: tracking not implemented yet.");
FORGE_PLACEHOLDER_EDITOR(PreferencesEditor, EditorType::Preferences, "Preferences",
                         "Application preferences.",
                         "Preferences placeholder: full preferences UI arrives with the renderer.");

#undef FORGE_PLACEHOLDER_EDITOR

} // namespace forge::ui
