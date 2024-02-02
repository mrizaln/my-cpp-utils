#include <future>
#include <iostream>
#include <format>
#include <memory>
#include <optional>
#include <thread>

#include "timer.hpp"

class Singleton
{
private:
    inline static std::unique_ptr<Singleton> INSTANCE{ nullptr };
    util::Timer                              m_timer;

private:
    Singleton()
        : m_timer{ "Singleton" }
    {
    }

public:
    static Singleton& getInstance()
    {
        if (INSTANCE.get() == nullptr)
            INSTANCE.reset(new Singleton{});

        return *INSTANCE;
    }

    static void drop() { INSTANCE.reset(); }
};

void heavyTask()
{
    util::Timer timer{ "heavyTask" };

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);
}

int main()
{
    // enable printing
    util::Timer::s_doPrint = true;

    std::cout << "start\n";

    Singleton::getInstance();
    Singleton::getInstance();
    Singleton::getInstance();
    auto future1 = std::async(std::launch::async, [] { heavyTask(); });
    Singleton::drop();
    Singleton::getInstance();
    Singleton::getInstance();
    Singleton::drop();
    Singleton::getInstance();
    Singleton::getInstance();
    auto future2 = std::async(std::launch::async, [] { heavyTask(); });
    Singleton::getInstance();
    Singleton::getInstance();
    auto future3 = std::async(std::launch::async, [] { heavyTask(); });
    Singleton::drop();
    Singleton::getInstance();

    std::cout << "finish\n";

    return 0;
}
