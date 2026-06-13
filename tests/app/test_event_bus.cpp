#include "test_framework.hpp"

#include <vector>

#include "forge/app/event_bus.hpp"

FORGE_TEST_CASE("app.event_bus_subscriber_receives_event")
{
    forge::EventBus bus;
    int received = 0;
    bus.subscribe(forge::EventType::Quit, [&received](const forge::Event&) { ++received; });
    bus.publish(forge::Event::quit());
    FORGE_CHECK_EQ(received, 1);
}

FORGE_TEST_CASE("app.event_bus_only_matching_type_receives")
{
    forge::EventBus bus;
    int quit_count = 0;
    int resize_count = 0;
    bus.subscribe(forge::EventType::Quit, [&](const forge::Event&) { ++quit_count; });
    bus.subscribe(forge::EventType::WindowResize, [&](const forge::Event&) { ++resize_count; });

    bus.publish(forge::Event::quit());
    bus.publish(forge::Event::window_resize(1, 800, 600));
    bus.publish(forge::Event::quit());

    FORGE_CHECK_EQ(quit_count, 2);
    FORGE_CHECK_EQ(resize_count, 1);
}

FORGE_TEST_CASE("app.event_bus_multiple_subscribers")
{
    forge::EventBus bus;
    int a = 0;
    int b = 0;
    bus.subscribe(forge::EventType::FrameBegin, [&](const forge::Event&) { ++a; });
    bus.subscribe(forge::EventType::FrameBegin, [&](const forge::Event&) { ++b; });
    bus.publish(forge::Event::frame_begin());
    FORGE_CHECK_EQ(a, 1);
    FORGE_CHECK_EQ(b, 1);
}

FORGE_TEST_CASE("app.event_bus_subscribe_any")
{
    forge::EventBus bus;
    int count = 0;
    bus.subscribe_any([&count](const forge::Event&) { ++count; });
    bus.publish(forge::Event::quit());
    bus.publish(forge::Event::frame_end());
    FORGE_CHECK_EQ(count, 2);
}

FORGE_TEST_CASE("app.event_bus_unsubscribe")
{
    forge::EventBus bus;
    int count = 0;
    const auto id = bus.subscribe(forge::EventType::Quit,
                                  [&count](const forge::Event&) { ++count; });
    bus.publish(forge::Event::quit());
    FORGE_CHECK(bus.unsubscribe(id));
    bus.publish(forge::Event::quit());
    FORGE_CHECK_EQ(count, 1);
    FORGE_CHECK(!bus.unsubscribe(id)); // already removed
}

FORGE_TEST_CASE("app.event_bus_publish_with_no_subscribers_is_safe")
{
    forge::EventBus bus;
    bus.publish(forge::Event::quit()); // must not crash
    FORGE_CHECK_EQ(bus.subscriber_count(), static_cast<std::size_t>(0));
}

FORGE_TEST_CASE("app.event_bus_queued_dispatch_preserves_order")
{
    forge::EventBus bus;
    std::vector<forge::EventType> order;
    bus.subscribe_any([&order](const forge::Event& e) { order.push_back(e.type); });

    bus.queue(forge::Event::frame_begin());
    bus.queue(forge::Event::quit());
    bus.queue(forge::Event::frame_end());
    FORGE_CHECK_EQ(bus.queued_count(), static_cast<std::size_t>(3));

    bus.dispatch_queued();
    FORGE_CHECK_EQ(bus.queued_count(), static_cast<std::size_t>(0));
    FORGE_REQUIRE(order.size() == static_cast<std::size_t>(3));
    FORGE_CHECK(order[0] == forge::EventType::FrameBegin);
    FORGE_CHECK(order[1] == forge::EventType::Quit);
    FORGE_CHECK(order[2] == forge::EventType::FrameEnd);
}
