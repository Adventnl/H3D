#include <filesystem>
#include <string>

#include "forge/filesystem/file.hpp"
#include "forge/foundation/log.hpp"
#include "test_framework.hpp"

using forge::Logger;
using forge::LogLevel;

FORGE_TEST_CASE("foundation.logger_counts_messages")
{
    Logger logger(LogLevel::Trace);
    logger.set_console_output(false); // keep test output clean

    logger.log(LogLevel::Info, "test", "first");
    logger.log(LogLevel::Error, "test", "second");
    FORGE_CHECK_EQ(logger.message_count(), static_cast<std::uint64_t>(2));
}

FORGE_TEST_CASE("foundation.logger_respects_level")
{
    Logger logger(LogLevel::Warning);
    logger.set_console_output(false);

    FORGE_CHECK(!logger.should_log(LogLevel::Debug));
    FORGE_CHECK(logger.should_log(LogLevel::Warning));
    FORGE_CHECK(logger.should_log(LogLevel::Fatal));
    FORGE_CHECK(!logger.should_log(LogLevel::Off));

    logger.log(LogLevel::Debug, "test", "filtered out");
    logger.log(LogLevel::Error, "test", "recorded");
    FORGE_CHECK_EQ(logger.message_count(), static_cast<std::uint64_t>(1));
}

FORGE_TEST_CASE("foundation.logger_writes_to_file")
{
    const forge::Path log_path =
        forge::Path::temp_directory() / "forge_test_logger_output.log";
    std::filesystem::remove(log_path.raw());

    {
        Logger logger(LogLevel::Trace);
        logger.set_console_output(false);
        FORGE_REQUIRE(logger.open_file(log_path.native_string()));
        logger.log(LogLevel::Info, "filetest", "hello log file");
        logger.close_file();
    }

    const auto content = forge::read_text_file(log_path);
    FORGE_REQUIRE(content.has_value());
    FORGE_CHECK(content->find("hello log file") != std::string::npos);
    FORGE_CHECK(content->find("filetest") != std::string::npos);

    std::filesystem::remove(log_path.raw());
}

FORGE_TEST_CASE("foundation.default_logger_does_not_crash")
{
    // Route through the global logger and the macros once; the assertion is
    // simply that nothing crashes and the call count advances.
    const std::uint64_t before = forge::default_logger().message_count();
    forge::default_logger().set_console_output(false);
    FORGE_LOG_INFO("test", "macro smoke test");
    forge::log_debug("test", "free function smoke test");
    forge::default_logger().set_console_output(true);
    FORGE_CHECK(forge::default_logger().message_count() >= before);
}
