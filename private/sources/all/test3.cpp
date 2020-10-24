#include <iostream>

#include <rpnx/derivator.hpp>
#include <string>

int main()
{
    rpnx::derivator<int, std::string> test;
    test.as<0>() = 4;

    std::cout << test.as<0>()  << std::endl;


    test.emplace<1>("Testing!");

    std::cout << test.as<1>() << std::endl;

}