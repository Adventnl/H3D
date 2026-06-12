#pragma once

// RAII profiling scopes.
//
//   void evaluate()
//   {
//       FORGE_PROFILE_FUNCTION();
//       {
//           FORGE_PROFILE_SCOPE("inner work");
//           ...
//       }
//   }
//
// When FORGE_PROFILING_ENABLED is 0, both macros compile to nothing.

#include <string_view>

#include "forge/foundation/compiler.hpp"
#include "forge/foundation/config.hpp"
#include "forge/foundation/macros.hpp"
#include "forge/foundation/noncopyable.hpp"
#include "forge/profiling/profiler.hpp"

namespace forge
{

class ProfilerScope : NonMovable
{
public:
    explicit ProfilerScope(std::string_view name)
    {
        Profiler::instance().begin_event(name);
    }

    ~ProfilerScope()
    {
        Profiler::instance().end_event();
    }
};

} // namespace forge

#if FORGE_PROFILING_ENABLED
    #define FORGE_PROFILE_SCOPE(name) \
        ::forge::ProfilerScope FORGE_ANONYMOUS_VARIABLE(forge_profile_scope_)(name)
    #define FORGE_PROFILE_FUNCTION() FORGE_PROFILE_SCOPE(FORGE_FUNCTION_NAME)
#else
    #define FORGE_PROFILE_SCOPE(name) static_cast<void>(0)
    #define FORGE_PROFILE_FUNCTION() static_cast<void>(0)
#endif
