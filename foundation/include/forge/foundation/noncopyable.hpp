#pragma once

// Inheritance helpers that delete copy (and optionally move) operations.

namespace forge
{

/// Base class that disables copying but keeps move semantics.
class NonCopyable
{
public:
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;

protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
    NonCopyable(NonCopyable&&) = default;
    NonCopyable& operator=(NonCopyable&&) = default;
};

/// Base class that disables both copying and moving.
class NonMovable
{
public:
    NonMovable(const NonMovable&) = delete;
    NonMovable& operator=(const NonMovable&) = delete;
    NonMovable(NonMovable&&) = delete;
    NonMovable& operator=(NonMovable&&) = delete;

protected:
    NonMovable() = default;
    ~NonMovable() = default;
};

} // namespace forge
