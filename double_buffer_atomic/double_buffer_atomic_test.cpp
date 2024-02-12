#include "double_buffer_atomic.hpp"

#include "print.hpp"

#include <csignal>
#include <string>
#include <thread>

using namespace std::chrono_literals;
using Buffer = std::string;

std::atomic<bool> g_interrupt{ false };

void interruptHnadler(int /* sig */)
{
    // trigger tsan "signal-unsafe call inside of signal" possibly because this function may allocate memory
    // println("Interrupt signal received. Exiting...");
    std::puts("Interrupt signal received. Exiting...");
    g_interrupt.store(true);
    g_interrupt.notify_all();
}

int main()
{
    std::signal(SIGINT, interruptHnadler);

    // println("hello world");
    DoubleBufferAtomic<Buffer> db{ "start" };

    // producer thread
    std::jthread t1([&db](std::stop_token st) {
        int counter = 0;
        while (!st.stop_requested()) {
            db.updateBuffer([&counter](Buffer& buffer) {
                buffer = std::format("{0} ==> {0:032b}", counter);
                println("t1: updated buffer: {}", buffer);
                std::this_thread::sleep_for(239ms);
            });
            ++counter;
            println("t1: counter: {}", counter);
            std::this_thread::sleep_for(134ms);
        }
    });

    // consumer thread
    std::jthread t2([&db](std::stop_token st) {
        while (!st.stop_requested()) {
            const auto& buffer = db.swapBuffers();
            println("t2: swapped buffer: {}", buffer);
            std::this_thread::sleep_for(1008ms);
        }
    });

    g_interrupt.wait(false);

    return 0;
}
