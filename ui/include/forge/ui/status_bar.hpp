#pragma once

// StatusBar: the bottom strip that shows transient messages and an optional
// progress value. Logical only.

#include <string>
#include <vector>

namespace forge::ui
{

class StatusBar
{
public:
    /// Replace the primary message.
    void set_message(std::string message);
    [[nodiscard]] const std::string& message() const noexcept { return message_; }

    /// Push a message into the recent-message history (most recent last).
    void push_message(std::string message);
    [[nodiscard]] const std::vector<std::string>& history() const noexcept { return history_; }

    void clear();

    // Progress in [0, 1], or a negative value meaning "no progress shown".
    void set_progress(float progress);
    void clear_progress() { progress_ = -1.0f; }
    [[nodiscard]] float progress() const noexcept { return progress_; }
    [[nodiscard]] bool has_progress() const noexcept { return progress_ >= 0.0f; }

    /// Cap on retained history entries.
    void set_history_limit(std::size_t limit);

private:
    std::string message_;
    std::vector<std::string> history_;
    float progress_ = -1.0f;
    std::size_t history_limit_ = 64;
};

} // namespace forge::ui
