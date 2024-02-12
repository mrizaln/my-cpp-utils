#include "double_buffer_atomic.hpp"

#include "print.hpp"

#include <csignal>
#include <string>
#include <thread>

std::atomic<bool> g_interrupt{ false };

void interruptHandler(int /* sig */)
{
    std::puts("Interrupt signal received. Exiting...");
    g_interrupt.store(true);
    g_interrupt.notify_all();
}

int main()
{
    using namespace std::chrono_literals;

    using Buffer       = std::string;
    using DoubleBuffer = DoubleBufferAtomic<Buffer, false>;    // on the stack (using std::array)
    // using DoubleBuffer = DoubleBufferAtomic<Buffer, true>;     // on the heap (using std::unique_ptr<Buffer[]>)

    std::signal(SIGINT, interruptHandler);

    DoubleBuffer db{ "start" };

    println("sizeof DoubleBufferAtomic<Buffer>= {}", sizeof(DoubleBuffer));
    println("sizeof Buffer [array] (dy: {:<5})= {}", DoubleBuffer::s_dynamicAlloc, sizeof(DoubleBuffer::BuffersType));
    println("sizeof Buffer                    = {}", sizeof(DoubleBuffer::BufferType));
    println("sizeof BufferUpdateStatus        = {}", sizeof(DoubleBuffer::BufferUpdateStatus));
    println("sizeof BufferIndex               = {}", sizeof(DoubleBuffer::BufferIndex));

    // producer thread
    std::jthread t1([&db](const std::stop_token& st) {
        int counter = 0;
        while (!st.stop_requested()) {
            /* pretend to do some work */

            db.updateBuffer([&counter](Buffer& buffer) {
                buffer = std::format("{0} ==> {0:032b}", counter);
                println("t1: [U] buffer: {}", buffer);
                std::this_thread::sleep_for(239ms);
            });

            ++counter;
            println("t1: counter: {}", counter);
            std::this_thread::sleep_for(134ms);
        }
    });

    // consumer thread
    std::jthread t2([&db](const std::stop_token& st) {
        while (!st.stop_requested()) {
            /* pretend to do some work */

            const auto& buffer = db.swapBuffers();

            println("t2: (S) buffer: {}", buffer);
            std::this_thread::sleep_for(1078ms);
        }
    });

    // // second consumer thread (unsynchronized access to the buffer)
    // std::jthread t3([&db](const std::stop_token& st) {
    //     while (!st.stop_requested()) {
    //         /* pretend to do some work */
    //
    //         const auto& buffer = db.getFront();
    //
    //         println("t3: (F) buffer: {}", buffer); // unsynchronized access: the buffer might be swapped while read
    //         std::this_thread::sleep_for(108ms);
    //     }
    // });

    // multiple consumer might be possible with sub-consumer like following:
    // - consumer: swaps the buffer
    // - consumer: spawns (sync or async) sub-consumers
    //      - sub-consumer 1: reads the buffer
    //      - sub-consumer 2: reads the buffer
    //      - sub-consumer 3: reads the buffer
    //      - ...
    // - consumer: waits for all sub-consumers to finish (if async, possibly with a barrier)
    // - consumer: continues with the next swap

    g_interrupt.wait(false);

    return 0;
}
