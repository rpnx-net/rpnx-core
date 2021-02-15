#include "rpnx/environment.hpp"
#include <iostream>

int main()
{
    auto env = rpnx::get_environment();


    for (auto & x : env)
    {
        std::cout << "Key=" << x.first << " Value=" << x.second << std::endl;
    }

    auto path = rpnx::get_path();

    std::cout << "Path string: " << rpnx::get_environment_variable("PATH").value_or("") << std::endl;

    for (auto & x : path)
    {
        std::cout << "Value=" << x << std::endl;
    }

    std::cout << "Current directory: " << rpnx::current_path() << std::endl;

}