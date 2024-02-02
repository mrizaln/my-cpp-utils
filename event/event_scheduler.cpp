#include "event_scheduler.hpp"

#include <bits/ranges_util.h>
#include <chrono>
#include <mutex>
#include <stdexcept>
#include <thread>

/// EventScheduler
EventScheduler::EventScheduler(std::chrono::milliseconds updateInterval)
    : m_updateInterval{ updateInterval }
    , m_stop{ false }
{
    if (updateInterval < s_minimumInterval) {
        throw std::runtime_error{ std::format("Interval must be higher or equal to {}", s_minimumInterval) };
    }
}

EventHandle EventScheduler::addEvent(std::string name, Callback&& callback, Interval interval)
{
    std::scoped_lock lock{ m_mutex };
    EventHandle      handle{ this, name };
    m_events.emplace_back(std::move(name), std::forward<Callback>(callback), interval);
    return handle;
}

void EventScheduler::removeEvent(const EventHandle& handle)
{
    std::scoped_lock lock{ m_mutex };
    std::erase_if(m_events, [&](const Event& e) { return handle.m_name == e.getName(); });
}

void EventScheduler::start()
{
    while (!m_stop) {
        std::this_thread::sleep_for(m_updateInterval);    // Adjust as needed

        for (std::scoped_lock lock{ m_mutex }; auto& event : m_events) {
            if (event.isActive() && event.isReady()) {
                event();
            }
        }
    }
}

EventScheduler::Event* EventScheduler::findEvent(const std::string& name)
{
    std::scoped_lock lock{ m_mutex };
    auto             found = std::ranges::find_if(m_events, [&](const Event& e) { return name == e.getName(); });
    if (found == m_events.end()) {
        return nullptr;
    }
    return &*found;
}

/// EventScheduler::Event

EventScheduler::Event::Event(std::string name, Callback&& callback, EventScheduler::Interval interval)
    : m_name{ std::move(name) }
    , m_callback{ std::move(callback) }
    , m_interval{ interval }
    , m_lastExecutionTime{ Clock::now() }
    , m_active{ true }
{
    if (interval < s_minimumInterval) {
        throw std::runtime_error{ std::format("Interval must be higher or equal to {}", s_minimumInterval) };
    }
}

/// EventHandle

EventHandle::EventHandle(EventScheduler* scheduler, std::string name)
    : m_scheduler{ scheduler }
    , m_name{ std::move(name) }
{
}

EventScheduler::Event* EventHandle::getEvent()
{
    using E    = EventScheduler::Event;
    auto found = std::ranges::find_if(m_scheduler->m_events, [this](const E& e) { return e.getName() == m_name; });
    if (found == m_scheduler->m_events.end()) {
        return nullptr;
    }
    return &*found;
}

EventScheduler::Event* EventHandle::getEvent() const
{
    return m_scheduler->findEvent(m_name);
}
