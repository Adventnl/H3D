#pragma once

// Whole-file read/write helpers. All functions report recoverable failures
// through Result instead of throwing.

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "forge/filesystem/path.hpp"
#include "forge/foundation/result.hpp"

namespace forge
{

[[nodiscard]] Result<std::string> read_text_file(const Path& path);

[[nodiscard]] Result<void> write_text_file(const Path& path, std::string_view content);

[[nodiscard]] Result<void> append_text_file(const Path& path, std::string_view content);

[[nodiscard]] Result<std::vector<std::byte>> read_binary_file(const Path& path);

[[nodiscard]] Result<void> write_binary_file(const Path& path,
                                             std::span<const std::byte> content);

[[nodiscard]] Result<std::uint64_t> file_size(const Path& path);

/// Create the directory and all missing parents. Succeeds if it already
/// exists as a directory.
[[nodiscard]] Result<void> create_directories(const Path& path);

} // namespace forge
