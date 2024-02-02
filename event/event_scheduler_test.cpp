#include "event_scheduler.hpp"

#include <iostream>
#include <thread>

int main()
{
    using namespace std::chrono_literals;
    EventScheduler scheduler{ 50ms };

    // Example events
    auto event1 = scheduler.addEvent(
        "A", []() { std::cout << "A executed!\n"; }, 2'000ms
    );
    auto event2 = scheduler.addEvent(
        "B", []() { std::cout << "B executed!\n"; }, 500ms
    );

    // Start the event scheduler in a separate thread
    std::jthread schedulerThread(&EventScheduler::start, &scheduler);

    // Do other work in the main thread
    std::this_thread::sleep_for(10s);

    // Deactivate an event
    std::cout << "deactivating " << event2.getName() << '\n';
    event2.deactivate();

    std::this_thread::sleep_for(6s);

    // activate an event
    std::cout << "activating " << event2.getName() << '\n';
    event2.activate();

    // add new event
    auto event3 = scheduler.addEvent(
        "C", []() { std::cout << "C executed!\n"; }, 100ms
    );
    std::cout << "add new event " << event3.getName() << '\n';

    std::this_thread::sleep_for(3s);

    // remove an event
    std::cout << "removing " << event3.getName() << '\n';
    scheduler.removeEvent(event3);

    std::this_thread::sleep_for(7s);

    // Wait for the scheduler thread to finish
    scheduler.stop();

    return 0;
}
