#include "forge/ui/layout.hpp"

#include <algorithm>
#include <numeric>

namespace forge::ui
{

std::pair<Rect, Rect> split_horizontal(const Rect& bounds, float ratio, float gap)
{
    const float clamped = std::clamp(ratio, 0.0f, 1.0f);
    const float usable = std::max(0.0f, bounds.width - gap);
    const float left_width = usable * clamped;
    const Rect left{bounds.x, bounds.y, left_width, bounds.height};
    const Rect right{bounds.x + left_width + gap, bounds.y, usable - left_width,
                     bounds.height};
    return {left, right};
}

std::pair<Rect, Rect> split_vertical(const Rect& bounds, float ratio, float gap)
{
    const float clamped = std::clamp(ratio, 0.0f, 1.0f);
    const float usable = std::max(0.0f, bounds.height - gap);
    const float top_height = usable * clamped;
    const Rect top{bounds.x, bounds.y, bounds.width, top_height};
    const Rect bottom{bounds.x, bounds.y + top_height + gap, bounds.width,
                      usable - top_height};
    return {top, bottom};
}

std::vector<Rect> layout_columns(const Rect& bounds, std::size_t count, float gap)
{
    std::vector<Rect> result;
    if (count == 0)
    {
        return result;
    }
    result.reserve(count);
    const float total_gap = gap * static_cast<float>(count - 1);
    const float column_width =
        std::max(0.0f, (bounds.width - total_gap) / static_cast<float>(count));
    for (std::size_t i = 0; i < count; ++i)
    {
        const float x = bounds.x + static_cast<float>(i) * (column_width + gap);
        result.push_back(Rect{x, bounds.y, column_width, bounds.height});
    }
    return result;
}

std::vector<Rect> layout_rows(const Rect& bounds, std::size_t count, float gap)
{
    std::vector<Rect> result;
    if (count == 0)
    {
        return result;
    }
    result.reserve(count);
    const float total_gap = gap * static_cast<float>(count - 1);
    const float row_height =
        std::max(0.0f, (bounds.height - total_gap) / static_cast<float>(count));
    for (std::size_t i = 0; i < count; ++i)
    {
        const float y = bounds.y + static_cast<float>(i) * (row_height + gap);
        result.push_back(Rect{bounds.x, y, bounds.width, row_height});
    }
    return result;
}

std::vector<Rect> layout_weighted_columns(const Rect& bounds,
                                          const std::vector<float>& weights, float gap)
{
    std::vector<Rect> result;
    if (weights.empty())
    {
        return result;
    }
    float total_weight = std::accumulate(weights.begin(), weights.end(), 0.0f);
    if (total_weight <= 0.0f)
    {
        total_weight = static_cast<float>(weights.size());
    }
    const float total_gap = gap * static_cast<float>(weights.size() - 1);
    const float usable = std::max(0.0f, bounds.width - total_gap);

    result.reserve(weights.size());
    float cursor = bounds.x;
    for (const float weight : weights)
    {
        const float w = usable * (std::max(0.0f, weight) / total_weight);
        result.push_back(Rect{cursor, bounds.y, w, bounds.height});
        cursor += w + gap;
    }
    return result;
}

Rect grid_cell(const Rect& bounds, std::size_t columns, std::size_t rows,
               std::size_t column, std::size_t row, float gap)
{
    if (columns == 0 || rows == 0 || column >= columns || row >= rows)
    {
        return Rect{};
    }
    const float total_h_gap = gap * static_cast<float>(columns - 1);
    const float total_v_gap = gap * static_cast<float>(rows - 1);
    const float cell_w =
        std::max(0.0f, (bounds.width - total_h_gap) / static_cast<float>(columns));
    const float cell_h =
        std::max(0.0f, (bounds.height - total_v_gap) / static_cast<float>(rows));
    const float x = bounds.x + static_cast<float>(column) * (cell_w + gap);
    const float y = bounds.y + static_cast<float>(row) * (cell_h + gap);
    return Rect{x, y, cell_w, cell_h};
}

} // namespace forge::ui
