#include "rpnx/experimental/network.hpp"

#include "rpnx/experimental/thread_pool.hpp"
#include <chrono>
#include <iostream>
#include <thread>

int main()
{
	try
	{
                rpnx::experimental::ip4_tcp_acceptor server;

		server.listen(rpnx::experimental::ip4_tcp_endpoint( rpnx::experimental::ip4_address(0,0,0,0), 8080 ));

		
		rpnx::experimental::ip4_tcp_connection con = rpnx::experimental::net_accept_connection(server);

		std::cout << con.endpoint() << std::endl;
		std::cout << con.peer_endpoint() << std::endl;

		std::string hello = "hello";


		con.send(hello.begin(), hello.end());


	
	}
	catch (std::exception const & err)
	{
		std::cerr << err.what() << std::endl;
	}
}