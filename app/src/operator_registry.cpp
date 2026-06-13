#include "forge/app/operator_registry.hpp"

#include <algorithm>
#include <format>
#include <set>
#include <utility>

#include "forge/app/app_context.hpp"
#include "forge/app/command_stack.hpp"
#include "forge/app/runtime.hpp"
#include "forge/app/window.hpp"
#include "forge/foundation/log.hpp"

namespace forge
{
namespace
{

constexpr std::string_view kLogCategory = "operator";

std::string to_lower(std::string_view text)
{
    std::string result(text);
    for (char& c : result)
    {
        if (c >= 'A' && c <= 'Z')
        {
            c = static_cast<char>(c - 'A' + 'a');
        }
    }
    return result;
}

/// Relevance score for `query` within `text` (already lower-cased). 0 = no
/// match. Exact > prefix > word-boundary > substring.
int field_score(const std::string& text, const std::string& query, int weight)
{
    const std::size_t pos = text.find(query);
    if (pos == std::string::npos)
    {
        return 0;
    }
    int score = weight;
    if (text == query)
    {
        score += 100;
    }
    else if (pos == 0)
    {
        score += 50;
    }
    else if (text[pos - 1] == ' ' || text[pos - 1] == '.')
    {
        score += 25;
    }
    return score;
}

} // namespace

Result<void> OperatorRegistry::register_operator(std::unique_ptr<Operator> op)
{
    if (!op)
    {
        return failure(ErrorCode::InvalidArgument, "cannot register a null operator");
    }
    const std::string id = op->id();
    if (id.empty())
    {
        return failure(ErrorCode::InvalidArgument, "operator id must not be empty");
    }
    if (index_.contains(id))
    {
        return failure(ErrorCode::AlreadyExists,
                       std::format("operator '{}' is already registered", id));
    }
    index_.emplace(id, operators_.size());
    operators_.push_back(std::move(op));
    return success();
}

bool OperatorRegistry::unregister(std::string_view id)
{
    const auto it = index_.find(std::string(id));
    if (it == index_.end())
    {
        return false;
    }
    const std::size_t removed = it->second;
    operators_.erase(operators_.begin() + static_cast<std::ptrdiff_t>(removed));
    index_.erase(it);
    // Reindex entries after the removed slot.
    for (auto& [key, value] : index_)
    {
        if (value > removed)
        {
            --value;
        }
    }
    return true;
}

Operator* OperatorRegistry::find(std::string_view id)
{
    const auto it = index_.find(std::string(id));
    return it == index_.end() ? nullptr : operators_[it->second].get();
}

const Operator* OperatorRegistry::find(std::string_view id) const
{
    const auto it = index_.find(std::string(id));
    return it == index_.end() ? nullptr : operators_[it->second].get();
}

bool OperatorRegistry::contains(std::string_view id) const
{
    return index_.contains(std::string(id));
}

OperatorResult OperatorRegistry::execute(std::string_view id, OperatorContext& context)
{
    Operator* op = find(id);
    if (op == nullptr)
    {
        FORGE_LOG_WARNING(kLogCategory, std::format("unknown operator '{}'", id));
        return OperatorResult::Failed;
    }
    if (!op->is_available(context))
    {
        FORGE_LOG_DEBUG(kLogCategory,
                        std::format("operator '{}' is unavailable", op->id()));
        return OperatorResult::Failed;
    }
    return op->invoke(context);
}

std::vector<const Operator*> OperatorRegistry::list() const
{
    std::vector<const Operator*> result;
    result.reserve(operators_.size());
    for (const auto& op : operators_)
    {
        result.push_back(op.get());
    }
    std::sort(result.begin(), result.end(), [](const Operator* a, const Operator* b) {
        return a->id() < b->id();
    });
    return result;
}

std::vector<std::string> OperatorRegistry::categories() const
{
    std::set<std::string> unique;
    for (const auto& op : operators_)
    {
        if (!op->category().empty())
        {
            unique.insert(op->category());
        }
    }
    return std::vector<std::string>(unique.begin(), unique.end());
}

std::vector<OperatorSearchHit> OperatorRegistry::search(std::string_view query,
                                                        std::size_t max_results) const
{
    std::vector<OperatorSearchHit> hits;

    if (query.empty())
    {
        for (const Operator* op : list())
        {
            hits.push_back(OperatorSearchHit{op, 0});
        }
    }
    else
    {
        const std::string needle = to_lower(query);
        for (const auto& op : operators_)
        {
            int score = 0;
            score += field_score(to_lower(op->id()), needle, 40);
            score += field_score(to_lower(op->display_name()), needle, 60);
            score += field_score(to_lower(op->description()), needle, 10);
            score += field_score(to_lower(op->category()), needle, 20);
            if (score > 0)
            {
                hits.push_back(OperatorSearchHit{op.get(), score});
            }
        }
        std::sort(hits.begin(), hits.end(), [](const OperatorSearchHit& a,
                                               const OperatorSearchHit& b) {
            if (a.score != b.score)
            {
                return a.score > b.score;
            }
            return a.op->id() < b.op->id();
        });
    }

    if (max_results != 0 && hits.size() > max_results)
    {
        hits.resize(max_results);
    }
    return hits;
}

// ---------------------------------------------------------------------------
// Built-in operators
// ---------------------------------------------------------------------------
namespace
{

OperatorResult log_placeholder(const OperatorContext& context, std::string_view id,
                               std::string_view note)
{
    FORGE_LOG_INFO(kLogCategory,
                   std::format("{}: {} (context {})", id, note,
                               context.app != nullptr ? "ready" : "detached"));
    return OperatorResult::Finished;
}

void add(OperatorRegistry& registry, OperatorInfo info,
         CallbackOperator::ExecuteFn execute,
         CallbackOperator::AvailableFn available = {})
{
    auto op = std::make_unique<CallbackOperator>(std::move(info), std::move(execute),
                                                 std::move(available));
    // Built-in ids are unique by construction; ignore the (always-ok) result.
    static_cast<void>(registry.register_operator(std::move(op)));
}

} // namespace

void register_default_operators(OperatorRegistry& registry)
{
    add(registry, {"app.quit", "Quit", "Request the application to exit", "Application"},
        [](OperatorContext& ctx) {
            if (ctx.app != nullptr && ctx.app->runtime != nullptr)
            {
                ctx.app->runtime->request_quit();
            }
            if (ctx.app != nullptr && ctx.app->windows != nullptr)
            {
                ctx.app->windows->request_quit();
            }
            return OperatorResult::Finished;
        });

    add(registry,
        {"command.search", "Command Search", "Open the command/operator search", "Application"},
        [](OperatorContext& ctx) {
            return log_placeholder(ctx, "command.search",
                                   "interactive search UI arrives with the editor framework");
        });

    add(registry, {"edit.undo", "Undo", "Undo the last command", "Edit"},
        [](OperatorContext& ctx) {
            if (ctx.app != nullptr && ctx.app->commands != nullptr &&
                ctx.app->commands->undo())
            {
                return OperatorResult::Finished;
            }
            return OperatorResult::Cancelled;
        });

    add(registry, {"edit.redo", "Redo", "Redo the last undone command", "Edit"},
        [](OperatorContext& ctx) {
            if (ctx.app != nullptr && ctx.app->commands != nullptr &&
                ctx.app->commands->redo())
            {
                return OperatorResult::Finished;
            }
            return OperatorResult::Cancelled;
        });

    add(registry, {"file.new", "New File", "Create a new project", "File"},
        [](OperatorContext& ctx) {
            return log_placeholder(ctx, "file.new",
                                   "scene database not implemented until Phase 3");
        });
    add(registry, {"file.open", "Open File", "Open an existing project", "File"},
        [](OperatorContext& ctx) {
            return log_placeholder(ctx, "file.open",
                                   "project I/O not implemented until a later phase");
        });
    add(registry, {"file.save", "Save File", "Save the current project", "File"},
        [](OperatorContext& ctx) {
            return log_placeholder(ctx, "file.save",
                                   "project I/O not implemented until a later phase");
        });

    add(registry, {"workspace.next", "Next Workspace", "Switch to the next workspace",
                   "Workspace"},
        [](OperatorContext& ctx) {
            if (ctx.app != nullptr && ctx.app->workspaces != nullptr &&
                ctx.app->workspaces->next_workspace())
            {
                return OperatorResult::Finished;
            }
            return OperatorResult::Cancelled;
        });
    add(registry, {"workspace.previous", "Previous Workspace",
                   "Switch to the previous workspace", "Workspace"},
        [](OperatorContext& ctx) {
            if (ctx.app != nullptr && ctx.app->workspaces != nullptr &&
                ctx.app->workspaces->previous_workspace())
            {
                return OperatorResult::Finished;
            }
            return OperatorResult::Cancelled;
        });

    add(registry, {"screen.split_vertical", "Split Area Vertically",
                   "Split the active area into left/right halves", "Screen"},
        [](OperatorContext& ctx) {
            if (ctx.app != nullptr && ctx.app->workspaces != nullptr &&
                ctx.app->workspaces->split_active_area_vertical())
            {
                return OperatorResult::Finished;
            }
            return OperatorResult::Cancelled;
        });
    add(registry, {"screen.split_horizontal", "Split Area Horizontally",
                   "Split the active area into top/bottom halves", "Screen"},
        [](OperatorContext& ctx) {
            if (ctx.app != nullptr && ctx.app->workspaces != nullptr &&
                ctx.app->workspaces->split_active_area_horizontal())
            {
                return OperatorResult::Finished;
            }
            return OperatorResult::Cancelled;
        });
    add(registry, {"screen.close_area", "Close Area", "Close the active area", "Screen"},
        [](OperatorContext& ctx) {
            if (ctx.app != nullptr && ctx.app->workspaces != nullptr &&
                ctx.app->workspaces->close_active_area())
            {
                return OperatorResult::Finished;
            }
            return OperatorResult::Cancelled;
        });

    add(registry, {"transform.move", "Move", "Move the selection", "Transform"},
        [](OperatorContext& ctx) {
            return log_placeholder(ctx, "transform.move",
                                   "no scene objects to transform until Phase 3");
        });
    add(registry, {"transform.rotate", "Rotate", "Rotate the selection", "Transform"},
        [](OperatorContext& ctx) {
            return log_placeholder(ctx, "transform.rotate",
                                   "no scene objects to transform until Phase 3");
        });
    add(registry, {"transform.scale", "Scale", "Scale the selection", "Transform"},
        [](OperatorContext& ctx) {
            return log_placeholder(ctx, "transform.scale",
                                   "no scene objects to transform until Phase 3");
        });

    add(registry, {"mode.toggle", "Toggle Mode", "Toggle between interaction modes",
                   "Edit"},
        [](OperatorContext& ctx) {
            return log_placeholder(ctx, "mode.toggle",
                                   "object/edit modes arrive with the scene database");
        });
}

} // namespace forge
