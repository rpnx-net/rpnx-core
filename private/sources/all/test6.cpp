#include <iostream>

#include "rpnx/experimental/bitwise.hpp"
#include "rpnx/experimental/monoque.hpp"

int main()
{
    std::cout <<"hello" << std::endl;
    std::cout << rpnx::bit_floor(3u) << std::endl;
    rpnx::experimental::monoque<int> m;

    return 0;
    for (int i = 0; i < 100; i++)
    {
        m.emplace_back(i);
    }

    for (std::size_t i = 0; i < m.size(); i++)
    {
        std::cout << m[i] << std::endl;
    }

    return 0;
}