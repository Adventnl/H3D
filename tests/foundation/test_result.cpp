#include <string>

#include "forge/foundation/result.hpp"
#include "test_framework.hpp"

using forge::ErrorCode;
using forge::Result;

namespace
{

Result<int> parse_positive(int input)
{
    if (input <= 0)
    {
        return forge::failure(ErrorCode::InvalidArgument, "input must be positive");
    }
    return input;
}

Result<void> do_nothing_successfully()
{
    return forge::success();
}

} // namespace

FORGE_TEST_CASE("foundation.result_success")
{
    const Result<int> result = parse_positive(5);
    FORGE_REQUIRE(result.has_value());
    FORGE_CHECK_EQ(result.value(), 5);
}

FORGE_TEST_CASE("foundation.result_failure")
{
    const Result<int> result = parse_positive(-3);
    FORGE_REQUIRE(!result.has_value());
    FORGE_CHECK(result.error().code == ErrorCode::InvalidArgument);
    FORGE_CHECK_EQ(result.error().message, std::string("input must be positive"));
}

FORGE_TEST_CASE("foundation.result_void_success")
{
    const Result<void> result = do_nothing_successfully();
    FORGE_CHECK(result.has_value());
}

FORGE_TEST_CASE("foundation.error_to_string")
{
    const forge::Error error(ErrorCode::NotFound, "missing.txt");
    FORGE_CHECK_EQ(error.to_string(), std::string("NotFound: missing.txt"));
    FORGE_CHECK_EQ(std::string(forge::error_code_name(ErrorCode::IOError)),
                   std::string("IOError"));
}
