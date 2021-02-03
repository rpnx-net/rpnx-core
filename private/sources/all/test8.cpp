#include "rpnx/experimental/network.hpp"

#include "rpnx/experimental/priority_dispatcher.hpp"
#include "rpnx/experimental/source_iterator.hpp"
#include <chrono>
#include <iostream>
#include <thread>

int main()
{
    std::string code = "#include <iostream>\r\n\r\nint main()\n{\n  std::cout << \"Hello World!\" << std::endl;\n  return 0;\n}\n";

    using namespace rpnx::experimental;
    auto it = source_iterator< std::string::iterator >(code.begin(), "hello.cpp");

    auto end = source_iterator< std::string::iterator >(code.end());

    int last_line = -1;
    while (it != end)
    {
        if (it.line() != last_line)
        {
            if (last_line != -1)
            {
                std::cout << '\n';
            }

            std::cout << it.name() << ":" << it.line() << ": ";

            last_line = it.line();
        }

        if (*it != '\r' && *it != '\n')
            std::cout << *it;
        it++;
    }

    std::cout.flush();
    // TODO
#if false
    try
    {

        rpnx::experimental::async_ip4_udp_socket socket;
        socket.open();
        socket.bind({ rpnx::experimental::ip4_address::any(), 0 });
        auto addr = socket.endpoint();

        std::cout << addr << std::endl;

        rpnx::experimental::ip4_udp_socket socket2;

        addr.address()[0] = 127;
        addr.address()[1] = 0;
        addr.address()[2] = 0;
        addr.address()[3] = 1;


        socket2.open();
        socket2.bind();

        rpnx::experimental::priority_dispatcher dispatch_engine;

        std::vector<std::byte> data_to_send = { (std::byte)1, (std::byte)2, (std::byte)3 };

        rpnx::experimental::net_send_async(socket2, addr, data_to_send.begin(), data_to_send.end(), dispatch_engine);

        std::vector<std::byte> data;
        rpnx::experimental::ip4_udp_endpoint from;
        rpnx::experimental::net_receive(socket, from, std::back_inserter(data));

        std::cout << from << std::endl;

        for (auto const& x : data)
        {
            std::cout << (int) x << std::endl;
        }

    }
    catch (std::exception const & err)
    {
        std::cerr << err.what() << std::endl;
    }
#endif
}