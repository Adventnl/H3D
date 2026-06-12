#pragma once

// A small, dependency-free Expected<T, E>.
//
// Forge3D uses its own implementation instead of std::expected so the
// foundation behaves identically on every supported standard library.
// The API follows std::expected closely for an easy migration later.
//
// Accessing value() on an error (or error() on a value) is a programmer
// error and asserts in debug builds.

#include <memory>
#include <type_traits>
#include <utility>

#include "forge/foundation/assert.hpp"

namespace forge
{

/// Wrapper that marks a value as an error when constructing an Expected.
template <typename E>
class Unexpected
{
    static_assert(!std::is_void_v<E>, "Unexpected<void> is not allowed");
    static_assert(!std::is_reference_v<E>, "Unexpected<E&> is not allowed");

public:
    constexpr explicit Unexpected(E error) : error_(std::move(error)) {}

    [[nodiscard]] constexpr E& error() & noexcept { return error_; }
    [[nodiscard]] constexpr const E& error() const& noexcept { return error_; }
    [[nodiscard]] constexpr E&& error() && noexcept { return std::move(error_); }

private:
    E error_;
};

template <typename E>
Unexpected(E) -> Unexpected<E>;

template <typename E>
[[nodiscard]] constexpr Unexpected<std::decay_t<E>> make_unexpected(E&& error)
{
    return Unexpected<std::decay_t<E>>(std::forward<E>(error));
}

template <typename T, typename E>
class Expected
{
    static_assert(!std::is_reference_v<T>, "Expected<T&, E> is not supported");
    static_assert(!std::is_reference_v<E>, "Expected<T, E&> is not supported");
    static_assert(!std::is_void_v<E>, "Expected<T, void> is not supported");

public:
    using value_type = T;
    using error_type = E;

    Expected()
        requires std::is_default_constructible_v<T>
        : has_value_(true)
    {
        std::construct_at(std::addressof(value_));
    }

    Expected(const T& value)
        requires std::is_copy_constructible_v<T>
        : has_value_(true)
    {
        std::construct_at(std::addressof(value_), value);
    }

    Expected(T&& value)
        requires std::is_move_constructible_v<T>
        : has_value_(true)
    {
        std::construct_at(std::addressof(value_), std::move(value));
    }

    template <typename E2>
        requires std::is_constructible_v<E, const E2&>
    Expected(const Unexpected<E2>& unexpected) : has_value_(false)
    {
        std::construct_at(std::addressof(error_), unexpected.error());
    }

    template <typename E2>
        requires std::is_constructible_v<E, E2&&>
    Expected(Unexpected<E2>&& unexpected) : has_value_(false)
    {
        std::construct_at(std::addressof(error_), std::move(unexpected).error());
    }

    Expected(const Expected& other) : has_value_(other.has_value_)
    {
        if (has_value_)
        {
            std::construct_at(std::addressof(value_), other.value_);
        }
        else
        {
            std::construct_at(std::addressof(error_), other.error_);
        }
    }

    Expected(Expected&& other) noexcept(std::is_nothrow_move_constructible_v<T> &&
                                        std::is_nothrow_move_constructible_v<E>)
        : has_value_(other.has_value_)
    {
        if (has_value_)
        {
            std::construct_at(std::addressof(value_), std::move(other.value_));
        }
        else
        {
            std::construct_at(std::addressof(error_), std::move(other.error_));
        }
    }

    Expected& operator=(const Expected& other)
    {
        if (this != std::addressof(other))
        {
            destroy();
            has_value_ = other.has_value_;
            if (has_value_)
            {
                std::construct_at(std::addressof(value_), other.value_);
            }
            else
            {
                std::construct_at(std::addressof(error_), other.error_);
            }
        }
        return *this;
    }

    Expected& operator=(Expected&& other) noexcept(std::is_nothrow_move_constructible_v<T> &&
                                                   std::is_nothrow_move_constructible_v<E>)
    {
        if (this != std::addressof(other))
        {
            destroy();
            has_value_ = other.has_value_;
            if (has_value_)
            {
                std::construct_at(std::addressof(value_), std::move(other.value_));
            }
            else
            {
                std::construct_at(std::addressof(error_), std::move(other.error_));
            }
        }
        return *this;
    }

    ~Expected()
    {
        destroy();
    }

    [[nodiscard]] bool has_value() const noexcept { return has_value_; }
    [[nodiscard]] explicit operator bool() const noexcept { return has_value_; }

    [[nodiscard]] T& value() &
    {
        FORGE_ASSERT_MSG(has_value_, "Expected::value() called on an error");
        return value_;
    }

    [[nodiscard]] const T& value() const&
    {
        FORGE_ASSERT_MSG(has_value_, "Expected::value() called on an error");
        return value_;
    }

