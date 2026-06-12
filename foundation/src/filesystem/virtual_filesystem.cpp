#include "forge/filesystem/virtual_filesystem.hpp"

#include <format>

#include "forge/filesystem/file.hpp"

namespace forge
{
namespace
{

bool is_valid_alias(std::string_view alias) noexcept
{
    if (alias.empty())
    {
        return false;
    }
    for (const char character : alias)
    {
        if (character == '/' || character == '\\' || character == ':')
        {
            return false;
        }
    }
    return true;
}

struct SplitLogicalPath
{
    std::string_view alias;
    std::string_view relative;
};

Result<SplitLogicalPath> split_logical_path(std::string_view logical_path)
{
    const std::size_t separator = logical_path.find(':');
    if (separator == std::string_view::npos || separator == 0)
    {
        return failure(ErrorCode::InvalidArgument,
                       std::format("logical path '{}' must look like 'alias:/relative/path'",
                                   logical_path));
    }

    std::string_view relative = logical_path.substr(separator + 1);
    while (!relative.empty() && (relative.front() == '/' || relative.front() == '\\'))
    {
        relative.remove_prefix(1);
    }
    if (relative.empty())
    {
        return failure(ErrorCode::InvalidArgument,
                       std::format("logical path '{}' has no relative part", logical_path));
    }

    return SplitLogicalPath{logical_path.substr(0, separator), relative};
}

/// Reject anything that could escape the mount root: absolute paths, drive
/// letters and ".." segments. "." segments are harmless and allowed.
bool is_safe_relative_path(std::string_view relative) noexcept
{
    if (relative.find(':') != std::string_view::npos)
    {
        return false;
    }

    std::size_t segment_start = 0;
    for (std::size_t index = 0; index <= relative.size(); ++index)
    {
        if (index == relative.size() || relative[index] == '/' || relative[index] == '\\')
        {
            const std::string_view segment =
                relative.substr(segment_start, index - segment_start);
            if (segment == "..")
            {
                return false;
            }
            segment_start = index + 1;
        }
    }
    return true;
}

} // namespace

Result<void> VirtualFileSystem::mount(std::string_view alias, const Path& physical_root)
{
    if (!is_valid_alias(alias))
    {
        return failure(ErrorCode::InvalidArgument,
                       std::format("invalid mount alias '{}'", alias));
    }
    if (!physical_root.is_directory())
    {
        return failure(ErrorCode::NotFound,
                       std::format("mount root is not an existing directory: {}",
                                   physical_root.string()));
    }

    std::scoped_lock lock(mutex_);
    const auto [iterator, inserted] =
        mounts_.emplace(std::string(alias), physical_root.normalized());
    static_cast<void>(iterator);
    if (!inserted)
    {
        return failure(ErrorCode::AlreadyExists,
                       std::format("alias '{}' is already mounted", alias));
    }
    return success();
}

Result<void> VirtualFileSystem::unmount(std::string_view alias)
{
    std::scoped_lock lock(mutex_);
    if (mounts_.erase(std::string(alias)) == 0)
    {
        return failure(ErrorCode::NotFound, std::format("alias '{}' is not mounted", alias));
    }
    return success();
}

bool VirtualFileSystem::is_mounted(std::string_view alias) const
{
    std::scoped_lock lock(mutex_);
    return mounts_.contains(std::string(alias));
}

Result<Path> VirtualFileSystem::resolve(std::string_view logical_path) const
{
    auto split = split_logical_path(logical_path);
    if (!split)
    {
        return failure(std::move(split).error());
    }

    if (!is_safe_relative_path(split->relative))
    {
        return failure(ErrorCode::PermissionDenied,
                       std::format("logical path '{}' attempts to escape its mount",
                                   logical_path));
    }

    Path root;
    {
        std::scoped_lock lock(mutex_);
        const auto found = mounts_.find(std::string(split->alias));
        if (found == mounts_.end())
        {
            return failure(ErrorCode::NotFound,
                           std::format("alias '{}' is not mounted", split->alias));
        }
        root = found->second;
    }

    return (root / Path(split->relative)).normalized();
}

Result<Path> VirtualFileSystem::resolve_for_write(std::string_view logical_path) const
{
    auto resolved = resolve(logical_path);
    if (!resolved)
    {
        return resolved;
    }
    if (auto created = create_directories(resolved->parent()); !created)
    {
        return failure(std::move(created).error());
    }
    return resolved;
}

Result<std::string> VirtualFileSystem::read_text(std::string_view logical_path) const
{
    auto resolved = resolve(logical_path);
    if (!resolved)
    {
        return failure(std::move(resolved).error());
    }
    return read_text_file(*resolved);
}

Result<void> VirtualFileSystem::write_text(std::string_view logical_path,
                                           std::string_view content)
{
    auto resolved = resolve_for_write(logical_path);
    if (!resolved)
    {
        return failure(std::move(resolved).error());
    }
    return write_text_file(*resolved, content);
}

Result<std::vector<std::byte>> VirtualFileSystem::read_binary(
    std::string_view logical_path) const
{
    auto resolved = resolve(logical_path);
    if (!resolved)
    {
        return failure(std::move(resolved).error());
    }
    return read_binary_file(*resolved);
}

Result<void> VirtualFileSystem::write_binary(std::string_view logical_path,
                                             std::span<const std::byte> content)
{
    auto resolved = resolve_for_write(logical_path);
    if (!resolved)
    {
        return failure(std::move(resolved).error());
    }
    return write_binary_file(*resolved, content);
}

} // namespace forge
