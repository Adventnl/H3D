#include "forge/foundation/log.hpp"

#include <chrono>
#include <cstdio>
#include <ctime>
#include <format>

#include "forge/foundation/platform.hpp"

namespace forge
{
namespace
{

std::string format_timestamp()
{
    using namespace std::chrono;
    const auto now = system_clock::now();
    const auto milliseconds_part =
        duration_cast<milliseconds>(now.time_since_epoch()).count() % 1000;

    const std::time_t time = system_clock::to_time_t(now);
    std::tm local{};
#if FORGE_PLATFORM_WINDOWS
    localtime_s(&local, &time);
#else
    localtime_r(&time, &local);
#endif

    return std::format("{:04}-{:02}-{:02} {:02}:{:02}:{:02}.{:03}",
                       local.tm_year + 1900, local.tm_mon + 1, local.tm_mday,
                       local.tm_hour, local.tm_min, local.tm_sec,
                       milliseconds_part);
}

} // namespace

std::string_view log_level_name(LogLevel level) noexcept
{
    switch (level)
    {
    case LogLevel::Trace: return "trace";
    case LogLevel::Debug: return "debug";
    case LogLevel::Info: return "info ";
    case LogLevel::Warning: return "warn ";
    case LogLevel::Error: return "error";
    case LogLevel::Fatal: return "fatal";
    case LogLevel::Off: return "off  ";
    }
    return "?????";
}

Logger::Logger() = default;

Logger::Logger(LogLevel level) : level_(level) {}

Logger::~Logger()
{
    close_file();
}

void Logger::set_level(LogLevel level) noexcept
{
    level_.store(level, std::memory_order_relaxed);
}

LogLevel Logger::level() const noexcept
{
    return level_.load(std::memory_order_relaxed);
}

bool Logger::should_log(LogLevel level) const noexcept
{
    return level != LogLevel::Off && level >= level_.load(std::memory_order_relaxed);
}

void Logger::set_console_output(bool enabled)
{
    std::scoped_lock lock(mutex_);
    console_enabled_ = enabled;
}

bool Logger::open_file(const std::string& path)
{
    std::scoped_lock lock(mutex_);
    if (file_.is_open())
    {
        file_.close();
    }
    file_.open(path, std::ios::out | std::ios::app);
    return file_.is_open();
}

void Logger::close_file()
{
    std::scoped_lock lock(mutex_);
    if (file_.is_open())
    {
        file_.flush();
        file_.close();
    }
}

bool Logger::has_file() const
{
    std::scoped_lock lock(mutex_);
    return file_.is_open();
}

void Logger::log(LogLevel level, std::string_view category, std::string_view message)
{
    if (!should_log(level))
    {
        return;
    }

    const std::string line =
        std::format("[{}] [{}] [tid {}] [{}] {}\n", format_timestamp(),
                    log_level_name(level), current_thread_id(), category, message);

    std::scoped_lock lock(mutex_);
    if (console_enabled_)
    {
        std::FILE* stream = (level >= LogLevel::Error) ? stderr : stdout;
        std::fwrite(line.data(), 1, line.size(), stream);
        if (level >= LogLevel::Error)
        {
            std::fflush(stream);
        }
    }
    if (file_.is_open())
    {
        file_.write(line.data(), static_cast<std::streamsize>(line.size()));
        if (level >= LogLevel::Error)
        {
            file_.flush();
        }
    }
    message_count_.fetch_add(1, std::memory_order_relaxed);
}

void Logger::flush()
{
    std::scoped_lock lock(mutex_);
    std::fflush(stdout);
    std::fflush(stderr);
    if (file_.is_open())
    {
        file_.flush();
    }
}

std::uint64_t Logger::message_count() const noexcept
{
    return message_count_.load(std::memory_order_relaxed);
}

Logger& default_logger()
{
    static Logger logger;
    return logger;
}

void log_trace(std::string_view category, std::string_view message)
{
    default_logger().log(LogLevel::Trace, category, message);
}

void log_debug(std::string_view category, std::string_view message)
{
    default_logger().log(LogLevel::Debug, category, message);
}

void log_info(std::string_view category, std::string_view message)
{
    default_logger().log(LogLevel::Info, category, message);
}

void log_warning(std::string_view category, std::string_view message)
{
    default_logger().log(LogLevel::Warning, category, message);
}

void log_error(std::string_view category, std::string_view message)
{
    default_logger().log(LogLevel::Error, category, message);
}

void log_fatal(std::string_view category, std::string_view message)
{
    default_logger().log(LogLevel::Fatal, category, message);
}

} // namespace forge
