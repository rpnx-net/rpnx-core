#include "rpnx/serial_traits.hpp"

#include <iostream>
#include <thread>
#include <chrono>

int main()
{
	std::string input = "Hello world, serial test!";
	std::cout << "size=" << input.size() << std::endl;


	std::vector<char> input2(input.begin(), input.end());

	std::vector<std::vector<char>> input3 { input2, {}, input2 };
	std::vector<std::uint8_t> output;
	rpnx::quick_functor_serialize(input3, [&output](std::size_t n)
		{
			std::cout << "quick_functor_serialize requested an iterator that can store " << n << " bytes" << std::endl;
			std::size_t size_old = output.size();
			output.resize(size_old + n);
			return output.begin() + size_old;
		});
	for (auto const& x : output)
	{
		std::cout << (int)x << " ";
	}
	std::cout << std::endl;
}