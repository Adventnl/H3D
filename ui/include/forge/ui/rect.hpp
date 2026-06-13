#pragma once

// Geometry primitives for the logical UI layout: Point, Size and Rect.
//
// These are pure value types. Rect uses a top-left origin with y growing down,
// which matches the windowing conventions later phases will render with. No
// pixels are drawn in Phase 2 — this only computes bounds.

namespace forge::ui
{

struct Point
{
    float x = 0.0f;
    float y = 0.0f;
};

struct Size
{
    float width = 0.0f;
    float height = 0.0f;

    [[nodiscard]] bool is_empty() const noexcept { return width <= 0.0f || height <= 0.0f; }
};

struct Rect
{
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;

    constexpr Rect() = default;
    constexpr Rect(float rect_x, float rect_y, float rect_width, float rect_height)
        : x(rect_x), y(rect_y), width(rect_width), height(rect_height)
    {
    }

    [[nodiscard]] constexpr float left() const noexcept { return x; }
    [[nodiscard]] constexpr float top() const noexcept { return y; }
    [[nodiscard]] constexpr float right() const noexcept { return x + width; }
    [[nodiscard]] constexpr float bottom() const noexcept { return y + height; }
    [[nodiscard]] constexpr Point center() const noexcept
    {
        return Point{x + width * 0.5f, y + height * 0.5f};
    }
    [[nodiscard]] constexpr Size size() const noexcept { return Size{width, height}; }
    [[nodiscard]] constexpr float area() const noexcept { return width * height; }
    [[nodiscard]] constexpr bool is_empty() const noexcept
    {
        return width <= 0.0f || height <= 0.0f;
    }

    [[nodiscard]] bool contains(float px, float py) const noexcept;
    [[nodiscard]] bool contains(Point point) const noexcept;
    [[nodiscard]] bool intersects(const Rect& other) const noexcept;

    /// The carved-off sub-rect of the given thickness on one edge. The original
    /// rect is unchanged; combine with the matching remainder if needed.
    [[nodiscard]] Rect split_left(float amount) const noexcept;
    [[nodiscard]] Rect split_right(float amount) const noexcept;
    [[nodiscard]] Rect split_top(float amount) const noexcept;
    [[nodiscard]] Rect split_bottom(float amount) const noexcept;

    /// Copy shrunk by `margin` on every side (clamped so it never inverts).
    [[nodiscard]] Rect inset(float margin) const noexcept;

    [[nodiscard]] bool operator==(const Rect& other) const noexcept;
};

} // namespace forge::ui
