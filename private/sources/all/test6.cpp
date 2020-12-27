#include <iostream>

#include "rpnx/experimental/bitwise.hpp"
#include "rpnx/experimental/monoque.hpp"
#include <sstream>
#include <random>
#include <algorithm>

int main()
{
    std::cout <<"hello" << std::endl;
    std::cout << rpnx::bit_floor(5u) << std::endl;
    rpnx::experimental::monoque<std::string> m;
    //std::vector<std::string> m;

    //return 0;

    auto it = m.begin();

    RPNX_ASSERT(5 + it == it + 5);

    std::random_device r;
    for (int i = 0; i < 100; i++)
    {
        std::stringstream ss;
        ss << "hello " <<  i;
        m.emplace_back(ss.str());
        RPNX_ASSERT(m[i] == ss.str());
    }


    for (std::string & x : m)
    {
        std::cout << "line " <<  x << std::endl;
    }

    std::sort(m.begin(), m.end());

    std::cout << "===========" << std::endl;

    int c = 0;
    for (std::string & x : m)
    {
        std::cout << "line "<< c++ << " " << x << std::endl;
    }


     auto const & const_m = m;

     auto copy = m;
    for (std::string const & x : const_m)
    {
        std::cout << "line "<< c++ << " " << x << std::endl;
    }

    return 0;
}