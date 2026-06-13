#pragma once

// Panel: a titled, collapsible container of widgets. When collapsed it shows
// only its header; layout() stacks visible children vertically otherwise.

#include <string>

#include "forge/ui/widget.hpp"

namespace forge::ui
{

class Panel : public Widget
{
public:
    Panel(std::string id, std::string title);

    [[nodiscard]] const std::string& title() const noexcept { return title_; }
    void set_title(std::string title) { title_ = std::move(title); }

    [[nodiscard]] bool collapsed() const noexcept { return collapsed_; }
    void set_collapsed(bool collapsed) { collapsed_ = collapsed; }
    void toggle_collapsed() { collapsed_ = !collapsed_; }

    /// Height reserved for the panel header.
    [[nodiscard]] float header_height() const noexcept { return header_height_; }
    void set_header_height(float height) { header_height_ = height; }

    void layout() override;

private:
    std::string title_;
    bool collapsed_ = false;
    float header_height_ = 22.0f;
};

} // namespace forge::ui
