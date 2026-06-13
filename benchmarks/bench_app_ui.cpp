#include <cstdint>

#include "bench_framework.hpp"
#include "forge/app/keymap.hpp"
#include "forge/app/operator_registry.hpp"
#include "forge/ui/screen.hpp"
#include "forge/ui/workspace.hpp"
#include "forge/ui/workspace_registry.hpp"

using forge::bench::do_not_optimize;

FORGE_BENCHMARK("app.operator_search")
{
    forge::OperatorRegistry registry;
    forge::register_default_operators(registry);
    const char* queries[] = {"transform", "save", "workspace", "undo", "zzz"};
    for (std::uint64_t index = 0; index < state.iterations(); ++index)
    {
        const auto hits = registry.search(queries[index % 5], 8);
        do_not_optimize(hits.size());
    }
}

FORGE_BENCHMARK("app.keymap_lookup")
{
    const forge::Keymap keymap = forge::Keymap::default_keymap();
    const forge::Shortcut shortcuts[] = {
        forge::Shortcut(forge::Key::S, forge::ModifierKey::Ctrl),
        forge::Shortcut(forge::Key::G),
        forge::Shortcut(forge::Key::Tab),
        forge::Shortcut(forge::Key::F3),
    };
    for (std::uint64_t index = 0; index < state.iterations(); ++index)
    {
        const auto action = keymap.resolve(shortcuts[index % 4]);
        do_not_optimize(action.has_value());
    }
}

FORGE_BENCHMARK("ui.screen_split")
{
    for (std::uint64_t index = 0; index < state.iterations(); ++index)
    {
        forge::ui::Screen screen(forge::ui::EditorType::Viewport3D);
        const forge::ui::AreaId a = screen.active_area_id();
        const forge::ui::AreaId b =
            screen.split_area(a, forge::ui::SplitDirection::Vertical, 0.5f);
        screen.split_area(b, forge::ui::SplitDirection::Horizontal, 0.5f);
        screen.compute_layout(forge::ui::Rect{0, 0, 1920, 1080});
        do_not_optimize(screen.area_count());
    }
}

FORGE_BENCHMARK("ui.workspace_creation")
{
    for (std::uint64_t index = 0; index < state.iterations(); ++index)
    {
        forge::ui::WorkspaceRegistry registry;
        forge::ui::register_default_workspaces(registry);
        do_not_optimize(registry.size());
    }
}
