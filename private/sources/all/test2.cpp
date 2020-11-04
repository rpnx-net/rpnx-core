#include "rpnx/network.hpp"

#include <iostream>
#include <thread>
#include <chrono>

int main()
{
	try
	{
		rpnx::ip4_tcp_acceptor server;

		server.listen(rpnx::ip4_tcp_endpoint( rpnx::ip4_address(0,0,0,0), 8080 ));

		
		rpnx::ip4_tcp_connection con = rpnx::net_accept_connection(server);

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