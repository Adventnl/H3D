#include "forge/ui/editor_registry.hpp"

#include <utility>

namespace forge::ui
{

void EditorRegistry::register_factory(EditorType type, EditorFactory factory)
{
    const auto index = static_cast<std::size_t>(type);
    if (index < factories_.size())
    {
        factories_[index] = std::move(factory);
    }
}

bool EditorRegistry::is_registered(EditorType type) const
{
    const auto index = static_cast<std::size_t>(type);
    return index < factories_.size() && static_cast<bool>(factories_[index]);
}

std::unique_ptr<Editor> EditorRegistry::create(EditorType type) const
{
    const auto index = static_cast<std::size_t>(type);
    if (index >= factories_.size() || !factories_[index])
    {
        return nullptr;
    }
    return factories_[index]();
}

std::vector<EditorType> EditorRegistry::registered_types() const
{
    std::vector<EditorType> result;
    for (std::size_t i = 0; i < factories_.size(); ++i)
    {
        if (factories_[i])
        {
            result.push_back(static_cast<EditorType>(i));
        }
    }
    return result;
}

std::size_t EditorRegistry::size() const
{
    std::size_t count = 0;
    for (const EditorFactory& factory : factories_)
    {
        if (factory)
        {
            ++count;
        }
    }
    return count;
}

void register_default_editors(EditorRegistry& registry)
{
    registry.register_factory(EditorType::Viewport3D,
                              [] { return std::make_unique<ViewportEditor>(); });
    registry.register_factory(EditorType::Outliner,
                              [] { return std::make_unique<OutlinerEditor>(); });
    registry.register_factory(EditorType::Properties,
                              [] { return std::make_unique<PropertiesEditor>(); });
    registry.register_factory(EditorType::Timeline,
                              [] { return std::make_unique<TimelineEditor>(); });
    registry.register_factory(EditorType::DopeSheet,
                              [] { return std::make_unique<DopeSheetEditor>(); });
    registry.register_factory(EditorType::GraphEditor,
                              [] { return std::make_unique<GraphEditor>(); });
    registry.register_factory(EditorType::NLAEditor,
                              [] { return std::make_unique<NLAEditor>(); });
    registry.register_factory(EditorType::ShaderNodeEditor,
                              [] { return std::make_unique<ShaderNodeEditor>(); });
    registry.register_factory(EditorType::GeometryNodeEditor,
                              [] { return std::make_unique<GeometryNodeEditor>(); });
    registry.register_factory(EditorType::Compositor,
                              [] { return std::make_unique<CompositorEditor>(); });
    registry.register_factory(EditorType::UVEditor,
                              [] { return std::make_unique<UVEditor>(); });
    registry.register_factory(EditorType::ImageEditor,
                              [] { return std::make_unique<ImageEditor>(); });
    registry.register_factory(EditorType::TextEditor,
                              [] { return std::make_unique<TextEditor>(); });
    registry.register_factory(EditorType::PythonConsole,
                              [] { return std::make_unique<PythonConsoleEditor>(); });
    registry.register_factory(EditorType::AssetBrowser,
                              [] { return std::make_unique<AssetBrowserEditor>(); });
    registry.register_factory(EditorType::Spreadsheet,
                              [] { return std::make_unique<SpreadsheetEditor>(); });
    registry.register_factory(EditorType::VideoSequencer,
                              [] { return std::make_unique<VideoSequencerEditor>(); });
    registry.register_factory(EditorType::MovieClipEditor,
                              [] { return std::make_unique<MovieClipEditor>(); });
    registry.register_factory(EditorType::Preferences,
                              [] { return std::make_unique<PreferencesEditor>(); });
}

} // namespace forge::ui