    [[nodiscard]] T&& value() &&
    {
        FORGE_ASSERT_MSG(has_value_, "Expected::value() called on an error");
        return std::move(value_);
    }

    [[nodiscard]] E& error() &
    {
        FORGE_ASSERT_MSG(!has_value_, "Expected::error() called on a value");
        return error_;
    }

    [[nodiscard]] const E& error() const&
    {
        FORGE_ASSERT_MSG(!has_value_, "Expected::error() called on a value");
        return error_;
    }

    [[nodiscard]] E&& error() &&
    {
        FORGE_ASSERT_MSG(!has_value_, "Expected::error() called on a value");
        return std::move(error_);
    }

    template <typename U>
    [[nodiscard]] T value_or(U&& fallback) const&
    {
        return has_value_ ? value_ : static_cast<T>(std::forward<U>(fallback));
    }

    template <typename U>
    [[nodiscard]] T value_or(U&& fallback) &&
    {
        return has_value_ ? std::move(value_) : static_cast<T>(std::forward<U>(fallback));
    }

    [[nodiscard]] T* operator->()
    {
        FORGE_ASSERT(has_value_);
        return std::addressof(value_);
    }

    [[nodiscard]] const T* operator->() const
    {
        FORGE_ASSERT(has_value_);
        return std::addressof(value_);
    }

    [[nodiscard]] T& operator*() & { return value(); }
    [[nodiscard]] const T& operator*() const& { return value(); }
    [[nodiscard]] T&& operator*() && { return std::move(*this).value(); }

private:
    void destroy() noexcept
    {
        if (has_value_)
        {
            std::destroy_at(std::addressof(value_));
        }
        else
        {
            std::destroy_at(std::addressof(error_));
        }
    }

    union
    {
        T value_;
        E error_;
    };
    bool has_value_;
};

/// Specialization for operations that succeed without producing a value.
template <typename E>
class Expected<void, E>
{
    static_assert(!std::is_reference_v<E>, "Expected<void, E&> is not supported");

public:
    using value_type = void;
    using error_type = E;

    Expected() noexcept : has_value_(true) {}

    template <typename E2>
        requires std::is_constructible_v<E, const E2&>
    Expected(const Unexpected<E2>& unexpected) : has_value_(false)
    {
        std::construct_at(std::addressof(error_), unexpected.error());
    }

    template <typename E2>
        requires std::is_constructible_v<E, E2&&>
    Expected(Unexpected<E2>&& unexpected) : has_value_(false)
    {
        std::construct_at(std::addressof(error_), std::move(unexpected).error());
    }

    Expected(const Expected& other) : has_value_(other.has_value_)
    {
        if (!has_value_)
        {
            std::construct_at(std::addressof(error_), other.error_);
        }
    }

    Expected(Expected&& other) noexcept(std::is_nothrow_move_constructible_v<E>)
        : has_value_(other.has_value_)
    {
        if (!has_value_)
        {
            std::construct_at(std::addressof(error_), std::move(other.error_));
        }
    }

    Expected& operator=(const Expected& other)
    {
        if (this != std::addressof(other))
        {
            destroy();
            has_value_ = other.has_value_;
            if (!has_value_)
            {
                std::construct_at(std::addressof(error_), other.error_);
            }
        }
        return *this;
    }

    Expected& operator=(Expected&& other) noexcept(std::is_nothrow_move_constructible_v<E>)
    {
        if (this != std::addressof(other))
        {
            destroy();
            has_value_ = other.has_value_;
            if (!has_value_)
            {
                std::construct_at(std::addressof(error_), std::move(other.error_));
            }
        }
        return *this;
    }

    ~Expected()
    {
        destroy();
    }

    [[nodiscard]] bool has_value() const noexcept { return has_value_; }
    [[nodiscard]] explicit operator bool() const noexcept { return has_value_; }

    void value() const
    {
        FORGE_ASSERT_MSG(has_value_, "Expected<void>::value() called on an error");
    }

    [[nodiscard]] E& error() &
    {
        FORGE_ASSERT_MSG(!has_value_, "Expected::error() called on a value");
        return error_;
    }

    [[nodiscard]] const E& error() const&
    {
        FORGE_ASSERT_MSG(!has_value_, "Expected::error() called on a value");
        return error_;
    }

    [[nodiscard]] E&& error() &&
    {
        FORGE_ASSERT_MSG(!has_value_, "Expected::error() called on a value");
        return std::move(error_);
    }

private:
    void destroy() noexcept
    {
        if (!has_value_)
        {
            std::destroy_at(std::addressof(error_));
        }
    }

    union
    {
        char dummy_ = 0;
        E error_;
    };
    bool has_value_;
};

} // namespace forge
