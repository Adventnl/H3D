#include "forge/app/keymap.hpp"

#include <algorithm>

namespace forge
{

void Keymap::bind(const Shortcut& shortcut, std::string action_id)
{
    if (!shortcut.is_valid())
    {
        return;
    }
    bindings_[shortcut] = std::move(action_id);
}

bool Keymap::unbind(const Shortcut& shortcut)
{
    return bindings_.erase(shortcut) != 0;
}

std::optional<std::string> Keymap::resolve(const Shortcut& shortcut) const
{
    const auto it = bindings_.find(shortcut);
    if (it == bindings_.end())
    {
        return std::nullopt;
    }
    return it->second;
}

std::optional<Shortcut> Keymap::shortcut_for(std::string_view action_id) const
{
    std::optional<Shortcut> best;
    for (const auto& [shortcut, id] : bindings_)
    {
        if (id == action_id)
        {
            // Prefer the lexicographically smallest rendering for determinism.
            if (!best || shortcut.to_string() < best->to_string())
            {
                best = shortcut;
            }
        }
    }
    return best;
}

bool Keymap::contains(const Shortcut& shortcut) const
{
    return bindings_.contains(shortcut);
}

void Keymap::clear()
{
    bindings_.clear();
}

std::vector<Keymap::Binding> Keymap::bindings() const
{
    std::vector<Binding> result;
    result.reserve(bindings_.size());
    for (const auto& [shortcut, id] : bindings_)
    {
        result.push_back(Binding{shortcut, id});
    }
    std::sort(result.begin(), result.end(), [](const Binding& a, const Binding& b) {
        if (a.action_id != b.action_id)
        {
            return a.action_id < b.action_id;
        }
        return a.shortcut.to_string() < b.shortcut.to_string();
    });
    return result;
}

Keymap Keymap::default_keymap()
{
    Keymap keymap;
    keymap.bind(Shortcut(Key::S, ModifierKey::Ctrl), "file.save");
    keymap.bind(Shortcut(Key::O, ModifierKey::Ctrl), "file.open");
    keymap.bind(Shortcut(Key::N, ModifierKey::Ctrl), "file.new");
    keymap.bind(Shortcut(Key::Z, ModifierKey::Ctrl), "edit.undo");
    keymap.bind(Shortcut(Key::Z, ModifierKey::Ctrl | ModifierKey::Shift), "edit.redo");
    keymap.bind(Shortcut(Key::F3), "command.search");
    keymap.bind(Shortcut(Key::G), "transform.move");
    keymap.bind(Shortcut(Key::R), "transform.rotate");
    keymap.bind(Shortcut(Key::S), "transform.scale");
    keymap.bind(Shortcut(Key::Tab), "mode.toggle");
    keymap.bind(Shortcut(Key::PageDown, ModifierKey::Ctrl), "workspace.next");
    keymap.bind(Shortcut(Key::PageUp, ModifierKey::Ctrl), "workspace.previous");
    return keymap;
}

} // namespace forge
