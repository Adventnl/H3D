#pragma once

// Small, general-purpose preprocessor utilities.

#define FORGE_STRINGIFY_IMPL(x) #x
#define FORGE_STRINGIFY(x) FORGE_STRINGIFY_IMPL(x)

#define FORGE_CONCAT_IMPL(a, b) a##b
#define FORGE_CONCAT(a, b) FORGE_CONCAT_IMPL(a, b)

// Unique identifier per expansion site, e.g. for RAII guard variables.
#define FORGE_ANONYMOUS_VARIABLE(prefix) FORGE_CONCAT(prefix, __COUNTER__)

// Explicitly discard a value or parameter.
#define FORGE_UNUSED(x) static_cast<void>(x)

// Bit with the given zero-based index.
#define FORGE_BIT(index) (1ull << (index))

// Number of elements in a C array (compile-time, type-safe).
#include <cstddef>
namespace forge::detail
{
template <typename T, std::size_t N>
constexpr std::size_t array_count(const T (&)[N]) noexcept
{
    return N;
}
} // namespace forge::detail
#define FORGE_ARRAY_COUNT(array) (::forge::detail::array_count(array))
