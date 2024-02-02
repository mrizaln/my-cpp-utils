#include <chrono>
#include <functional>
#include <random>
#include <thread>
#include <map>

/*
 * TimerLoop is a class that runs a loop at a given interval.
 * Timer precision at minimum is 1ms.
 * It can be used to run a function at a given interval.
 * The decision of whether to run the loop in a separate thread or not is left to the user.
 * The loop is not started immediately at construction, but must be started manually with the `start()` function.
 */
class TimerLoop
{
public:
    using Interval   = std::chrono::milliseconds;
    using Clock      = std::chrono::steady_clock;
    using Callback   = std::function<void()>;
    using CallbackId = std::size_t;

private:
    Clock                          m_clock;
    Interval                       m_interval;
    std::map<CallbackId, Callback> m_callbacks;
    std::atomic<bool>              m_running = false;
    mutable std::mutex             m_runningMutex;
    mutable std::mutex             m_callbackMutex;

private:
    static CallbackId generateCallbackId()
    {
        thread_local std::mt19937_64                           generator{ std::random_device{}() };
        thread_local std::uniform_int_distribution<CallbackId> distribution;
        return distribution(generator);
    }

public:
    TimerLoop()                            = delete;
    TimerLoop(const TimerLoop&)            = delete;
    TimerLoop& operator=(const TimerLoop&) = delete;
    TimerLoop(TimerLoop&&)                 = delete;
    TimerLoop& operator=(TimerLoop&&)      = delete;

    TimerLoop(Interval interval)
        : m_interval(interval)
    {
        if (interval <= Interval::zero()) {
            throw std::invalid_argument("interval must be positive");
        }
    }

    ~TimerLoop()
    {
        stop();
        waitUntilStopped();
    }

    /*
     * Adds a callback to the loop. The callback will be called every interval.
     * The callback will be called from the loop thread, so it must be thread-safe and should not take too long, or
     * drift may occur.
     */
    CallbackId addCallback(Callback&& callback)
    {
        std::lock_guard lock{ m_callbackMutex };
        auto            id{ generateCallbackId() };
        m_callbacks.emplace(id, std::move(callback));
        return id;
    }

    /*
     * Removes a callback from the loop.
     */
    void removeCallback(CallbackId id)
    {
        std::lock_guard lock{ m_callbackMutex };
        m_callbacks.erase(id);
    }

    /*
     * Starts the loop. If `startsImmediately` is true, the first iteration will happen immediately, otherwise it will
     * happen after the interval. This function blocks until the loop is stopped.
     */
    void start(bool startsImmediately)
    {
        m_running = true;
        auto next = startsImmediately ? m_clock.now() : m_clock.now() + m_interval;

        while (m_running) {
            std::lock_guard runningLock{ m_runningMutex };

            std::this_thread::sleep_until(next);
            next += m_interval;

            // TODO: handle possible time drift because callbacks take too long
            for (std::lock_guard callbackLock{ m_callbackMutex }; auto& [id, callback] : m_callbacks) {
                callback();
            }
        }
    }

    /*
     * Waits until the loop is stopped. This function is blocking.
     */
    void waitUntilStopped() { std::lock_guard lock{ m_runningMutex }; }

    /*
     * Stops the loop (may take up to one interval to stop). This function is non-blocking. Use waitUntilStopped() to
     * wait until the loop is stopped. The loop can be restarted with start().
     */
    void stop() { m_running = false; }
};
