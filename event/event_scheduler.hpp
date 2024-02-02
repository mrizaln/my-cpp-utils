#ifndef EVENT_SCHEDULER_HPP_EUMDYTVA
#define EVENT_SCHEDULER_HPP_EUMDYTVA

#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>
#include <vector>

class EventHandle;

class EventScheduler
{
public:
    friend EventHandle;

    using Clock    = std::chrono::steady_clock;
    using Interval = std::chrono::milliseconds;
    using Callback = std::function<void()>;

    inline static const Interval s_minimumInterval{ 10 };

private:
    class Event
    {
    public:
        Event(std::string name, Callback&& callback, Interval interval);

        void operator()()
        {
            m_lastExecutionTime += m_interval;
            m_callback();
        }

        Interval           getInterval() const { return m_interval; }
        const std::string& getName() const { return m_name; }
        bool               isActive() const { return m_active; }
        void               activate() { m_active = true; }
        void               deactivate() { m_active = false; }
        bool               isReady() { return Clock::now() > (m_lastExecutionTime + m_interval); }

    private:
        std::string                    m_name;
        Callback                       m_callback;
        Interval                       m_interval;
        std::chrono::time_point<Clock> m_lastExecutionTime;
        bool                           m_active;
    };

public:
    EventScheduler(Interval updateInterval);

    [[nodiscard]]
    EventHandle addEvent(std::string name, Callback&& callback, Interval interval);
    void        removeEvent(const EventHandle& handle);
    void        start();
    void        stop() { m_stop = true; }

private:
    Event* findEvent(const std::string& name);

    Interval           m_updateInterval;
    std::vector<Event> m_events;
    std::mutex         m_mutex;
    std::atomic<bool>  m_stop;
};

class EventHandle
{
private:
    friend EventScheduler;

public:
    const std::string&       getName() const { return m_name; }
    EventScheduler::Interval getInterval() const { return getEvent()->getInterval(); }
    bool                     isActive() const { return getEvent()->isActive(); }
    void                     activate() { getEvent()->activate(); }
    void                     deactivate() { getEvent()->deactivate(); }

private:
    EventHandle(EventScheduler* scheduler, std::string name);
    EventScheduler::Event* getEvent();
    EventScheduler::Event* getEvent() const;

private:
    EventScheduler* m_scheduler;
    std::string     m_name;
};

#endif /* end of include guard: EVENT_SCHEDULER_HPP_EUMDYTVA */
