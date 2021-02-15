#include "rpnx/environment.hpp"
#include <iostream>

int main()
{
    auto env = rpnx::os::environment();


    for (auto & x : env)
    {
        std::cout << "Key=" << x.first << " Value=" << x.second << std::endl;
    }

}