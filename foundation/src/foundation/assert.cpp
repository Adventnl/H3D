#include "forge/foundation/assert.hpp"

#include <cstdio>
#include <cstdlib>

namespace forge::detail
{

void assert_fail(const char* expression, const char* file, int line, const char* message) noexcept
{
    // Use stdio directly: the logger may itself be the code under suspicion,
    // and an assertion path must not allocate or lock more than necessary.
    std::fprintf(stderr,
                 "\n================ FORGE ASSERTION FAILED ================\n"
                 "Expression: %s\n"
                 "Location:   %s:%d\n",
                 expression != nullptr ? expression : "(null)",
                 file != nullptr ? file : "(unknown)",
                 line);
    if (message != nullptr)
    {
        std::fprintf(stderr, "Message:    %s\n", message);
    }
    std::fprintf(stderr, "========================================================\n");
    std::fflush(stderr);

    std::abort();
}

} // namespace forge::detail
