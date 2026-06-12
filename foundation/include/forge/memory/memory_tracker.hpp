#pragma once

// Process-wide allocation statistics. All counters are atomic, so recording
// and reading are thread-safe; a snapshot of multiple counters is not a
// single atomic observation (acceptable for diagnostics).

#include <atomic>
#include <cstddef>
#include <cstdint>

#include "forge/foundation/noncopyable.hpp"

namespace forge
{

struct MemoryStats
{
    std::uint64_t allocation_count = 0;
    std::uint64_t deallocation_count = 0;
    std::uint64_t total_allocated_bytes = 0; // lifetime sum of all allocations
    std::uint64_t current_bytes = 0;
    std::uint64_t peak_bytes = 0;
};

class MemoryTracker : NonMovable
{
public:
    [[nodiscard]] static MemoryTracker& instance();

    void record_allocation(std::size_t bytes) noexcept;
    void record_deallocation(std::size_t bytes) noexcept;

    [[nodiscard]] std::uint64_t allocation_count() const noexcept;
    [[nodiscard]] std::uint64_t deallocation_count() const noexcept;
    [[nodiscard]] std::uint64_t total_allocated_bytes() const noexcept;
    [[nodiscard]] std::uint64_t current_bytes() const noexcept;
    [[nodiscard]] std::uint64_t peak_bytes() const noexcept;

    [[nodiscard]] MemoryStats stats() const noexcept;

    /// Reset all counters to zero (intended for tests).
    void reset() noexcept;

private:
    MemoryTracker() = default;

    std::atomic<std::uint64_t> allocation_count_{0};
    std::atomic<std::uint64_t> deallocation_count_{0};
    std::atomic<std::uint64_t> total_allocated_bytes_{0};
    std::atomic<std::uint64_t> current_bytes_{0};
    std::atomic<std::uint64_t> peak_bytes_{0};
};

} // namespace forge
