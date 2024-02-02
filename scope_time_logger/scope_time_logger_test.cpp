#include <iostream>
#include <thread>

#include "scope_time_logger.hpp"

using namespace std::chrono_literals;

using Logger = util::ScopeTimeLogger<>;

void func1()
{
    auto handle = Logger::add("func1");
    std::this_thread::sleep_for(10ms);
}

void func2()
{
    auto handle = Logger::add("func2");
    std::this_thread::sleep_for(20ms);
}

void func3()
{
    auto handle = Logger::add("func3");
    std::this_thread::sleep_for(15ms);
}

void func4()
{
    auto handle = Logger::add("func4");
    std::this_thread::sleep_for(49ms);
}

int main()
{
    Logger::start();

    int counter{ 0 };
    while (true) {
        auto handle{ Logger::add("main") };
        func1();
        func2();
        func3();

        if (counter > 100 && counter < 150) {
            func4();
        }

        if (const auto& data{ Logger::read(Logger::ACTIVE) }; data.has_value()) {
            for (const auto& e : data.value()) {
                std::cout << e.first << " " << e.second << '\n';
            }
        }
        counter = (counter + 1) % 150;
    }

    return 0;
}
