#include <future>
#include <iostream>



class foo
{
  public:
    foo() 
    {
        std::cout << "foo()" << std::endl;

    }

    foo(foo const&)
    {
        std::cout << "foo(foo const&)" << std::endl;
    }

    ~foo()
    {
        std::cout << "foo(foo const&)" << std::endl;
    }

    foo(foo&&) 
    {
        std::cout << "foo(foo&&)" << std::endl;
    }

    void operator()()
    {
        std::cout << "foo::operator()()" << std::endl;
    }

    
};


int main()
{
    foo f;
    std::function< void() > task;
    std::cout << "A" << std::endl;
    task = std::function< void() >(f);
    std::cout << "B" << std::endl;
    std::function< void() > task2 = std::move(task);
    std::cout << "C" << std::endl;

}