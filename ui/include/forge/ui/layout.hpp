#pragma once

// Stateless layout helpers that carve a Rect into sub-rects. The screen/area
// docking tree (docking.hpp) handles persistent splits; these are for laying
// out widgets within a region.

#include <utility>
#include <vector>

#include "forge/ui/rect.hpp"

namespace forge::ui
{

/// Split into left/right by a fraction in [0, 1] of the width.
[[nodiscard]] std::pair<Rect, Rect> split_horizontal(const Rect& bounds, float ratio,
                                                     float gap = 0.0f);

/// Split into top/bottom by a fraction in [0, 1] of the height.
[[nodiscard]] std::pair<Rect, Rect> split_vertical(const Rect& bounds, float ratio,
                                                   float gap = 0.0f);

/// `count` equal-width columns left to right.
[[nodiscard]] std::vector<Rect> layout_columns(const Rect& bounds, std::size_t count,
                                               float gap = 0.0f);

/// `count` equal-height rows top to bottom.
[[nodiscard]] std::vector<Rect> layout_rows(const Rect& bounds, std::size_t count,
                                            float gap = 0.0f);

/// Columns sized proportionally to `weights`.
[[nodiscard]] std::vector<Rect> layout_weighted_columns(const Rect& bounds,
                                                        const std::vector<float>& weights,
                                                        float gap = 0.0f);

/// One cell of a `columns` x `rows` grid (zero-based indices).
[[nodiscard]] Rect grid_cell(const Rect& bounds, std::size_t columns, std::size_t rows,
                             std::size_t column, std::size_t row, float gap = 0.0f);

} // namespace forge::ui
