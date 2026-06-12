#include "forge/memory/linear_allocator.hpp"

#include <cstdint>

#include "forge/foundation/assert.hpp"

namespace forge
{

LinearAllocator::LinearAllocator(std::size_t capacity_bytes)
    : buffer_(std::make_unique<std::byte[]>(capacity_bytes)), capacity_(capacity_bytes)
{
    FORGE_ASSERT_MSG(capacity_bytes > 0, "LinearAllocator capacity must be non-zero");
}

void* LinearAllocator::allocate(std::size_t size, std::size_t alignment)
{
    FORGE_ASSERT_MSG(alignment != 0 && (alignment & (alignment - 1)) == 0,
                     "alignment must be a power of two");
    if (size == 0)
    {
        size = 1;
    }

    const std::uintptr_t base = reinterpret_cast<std::uintptr_t>(buffer_.get());
    const std::uintptr_t current = base + offset_;
    const std::uintptr_t mask = static_cast<std::uintptr_t>(alignment - 1);
    const std::uintptr_t aligned = (current + mask) & ~mask;

    const std::size_t end_offset = static_cast<std::size_t>(aligned - base) + size;
    if (end_offset > capacity_)
    {
        return nullptr;
    }

    offset_ = end_offset;
    ++allocation_count_;
    return reinterpret_cast<void*>(aligned);
}

void LinearAllocator::deallocate(void* pointer, std::size_t size, std::size_t alignment)
{
    // Individual deallocation is intentionally a no-op; reset() reclaims
    // the whole buffer at once.
    static_cast<void>(pointer);
    static_cast<void>(size);
    static_cast<void>(alignment);
}

void LinearAllocator::reset() noexcept
{
    offset_ = 0;
    allocation_count_ = 0;
}

} // namespace forge
