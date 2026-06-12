#pragma once

// Bump allocator over a single fixed buffer.
//
// allocate() advances an offset; deallocate() is a no-op; reset() reclaims
// everything at once. Ideal for per-frame or per-task scratch memory.
//
// Not thread-safe: each thread should own its own LinearAllocator, or
// callers must synchronize externally.

#include <cstddef>
#include <memory>

#include "forge/foundation/noncopyable.hpp"
#include "forge/memory/allocator.hpp"

namespace forge
{

class LinearAllocator final : public Allocator, NonCopyable
{
public:
    /// Creates an allocator owning a buffer of `capacity_bytes`.
    explicit LinearAllocator(std::size_t capacity_bytes);

    /// Returns nullptr when the remaining space cannot satisfy the request.
    [[nodiscard]] void* allocate(std::size_t size,
                                 std::size_t alignment = kDefaultAlignment) override;

    /// No-op: linear allocations are reclaimed in bulk by reset().
    void deallocate(void* pointer, std::size_t size,
                    std::size_t alignment = kDefaultAlignment) override;

    /// Reclaim the entire buffer. Pointers handed out earlier become invalid.
    void reset() noexcept;

    [[nodiscard]] std::size_t used_bytes() const noexcept { return offset_; }
    [[nodiscard]] std::size_t capacity() const noexcept { return capacity_; }
    [[nodiscard]] std::size_t allocation_count() const noexcept { return allocation_count_; }

private:
    std::unique_ptr<std::byte[]> buffer_;
    std::size_t capacity_ = 0;
    std::size_t offset_ = 0;
    std::size_t allocation_count_ = 0;
};

} // namespace forge
