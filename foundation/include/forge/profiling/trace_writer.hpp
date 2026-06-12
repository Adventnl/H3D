#pragma once

// Serializes profiler events to the Chrome trace event format. Open the
// resulting file in chrome://tracing or https://ui.perfetto.dev.

#include <span>
#include <string>

#include "forge/filesystem/path.hpp"
#include "forge/foundation/result.hpp"
#include "forge/profiling/profiler.hpp"

namespace forge
{

/// Render events as a Chrome trace JSON document.
[[nodiscard]] std::string write_chrome_trace_json(std::span<const ProfileEvent> events);

/// Write events as Chrome trace JSON to a file.
[[nodiscard]] Result<void> write_chrome_trace_file(const Path& path,
                                                   std::span<const ProfileEvent> events);

} // namespace forge
