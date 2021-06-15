#include "rpnx/experimental/network.hpp"

#include "rpnx/experimental/thread_pool.hpp"
#include <chrono>
#include <iostream>
#include <optional>
#include <rpnx/experimental/priority_dispatcher.hpp>
#include <thread>

int main()
{
    try
    {
        rpnx::experimental::network_enabled_context context;


        rpnx::experimental::ip6_address addr6;

        auto addr_native = addr6.native();

        //rpnx::experimental::ip6_tcp_acceptor acceptor6;
        rpnx::experimental::priority_dispatcher dispatch;
        rpnx::experimental::async_service async_svc;
        rpnx::experimental::priority_submitter submitter(dispatch, 1);

        std::cout << sizeof(std::promise<int>) << " " << sizeof(std::exception_ptr) <<  std::endl;

        rpnx::experimental::async_ip6_tcp_acceptor acceptor6;


    }
    catch (std::exception const& err)
    {
        std::cerr << err.what() << std::endl;
    }
}