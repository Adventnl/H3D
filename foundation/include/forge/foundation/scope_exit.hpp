#pragma once

// RAII helper that runs a callable when the enclosing scope ends.
//
//   auto guard = forge::make_scope_exit([&] { close_handle(handle); });
//   guard.release(); // dismiss; the callable will not run
//
//   FORGE_SCOPE_EXIT(file.close());

#include <type_traits>
#include <utility>

#include "forge/foundation/macros.hpp"

namespace forge
{

template <typename Callback>
class ScopeExit
{
public:
    explicit ScopeExit(Callback callback) noexcept(std::is_nothrow_move_constructible_v<Callback>)
        : callback_(std::move(callback))
    {
    }

    ScopeExit(ScopeExit&& other) noexcept(std::is_nothrow_move_constructible_v<Callback>)
        : callback_(std::move(other.callback_)), active_(other.active_)
    {
        other.active_ = false;
    }

    ScopeExit(const ScopeExit&) = delete;
    ScopeExit& operator=(const ScopeExit&) = delete;
    ScopeExit& operator=(ScopeExit&&) = delete;

    ~ScopeExit()
    {
        if (active_)
        {
            callback_();
        }
    }

    /// Dismiss the guard; the callback will not be invoked.
    void release() noexcept
    {
        active_ = false;
    }

    [[nodiscard]] bool is_active() const noexcept
    {
        return active_;
    }

private:
    Callback callback_;
    bool active_ = true;
};

template <typename Callback>
[[nodiscard]] auto make_scope_exit(Callback&& callback)
{
    return ScopeExit<std::decay_t<Callback>>(std::forward<Callback>(callback));
}

} // namespace forge

/// Run the given statement(s) when the current scope exits.
#define FORGE_SCOPE_EXIT(...) \
    auto FORGE_ANONYMOUS_VARIABLE(forge_scope_exit_) = \
        ::forge::make_scope_exit([&]() { __VA_ARGS__; })
