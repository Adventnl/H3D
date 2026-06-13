#pragma once

// OperatorRegistry: the catalogue of operators, keyed by id. Also powers
// command search (case-insensitive substring matching over id / name /
// description / category, ranked by a simple relevance score).

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "forge/app/operator.hpp"
#include "forge/foundation/result.hpp"

namespace forge
{

/// A single command-search hit and its score (higher is better).
struct OperatorSearchHit
{
    const Operator* op = nullptr;
    int score = 0;
};

class OperatorRegistry
{
public:
    OperatorRegistry() = default;

    /// Register an operator. Fails with AlreadyExists on a duplicate id.
    Result<void> register_operator(std::unique_ptr<Operator> op);

    /// Remove an operator by id. Returns true when one was removed.
    bool unregister(std::string_view id);

    [[nodiscard]] Operator* find(std::string_view id);
    [[nodiscard]] const Operator* find(std::string_view id) const;
    [[nodiscard]] bool contains(std::string_view id) const;
    [[nodiscard]] std::size_t size() const noexcept { return operators_.size(); }

    /// Invoke the operator with the given id. Returns Failed when the id is
    /// unknown or the operator reports it is unavailable.
    [[nodiscard]] OperatorResult execute(std::string_view id, OperatorContext& context);

    /// All operators, sorted by id for stable listing.
    [[nodiscard]] std::vector<const Operator*> list() const;

    /// Distinct categories, sorted.
    [[nodiscard]] std::vector<std::string> categories() const;

    /// Ranked command search. Empty query returns every operator (by id).
    /// Results are capped to `max_results` (0 means unlimited).
    [[nodiscard]] std::vector<OperatorSearchHit> search(std::string_view query,
                                                        std::size_t max_results = 0) const;

private:
    // insertion vector keeps stable Operator* addresses; map indexes by id.
    std::vector<std::unique_ptr<Operator>> operators_;
    std::unordered_map<std::string, std::size_t> index_;
};

/// Register the built-in Forge3D operators (app.quit, edit.undo, transform.*,
/// workspace.*, screen.*, ...). See docs/architecture/command_operator_undo.md.
void register_default_operators(OperatorRegistry& registry);

} // namespace forge
