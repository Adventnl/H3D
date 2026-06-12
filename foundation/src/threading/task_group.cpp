#include "forge/threading/task_group.hpp"

namespace forge
{

TaskGroup::~TaskGroup()
{
    wait();
}

void TaskGroup::wait()
{
    std::unique_lock lock(mutex_);
    condition_.wait(lock, [this] { return pending_.load(std::memory_order_acquire) == 0; });
}

void TaskGroup::on_task_finished(bool failed) noexcept
{
    completed_.fetch_add(1, std::memory_order_relaxed);
    if (failed)
    {
        failed_.fetch_add(1, std::memory_order_relaxed);
    }

    // The lock pairs with the predicate check in wait(): without it a waiter
    // could test the predicate and block just after the final decrement,
    // missing the notification forever.
    std::size_t remaining;
    {
        std::scoped_lock lock(mutex_);
        remaining = pending_.fetch_sub(1, std::memory_order_acq_rel) - 1;
    }
    if (remaining == 0)
    {
        condition_.notify_all();
    }
}

} // namespace forge
