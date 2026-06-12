#pragma once

// Atomic aliases and a minimal spin lock for very short critical sections.

#include <atomic>
#include <cstdint>
#include <thread>

#include "forge/foundation/noncopyable.hpp"

namespace forge
{

using AtomicBool = std::atomic<bool>;
using AtomicI32 = std::atomic<std::int32_t>;
using AtomicU32 = std::atomic<std::uint32_t>;
using AtomicI64 = std::atomic<std::int64_t>;
using AtomicU64 = std::atomic<std::uint64_t>;
using AtomicSize = std::atomic<std::size_t>;

/// Busy-wait mutex. Only appropriate when the protected region is a handful
/// of instructions; otherwise use std::mutex. Satisfies the Lockable
/// requirements, so it works with std::scoped_lock.
class SpinLock : NonMovable
{
public:
    SpinLock() = default;

    void lock() noexcept
    {
        while (flag_.test_and_set(std::memory_order_acquire))
        {
            std::this_thread::yield();
        }
    }

    [[nodiscard]] bool try_lock() noexcept
    {
        return !flag_.test_and_set(std::memory_order_acquire);
    }

    void unlock() noexcept
    {
        flag_.clear(std::memory_order_release);
    }

private:
    std::atomic_flag flag_;
};

} // namespace forge
