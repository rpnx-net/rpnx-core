#include "rpnx/experimental/priority_dispatcher.hpp"

#include <chrono>
#include <thread>
#include <iostream>

int main()
{
    rpnx::experimental::priority_dispatcher dispatch_engine;

    dispatch_engine.set_service_thread_count(1);

    std::this_thread::sleep_for(std::chrono::seconds(1));
    // there should only be one service thread now...

    for (int i = 0; i < 10; i++)
    {
        dispatch_engine.submit(
            [&] {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                std::cout << "Low priority operation" << std::endl;
            },
            1);
    }
    for (int i = 0; i < 10; i++)
    {
        dispatch_engine.submit(
            [&] {
              std::this_thread::sleep_for(std::chrono::seconds(1));
              std::cout << "High priority operation" << std::endl;
            },
            2);
    }

    dispatch_engine.finish_all();
}