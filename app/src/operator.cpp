#include "forge/app/operator.hpp"

#include <utility>

namespace forge
{

std::string_view operator_result_name(OperatorResult result) noexcept
{
    switch (result)
    {
    case OperatorResult::Finished: return "Finished";
    case OperatorResult::Cancelled: return "Cancelled";
    case OperatorResult::RunningModal: return "RunningModal";
    case OperatorResult::Failed: return "Failed";
    }
    return "Unknown";
}

Operator::Operator(OperatorInfo info) : info_(std::move(info)) {}

Operator::~Operator() = default;

bool Operator::is_available(const OperatorContext&) const
{
    return true;
}

OperatorResult Operator::invoke(OperatorContext& context)
{
    return execute(context);
}

OperatorResult Operator::cancel(OperatorContext&)
{
    return OperatorResult::Cancelled;
}

CallbackOperator::CallbackOperator(OperatorInfo info, ExecuteFn execute_fn,
                                   AvailableFn available_fn)
    : Operator(std::move(info)),
      execute_(std::move(execute_fn)),
      available_(std::move(available_fn))
{
}

bool CallbackOperator::is_available(const OperatorContext& context) const
{
    return available_ ? available_(context) : true;
}

OperatorResult CallbackOperator::execute(OperatorContext& context)
{
    if (!execute_)
    {
        return OperatorResult::Failed;
    }
    return execute_(context);
}

} // namespace forge
