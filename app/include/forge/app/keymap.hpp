#pragma once

// Keymap: a bidirectional-ish mapping from Shortcut to a command/operator id.
//
// A keymap resolves a Shortcut to the id of the action it should invoke. The
// default keymap wires the standard Forge3D shortcuts (see default_keymap()).

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "forge/app/shortcut.hpp"

namespace forge
{

class Keymap
{
public:
    struct Binding
    {
        Shortcut shortcut;
        std::string action_id;
    };

    /// Bind a shortcut to an action id, replacing any existing binding.
    void bind(const Shortcut& shortcut, std::string action_id);

    /// Remove a binding. Returns true when one was removed.
    bool unbind(const Shortcut& shortcut);

    /// Action id bound to the shortcut, or nullopt when unbound.
    [[nodiscard]] std::optional<std::string> resolve(const Shortcut& shortcut) const;

    /// First shortcut bound to the given action id, or nullopt.
    [[nodiscard]] std::optional<Shortcut> shortcut_for(std::string_view action_id) const;

    [[nodiscard]] bool contains(const Shortcut& shortcut) const;
    [[nodiscard]] std::size_t size() const noexcept { return bindings_.size(); }
    void clear();

    /// All bindings, sorted by action id for stable listing.
    [[nodiscard]] std::vector<Binding> bindings() const;

    /// The standard Forge3D keymap (ForgeDefault preset).
    [[nodiscard]] static Keymap default_keymap();

private:
    std::unordered_map<Shortcut, std::string> bindings_;
};

} // namespace forge
