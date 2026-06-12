#include "forge/filesystem/file.hpp"

#include <fstream>
#include <system_error>

namespace forge
{
namespace
{

Error make_io_error(ErrorCode code, const Path& path, std::string_view what)
{
    std::string message(what);
    message += ": ";
    message += path.string();
    return Error(code, std::move(message));
}

ErrorCode error_code_from_std(const std::error_code& error) noexcept
{
    if (error == std::errc::no_such_file_or_directory)
    {
        return ErrorCode::NotFound;
    }
    if (error == std::errc::permission_denied)
    {
        return ErrorCode::PermissionDenied;
    }
    if (error == std::errc::file_exists)
    {
        return ErrorCode::AlreadyExists;
    }
    if (error == std::errc::not_enough_memory)
    {
        return ErrorCode::OutOfMemory;
    }
    return ErrorCode::IOError;
}

} // namespace

Result<std::string> read_text_file(const Path& path)
{
    if (!path.exists())
    {
        return failure(make_io_error(ErrorCode::NotFound, path, "file does not exist"));
    }
    if (!path.is_file())
    {
        return failure(make_io_error(ErrorCode::InvalidArgument, path, "not a regular file"));
    }

    std::ifstream stream(path.raw(), std::ios::in | std::ios::binary);
    if (!stream.is_open())
    {
        return failure(make_io_error(ErrorCode::IOError, path, "failed to open for reading"));
    }

    std::string content;
    stream.seekg(0, std::ios::end);
    const std::streampos end = stream.tellg();
    if (end > 0)
    {
        content.resize(static_cast<std::size_t>(end));
        stream.seekg(0, std::ios::beg);
        stream.read(content.data(), end);
    }
    if (stream.bad())
    {
        return failure(make_io_error(ErrorCode::IOError, path, "read failed"));
    }
    return content;
}

Result<void> write_text_file(const Path& path, std::string_view content)
{
    std::ofstream stream(path.raw(), std::ios::out | std::ios::binary | std::ios::trunc);
    if (!stream.is_open())
    {
        return failure(make_io_error(ErrorCode::IOError, path, "failed to open for writing"));
    }
    stream.write(content.data(), static_cast<std::streamsize>(content.size()));
    stream.flush();
    if (!stream.good())
    {
        return failure(make_io_error(ErrorCode::IOError, path, "write failed"));
    }
    return success();
}

Result<void> append_text_file(const Path& path, std::string_view content)
{
    std::ofstream stream(path.raw(), std::ios::out | std::ios::binary | std::ios::app);
    if (!stream.is_open())
    {
        return failure(make_io_error(ErrorCode::IOError, path, "failed to open for appending"));
    }
    stream.write(content.data(), static_cast<std::streamsize>(content.size()));
    stream.flush();
    if (!stream.good())
    {
        return failure(make_io_error(ErrorCode::IOError, path, "append failed"));
    }
    return success();
}

Result<std::vector<std::byte>> read_binary_file(const Path& path)
{
    if (!path.exists())
    {
        return failure(make_io_error(ErrorCode::NotFound, path, "file does not exist"));
    }
    if (!path.is_file())
    {
        return failure(make_io_error(ErrorCode::InvalidArgument, path, "not a regular file"));
    }

    std::ifstream stream(path.raw(), std::ios::in | std::ios::binary);
    if (!stream.is_open())
    {
        return failure(make_io_error(ErrorCode::IOError, path, "failed to open for reading"));
    }

    std::vector<std::byte> content;
    stream.seekg(0, std::ios::end);
    const std::streampos end = stream.tellg();
    if (end > 0)
    {
        content.resize(static_cast<std::size_t>(end));
        stream.seekg(0, std::ios::beg);
        stream.read(reinterpret_cast<char*>(content.data()), end);
    }
    if (stream.bad())
    {
        return failure(make_io_error(ErrorCode::IOError, path, "read failed"));
    }
    return content;
}

Result<void> write_binary_file(const Path& path, std::span<const std::byte> content)
{
    std::ofstream stream(path.raw(), std::ios::out | std::ios::binary | std::ios::trunc);
    if (!stream.is_open())
    {
        return failure(make_io_error(ErrorCode::IOError, path, "failed to open for writing"));
    }
    stream.write(reinterpret_cast<const char*>(content.data()),
                 static_cast<std::streamsize>(content.size()));
    stream.flush();
    if (!stream.good())
    {
        return failure(make_io_error(ErrorCode::IOError, path, "write failed"));
    }
    return success();
}

Result<std::uint64_t> file_size(const Path& path)
{
    std::error_code error;
    const std::uintmax_t size = std::filesystem::file_size(path.raw(), error);
    if (error)
    {
        return failure(make_io_error(error_code_from_std(error), path, "failed to stat"));
    }
    return static_cast<std::uint64_t>(size);
}

Result<void> create_directories(const Path& path)
{
    std::error_code error;
    std::filesystem::create_directories(path.raw(), error);
    if (error)
    {
        return failure(
            make_io_error(error_code_from_std(error), path, "failed to create directories"));
    }
    return success();
}

} // namespace forge
