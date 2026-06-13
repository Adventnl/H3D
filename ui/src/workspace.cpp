#include "forge/ui/workspace.hpp"

#include <utility>

#include "forge/ui/docking.hpp"

namespace forge::ui
{

Workspace::Workspace(std::string id, std::string display_name, Screen screen,
                     std::string description)
    : id_(std::move(id)),
      display_name_(std::move(display_name)),
      description_(std::move(description)),
      screen_(std::move(screen))
{
}

Screen build_workspace_screen(EditorType primary, const std::vector<EditorType>& secondaries)
{
    Screen screen(primary);
    const AreaId main_id = screen.active_area_id();

    if (!secondaries.empty())
    {
        const std::size_t n = secondaries.size();
        // Carve a side column for the first secondary, then stack the rest in it
        // so each occupies an even slice of the column.
        AreaId current = screen.split_area(main_id, SplitDirection::Vertical, 0.70f,
                                           secondaries[0]);
        for (std::size_t i = 1; i < n; ++i)
        {
            const float ratio = 1.0f / static_cast<float>(n - i + 1);
            current = screen.split_area(current, SplitDirection::Horizontal, ratio,
                                        secondaries[i]);
        }
        screen.set_active_area(main_id);
    }

    return screen;
}

} // namespace forge::ui
