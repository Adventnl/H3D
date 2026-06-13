#pragma once

// EventBus: a small publish/subscribe hub with both immediate and queued
// dispatch.
//
// Handlers subscribe to a specific EventType (or to every event). publish()
// dispatches immediately; queue() defers an event until dispatch_queued().
// Handlers are invoked in subscription order. The bus is safe to mutate from
// within a handler: the matching handler list is snapshotted before dispatch.

#include <cstdint>
#include <deque>
#include <functional>
#include <mutex>
#include <vector>

#include "forge/app/event.hpp"

namespace forge
{

class EventBus
{
public:
    using Handler = std::function<void(const Event&)>;
    using SubscriptionId = std::uint64_t;

    /// Subscribe to a single event type. Returns a token for unsubscribe().
    SubscriptionId subscribe(EventType type, Handler handler);

    /// Subscribe to every event type.
    SubscriptionId subscribe_any(Handler handler);

    /// Remove a subscription. Returns true if a subscription was removed.
    bool unsubscribe(SubscriptionId id);

    /// Dispatch an event to all matching handlers immediately.
    void publish(const Event& event);

    /// Append an event to the queue for later dispatch_queued().
    void queue(Event event);

    /// Dispatch all queued events in FIFO order, then clear the queue. Events
    /// queued by handlers during dispatch are processed in the same drain.
    void dispatch_queued();

    /// Remove all subscriptions and queued events.
    void clear();

    [[nodiscard]] std::size_t subscriber_count() const;
    [[nodiscard]] std::size_t queued_count() const;

private:
    struct Entry
    {
        SubscriptionId id;
        EventType type;
        bool any;
        Handler handler;
    };

    mutable std::mutex mutex_;
    std::vector<Entry> entries_;
    std::deque<Event> queue_;
    SubscriptionId next_id_ = 1;
};

} // namespace forge
