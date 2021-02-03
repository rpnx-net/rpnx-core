#include "rpnx/experimental/network.hpp"

#include "rpnx/experimental/priority_dispatcher.hpp"
#include "rpnx/experimental/source_iterator.hpp"
#include <chrono>
#include <iostream>
#include <regex>
#include <thread>

int main()
{
    std::string code = "#include <iostream>\r\n\r\nint main()\n{\r  std::cout << \"Hello World!\" << std::endl;\n  return 0;\n}\n";

    using namespace rpnx::experimental;
    auto [it, end] = make_source_iterator_pair("hello.cpp", code);


    std::string search_string = "Hello";

    auto result = std::search(it, end, search_string.begin(), search_string.end());

    if (result != end)
    {
        std::cout << "Found " << search_string << " at " << result.file_name()<< ":" << result.line() << ":" << result.column() << std::endl;
    }

    int last_line = -1;
    while (it != end)
    {
        if (it.line() != last_line)
        {
            if (last_line != -1)
            {
                std::cout << '\n';
            }

            std::cout << it.file_name() << ":" << it.line() << ": ";

            last_line = it.line();
        }

        if (*it != '\r' && *it != '\n')
            std::cout << *it;
        it++;
    }

    std::cout.flush();

}