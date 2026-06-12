#include "forge/threading/job_system.hpp"

#include <memory>

#include "forge/foundation/assert.hpp"

namespace forge
{
namespace
{

std::unique_ptr<ThreadPool>& global_pool() noexcept
{
    static std::unique_ptr<ThreadPool> pool;
    return pool;
}

} // namespace

void JobSystem::initialize(std::size_t worker_count)
{
    FORGE_ASSERT_MSG(!global_pool(), "JobSystem::initialize called twice");
    if (!global_pool())
    {
        global_pool() = std::make_unique<ThreadPool>(worker_count);
    }
}

void JobSystem::shutdown()
{
    global_pool().reset();
}

bool JobSystem::is_initialized() noexcept
{
    return global_pool() != nullptr;
}

std::size_t JobSystem::worker_count() noexcept
{
    return is_initialized() ? global_pool()->worker_count() : 0;
}

ThreadPool& JobSystem::pool()
{
    FORGE_ASSERT_MSG(is_initialized(), "JobSystem used before initialize()");
    return *global_pool();
}

} // namespace forge
