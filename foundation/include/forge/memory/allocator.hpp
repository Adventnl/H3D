#pragma once

// Abstract allocator interface. All Forge3D allocators implement this so
// systems can be written against the interface and re-targeted to arenas,
// pools or tracked heaps later without code changes.

#include <cstddef>
#include <memory>
#include <utility>

namespace forge
{

class Allocator
{
public:
    static constexpr std::size_t kDefaultAlignment = alignof(std::max_align_t);

    virtual ~Allocator() = default;

    /// Allocate `size` bytes aligned to `alignment` (a power of two).
    /// Returns nullptr on failure; never throws.
    [[nodiscard]] virtual void* allocate(std::size_t size,
                                         std::size_t alignment = kDefaultAlignment) = 0;

    /// Release memory previously returned by allocate() on the same
    /// allocator, with the same size and alignment. nullptr is a no-op.
    virtual void deallocate(void* pointer, std::size_t size,
                            std::size_t alignment = kDefaultAlignment) = 0;

    /// Allocate and construct a T. Returns nullptr if allocation fails.
    template <typename T, typename... Args>
    [[nodiscard]] T* create(Args&&... args)
    {
        void* memory = allocate(sizeof(T), alignof(T));
        if (memory == nullptr)
        {
            return nullptr;
        }
        return std::construct_at(static_cast<T*>(memory), std::forward<Args>(args)...);
    }

    /// Destroy and deallocate an object created with create<T>().
    template <typename T>
    void destroy(T* object)
    {
        if (object != nullptr)
        {
            std::destroy_at(object);
            deallocate(object, sizeof(T), alignof(T));
        }
    }
};

} // namespace forge
