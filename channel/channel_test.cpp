#include "channel.hpp"

#include <format>
#include <iostream>
#include <ranges>
#include <thread>

namespace rv = std::views;

int main()
{
    using namespace std::chrono_literals;

    auto [tx, rx] = makeChannel<int>();

    std::jthread producer1{ [tx]() mutable {
        for (auto i : rv::iota(0, 50)) {
            auto err = tx.send(std::move(i));
            if (err) {
                break;
            }
            std::this_thread::sleep_for(100ms);
        }
        std::cout << "producer1 done\n";
    } };

    std::jthread producer2{ [tx]() mutable {
        for (auto i : rv::iota(100, 119)) {
            auto err = tx.send(std::move(i));
            if (err) {
                break;
            }
            std::this_thread::sleep_for(211ms);
        }
        std::cout << "producer2 done\n";
    } };

    tx.close();

    while (true) {
        auto [value, err] = rx.receive();
        if (err) {
            std::cout << std::format("Error: {}\n", err.message());
            break;
        }
        std::this_thread::sleep_for(220ms);
        std::cout << std::format("received: {}\n", value);
    }
}
