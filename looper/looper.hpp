#ifndef LOOPER_HPP_T34DH4FJ
#define LOOPER_HPP_T34DH4FJ

#include <atomic>
#include <concepts>
#include <condition_variable>
#include <limits>
#include <mutex>
#include <stop_token>
#include <thread>
#include <utility>

class Looper
{
public:
    Looper()
        : m_resumeCount{ s_foreverFlag }
        , m_suspended{ true }
    {
    }

    // Will block if the loop is running until stop is requested, only then this function returns
    template <typename Fn, typename... Args>
        requires std::invocable<Fn, Args...>
    void run(Fn&& fn, Args&&... args)
    {
        if (m_running) {
            stop();
            std::unique_lock lock(m_mutex);
        }

        m_suspended = false;

        // blocking until the underlying thread is replaced
        m_thread = std::jthread{
            [this, fn = std::forward<Fn>(fn), ... args = std::forward<Args>(args)](std::stop_token stopToken) mutable {
                m_running = true;
                while (!stopToken.stop_requested()) {
                    std::unique_lock lock(m_mutex);

                    fn(std::forward<Args>(args)...);

                    m_cv.wait(lock, [this, stopToken] {
                        if (m_resumeCount != s_foreverFlag) {
                            if (m_resumeCount > 0) {
                                --m_resumeCount;
                                return true;
                            }
                            m_resumeCount = m_suspended ? s_foreverFlag : m_resumeCount.load();
                        }
                        return !m_suspended || stopToken.stop_requested();
                    });
                }
                m_running = false;
            }
        };
    }

    void suspend()
    {
        m_suspended = true;
        m_cv.notify_one();
    }

    void resume()
    {
        m_suspended = false;
        m_cv.notify_one();
    }

    void stop()
    {
        if (m_running) {
            m_thread.request_stop();
            m_cv.notify_one();
        }
    }

    void resumeCount(unsigned long count)
    {
        if (!m_suspended || count <= 0) {
            return;
        }

        m_resumeCount = count;
        m_cv.notify_one();
        return;
    }

    bool isRunning() { return m_running; }
    bool isSuspended() { return m_suspended; }

private:
    static constexpr auto s_foreverFlag = std::numeric_limits<unsigned long>::min();

    std::mutex                 m_mutex;
    std::condition_variable    m_cv;
    std::jthread               m_thread;
    std::atomic<unsigned long> m_resumeCount;
    std::atomic<bool>          m_suspended;
    std::atomic<bool>          m_running;
};

#endif /* end of include guard: LOOPER_HPP_T34DH4FJ */
