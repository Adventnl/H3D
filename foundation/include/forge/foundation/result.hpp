#pragma once

// Result<T> — the standard return type for recoverable operations.
//
//   forge::Result<std::string> read() { ... return forge::failure(ErrorCode::NotFound, "..."); }
//   forge::Result<void> save() { ... return forge::success(); }

#include <string>
#include <utility>

#include "forge/foundation/error.hpp"
#include "forge/foundation/expected.hpp"

namespace forge
{

template <typename T = void>
using Result = Expected<T, Error>;

/// Build a failed Result with the given code and message.
[[nodiscard]] inline Unexpected<Error> failure(ErrorCode code, std::string message = {})
{
    return Unexpected<Error>(Error(code, std::move(message)));
}

/// Build a failed Result from an existing Error value.
[[nodiscard]] inline Unexpected<Error> failure(Error error)
{
    return Unexpected<Error>(std::move(error));
}

/// Build a successful Result<void>.
[[nodiscard]] inline Result<void> success()
{
    return Result<void>();
}

} // namespace forge
