#include <cstdint>

#include "bench_framework.hpp"
#include "forge/memory/linear_allocator.hpp"
#include "forge/memory/system_allocator.hpp"

using forge::LinearAllocator;
using forge::bench::do_not_optimize;

FORGE_BENCHMARK("memory.linear_allocator_allocate_reset")
{
    // 1 MiB arena; reset whenever it fills, mimicking per-frame usage.
    LinearAllocator allocator(1024 * 1024);
    for (std::uint64_t index = 0; index < state.iterations(); ++index)
    {
        void* memory = allocator.allocate(64, 16);
        if (memory == nullptr)
        {
            allocator.reset();
            memory = allocator.allocate(64, 16);
        }
        do_not_optimize(memory);
    }
}

FORGE_BENCHMARK("memory.system_allocator_alloc_free")
{
    forge::SystemAllocator& allocator = forge::system_allocator();
    for (std::uint64_t index = 0; index < state.iterations(); ++index)
    {
        void* memory = allocator.allocate(64, 16);
        do_not_optimize(memory);
        allocator.deallocate(memory, 64, 16);
    }
}
