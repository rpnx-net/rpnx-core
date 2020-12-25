#include <iostream>

#include "rpnx/experimental/bitwise.hpp"

int main()
{
    std::cout << rpnx::bit_reverse(std::uint32_t(2)) << std::endl;
}