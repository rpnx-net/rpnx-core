#include "rpnx/network.hpp"

#include <iostream>
#include <thread>
#include <chrono>

int main()
{
	try
	{

		rpnx::ip4_udp_socket socket;
		socket.open();
		socket.bind({ rpnx::ip4_address::any(), 0 });
		auto addr = socket.endpoint();

		std::cout << addr << std::endl;

		rpnx::ip4_udp_socket socket2;

		addr.address()[0] = 127;
		addr.address()[1] = 0;
		addr.address()[2] = 0;
		addr.address()[3] = 1;


		socket2.open();
		socket2.bind({ rpnx::ip4_address::any(), 0 });
		std::vector<std::byte> data_to_send = { (std::byte)1, (std::byte)2, (std::byte)3 };

		rpnx::net_send(socket2, addr, data_to_send.begin(), data_to_send.end());

		
		std::vector<std::byte> data;
		rpnx::ip4_udp_endpoint from;
		rpnx::net_receive(socket, from, std::back_inserter(data));

		std::cout << from << std::endl;

		for (auto const& x : data)
		{
			std::cout << (int) x << std::endl;
		}


	
	}
	catch (std::exception const & err)
	{
		std::cerr << err.what() << std::endl;
	}
}