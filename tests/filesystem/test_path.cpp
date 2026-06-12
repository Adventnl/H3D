#include <string>

#include "forge/filesystem/path.hpp"
#include "test_framework.hpp"

using forge::Path;

FORGE_TEST_CASE("filesystem.path_join")
{
    const Path joined = Path("assets") / Path("textures") / Path("wood.png");
    FORGE_CHECK_EQ(joined.string(), std::string("assets/textures/wood.png"));
    FORGE_CHECK_EQ(Path("a").joined("b/c").string(), std::string("a/b/c"));
}

FORGE_TEST_CASE("filesystem.path_filename_and_extension")
{
    const Path path("assets/textures/wood.png");
    FORGE_CHECK_EQ(path.filename(), std::string("wood.png"));
    FORGE_CHECK_EQ(path.stem(), std::string("wood"));
    FORGE_CHECK_EQ(path.extension(), std::string(".png"));
    FORGE_CHECK_EQ(path.parent().string(), std::string("assets/textures"));

    FORGE_CHECK_EQ(Path("no_extension").extension(), std::string());
}

FORGE_TEST_CASE("filesystem.path_normalize")
{
    FORGE_CHECK_EQ(Path("a/b/../c").normalized().string(), std::string("a/c"));
    FORGE_CHECK_EQ(Path("a/./b").normalized().string(), std::string("a/b"));
    FORGE_CHECK_EQ(Path("a//b///c").normalized().string(), std::string("a/b/c"));
}

FORGE_TEST_CASE("filesystem.path_queries")
{
    const Path missing("this/path/should/not/exist/anywhere.xyz");
    FORGE_CHECK(!missing.exists());
    FORGE_CHECK(!missing.is_file());
    FORGE_CHECK(!missing.is_directory());

    const Path current = Path::current_directory();
    FORGE_CHECK(!current.empty());
    FORGE_CHECK(current.is_directory());

    const Path temp = Path::temp_directory();
    FORGE_CHECK(!temp.empty());
    FORGE_CHECK(temp.is_directory());
}

FORGE_TEST_CASE("filesystem.path_empty_and_absolute")
{
    FORGE_CHECK(Path().empty());
    FORGE_CHECK(!Path("x").empty());

    const Path absolute = Path("relative/file.txt").absolute();
    FORGE_CHECK(absolute.is_absolute());
}
