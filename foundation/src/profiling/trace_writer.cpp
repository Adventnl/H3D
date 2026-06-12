#include "forge/profiling/trace_writer.hpp"

#include <format>

#include "forge/filesystem/file.hpp"

namespace forge
{
namespace
{

void append_json_escaped(std::string& output, std::string_view text)
{
    for (const char character : text)
    {
        switch (character)
        {
        case '"': output += "\\\""; break;
        case '\\': output += "\\\\"; break;
        case '\n': output += "\\n"; break;
        case '\r': output += "\\r"; break;
        case '\t': output += "\\t"; break;
        default:
            if (static_cast<unsigned char>(character) < 0x20)
            {
                output += std::format("\\u{:04x}", static_cast<unsigned char>(character));
            }
            else
            {
                output += character;
            }
            break;
        }
    }
}

} // namespace

std::string write_chrome_trace_json(std::span<const ProfileEvent> events)
{
    std::string json;
    json.reserve(64 + events.size() * 96);
    json += "{\"displayTimeUnit\":\"ms\",\"traceEvents\":[";

    bool first = true;
    for (const ProfileEvent& event : events)
    {
        if (!first)
        {
            json += ',';
        }
        first = false;

        json += "{\"name\":\"";
        append_json_escaped(json, event.name);
        json += std::format(
            "\",\"cat\":\"forge\",\"ph\":\"X\",\"ts\":{},\"dur\":{},\"pid\":0,\"tid\":{}}}",
            event.start_microseconds, event.duration_microseconds, event.thread_id);
    }

    json += "]}";
    return json;
}

Result<void> write_chrome_trace_file(const Path& path, std::span<const ProfileEvent> events)
{
    return write_text_file(path, write_chrome_trace_json(events));
}

} // namespace forge
