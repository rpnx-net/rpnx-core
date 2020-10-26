#include <iostream>

#include <rpnx/derivator.hpp>
#include <string>
#include <rpnx/meta.hpp>

struct foo;

struct bar;


struct foo
{
    rpnx::derivator<void, foo, bar> thing_foo;

    foo() { std::cout << "foo::foo()" << std::endl;
    }

    ~foo() { std::cout << "foo::~foo()" << std::endl;
    }
};

struct bar
{
    std::string value;


    bar(std::string const & init)
    : value(init) { std::cout << "bar::bar(\""<< init <<"\")" << std::endl; }

    bar(bar const& other) 
    {
        value = other.value;
        std::cout << "bar::bar(bar const&)" << std::endl;
    }

    bar(bar && other) 
    {
        value = std::move(other.value);
        std::cout << "bar::bar(bar &&)" << std::endl;
    }



    ~bar() { std::cout << "bar::~bar()" << std::endl;
    }
   rpnx::derivator<void, foo, bar> thing_bar;
   std::string test()
   {
       return "baz " + value;
   }
};

class tester_c
{
public:
    void operator()(foo const &)
    {
        std::cout << "tester foo" << std::endl;
    }

    void operator()(bar const &)
    {
        std::cout << "tester bar" << std::endl;
    }

    void operator()()
    {
        std::cout << "tester Valueless" << std::endl;
    }

};

int main()
{
    rpnx::derivator<int, std::string> test;
    test.as_checked<0>() = 4;

    std::cout << test.as_checked<0>()  << std::endl;
    std::cout << test.as_checked<int>()  << std::endl;

    test.emplace<1>("Testing!");

    std::cout << test.as_checked<1>() << std::endl;
    std::cout << test.as_checked<std::string>() << std::endl;

    foo f;

    f.thing_foo.emplace<foo>();
    f.thing_foo.as_checked<foo>().thing_foo.emplace<bar>("penguin");

    bar & the_bar = f.thing_foo.as_checked<foo>().thing_foo.as_checked<bar>();

    std::cout << the_bar.test() << std::endl;

    std::cout << std::boolalpha << "valueless? " << the_bar.thing_bar.holds_alternative<void>() << std::endl;

    the_bar.thing_bar.emplace<foo>();

    std::cout << std::boolalpha << "valueless? " << the_bar.thing_bar.holds_alternative<void>() << std::endl;

    std::cout << "Replace the_bar.thing_bar" << std::endl;

    the_bar.thing_bar.emplace< bar >("blah");

    std::cout << "Try assignment operator" << std::endl;

    the_bar.thing_bar = bar("test");

    tester_c tester_v;

    rpnx::visit(tester_v, the_bar.thing_bar);

    std::cout << "Valueless visit test" << std::endl;

    rpnx::derivator<void, bar> d;
    rpnx::visit(tester_v, d);

}