#pragma once

// Project version. The actual numbers are injected by the build system into
// version.cpp; this header only exposes accessors so version bumps never
// trigger a full rebuild.

#include <string_view>

namespace forge
{

[[nodiscard]] std::string_view project_name() noexcept;
[[nodiscard]] int version_major() noexcept;
[[nodiscard]] int version_minor() noexcept;
[[nodiscard]] int version_patch() noexcept;

/// "0.1.0"
[[nodiscard]] std::string_view version_string() noexcept;

} // namespace forge
