#include "forge/memory/memory_tracker.hpp"

namespace forge
{

MemoryTracker& MemoryTracker::instance()
{
    static MemoryTracker tracker;
    return tracker;
}

void MemoryTracker::record_allocation(std::size_t bytes) noexcept
{
    allocation_count_.fetch_add(1, std::memory_order_relaxed);
    total_allocated_bytes_.fetch_add(bytes, std::memory_order_relaxed);

    const std::uint64_t current =
        current_bytes_.fetch_add(bytes, std::memory_order_relaxed) + bytes;

    // Lock-free peak update.
    std::uint64_t peak = peak_bytes_.load(std::memory_order_relaxed);
    while (current > peak &&
           !peak_bytes_.compare_exchange_weak(peak, current, std::memory_order_relaxed))
    {
    }
}

void MemoryTracker::record_deallocation(std::size_t bytes) noexcept
{
    deallocation_count_.fetch_add(1, std::memory_order_relaxed);
    current_bytes_.fetch_sub(bytes, std::memory_order_relaxed);
}

std::uint64_t MemoryTracker::allocation_count() const noexcept
{
    return allocation_count_.load(std::memory_order_relaxed);
}

std::uint64_t MemoryTracker::deallocation_count() const noexcept
{
    return deallocation_count_.load(std::memory_order_relaxed);
}

std::uint64_t MemoryTracker::total_allocated_bytes() const noexcept
{
    return total_allocated_bytes_.load(std::memory_order_relaxed);
}

std::uint64_t MemoryTracker::current_bytes() const noexcept
{
    return current_bytes_.load(std::memory_order_relaxed);
}

std::uint64_t MemoryTracker::peak_bytes() const noexcept
{
    return peak_bytes_.load(std::memory_order_relaxed);
}

MemoryStats MemoryTracker::stats() const noexcept
{
    return MemoryStats{
        .allocation_count = allocation_count(),
        .deallocation_count = deallocation_count(),
        .total_allocated_bytes = total_allocated_bytes(),
        .current_bytes = current_bytes(),
        .peak_bytes = peak_bytes(),
    };
}

void MemoryTracker::reset() noexcept
{
    allocation_count_.store(0, std::memory_order_relaxed);
    deallocation_count_.store(0, std::memory_order_relaxed);
    total_allocated_bytes_.store(0, std::memory_order_relaxed);
    current_bytes_.store(0, std::memory_order_relaxed);
    peak_bytes_.store(0, std::memory_order_relaxed);
}

} // namespace forge
