#pragma once

// Operator: a user-facing action (the thing a menu item, shortcut or command
// search invokes). An operator may push commands onto the undo stack, request
// application state changes, or start a modal tool. Some operators are not
// undoable.
//
// Operators are stateless and registered once; per-invocation data travels in
// the OperatorContext.

#include <functional>
#include <string>
#include <string_view>

namespace forge
{

class AppContext; // defined in app_context.hpp

/// Outcome of invoking an operator.
enum class OperatorResult
{
    Finished,      // completed successfully (may have produced an undo step)
    Cancelled,     // user/condition cancelled; no lasting effect
    RunningModal,  // started a modal interaction; awaits further events
    Failed,        // could not run (precondition failed, error)
};

[[nodiscard]] std::string_view operator_result_name(OperatorResult result) noexcept;

/// Per-invocation context handed to an operator. Holds a reference to the
/// application services it may use; the optional argument string carries
/// operator-specific parameters (kept simple for Phase 2).
struct OperatorContext
{
    AppContext* app = nullptr;
    std::string argument;
};

struct OperatorInfo
{
    std::string id;            // e.g. "transform.move"
    std::string display_name;  // e.g. "Move"
    std::string description;
    std::string category;      // e.g. "Transform"
};

class Operator
{
public:
    explicit Operator(OperatorInfo info);
    virtual ~Operator();

    Operator(const Operator&) = delete;
    Operator& operator=(const Operator&) = delete;

    [[nodiscard]] const std::string& id() const noexcept { return info_.id; }
    [[nodiscard]] const std::string& display_name() const noexcept { return info_.display_name; }
    [[nodiscard]] const std::string& description() const noexcept { return info_.description; }
    [[nodiscard]] const std::string& category() const noexcept { return info_.category; }
    [[nodiscard]] const OperatorInfo& info() const noexcept { return info_; }

    /// Whether the operator can run in the given context. Default: always.
    [[nodiscard]] virtual bool is_available(const OperatorContext& context) const;

    /// Entry point used by the registry. Default forwards to execute(); modal
    /// operators override this to start a modal interaction.
    [[nodiscard]] virtual OperatorResult invoke(OperatorContext& context);

    /// The operator's actual work.
    [[nodiscard]] virtual OperatorResult execute(OperatorContext& context) = 0;

    /// Cancel an in-progress modal operator. Default: Cancelled.
    [[nodiscard]] virtual OperatorResult cancel(OperatorContext& context);

private:
    OperatorInfo info_;
};

/// An operator implemented with callbacks; used for the built-in operators and
/// tests so they need not each be a dedicated subclass.
class CallbackOperator final : public Operator
{
public:
    using ExecuteFn = std::function<OperatorResult(OperatorContext&)>;
    using AvailableFn = std::function<bool(const OperatorContext&)>;

    CallbackOperator(OperatorInfo info, ExecuteFn execute_fn,
                     AvailableFn available_fn = {});

    [[nodiscard]] bool is_available(const OperatorContext& context) const override;
    [[nodiscard]] OperatorResult execute(OperatorContext& context) override;

private:
    ExecuteFn execute_;
    AvailableFn available_;
};

} // namespace forge
