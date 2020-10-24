#include <iostream>

#include <rpnx/derivator.hpp>
#include <string>
#include <rpnx/meta.hpp>

struct foo;

struct bar;


struct foo
{
    rpnx::derivator<void, foo, bar> thing_foo;
};

struct bar
{
    std::string value;
    bar(std::string const & init)
    : value(init)
    {

    }
   rpnx::derivator<void, foo, bar> thing_bar;
   std::string test()
   {
       return "baz " + value;
   }
};

int main()
{
    rpnx::derivator<int, std::string> test;
    test.as<0>() = 4;

    std::cout << test.as<0>()  << std::endl;
    std::cout << test.as<int>()  << std::endl;

    test.emplace<1>("Testing!");

    std::cout << test.as<1>() << std::endl;
    std::cout << test.as<std::string>() << std::endl;

    foo f;

    f.thing_foo.emplace<foo>();
    f.thing_foo.as<foo>().thing_foo.emplace<bar>("penguin");

    bar & the_bar = f.thing_foo.as<foo>().thing_foo.as<bar>();

    std::cout << the_bar.test() << std::endl;

    std::cout << std::boolalpha << "valueless? " << the_bar.thing_bar.holds_alternative<void>() << std::endl;

    the_bar.thing_bar.emplace<foo>();

    std::cout << std::boolalpha << "valueless? " << the_bar.thing_bar.holds_alternative<void>() << std::endl;

}