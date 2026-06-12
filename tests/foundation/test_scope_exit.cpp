#include "forge/foundation/scope_exit.hpp"
#include "test_framework.hpp"

FORGE_TEST_CASE("foundation.scope_exit_runs_on_scope_end")
{
    int counter = 0;
    {
        auto guard = forge::make_scope_exit([&counter] { ++counter; });
        FORGE_CHECK(guard.is_active());
        FORGE_CHECK_EQ(counter, 0);
    }
    FORGE_CHECK_EQ(counter, 1);
}

FORGE_TEST_CASE("foundation.scope_exit_release_dismisses")
{
    int counter = 0;
    {
        auto guard = forge::make_scope_exit([&counter] { ++counter; });
        guard.release();
        FORGE_CHECK(!guard.is_active());
    }
    FORGE_CHECK_EQ(counter, 0);
}

FORGE_TEST_CASE("foundation.scope_exit_macro")
{
    int counter = 0;
    {
        FORGE_SCOPE_EXIT(++counter);
        FORGE_SCOPE_EXIT(counter += 10);
    }
    FORGE_CHECK_EQ(counter, 11);
}
