#include "forge/app/event_bus.hpp"

#include <algorithm>

namespace forge
{

EventBus::SubscriptionId EventBus::subscribe(EventType type, Handler handler)
{
    std::scoped_lock lock(mutex_);
    const SubscriptionId id = next_id_++;
    entries_.push_back(Entry{id, type, false, std::move(handler)});
    return id;
}

EventBus::SubscriptionId EventBus::subscribe_any(Handler handler)
{
    std::scoped_lock lock(mutex_);
    const SubscriptionId id = next_id_++;
    entries_.push_back(Entry{id, EventType::None, true, std::move(handler)});
    return id;
}

bool EventBus::unsubscribe(SubscriptionId id)
{
    std::scoped_lock lock(mutex_);
    const auto before = entries_.size();
    std::erase_if(entries_, [id](const Entry& entry) { return entry.id == id; });
    return entries_.size() != before;
}

void EventBus::publish(const Event& event)
{
    // Snapshot matching handlers so subscribe/unsubscribe from within a handler
    // is safe and does not affect this dispatch.
    std::vector<Handler> matching;
    {
        std::scoped_lock lock(mutex_);
        matching.reserve(entries_.size());
        for (const Entry& entry : entries_)
        {
            if (entry.any || entry.type == event.type)
            {
                matching.push_back(entry.handler);
            }
        }
    }
    for (const Handler& handler : matching)
    {
        handler(event);
    }
}

void EventBus::queue(Event event)
{
    std::scoped_lock lock(mutex_);
    queue_.push_back(std::move(event));
}

void EventBus::dispatch_queued()
{
    // Drain until empty so events queued by handlers are also processed. Pop
    // one at a time (handlers may publish/queue more).
    while (true)
    {
        Event event;
        {
            std::scoped_lock lock(mutex_);
            if (queue_.empty())
            {
                break;
            }
            event = std::move(queue_.front());
            queue_.pop_front();
        }
        publish(event);
    }
}

void EventBus::clear()
{
    std::scoped_lock lock(mutex_);
    entries_.clear();
    queue_.clear();
}

std::size_t EventBus::subscriber_count() const
{
    std::scoped_lock lock(mutex_);
    return entries_.size();
}

std::size_t EventBus::queued_count() const
{
    std::scoped_lock lock(mutex_);
    return queue_.size();
}

} // namespace forge
