#pragma once

// Path value type wrapping std::filesystem::path.
//
// string() always returns the generic (forward-slash) form so paths are
// stable across platforms in logs, files and tests; native_string() returns
// the OS-preferred form for display and OS calls.

#include <filesystem>
#include <string>
#include <string_view>

namespace forge
{

class Path
{
public:
    Path() = default;
    Path(std::string_view path) : path_(path) {}
    Path(const char* path) : path_(path) {}
    Path(const std::string& path) : path_(path) {}
    Path(std::filesystem::path path) : path_(std::move(path)) {}

    /// Generic form with forward slashes, e.g. "assets/textures/wood.png".
    [[nodiscard]] std::string string() const { return path_.generic_string(); }

    /// OS-preferred form (backslashes on Windows).
    [[nodiscard]] std::string native_string() const { return path_.string(); }

    [[nodiscard]] std::string filename() const { return path_.filename().generic_string(); }
    [[nodiscard]] std::string stem() const { return path_.stem().generic_string(); }
    [[nodiscard]] std::string extension() const { return path_.extension().generic_string(); }

    [[nodiscard]] Path parent() const { return Path(path_.parent_path()); }

    [[nodiscard]] Path joined(const Path& other) const { return Path(path_ / other.path_); }

    /// Lexically normalized copy: "a/b/../c" -> "a/c". Purely textual.
    [[nodiscard]] Path normalized() const { return Path(path_.lexically_normal()); }

    [[nodiscard]] bool empty() const noexcept { return path_.empty(); }
    [[nodiscard]] bool is_absolute() const { return path_.is_absolute(); }

    // Filesystem queries; all swallow errors and report false instead of
    // throwing (failure to stat is treated as "does not exist").
    [[nodiscard]] bool exists() const noexcept;
    [[nodiscard]] bool is_file() const noexcept;
    [[nodiscard]] bool is_directory() const noexcept;

    /// Absolute form of this path (resolved against the working directory).
    [[nodiscard]] Path absolute() const;

    [[nodiscard]] const std::filesystem::path& raw() const noexcept { return path_; }

    [[nodiscard]] static Path current_directory();
    [[nodiscard]] static Path temp_directory();

    [[nodiscard]] bool operator==(const Path& other) const { return path_ == other.path_; }

private:
    std::filesystem::path path_;
};

[[nodiscard]] inline Path operator/(const Path& left, const Path& right)
{
    return left.joined(right);
}

} // namespace forge
