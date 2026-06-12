#pragma once

// Error codes and the Error value used by Result / Expected.

#include <cstdint>
#include <string>
#include <string_view>
#include <utility>

namespace forge
{

enum class ErrorCode : std::uint32_t
{
    None = 0,
    Unknown,
    InvalidArgument,
    NotFound,
    AlreadyExists,
    PermissionDenied,
    IOError,
    OutOfMemory,
    Timeout,
    Unsupported,
    InternalError,
};

/// Stable name of an error code, e.g. "NotFound".
[[nodiscard]] std::string_view error_code_name(ErrorCode code) noexcept;

/// A recoverable error: a code plus a human-readable message.
struct Error
{
    ErrorCode code = ErrorCode::None;
    std::string message;

    Error() = default;

    explicit Error(ErrorCode error_code, std::string error_message = {})
        : code(error_code), message(std::move(error_message))
    {
    }

    /// "NotFound: config.json does not exist"
    [[nodiscard]] std::string to_string() const;

    [[nodiscard]] bool operator==(const Error& other) const noexcept
    {
        return code == other.code && message == other.message;
    }
};

} // namespace forge
