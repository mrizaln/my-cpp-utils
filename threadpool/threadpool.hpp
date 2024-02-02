#ifndef THREADPOOL_HPP_YWONTBSQ
#define THREADPOOL_HPP_YWONTBSQ

#if __cplusplus >= 202302L
#    include <functional>
#else
#    include "move_only_function.hpp"
#endif

#include <format>
#include <future>
#include <iostream>
#include <thread>
#include <type_traits>
#include <vector>
#include <deque>
#include <mutex>
#include <condition_variable>

#define USE_PACKAGED_TASK 0

class ThreadPool
{
public:
#if __cplusplus >= 202302L
    using Task_type = std::move_only_function<void()>;
#else
    using Task_type = MoveOnlyFunction<void()>;    // until C++23's std::move_only_function is available, use this
#endif

private:
    std::vector<std::jthread> m_threads;
    std::deque<Task_type>     m_tasks;
    std::mutex                m_mutex;
    std::condition_variable   m_condition;
    bool                      m_stop = false;

public:
    ThreadPool(size_t numThreads)
    {
        for (size_t i = 0; i < numThreads; ++i) {
            m_threads.emplace_back([this] {
                while (true) {
                    Task_type task;
                    {
                        std::unique_lock lock{ m_mutex };

                        m_condition.wait(lock, [this]() {
                            auto condition = !m_tasks.empty() || m_stop;
                            return condition;
                        });

                        if (m_stop && m_tasks.empty()) {
                            return;
                        }

                        task = std::move(m_tasks.front());
                        m_tasks.pop_front();
                    }
                    task();
                }
            });
        }
    }

    ~ThreadPool()
    {
        std::cout << std::format("ThreadPool destructor called, there are [{}] tasks left\n", m_tasks.size());
        stopPool();
    }

    template <typename... Args, std::invocable<Args...> Func>
    [[nodiscard]] auto enqueue(Func&& func, Args&&... args) -> std::future<std::invoke_result_t<Func, Args...>>
    {
#if USE_PACKAGED_TASK
        auto packagedTask = std::packaged_task<std::invoke_result_t<Func, Args...>()>{
            [func = std::forward<Func>(func), ... args = std::forward<Args>(args)]() mutable {
                return func(std::forward<Args>(args)...);
            }
        };
        auto res = packagedTask.get_future();
        {
            std::unique_lock lock{ m_mutex };
            m_tasks.emplace_back([packagedTask = std::move(packagedTask)]() mutable { packagedTask(); });
        }
        m_condition.notify_one();

        return res;
#else
        using Return_type = std::invoke_result_t<Func, Args...>;
        std::promise<Return_type> promise;

        auto future{ promise.get_future() };
        {
            std::unique_lock lock{ m_mutex };
            m_tasks.emplace_back([promise  = std::move(promise),
                                  func     = std::forward<Func>(func),
                                  ... args = std::forward<Args>(args)]() mutable {
                try {
                    if constexpr (std::same_as<Return_type, void>) {
                        func(std::forward<Args>(args)...);
                        promise.set_value();
                    } else {
                        promise.set_value(func(std::forward<Args>(args)...));
                    }
                } catch (...) {
                    promise.set_exception(std::current_exception());
                }
            });
        }
        m_condition.notify_one();

        return future;
#endif
    }

    std::size_t queuedTasks()
    {
        std::unique_lock lock{ m_mutex };
        return m_tasks.size();
    }

    // after this call, the instance will effectively become unusable.
    // create a new instance if you want to use ThreadPool again.
    void stopPool(bool ignoreQueuedTasks = false)
    {
        {
            std::unique_lock lock{ m_mutex };
            if (ignoreQueuedTasks) {
                m_tasks.clear();
            }
            m_stop = true;
        }
        m_condition.notify_all();

        // for some reason, this prevents stray func destructor to be ran
        for (auto& thread : m_threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }
};

#endif /* end of include guard: THREADPOOL_HPP_YWONTBSQ */
