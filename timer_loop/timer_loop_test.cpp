#include <iostream>
#include <thread>

#include "timer_loop.hpp"

int main()
{
    using namespace std::chrono_literals;

    std::jthread thread;
    TimerLoop    loop{ 1s };

    auto callbackId1 = loop.addCallback([]() { std::cout << "Hello, world!\n"; });
    auto callbackId2 = loop.addCallback([]() { std::cout << "Goodbye, world!\n"; });

    thread = std::jthread{ [&loop]() { loop.start(true); } };

    std::this_thread::sleep_for(5s);

    loop.removeCallback(callbackId2);
    auto callbackId3 = loop.addCallback([]() { std::cout << "Hello again, world!\n"; });

    std::this_thread::sleep_for(5s);

    loop.removeCallback(callbackId1);
    loop.removeCallback(callbackId3);
    loop.addCallback([]() { std::cout << "Goodbye again, world!\n"; });

    std::this_thread::sleep_for(5s);
}
