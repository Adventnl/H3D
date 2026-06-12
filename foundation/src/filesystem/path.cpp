#include "forge/filesystem/path.hpp"

#include <system_error>

namespace forge
{

bool Path::exists() const noexcept
{
    std::error_code error;
    return std::filesystem::exists(path_, error) && !error;
}

bool Path::is_file() const noexcept
{
    std::error_code error;
    return std::filesystem::is_regular_file(path_, error) && !error;
}

bool Path::is_directory() const noexcept
{
    std::error_code error;
    return std::filesystem::is_directory(path_, error) && !error;
}

Path Path::absolute() const
{
    std::error_code error;
    std::filesystem::path result = std::filesystem::absolute(path_, error);
    if (error)
    {
        return *this;
    }
    return Path(std::move(result));
}

Path Path::current_directory()
{
    std::error_code error;
    std::filesystem::path result = std::filesystem::current_path(error);
    if (error)
    {
        return Path();
    }
    return Path(std::move(result));
}

Path Path::temp_directory()
{
    std::error_code error;
    std::filesystem::path result = std::filesystem::temp_directory_path(error);
    if (error)
    {
        return Path();
    }
    return Path(std::move(result));
}

} // namespace forge
