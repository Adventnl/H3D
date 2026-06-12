#include "forge/threading/thread_pool.hpp"

namespace forge
{

ThreadPool::ThreadPool(std::size_t worker_count)
{
    if (worker_count == 0)
    {
        worker_count = std::thread::hardware_concurrency();
        if (worker_count == 0)
        {
            worker_count = 1;
        }
    }

    workers_.reserve(worker_count);
    for (std::size_t index = 0; index < worker_count; ++index)
    {
        workers_.emplace_back([this] { worker_loop(); });
    }
}

ThreadPool::~ThreadPool()
{
    shutdown();
}

void ThreadPool::shutdown()
{
    {
        std::scoped_lock lock(mutex_);
        if (stopping_)
        {
            // Already shutting down; the first caller joins the workers.
        }
        stopping_ = true;
    }
    condition_.notify_all();

    for (std::thread& worker : workers_)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }
    workers_.clear();
}

std::size_t ThreadPool::pending_count() const
{
    std::scoped_lock lock(mutex_);
    return queue_.size();
}

bool ThreadPool::is_running() const
{
    std::scoped_lock lock(mutex_);
    return !stopping_;
}

bool ThreadPool::try_enqueue(UniqueFunction&& job)
{
    {
        std::scoped_lock lock(mutex_);
        if (stopping_)
        {
            return false;
        }
        queue_.push_back(std::move(job));
    }
    condition_.notify_one();
    return true;
}

void ThreadPool::worker_loop()
{
    for (;;)
    {
        UniqueFunction job;
        {
            std::unique_lock lock(mutex_);
            condition_.wait(lock, [this] { return stopping_ || !queue_.empty(); });

            if (queue_.empty())
            {
                // stopping_ must be true here; the queue is drained, so exit.
                return;
            }
            job = std::move(queue_.front());
            queue_.pop_front();
        }
        job();
    }
}

} // namespace forge
