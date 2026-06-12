#include "forge/memory/system_allocator.hpp"

#include <new>

#include "forge/foundation/assert.hpp"
#include "forge/memory/memory_tracker.hpp"

namespace forge
{
namespace
{

// Both allocate and deallocate must agree on the effective alignment passed
// to operator new/delete, so the normalization lives in one place.
std::size_t normalize_alignment(std::size_t alignment) noexcept
{
    return alignment < alignof(void*) ? alignof(void*) : alignment;
}

} // namespace

void* SystemAllocator::allocate(std::size_t size, std::size_t alignment)
{
    FORGE_ASSERT_MSG(alignment != 0 && (alignment & (alignment - 1)) == 0,
                     "alignment must be a power of two");
    if (size == 0)
    {
        size = 1;
    }

    const std::size_t effective_alignment = normalize_alignment(alignment);
    void* pointer = ::operator new(size, std::align_val_t{effective_alignment},
                                   std::nothrow);
    if (pointer != nullptr)
    {
        MemoryTracker::instance().record_allocation(size);
    }
    return pointer;
}

void SystemAllocator::deallocate(void* pointer, std::size_t size, std::size_t alignment)
{
    if (pointer == nullptr)
    {
        return;
    }
    if (size == 0)
    {
        size = 1;
    }

    const std::size_t effective_alignment = normalize_alignment(alignment);
    ::operator delete(pointer, std::align_val_t{effective_alignment});
    MemoryTracker::instance().record_deallocation(size);
}

SystemAllocator& system_allocator()
{
    static SystemAllocator allocator;
    return allocator;
}

} // namespace forge
