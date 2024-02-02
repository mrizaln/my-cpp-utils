#include "suspendable_loop.hpp"

#include "print.hpp"
#include "type_name.hpp"

#include <iostream>
#include <thread>

int main()
{
    using namespace std::chrono_literals;

    auto func = [n = 0]() mutable {
        std::cout << n++ << '\n';
        std::this_thread::sleep_for(100ms);
    };
    Looper loop{};

    print("sizeof {} = {}\n", type_name(loop), sizeof(loop));
    print("sizeof {} = {}\n", type_name<std::atomic<bool>>(), sizeof(std::atomic<bool>));
    print("sizeof {} = {}\n", type_name<std::atomic<long>>(), sizeof(std::atomic<long>));

    print("Nothing happens for 5000ms\n");
    std::this_thread::sleep_for(5s);

    print("Loop started\n");
    loop.run(func);

    for (int i = 0; i < 10; ++i) {
        print("Main: do something\n");
        std::this_thread::sleep_for(150ms);
    }
    print("Loop suspended\n");
    loop.suspend();

    print("Nothing happens for 5000ms\n");
    std::this_thread::sleep_for(5000ms);

    print("Loop resumed\n");
    loop.resume();

    for (int i = 0; i < 10; ++i) {
        print("Main: do something\n");
        std::this_thread::sleep_for(150ms);
    }
    print("Loop suspended\n");
    loop.suspend();

    for (int i = 0; i < 5; ++i) {
        print("Loop resume once\n");
        loop.resumeCount(1);
        print("Main: do something\n");
        std::this_thread::sleep_for(500ms);
    }
    print("Nothing happens for 5000ms\n");
    std::this_thread::sleep_for(5000ms);

    print("Requesting loop to stop\n");
    loop.stop();
    print("Nothing happens for 1000ms\n");
    std::this_thread::sleep_for(1000ms);
    print("Loop stopped\n");

    loop.resume();     // will do nothing
    loop.suspend();    // will do nothing

    print("Starting loop again\n");
    loop.run(func);

    for (int i = 0; i < 10; ++i) {
        print("Main: do something\n");
        std::this_thread::sleep_for(150ms);
    }

    print("Requesting loop to stop\n");
    loop.stop();
    print("Loop stopped\n");
}
