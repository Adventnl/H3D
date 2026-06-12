#pragma once

// Forge3D's internal unit-test framework.
//
//   FORGE_TEST_CASE("math.vec3_cross")
//   {
//       FORGE_CHECK_NEAR(cross(a, b).x, 1.0f, 1e-6f);
//       FORGE_REQUIRE(result.has_value());  // aborts this test on failure
//   }
//
// Tests self-register at static-initialization time. The runner prints each
// failure with its file/line and returns the number of failed tests.

#include <cmath>
#include <format>
#include <string>
#include <string_view>

#include "forge/foundation/macros.hpp"

namespace forge::test
{

using TestFunction = void (*)();

/// Register a test; invoked via FORGE_TEST_CASE. Returns true so it can
/// initialize a namespace-scope constant.
bool register_test(std::string name, TestFunction function, const char* file, int line);

/// Thrown by FORGE_REQUIRE to abort the current test only.
struct RequireFailure
{
};

/// Record a check outcome; prints location and details on failure.
void report_check(bool passed, const char* expression, const char* file, int line,
                  const std::string& details = {});

/// Run every registered test whose name contains `filter` (all when empty).
/// Returns the number of failing tests.
int run_all_tests(std::string_view filter = {});

/// Print the registered test names to stdout.
void list_tests();

namespace detail
{

template <typename Value>
std::string format_value(const Value& value)
{
    if constexpr (requires { std::format("{}", value); })
    {
        return std::format("{}", value);
    }
    else
    {
        return "<unformattable>";
    }
}

template <typename Left, typename Right>
void check_equal(const Left& left, const Right& right, const char* expression,
                 const char* file, int line)
{
    const bool passed = (left == right);
    std::string details;
    if (!passed)
    {
        details = std::format("left = {}, right = {}", format_value(left),
                              format_value(right));
    }
    report_check(passed, expression, file, line, details);
}

inline void check_near(double left, double right, double epsilon, const char* expression,
                       const char* file, int line)
{
    const bool passed = std::abs(left - right) <= epsilon;
    std::string details;
    if (!passed)
    {
        details = std::format("left = {}, right = {}, epsilon = {}", left, right, epsilon);
    }
    report_check(passed, expression, file, line, details);
}

} // namespace detail
} // namespace forge::test

#define FORGE_TEST_CASE_IMPL(name, function_name, registrar_name)                       \
    static void function_name();                                                         \
    [[maybe_unused]] static const bool registrar_name =                                  \
        ::forge::test::register_test(name, &function_name, __FILE__, __LINE__);         \
    static void function_name()

#define FORGE_TEST_CASE_EXPAND(name, id) \
    FORGE_TEST_CASE_IMPL(name, FORGE_CONCAT(forge_test_function_, id), \
                         FORGE_CONCAT(forge_test_registrar_, id))

#define FORGE_TEST_CASE(name) FORGE_TEST_CASE_EXPAND(name, __COUNTER__)

#define FORGE_CHECK(condition) \
    ::forge::test::report_check(static_cast<bool>(condition), #condition, __FILE__, __LINE__)

#define FORGE_REQUIRE(condition)                                                          \
    do                                                                                    \
    {                                                                                     \
        const bool forge_require_result = static_cast<bool>(condition);                  \
        ::forge::test::report_check(forge_require_result, #condition, __FILE__, __LINE__); \
        if (!forge_require_result)                                                        \
        {                                                                                 \
            throw ::forge::test::RequireFailure{};                                       \
        }                                                                                 \
    } while (false)

#define FORGE_CHECK_EQ(left, right) \
    ::forge::test::detail::check_equal((left), (right), #left " == " #right, __FILE__, __LINE__)

#define FORGE_CHECK_NEAR(left, right, epsilon)                                  \
    ::forge::test::detail::check_near(static_cast<double>(left),                \
                                      static_cast<double>(right),               \
                                      static_cast<double>(epsilon),             \
                                      #left " ~= " #right, __FILE__, __LINE__)
