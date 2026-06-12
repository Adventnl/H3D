#pragma once

// Maps logical paths of the form "alias:/relative/path" onto mounted
// physical directories.
//
//   VirtualFileSystem vfs;
//   vfs.mount("assets", "C:/forge/assets");
//   auto text = vfs.read_text("assets:/defaults/config.json");
//
// resolve() rejects absolute relative-parts and any ".." segment, so a
// logical path can never escape its mount root. Writing through the VFS
// creates missing parent directories inside the mount.
//
// Thread-safe: the mount table is guarded by a mutex.

#include <cstddef>
#include <mutex>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "forge/filesystem/path.hpp"
#include "forge/foundation/noncopyable.hpp"
#include "forge/foundation/result.hpp"

namespace forge
{

class VirtualFileSystem : NonMovable
{
public:
    VirtualFileSystem() = default;

    /// Mount `physical_root` (an existing directory) under `alias`.
    /// The alias must be non-empty and must not contain '/', '\' or ':'.
    [[nodiscard]] Result<void> mount(std::string_view alias, const Path& physical_root);

    [[nodiscard]] Result<void> unmount(std::string_view alias);

    [[nodiscard]] bool is_mounted(std::string_view alias) const;

    /// Translate "alias:/relative/path" to the physical path.
    [[nodiscard]] Result<Path> resolve(std::string_view logical_path) const;

    [[nodiscard]] Result<std::string> read_text(std::string_view logical_path) const;
    [[nodiscard]] Result<void> write_text(std::string_view logical_path,
                                          std::string_view content);

    [[nodiscard]] Result<std::vector<std::byte>> read_binary(
        std::string_view logical_path) const;
    [[nodiscard]] Result<void> write_binary(std::string_view logical_path,
                                            std::span<const std::byte> content);

private:
    /// Ensures the parent directory of a resolved write target exists.
    [[nodiscard]] Result<Path> resolve_for_write(std::string_view logical_path) const;

    mutable std::mutex mutex_;
    std::unordered_map<std::string, Path> mounts_;
};

} // namespace forge
