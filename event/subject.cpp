#include <cstdlib>
#include <future>
#include <iostream>
#include <map>
#include <string>
#include <functional>
#include <thread>

class Subject
{
public:
    using EventHandler = std::function<void(const std::string&)>;

private:
    std::multimap<std::string, EventHandler> m_eventsHandlers;
    std::atomic<bool>                        m_isRunning = true;

public:
    void main()
    {
        auto counter = 0;
        while (m_isRunning) {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(1s);

            for (const auto& [name, handler] : m_eventsHandlers) {
                if (name == "end_event") {
                    continue;
                }
                handler(name);
            }

            if (counter++ > 10) {
                if (auto found = m_eventsHandlers.find("end_event"); found != m_eventsHandlers.end()) {
                    const auto& [name, handler] = *found;
                    handler(name);
                }
            }
            std::cout << "subject: " << counter << '\n';
        }
    }

    std::size_t hasEvent(std::string eventName) const { return m_eventsHandlers.count(eventName); }

    void stop() { m_isRunning = false; }

    Subject& subscribe(std::string eventName, EventHandler handler)
    {
        m_eventsHandlers.emplace(eventName, handler);
        return *this;
    }
};

void freeFunction(const std::string& string)
{
    std::cout << "Free function: " << string << '\n';
}

int main()
{
    Subject subject;

    auto subjectDefer = std::async(std::launch::deferred, &Subject::main, &subject);

    subject
        .subscribe(
            "event1",
            [](const std::string& name) {
                std::cout << name << ": Hello, world!\n";
                freeFunction(name);
            }
        )
        .subscribe("event1", [](const std::string& name) { std::cout << name << ": Hi mom!\n"; })
        .subscribe(
            "event2", [](const std::string& name) { std::cout << name << ": i'm kinda liking this event thing!\n"; }
        )
        .subscribe("end_event", [&subject](const std::string& name) {
            subject.stop();
            printf("end_event: %s\n", name.c_str());
            std::exit(0);
        });

    subjectDefer.wait();

    return 0;
}
