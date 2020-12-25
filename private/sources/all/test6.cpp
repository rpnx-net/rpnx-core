#include <iostream>

#include "rpnx/experimental/bitwise.hpp"
#include "rpnx/experimental/monoque.hpp"

int main()
{
    rpnx::experimental::monoque<int> m;

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