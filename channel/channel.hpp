#ifndef CHANNEL_HPP_3QWAFGF4
#define CHANNEL_HPP_3QWAFGF4

#include <atomic>
#include <cassert>
#include <concepts>
#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <utility>

#if defined(CHANNEL_RECEIVE_TYPE) or defined(CHANNEL_SEND_TYPE)
#    error "CHANNEL_RECEIVE_TYPE or CHANNEL_SEND_TYPE should not be defined"
#endif

#if not defined(CHANNEL_THROW)
#    ifdef __cpp_lib_expected
#        include <expected>

#        define CHANNEL_RECEIVE_TYPE(T) std::expected<T, std::error_code>
#        define CHANNEL_SEND_TYPE       std::error_code
#        define CHANNEL_EXPECTED        1
#        define CHANNEL_THROW           0
#    else
#        define CHANNEL_RECEIVE_TYPE(T) std::pair<T, std::error_code>
#        define CHANNEL_SEND_TYPE       std::error_code
#        define CHANNEL_EXPECTED        0
#        define CHANNEL_THROW           0
#    endif
#elif defined(CHANNEL_THROW)
#    define CHANNEL_RECEIVE_TYPE(T) T
#    define CHANNEL_SEND_TYPE       void
#    define CHANNEL_EXPECTED        0
#    define CHANNEL_THROW           1
#else
#    error "CHANNEL_RECEIVE_TYPE should not be defined"
#endif

template <std::move_constructible T>
class Sender;

template <std::move_constructible T>
class Receiver;

template <std::move_constructible T>
struct Channel
{
    friend class Sender<T>;
    friend class Receiver<T>;

private:
    std::deque<T>            m_queue;
    std::mutex               m_mutex;
    std::condition_variable  m_cv;
    std::atomic<std::size_t> m_senders   = 0;
    std::atomic<std::size_t> m_receivers = 0;
};

template <std::move_constructible T>
class Sender
{
public:
    Sender(std::shared_ptr<Channel<T>> channel)
        : m_channel{ std::move(channel) }
    {
        auto count = m_channel->m_senders.fetch_add(1, std::memory_order_relaxed);
    }

    ~Sender() { close(); }

    void close()
    {
        if (m_channel) {
            auto count = m_channel->m_senders.fetch_sub(1, std::memory_order_acq_rel);
            m_channel->m_cv.notify_all();
            m_channel.reset();
        }
    }

    Sender(const Sender& other)
        : m_channel{ other.m_channel }
    {
        auto count = m_channel->m_senders.fetch_add(1, std::memory_order_relaxed);
    }

    Sender<T>& operator=(const Sender& other)
    {
        if (this != &other && m_channel != other.m_channel) {
            if (m_channel) {
                other.close();
            }
            m_channel  = other.m_channel;
            auto count = m_channel->m_senders.fetch_add(1, std::memory_order_relaxed);
        }
        return *this;
    }

    Sender(Sender&&)               = default;
    Sender<T>& operator=(Sender&&) = default;

    CHANNEL_SEND_TYPE send(T&& value)
    {
        assert(m_channel);
        auto& [queue, mutex, cv, sc, rc] = *m_channel;
        {
            std::unique_lock lock{ m_channel->m_mutex };
            if (rc.load() == 0) {
#if CHANNEL_THROW
                throw std::runtime_error{ "receive on closed channel" };
#else
                return std::make_error_code(std::errc::broken_pipe);
#endif
            }
            queue.push_back(std::move(value));
        }
        m_channel->m_cv.notify_one();
#if CHANNEL_THROW
        return;
#else
        return std::error_code{};
#endif
    }

private:
    std::shared_ptr<Channel<T>> m_channel;
};

template <std::move_constructible T>
class Receiver
{
public:
    Receiver(std::shared_ptr<Channel<T>> channel)
        : m_channel{ std::move(channel) }
    {
        m_channel->m_receivers.fetch_add(1);
    }

    ~Receiver() { close(); }

    void close()
    {
        if (m_channel) {
            m_channel->m_receivers.fetch_sub(1);
            m_channel->m_cv.notify_all();
            m_channel.reset();
        }
    }

    Receiver(const Receiver&)               = delete;
    Receiver<T>& operator=(const Receiver&) = delete;

    Receiver(Receiver&&)               = default;
    Receiver<T>& operator=(Receiver&&) = default;

    CHANNEL_RECEIVE_TYPE(T) receive()
    {
        assert(m_channel);
        auto& [queue, mutex, cv, sc, rc] = *m_channel;
        {
            std::unique_lock lock{ m_channel->m_mutex };
            if (queue.empty()) {
                m_channel->m_cv.wait(lock, [&] { return !queue.empty() || sc.load() == 0; });

                if (sc.load() == 0) {
#if CHANNEL_THROW
                    throw std::runtime_error{ "receive on closed channel" };
#elif CHANNEL_EXPECTED
                    return std::unexpected{ std::make_error_code(std::errc::broken_pipe) };
#else
                    return std::make_pair(T{}, std::make_error_code(std::errc::broken_pipe));
#endif
                }
            }
            auto value = std::move(queue.front());
            queue.pop_front();

#if CHANNEL_EXPECTED
            return std::expected<T, std::error_code>{ std::move(value) };
#else
            return std::make_pair(std::move(value), std::error_code{});
#endif
        }
    }

private:
    std::shared_ptr<Channel<T>> m_channel;
};

template <std::move_constructible T>
std::pair<Sender<T>, Receiver<T>> makeChannel()
{
    auto channel = std::make_shared<Channel<T>>();
    return std::make_pair(Sender<T>{ channel }, Receiver<T>{ channel });
}

#endif /* end of include guard: CHANNEL_HPP_3QWAFGF4 */
