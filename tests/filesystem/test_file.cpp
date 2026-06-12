#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

#include "forge/filesystem/file.hpp"
#include "forge/filesystem/virtual_filesystem.hpp"
#include "forge/foundation/scope_exit.hpp"
#include "test_framework.hpp"

using forge::ErrorCode;
using forge::Path;

namespace
{

/// Fresh scratch directory under the system temp directory.
Path make_test_directory(const char* name)
{
    const Path directory = Path::temp_directory() / "forge3d_tests" / name;
    std::filesystem::remove_all(directory.raw());
    std::filesystem::create_directories(directory.raw());
    return directory;
}

void remove_test_directory(const Path& directory)
{
    std::error_code ignored;
    std::filesystem::remove_all(directory.raw(), ignored);
}

} // namespace

FORGE_TEST_CASE("filesystem.write_and_read_text_file")
{
    const Path directory = make_test_directory("text_io");
    FORGE_SCOPE_EXIT(remove_test_directory(directory));
    const Path file = directory / "sample.txt";

    const std::string content = "Forge3D\nline two\n";
    FORGE_REQUIRE(forge::write_text_file(file, content).has_value());

    const auto read_back = forge::read_text_file(file);
    FORGE_REQUIRE(read_back.has_value());
    FORGE_CHECK_EQ(*read_back, content);

    const auto size = forge::file_size(file);
    FORGE_REQUIRE(size.has_value());
    FORGE_CHECK_EQ(*size, static_cast<std::uint64_t>(content.size()));
}

FORGE_TEST_CASE("filesystem.append_text_file")
{
    const Path directory = make_test_directory("append_io");
    FORGE_SCOPE_EXIT(remove_test_directory(directory));
    const Path file = directory / "log.txt";

    FORGE_REQUIRE(forge::write_text_file(file, "first").has_value());
    FORGE_REQUIRE(forge::append_text_file(file, " second").has_value());

    const auto content = forge::read_text_file(file);
    FORGE_REQUIRE(content.has_value());
    FORGE_CHECK_EQ(*content, std::string("first second"));
}

FORGE_TEST_CASE("filesystem.write_and_read_binary_file")
{
    const Path directory = make_test_directory("binary_io");
    FORGE_SCOPE_EXIT(remove_test_directory(directory));
    const Path file = directory / "blob.bin";

    std::vector<std::byte> payload;
    for (int index = 0; index < 256; ++index)
    {
        payload.push_back(static_cast<std::byte>(index));
    }
    FORGE_REQUIRE(forge::write_binary_file(file, payload).has_value());

    const auto read_back = forge::read_binary_file(file);
    FORGE_REQUIRE(read_back.has_value());
    FORGE_CHECK_EQ(read_back->size(), payload.size());
    FORGE_CHECK(*read_back == payload);
}

FORGE_TEST_CASE("filesystem.read_missing_file_returns_not_found")
{
    const auto result = forge::read_text_file(Path("definitely/missing/file.txt"));
    FORGE_REQUIRE(!result.has_value());
    FORGE_CHECK(result.error().code == ErrorCode::NotFound);
}

FORGE_TEST_CASE("filesystem.create_directories")
{
    const Path directory = make_test_directory("mkdir");
    FORGE_SCOPE_EXIT(remove_test_directory(directory));

    const Path nested = directory / "a" / "b" / "c";
    FORGE_REQUIRE(forge::create_directories(nested).has_value());
    FORGE_CHECK(nested.is_directory());

    // Creating an existing directory succeeds.
    FORGE_CHECK(forge::create_directories(nested).has_value());
}

FORGE_TEST_CASE("filesystem.vfs_mount_resolve_read_write")
{
    const Path directory = make_test_directory("vfs");
    FORGE_SCOPE_EXIT(remove_test_directory(directory));

    forge::VirtualFileSystem vfs;
    FORGE_REQUIRE(vfs.mount("assets", directory).has_value());
    FORGE_CHECK(vfs.is_mounted("assets"));

    // Write through the mount (parent directories are created on demand).
    FORGE_REQUIRE(vfs.write_text("assets:/configs/default.txt", "hello vfs").has_value());

    const auto resolved = vfs.resolve("assets:/configs/default.txt");
    FORGE_REQUIRE(resolved.has_value());
    FORGE_CHECK(resolved->is_file());

    const auto text = vfs.read_text("assets:/configs/default.txt");
    FORGE_REQUIRE(text.has_value());
    FORGE_CHECK_EQ(*text, std::string("hello vfs"));

    const std::vector<std::byte> blob{std::byte{1}, std::byte{2}, std::byte{3}};
    FORGE_REQUIRE(vfs.write_binary("assets:/blobs/data.bin", blob).has_value());
    const auto binary = vfs.read_binary("assets:/blobs/data.bin");
    FORGE_REQUIRE(binary.has_value());
    FORGE_CHECK(*binary == blob);
}

FORGE_TEST_CASE("filesystem.vfs_rejects_path_traversal")
{
    const Path directory = make_test_directory("vfs_security");
    FORGE_SCOPE_EXIT(remove_test_directory(directory));

    forge::VirtualFileSystem vfs;
    FORGE_REQUIRE(vfs.mount("assets", directory).has_value());

    const auto escape = vfs.resolve("assets:/../secrets.txt");
    FORGE_REQUIRE(!escape.has_value());
    FORGE_CHECK(escape.error().code == ErrorCode::PermissionDenied);

    const auto nested_escape = vfs.resolve("assets:/sub/../../secrets.txt");
    FORGE_CHECK(!nested_escape.has_value());

    const auto unknown_alias = vfs.resolve("nope:/file.txt");
    FORGE_REQUIRE(!unknown_alias.has_value());
    FORGE_CHECK(unknown_alias.error().code == ErrorCode::NotFound);

    const auto malformed = vfs.resolve("no-alias-here");
    FORGE_REQUIRE(!malformed.has_value());
    FORGE_CHECK(malformed.error().code == ErrorCode::InvalidArgument);
}

FORGE_TEST_CASE("filesystem.vfs_mount_validation")
{
    forge::VirtualFileSystem vfs;

    // Aliases with separators are rejected.
    const Path temp = Path::temp_directory();
    FORGE_CHECK(!vfs.mount("bad/alias", temp).has_value());
    FORGE_CHECK(!vfs.mount("", temp).has_value());

    // Mount roots must exist.
    FORGE_CHECK(!vfs.mount("ghost", Path("missing/root/dir")).has_value());

    // Duplicate mounts are rejected.
    FORGE_REQUIRE(vfs.mount("temp", temp).has_value());
    const auto duplicate = vfs.mount("temp", temp);
    FORGE_REQUIRE(!duplicate.has_value());
    FORGE_CHECK(duplicate.error().code == ErrorCode::AlreadyExists);

    FORGE_CHECK(vfs.unmount("temp").has_value());
    FORGE_CHECK(!vfs.is_mounted("temp"));
}
