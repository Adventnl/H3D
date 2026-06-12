#include <chrono>
#include <filesystem>
#include <string>
#include <thread>

#include "forge/filesystem/file.hpp"
#include "forge/profiling/profiler.hpp"
#include "forge/profiling/profiler_scope.hpp"
#include "forge/profiling/trace_writer.hpp"
#include "test_framework.hpp"

using forge::ProfileEvent;
using forge::Profiler;

FORGE_TEST_CASE("profiling.profiler_records_event")
{
    Profiler& profiler = Profiler::instance();
    profiler.clear();
    profiler.set_enabled(true);

    profiler.begin_event("manual_event");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    profiler.end_event();

    const auto events = profiler.events();
    FORGE_REQUIRE(events.size() == 1);
    FORGE_CHECK_EQ(events[0].name, std::string("manual_event"));
    FORGE_CHECK(events[0].duration_microseconds > 0);
    FORGE_CHECK(events[0].start_microseconds >= 0);
    profiler.clear();
}

FORGE_TEST_CASE("profiling.profiler_scope_macro_records")
{
    Profiler& profiler = Profiler::instance();
    profiler.clear();
    profiler.set_enabled(true);

    {
        FORGE_PROFILE_SCOPE("scoped_event");
        FORGE_PROFILE_FUNCTION();
    }

#if FORGE_PROFILING_ENABLED
    const auto events = profiler.events();
    FORGE_REQUIRE(events.size() == 2);
    // Inner scopes close first.
    FORGE_CHECK_EQ(events[1].name, std::string("scoped_event"));
#endif
    profiler.clear();
}

FORGE_TEST_CASE("profiling.profiler_nested_events")
{
    Profiler& profiler = Profiler::instance();
    profiler.clear();
    profiler.set_enabled(true);

    profiler.begin_event("outer");
    profiler.begin_event("inner");
    profiler.end_event();
    profiler.end_event();

    const auto events = profiler.events();
    FORGE_REQUIRE(events.size() == 2);
    FORGE_CHECK_EQ(events[0].name, std::string("inner"));
    FORGE_CHECK_EQ(events[1].name, std::string("outer"));
    // The outer event spans the inner one.
    FORGE_CHECK(events[1].start_microseconds <= events[0].start_microseconds);
    FORGE_CHECK(events[1].duration_microseconds >= events[0].duration_microseconds);
    profiler.clear();
}

FORGE_TEST_CASE("profiling.profiler_disabled_records_nothing")
{
    Profiler& profiler = Profiler::instance();
    profiler.clear();
    profiler.set_enabled(false);

    profiler.begin_event("ignored");
    profiler.end_event();
    FORGE_CHECK_EQ(profiler.event_count(), static_cast<std::size_t>(0));

    profiler.set_enabled(true);
}

FORGE_TEST_CASE("profiling.trace_writer_produces_json")
{
    ProfileEvent event;
    event.name = "sample \"quoted\" event";
    event.thread_id = 7;
    event.start_microseconds = 100;
    event.duration_microseconds = 250;

    const std::string json = forge::write_chrome_trace_json({&event, 1});
    FORGE_CHECK(!json.empty());
    FORGE_CHECK(json.find("traceEvents") != std::string::npos);
    FORGE_CHECK(json.find("\\\"quoted\\\"") != std::string::npos);
    FORGE_CHECK(json.find("\"ts\":100") != std::string::npos);
    FORGE_CHECK(json.find("\"dur\":250") != std::string::npos);
}

FORGE_TEST_CASE("profiling.trace_writer_writes_file")
{
    const forge::Path path =
        forge::Path::temp_directory() / "forge3d_tests_trace.json";
    std::filesystem::remove(path.raw());

    ProfileEvent event;
    event.name = "file_event";
    event.start_microseconds = 1;
    event.duration_microseconds = 2;

    FORGE_REQUIRE(forge::write_chrome_trace_file(path, {&event, 1}).has_value());

    const auto content = forge::read_text_file(path);
    FORGE_REQUIRE(content.has_value());
    FORGE_CHECK(content->find("file_event") != std::string::npos);

    std::filesystem::remove(path.raw());
}
