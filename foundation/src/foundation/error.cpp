#include "forge/foundation/error.hpp"

namespace forge
{

std::string_view error_code_name(ErrorCode code) noexcept
{
    switch (code)
    {
    case ErrorCode::None: return "None";
    case ErrorCode::Unknown: return "Unknown";
    case ErrorCode::InvalidArgument: return "InvalidArgument";
    case ErrorCode::NotFound: return "NotFound";
    case ErrorCode::AlreadyExists: return "AlreadyExists";
    case ErrorCode::PermissionDenied: return "PermissionDenied";
    case ErrorCode::IOError: return "IOError";
    case ErrorCode::OutOfMemory: return "OutOfMemory";
    case ErrorCode::Timeout: return "Timeout";
    case ErrorCode::Unsupported: return "Unsupported";
    case ErrorCode::InternalError: return "InternalError";
    }
    return "Invalid";
}

std::string Error::to_string() const
{
    std::string text(error_code_name(code));
    if (!message.empty())
    {
        text += ": ";
        text += message;
    }
    return text;
}

} // namespace forge
