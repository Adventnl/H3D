#include <string>
#include <utility>

#include "forge/foundation/expected.hpp"
#include "test_framework.hpp"

using forge::Expected;
using forge::Unexpected;

FORGE_TEST_CASE("foundation.expected_holds_value")
{
    Expected<int, std::string> expected(42);
    FORGE_REQUIRE(expected.has_value());
    FORGE_CHECK(static_cast<bool>(expected));
    FORGE_CHECK_EQ(expected.value(), 42);
    FORGE_CHECK_EQ(*expected, 42);
}

FORGE_TEST_CASE("foundation.expected_holds_error")
{
    Expected<int, std::string> expected = Unexpected<std::string>("broken");
    FORGE_REQUIRE(!expected.has_value());
    FORGE_CHECK_EQ(expected.error(), std::string("broken"));
}

FORGE_TEST_CASE("foundation.expected_value_or")
{
    const Expected<int, std::string> good(7);
    const Expected<int, std::string> bad = Unexpected<std::string>("nope");
    FORGE_CHECK_EQ(good.value_or(-1), 7);
    FORGE_CHECK_EQ(bad.value_or(-1), -1);
}

FORGE_TEST_CASE("foundation.expected_copy_and_move")
{
    Expected<std::string, int> original(std::string("payload"));
    Expected<std::string, int> copy = original;
    FORGE_REQUIRE(copy.has_value());
    FORGE_CHECK_EQ(copy.value(), std::string("payload"));

    Expected<std::string, int> moved = std::move(original);
    FORGE_REQUIRE(moved.has_value());
    FORGE_CHECK_EQ(moved.value(), std::string("payload"));
}

FORGE_TEST_CASE("foundation.expected_assignment_switches_state")
{
    Expected<std::string, int> expected(std::string("value"));
    expected = Unexpected<int>(13);
    FORGE_REQUIRE(!expected.has_value());
    FORGE_CHECK_EQ(expected.error(), 13);

    expected = Expected<std::string, int>(std::string("again"));
    FORGE_REQUIRE(expected.has_value());
    FORGE_CHECK_EQ(expected.value(), std::string("again"));
}

FORGE_TEST_CASE("foundation.expected_void_specialization")
{
    Expected<void, std::string> ok;
    FORGE_CHECK(ok.has_value());

    Expected<void, std::string> bad = Unexpected<std::string>("io failure");
    FORGE_REQUIRE(!bad.has_value());
    FORGE_CHECK_EQ(bad.error(), std::string("io failure"));
}
