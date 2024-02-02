#include "threadpool.hpp"

#include <format>
#include <ostream>
#include <sstream>
#include <thread>
#include <tuple>

std::size_t getTheadIdAsInteger()
{
    std::stringstream ss;
    ss << std::this_thread::get_id();
    return std::stoull(ss.str());
}

auto mainThreadId = getTheadIdAsInteger();
auto _            = std::ignore;

template <typename... Args>
void print(std::format_string<Args...> fmt, Args... args)
{
    auto string   = std::format(fmt, std::forward<Args>(args)...);
    auto threadId = getTheadIdAsInteger();
    std::cout << std::format("t[{:x}] {}", threadId, string);
}

void demo(std::size_t numThread)
{
    ThreadPool threadPool{ numThread };

    using namespace std::chrono_literals;
    _ = threadPool.enqueue([] {
        int counter = 10;
        while (counter--) {
            std::this_thread::sleep_for(100ms);
            print("task 0: {}\n", counter);
        }
        print("task 0 done\n");
    });

    _ = threadPool.enqueue([] {
        int counter = 10;
        while (counter--) {
            std::this_thread::sleep_for(250ms);
            print("task 1: {}\n", counter);
        }
        print("task 1 done\n");
    });

    _ = threadPool.enqueue([] {
        int counter = 10;
        while (counter--) {
            std::this_thread::sleep_for(700ms);
            print("task 2: {}\n", counter);
        }
        print("task 2 done\n");
    });

    auto fut3 = threadPool.enqueue([] {
        int counter = 10;
        while (counter--) {
            std::this_thread::sleep_for(250ms);
            print("task 3: {}\n", counter);
        }
        print("task 3 done\n");
        return 12;
    });

    auto fut4 = threadPool.enqueue([] {
        int counter = 10;
        while (counter--) {
            std::this_thread::sleep_for(700ms);
            print("task 4: {}\n", counter);
        }
        print("task 4 done\n");
        return 42;
    });

    _ = threadPool.enqueue([] {
        int counter = 10;
        while (counter--) {
            std::this_thread::sleep_for(100ms);
            print("task 5: {}\n", counter);
        }
        print("task 5 done\n");
    });

    int counter = 5;
    while (counter--) {
        std::this_thread::sleep_for(1s);
        print("main: threadPool queued tasks: {}\n", threadPool.queuedTasks());
    }

    _ = threadPool.enqueue([] {
        int counter = 10;
        while (counter--) {
            std::this_thread::sleep_for(100ms);
            print("task 6: {}\n", counter);
        }
        print("task 6 done\n");
    });

    std::cout << "Future: blocking main thread to get task 4 future...\n";
    auto fut4Value = fut4.get();
    std::cout << "Future: task 4 future: " << fut4Value << '\n';
    std::cout << "Future: blocking main thread to get task 3 future...\n";
    auto fut3Value = fut3.get();
    std::cout << "Future: task 3 future: " << fut3Value << '\n';
}

class Args
{
private:
    inline static int m_counter{ 0 };
    std::vector<int>  m_vec;
    int               m_id;
    int               m_moveCount{ 0 };

public:
    Args(std::size_t a)
        : m_vec(a)
        , m_id{ m_counter++ }
    {
        print("Args main constructor called: {}\n", str());
    }

    Args(const Args& other)
        : m_vec{}
        , m_id{ m_counter++ }
    {
        m_vec = other.m_vec;
        print("Args copy constructor called: {}\n", str());
    }

    Args(Args&& other)
        : m_vec{}
        , m_id{ m_counter++ }
    {
        m_vec.swap(other.m_vec);
        // m_vec = other.m_vec;
        print("Args move constructor called: {}\n", str());
        m_moveCount = other.m_moveCount + 1;
    }

    void push(int v) { m_vec.push_back(v); }

    ~Args() { print("Args destructor called: {}\n", str()); }

    std::string str() const
    {
        return std::format(
            "Args [{} | {} | {} | {:x}]",
            m_id,
            m_moveCount,
            static_cast<int>(m_vec.size()),
            m_vec.size() == 0 ? (std::size_t)-1 : (std::size_t)&m_vec.front()
        );
    }

    friend std::ostream& operator<<(std::ostream& out, const Args& args)
    {
        out << args.str();
        return out;
    }
};

void nonTrivialArgs(std::size_t numThread)
{
    ThreadPool threadPool{ numThread };

    using namespace std::chrono_literals;

    Args args{ 100'000 };

    print("before enqueu: {}\n", args.str());
    _ = threadPool.enqueue([args = std::move(args)]() mutable {
        int counter = 10;
        while (counter--) {
            args.push(counter);
            print("task: {} | {}\n", counter, args.str());
            std::this_thread::sleep_for(100ms);
        }
    });

    print("after enqueue (args moved): {}\n", args.str());

    int counter = 5;
    while (counter--) {
        std::this_thread::sleep_for(1s);
        print("main: threadPool queued tasks: {}\n", threadPool.queuedTasks());
    }
}

int main(int argc, char* argv[])
{
    std::size_t numThread{ 2 };
    if (argc > 1) {
        std::stringstream ss{ argv[1] };
        ss >> numThread;
    }
    numThread = numThread > 0 ? numThread : 1;
    demo(numThread);

    nonTrivialArgs(numThread);

    return 0;
}
