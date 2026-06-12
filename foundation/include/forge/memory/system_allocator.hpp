#pragma once

// General-purpose heap allocator wrapping aligned operator new/delete.
// Every allocation and deallocation is reported to the MemoryTracker.

#include "forge/memory/allocator.hpp"

namespace forge
{

class SystemAllocator final : public Allocator
{
public:
    SystemAllocator() = default;

    [[nodiscard]] void* allocate(std::size_t size,
                                 std::size_t alignment = kDefaultAlignment) override;

    void deallocate(void* pointer, std::size_t size,
                    std::size_t alignment = kDefaultAlignment) override;
};

/// Shared process-wide system allocator instance.
[[nodiscard]] SystemAllocator& system_allocator();

} // namespace forge
