// Forge3D desktop shell — Phase 1.
//
// There is no window or UI yet (Phase 2 builds the window manager). This
// entry point exercises the real startup/shutdown path every later phase
// will extend: logging, build info, the job system, a frame loop with
// profiling, and clean teardown.
//
//   forge_desktop [--frames N] [--trace <file>] [--log-file <file>]

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <format>
#include <string>
#include <string_view>
#include <thread>

#include "forge/foundation/build_info.hpp"
#include "forge/foundation/log.hpp"
#include "forge/foundation/scope_exit.hpp"
#include "forge/foundation/time.hpp"
#include "forge/profiling/profiler.hpp"
#include "forge/profiling/profiler_scope.hpp"
#include "forge/profiling/trace_writer.hpp"
#include "forge/threading/job_system.hpp"

namespace
{

constexpr std::string_view kLogCategory = "desktop";

struct DesktopOptions
{
    int frames = 5;
    std::string trace_path;
    std::string log_file_path;
#if FORGE_TRACING_ENABLED
    bool tracing = true;
#else
    bool tracing = false;
#endif
};

/// Minimal runtime shell: the Phase 2 application object will grow from
/// this initialize / frame loop / shutdown skeleton.
class Runtime
{
public:
    bool initialize()
    {
        FORGE_LOG_INFO(kLogCategory, "initializing runtime");
        forge::JobSystem::initialize();
        FORGE_LOG_INFO(kLogCategory,
                       std::format("job system online with {} workers",
                                   forge::JobSystem::worker_count()));
        return true;
    }

    void run_frames(int frame_count)
    {
        forge::FrameTimer frame_timer;

        for (int frame = 0; frame < frame_count; ++frame)
        {
            FORGE_PROFILE_SCOPE("frame");

            // Placeholder workload standing in for scene evaluation: a
            // parallel reduction large enough to touch every worker.
            std::atomic<long long> accumulator{0};
            {
                FORGE_PROFILE_SCOPE("frame.parallel_work");
                forge::JobSystem::parallel_for(0, 100'000, [&accumulator](std::size_t index) {
                    accumulator.fetch_add(static_cast<long long>(index % 7),
                                          std::memory_order_relaxed);
                });
            }

            // Simulate a small amount of fixed frame cost.
            std::this_thread::sleep_for(std::chrono::milliseconds(1));

            const forge::Duration delta = frame_timer.tick();
            FORGE_LOG_DEBUG(kLogCategory,
                            std::format("frame {} took {:.3f} ms (checksum {})", frame,
                                        delta.seconds() * 1e3, accumulator.load()));
        }

        FORGE_LOG_INFO(kLogCategory,
                       std::format("ran {} frames, average fps estimate {:.1f}",
                                   frame_timer.frame_count(),
                                   frame_timer.frames_per_second()));
    }

    void shutdown()
    {
        FORGE_LOG_INFO(kLogCategory, "shutting down runtime");
        forge::JobSystem::shutdown();
    }
};

DesktopOptions parse_arguments(int argc, char** argv)
{
    DesktopOptions options;
    for (int index = 1; index < argc; ++index)
    {
        const std::string_view argument = argv[index];
        if (argument == "--frames" && index + 1 < argc)
        {
            options.frames = std::atoi(argv[++index]);
            if (options.frames < 0)
            {
                options.frames = 0;
            }
        }
        else if (argument == "--trace" && index + 1 < argc)
        {
            options.trace_path = argv[++index];
            options.tracing = true;
        }
        else if (argument == "--log-file" && index + 1 < argc)
        {
            options.log_file_path = argv[++index];
        }
        else if (argument == "--help" || argument == "-h")
        {
            std::printf("usage: forge_desktop [--frames N] [--trace <file>] "
                        "[--log-file <file>]\n");
            std::exit(0);
        }
    }
    if (options.tracing && options.trace_path.empty())
    {
        options.trace_path = "forge_trace.json";
    }
    return options;
}

} // namespace

int main(int argc, char** argv)
{
    const DesktopOptions options = parse_arguments(argc, argv);

    if (!options.log_file_path.empty() &&
        !forge::default_logger().open_file(options.log_file_path))
    {
        std::fprintf(stderr, "warning: could not open log file '%s'\n",
                     options.log_file_path.c_str());
    }

    FORGE_LOG_INFO(kLogCategory, "Forge3D desktop shell starting");
    FORGE_LOG_INFO(kLogCategory, forge::build_info_string());

    Runtime runtime;
    if (!runtime.initialize())
    {
        FORGE_LOG_FATAL(kLogCategory, "runtime initialization failed");
        return 1;
    }
    FORGE_SCOPE_EXIT(runtime.shutdown());

    runtime.run_frames(options.frames);

    if (options.tracing)
    {
        const auto events = forge::Profiler::instance().events();
        const auto written =
            forge::write_chrome_trace_file(forge::Path(options.trace_path), events);
        if (written)
        {
            FORGE_LOG_INFO(kLogCategory,
                           std::format("wrote {} profile events to {}", events.size(),
                                       options.trace_path));
        }
        else
        {
            FORGE_LOG_WARNING(kLogCategory,
                              std::format("failed to write trace: {}",
                                          written.error().to_string()));
        }
    }

    FORGE_LOG_INFO(kLogCategory, "Forge3D desktop shell exiting cleanly");
    return 0;
}
