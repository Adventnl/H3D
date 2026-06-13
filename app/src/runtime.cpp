#include "forge/app/runtime.hpp"

#include "forge/foundation/log.hpp"
#include "forge/threading/job_system.hpp"

namespace forge
{
namespace
{
constexpr std::string_view kLogCategory = "runtime";
}

Runtime::~Runtime()
{
    shutdown();
}

Result<void> Runtime::initialize()
{
    if (initialized_)
    {
        return success();
    }
    FORGE_LOG_INFO(kLogCategory, "initializing runtime");
    JobSystem::initialize();
    start_ = Instant::now();
    last_frame_ = start_;
    first_frame_ = true;
    frame_index_ = 0;
    delta_seconds_ = 0.0;
    quit_requested_ = false;
    initialized_ = true;
    return success();
}

void Runtime::shutdown()
{
    if (!initialized_)
    {
        return;
    }
    FORGE_LOG_INFO(kLogCategory, "shutting down runtime");
    // Reverse order of initialize(): the job system comes down last.
    JobSystem::shutdown();
    initialized_ = false;
}

void Runtime::begin_frame()
{
    const Instant now = Instant::now();
    if (first_frame_)
    {
        delta_seconds_ = 0.0;
        first_frame_ = false;
    }
    else
    {
        delta_seconds_ = (now - last_frame_).seconds();
        ++frame_index_;
    }
    last_frame_ = now;
}

void Runtime::end_frame()
{
    // Reserved for future per-frame bookkeeping (GPU fences, profiler flush).
}

double Runtime::time() const noexcept
{
    return (Instant::now() - start_).seconds();
}

} // namespace forge
