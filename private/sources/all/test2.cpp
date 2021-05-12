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
        /*
        rpnx::experimental::ip4_tcp_acceptor server;

        server.listen(rpnx::experimental::ip4_tcp_endpoint(rpnx::experimental::ip4_address(0, 0, 0, 0), 8080));

        rpnx::experimental::ip4_tcp_connection con = rpnx::experimental::net_accept_connection(server);

        std::cout << con.endpoint() << std::endl;
        std::cout << con.peer_endpoint() << std::endl;

        std::string hello = "hello";

        con.send(hello.begin(), hello.end());

         */
        rpnx::experimental::ip6_address addr6;

        auto addr_native = addr6.native();

        //rpnx::experimental::ip6_tcp_acceptor acceptor6;
        rpnx::experimental::priority_dispatcher dispatch;
        rpnx::experimental::async_service async_svc;
        rpnx::experimental::priority_submitter submitter(dispatch, 1);

        std::cout << sizeof(std::promise<int>) << " " << sizeof(std::exception_ptr) <<  std::endl;

        //std::optional<rpnx::experimental::async_ip6_tcp_autoacceptor> auto_acceptor;

        /*auto_acceptor.emplace(async_svc, dispatch, acceptor6, [this](rpnx::experimental:async_ip6_tcp_connection) {


                              });
        */



    }
    catch (std::exception const& err)
    {
        std::cerr << err.what() << std::endl;
    }
}