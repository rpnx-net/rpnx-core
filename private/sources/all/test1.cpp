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
		socket2.send({ (std::byte)1, (std::byte)2, (std::byte)3 }, addr);

		auto res = socket.receive();

		std::cout << res.first << std::endl;

		for (auto const& x : res.second)
		{
			std::cout << (int) x << std::endl;
		}


		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	catch (std::exception const & err)
	{
		std::cerr << err.what() << std::endl;
	}
}