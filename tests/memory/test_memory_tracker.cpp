#include "forge/memory/memory_tracker.hpp"
#include "forge/memory/system_allocator.hpp"
#include "test_framework.hpp"

using forge::MemoryTracker;

FORGE_TEST_CASE("memory.tracker_counts_allocations")
{
    MemoryTracker& tracker = MemoryTracker::instance();
    const std::uint64_t allocations_before = tracker.allocation_count();
    const std::uint64_t current_before = tracker.current_bytes();

    tracker.record_allocation(256);
    FORGE_CHECK_EQ(tracker.allocation_count(), allocations_before + 1);
    FORGE_CHECK_EQ(tracker.current_bytes(), current_before + 256);
    FORGE_CHECK(tracker.peak_bytes() >= current_before + 256);

    tracker.record_deallocation(256);
    FORGE_CHECK_EQ(tracker.current_bytes(), current_before);
    FORGE_CHECK_EQ(tracker.deallocation_count() > 0, true);
}

FORGE_TEST_CASE("memory.tracker_peak_tracks_high_water_mark")
{
    MemoryTracker& tracker = MemoryTracker::instance();

    tracker.record_allocation(10'000);
    const std::uint64_t peak_at_high = tracker.peak_bytes();
    tracker.record_deallocation(10'000);

    // Peak must not decrease when memory is freed.
    FORGE_CHECK(tracker.peak_bytes() >= peak_at_high);
}

FORGE_TEST_CASE("memory.system_allocator_allocates_and_tracks")
{
    forge::SystemAllocator& allocator = forge::system_allocator();
    MemoryTracker& tracker = MemoryTracker::instance();

    const std::uint64_t allocations_before = tracker.allocation_count();
    const std::uint64_t current_before = tracker.current_bytes();

    void* memory = allocator.allocate(512, 32);
    FORGE_REQUIRE(memory != nullptr);
    FORGE_CHECK_EQ(reinterpret_cast<std::uintptr_t>(memory) % 32,
                   static_cast<std::uintptr_t>(0));
    FORGE_CHECK_EQ(tracker.allocation_count(), allocations_before + 1);
    FORGE_CHECK_EQ(tracker.current_bytes(), current_before + 512);

    allocator.deallocate(memory, 512, 32);
    FORGE_CHECK_EQ(tracker.current_bytes(), current_before);
}

FORGE_TEST_CASE("memory.system_allocator_create_destroy")
{
    struct Sample
    {
        int value;
        explicit Sample(int v) : value(v) {}
    };

    forge::SystemAllocator& allocator = forge::system_allocator();
    Sample* object = allocator.create<Sample>(41);
    FORGE_REQUIRE(object != nullptr);
    FORGE_CHECK_EQ(object->value, 41);
    allocator.destroy(object);
}
