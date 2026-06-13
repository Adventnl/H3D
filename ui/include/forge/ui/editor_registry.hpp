#pragma once

// EditorRegistry: maps an EditorType to a factory that builds an Editor for it.
// Areas ask the registry to create the editor instance they host.

#include <functional>
#include <memory>
#include <vector>

#include "forge/ui/editor.hpp"
#include "forge/ui/editor_types.hpp"

namespace forge::ui
{

using EditorFactory = std::function<std::unique_ptr<Editor>()>;

class EditorRegistry
{
public:
    /// Register (or replace) the factory for an editor type.
    void register_factory(EditorType type, EditorFactory factory);

    [[nodiscard]] bool is_registered(EditorType type) const;

    /// Build an editor for the type. Returns nullptr when no factory exists.
    [[nodiscard]] std::unique_ptr<Editor> create(EditorType type) const;

    [[nodiscard]] std::vector<EditorType> registered_types() const;
    [[nodiscard]] std::size_t size() const;

private:
    std::vector<EditorFactory> factories_ = std::vector<EditorFactory>(kEditorTypeCount);
};

/// Register factories for every built-in editor type.
void register_default_editors(EditorRegistry& registry);

} // namespace forge::ui
