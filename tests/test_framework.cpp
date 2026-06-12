#include "test_framework.hpp"

#include <cstdio>
#include <exception>
#include <vector>

namespace forge::test
{
namespace
{

struct TestCase
{
    std::string name;
    TestFunction function = nullptr;
    const char* file = nullptr;
    int line = 0;
};

std::vector<TestCase>& registry()
{
    // Function-local static avoids static initialization order problems
    // between this vector and the per-test registrar constants.
    static std::vector<TestCase> tests;
    return tests;
}

// Failures recorded for the test currently executing.
int g_current_test_failures = 0;

} // namespace

bool register_test(std::string name, TestFunction function, const char* file, int line)
{
    registry().push_back(TestCase{std::move(name), function, file, line});
    return true;
}

void report_check(bool passed, const char* expression, const char* file, int line,
                  const std::string& details)
{
    if (passed)
    {
        return;
    }
    ++g_current_test_failures;
    std::printf("    FAILED: %s\n        at %s:%d\n", expression, file, line);
    if (!details.empty())
    {
        std::printf("        %s\n", details.c_str());
    }
}

int run_all_tests(std::string_view filter)
{
    int executed = 0;
    int failed = 0;

    for (const TestCase& test : registry())
    {
        if (!filter.empty() && test.name.find(filter) == std::string::npos)
        {
            continue;
        }
        ++executed;

        std::printf("[ RUN  ] %s\n", test.name.c_str());
        g_current_test_failures = 0;

        try
        {
            test.function();
        }
        catch (const RequireFailure&)
        {
            // Already reported by FORGE_REQUIRE; the test simply stops here.
        }
        catch (const std::exception& exception)
        {
            ++g_current_test_failures;
            std::printf("    FAILED: unexpected exception: %s\n", exception.what());
        }
        catch (...)
        {
            ++g_current_test_failures;
            std::printf("    FAILED: unexpected non-standard exception\n");
        }

        if (g_current_test_failures == 0)
        {
            std::printf("[ PASS ] %s\n", test.name.c_str());
        }
        else
        {
            ++failed;
            std::printf("[ FAIL ] %s (%d failed check%s)\n", test.name.c_str(),
                        g_current_test_failures, g_current_test_failures == 1 ? "" : "s");
        }
    }

    std::printf("\n%d test%s run, %d passed, %d failed\n", executed,
                executed == 1 ? "" : "s", executed - failed, failed);
    if (executed == 0 && !filter.empty())
    {
        std::printf("warning: filter '%.*s' matched no tests\n",
                    static_cast<int>(filter.size()), filter.data());
        return 1;
    }
    return failed;
}

void list_tests()
{
    for (const TestCase& test : registry())
    {
        std::printf("%s\n", test.name.c_str());
    }
}

} // namespace forge::test
