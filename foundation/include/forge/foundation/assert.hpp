#pragma once

// Assertion macros for programmer errors.
//
//   FORGE_ASSERT(condition)            checked only when FORGE_ASSERTS_ENABLED
//   FORGE_ASSERT_MSG(condition, msg)   same, with an explanation
//   FORGE_VERIFY(condition)            evaluated and checked in every build
//   FORGE_UNREACHABLE()                marks impossible code paths
//
// A failed assertion prints the expression, file, line and optional message
// to stderr and aborts. Assertions are for bugs, not for user input.

#include "forge/foundation/compiler.hpp"
#include "forge/foundation/config.hpp"

namespace forge::detail
{

[[noreturn]] void assert_fail(const char* expression,
                              const char* file,
                              int line,
                              const char* message) noexcept;

} // namespace forge::detail

#if FORGE_ASSERTS_ENABLED
    #define FORGE_ASSERT(condition)                                                  \
        do                                                                           \
        {                                                                            \
            if (!(condition)) [[unlikely]]                                           \
            {                                                                        \
                ::forge::detail::assert_fail(#condition, __FILE__, __LINE__, nullptr); \
            }                                                                        \
        } while (false)

    #define FORGE_ASSERT_MSG(condition, message)                                     \
        do                                                                           \
        {                                                                            \
            if (!(condition)) [[unlikely]]                                           \
            {                                                                        \
                ::forge::detail::assert_fail(#condition, __FILE__, __LINE__, message); \
            }                                                                        \
        } while (false)
#else
    #define FORGE_ASSERT(condition) static_cast<void>(0)
    #define FORGE_ASSERT_MSG(condition, message) static_cast<void>(0)
#endif

// Always evaluates and always checks, in every build configuration.
#define FORGE_VERIFY(condition)                                                      \
    do                                                                               \
    {                                                                                \
        if (!(condition)) [[unlikely]]                                               \
        {                                                                            \
            ::forge::detail::assert_fail(#condition, __FILE__, __LINE__,             \
                                         "FORGE_VERIFY failed");                     \
        }                                                                            \
    } while (false)

#define FORGE_UNREACHABLE() \
    ::forge::detail::assert_fail("unreachable code executed", __FILE__, __LINE__, nullptr)
