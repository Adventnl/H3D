#pragma once

// Thread-safe, category-based logging.
//
//   FORGE_LOG_INFO("startup", "Forge3D starting");
//   forge::log_error("filesystem", "failed to read file");
//
// Each line carries a timestamp, level, thread id and category:
//   [2026-06-12 10:31:02.123] [info ] [tid 4138214986] [startup] Forge3D starting
//
// Output goes to the console (stdout, stderr for Error/Fatal) and to an
// optional log file. The macros skip message evaluation when the level is
// filtered out.

#include <atomic>
#include <cstdint>
#include <fstream>
#include <mutex>
#include <string>
#include <string_view>

#include "forge/foundation/noncopyable.hpp"

namespace forge
{

enum class LogLevel : std::uint8_t
{
    Trace = 0,
    Debug,
    Info,
    Warning,
    Error,
    Fatal,
    Off,
};

/// Fixed-width level label, e.g. "info ".
[[nodiscard]] std::string_view log_level_name(LogLevel level) noexcept;

class Logger : NonMovable
{
public:
    Logger();
    explicit Logger(LogLevel level);
    ~Logger();

    void set_level(LogLevel level) noexcept;
    [[nodiscard]] LogLevel level() const noexcept;
    [[nodiscard]] bool should_log(LogLevel level) const noexcept;

    /// Enable or disable console output (enabled by default).
    void set_console_output(bool enabled);

    /// Mirror all messages to a file. Returns false if the file cannot be
    /// opened; console output is unaffected either way.
    bool open_file(const std::string& path);
    void close_file();
    [[nodiscard]] bool has_file() const;

    void log(LogLevel level, std::string_view category, std::string_view message);
    void flush();

    /// Number of messages actually written (after level filtering).
    [[nodiscard]] std::uint64_t message_count() const noexcept;

private:
    mutable std::mutex mutex_;
    std::ofstream file_;
    bool console_enabled_ = true;
    std::atomic<LogLevel> level_{LogLevel::Trace};
    std::atomic<std::uint64_t> message_count_{0};
};

/// The process-wide logger used by the FORGE_LOG_* macros.
[[nodiscard]] Logger& default_logger();

void log_trace(std::string_view category, std::string_view message);
void log_debug(std::string_view category, std::string_view message);
void log_info(std::string_view category, std::string_view message);
void log_warning(std::string_view category, std::string_view message);
void log_error(std::string_view category, std::string_view message);
void log_fatal(std::string_view category, std::string_view message);

} // namespace forge

#define FORGE_LOG_IMPL(log_level, category, message)                        \
    do                                                                      \
    {                                                                       \
        if (::forge::default_logger().should_log(log_level))               \
        {                                                                   \
            ::forge::default_logger().log(log_level, category, message);   \
        }                                                                   \
    } while (false)

#define FORGE_LOG_TRACE(category, message) FORGE_LOG_IMPL(::forge::LogLevel::Trace, category, message)
#define FORGE_LOG_DEBUG(category, message) FORGE_LOG_IMPL(::forge::LogLevel::Debug, category, message)
#define FORGE_LOG_INFO(category, message) FORGE_LOG_IMPL(::forge::LogLevel::Info, category, message)
#define FORGE_LOG_WARNING(category, message) FORGE_LOG_IMPL(::forge::LogLevel::Warning, category, message)
#define FORGE_LOG_ERROR(category, message) FORGE_LOG_IMPL(::forge::LogLevel::Error, category, message)
#define FORGE_LOG_FATAL(category, message) FORGE_LOG_IMPL(::forge::LogLevel::Fatal, category, message)
