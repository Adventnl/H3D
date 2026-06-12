#include <cstdint>

#include "forge/memory/linear_allocator.hpp"
#include "test_framework.hpp"

using forge::LinearAllocator;

FORGE_TEST_CASE("memory.linear_allocator_sequential_allocation")
{
    LinearAllocator allocator(1024);
    FORGE_CHECK_EQ(allocator.capacity(), static_cast<std::size_t>(1024));
    FORGE_CHECK_EQ(allocator.used_bytes(), static_cast<std::size_t>(0));

    void* first = allocator.allocate(100, 8);
    FORGE_REQUIRE(first != nullptr);
    FORGE_CHECK(allocator.used_bytes() >= 100);

    void* second = allocator.allocate(50, 8);
    FORGE_REQUIRE(second != nullptr);
    FORGE_CHECK(second != first);
    FORGE_CHECK_EQ(allocator.allocation_count(), static_cast<std::size_t>(2));

    // Sequential allocations move forward through the buffer.
    FORGE_CHECK(reinterpret_cast<std::uintptr_t>(second) >
                reinterpret_cast<std::uintptr_t>(first));
}

FORGE_TEST_CASE("memory.linear_allocator_respects_alignment")
{
    LinearAllocator allocator(1024);

    allocator.allocate(3, 1); // deliberately misalign the offset
    void* aligned = allocator.allocate(16, 64);
    FORGE_REQUIRE(aligned != nullptr);
    FORGE_CHECK_EQ(reinterpret_cast<std::uintptr_t>(aligned) % 64,
                   static_cast<std::uintptr_t>(0));
}

FORGE_TEST_CASE("memory.linear_allocator_reset")
{
    LinearAllocator allocator(256);
    FORGE_REQUIRE(allocator.allocate(200, 8) != nullptr);
    FORGE_CHECK(allocator.used_bytes() >= 200);

    allocator.reset();
    FORGE_CHECK_EQ(allocator.used_bytes(), static_cast<std::size_t>(0));
    FORGE_CHECK_EQ(allocator.allocation_count(), static_cast<std::size_t>(0));

    // The full capacity is available again.
    FORGE_CHECK(allocator.allocate(200, 8) != nullptr);
}

FORGE_TEST_CASE("memory.linear_allocator_exhaustion_returns_null")
{
    LinearAllocator allocator(128);
    FORGE_CHECK(allocator.allocate(100, 8) != nullptr);
    FORGE_CHECK(allocator.allocate(100, 8) == nullptr); // does not fit
    // A failed allocation must not corrupt the allocator.
    FORGE_CHECK(allocator.allocate(8, 8) != nullptr);
}
