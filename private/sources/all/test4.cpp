#include "rpnx/serial_traits.hpp"
#include <future>
#include <iostream>
#include <iomanip>
#include <string_view>

template < typename T >
void test(std::string_view const& str, T const& val, std::vector< char > const& expected, T t2 = T{})
{
    std::vector< char > output;
    std::vector< char > output2;
    std::vector< char > output3;


    auto print_output = [&] (std::vector<char> const & what){
        return;
        std::cout << "[";

        for (int i = 0; i < what.size(); i++) 
        {
            std::cout << std::hex << std::setw(2) << std::fixed << std::setfill('0') << "0x" << (unsigned int) what[i];
            if (i != what.size() - 1)
                std::cout << " ";

        }

        std::cout << "]" << std::endl;
        
    };

    rpnx::quick_generator_serialize(val, [&output](std::size_t n) {
        std::size_t size_old = output.size();
        output.resize(size_old + n);
        return output.begin() + size_old;
    });

    rpnx::quick_iterator_serialize(val, std::back_inserter(output2));

    if (output == expected)
    {
        std::cerr << str << ": Serialized generator output matches expected output." << std::endl;
    }
    else
    {
        print_output(output);
        throw std::runtime_error((std::string(str) + ": Serialized generator output does not match expected output").c_str());
    }

    if (output2 == expected)
    {
        std::cerr << str << ": Serialized iterator output matches expected output." << std::endl;
    }
    else
    {
        throw std::runtime_error((std::string(str) + ": Serialized iterator output does not match expected output").c_str());
    }

    

    std::size_t used = 0;
   // std::cout << "output A" << std::endl;
   // print_output(output);
    rpnx::quick_generator_deserialize(t2, [&](std::size_t n) {
        auto it = output.cbegin() + used;
        used += n;
        if (used > output.size())
            throw std::runtime_error((std::string(str) + ": deserialization out of bounds").c_str());
        return it;
    });
   // std::cout << "output B" << std::endl;
   // print_output(output);
    // Note: not checking quick_iterator_deserialize for now... It can exhibit UB if it doesn't work, need a bounds checked iterator wrapper thing.

    if (val == t2)
    {
        std::cerr << str << ": Deserialized value matches expected value." << std::endl;
    }
    else
    {
        rpnx::quick_iterator_serialize(t2, std::back_inserter(output3));
        print_output(output);
        print_output(output2);
        print_output(output3);
        throw std::runtime_error((std::string(str) + ": Deserialized value does not equal expected value").c_str());
    }
    
}

template <typename F, typename ...Ts>
void foo(F f, std::tuple<Ts...> const &v_tuple) 
{
    std::apply([&](auto const&... ts) {
        (f(ts), ...);
    }, v_tuple);
}




int main()
{
    foo([](auto x) { std::cout << "param " << x << std::endl; }, std::make_tuple(1, 3, 4));

   
    int f;
    bool b;
    std::tuple< int, int, bool, bool&, int const&, bool const& > br{0, 1, true, b, f, b};

    std::cout << typeid(br).name() << std::endl;

    using T2 = typename rpnx::untied<decltype(br)>::type;

    std::cout << typeid(T2).name() << std::endl;
    try
    {
        {
            std::tuple< bool, bool, bool, bool, bool, bool, bool, bool, int16_t > val{false, true, false, true, false, false, false, false, 5};
            
            test("std::tuple< bool, bool, bool, bool, bool, bool, bool, bool, int16_t >", val, {0b00001010, 5, 0});
        }

        {
            std::tuple< bool, bool, bool, bool, bool, bool, bool, int16_t > val{false, true, false, true, false, false, false,  5};

            test("std::tuple< bool, bool, bool, bool, bool, bool, bool, int16_t >", val, {0b00001010, 5, 0});
        }

        {
            std::tuple< bool, bool, bool, bool, bool, bool,int16_t > val{false, true, false, true, false, false, 5};

            test("std::tuple< bool, bool, bool, bool, bool, bool, int16_t >", val, {0b00001010, 5, 0});
        }

        {
            std::tuple< bool, bool, bool, bool, bool, int16_t > val{false, true, false, true, false, 5};

            test("std::tuple< bool, bool, bool, bool, bool,  int16_t >", val, {0b00001010, 5, 0});
        }

        {
            std::tuple< bool, bool, bool, bool, int16_t > val{false, true, false, true, 5};

            test("std::tuple< bool, bool, bool, bool, int16_t >", val, {0b00001010, 5, 0});
        }

        {
            std::tuple< bool, bool, bool, int16_t > val{false, true, false, 5};

            test("std::tuple< bool, bool, bool, int16_t >", val, {0b000000010, 5, 0});
        }

        {
            std::tuple< bool, bool, int16_t > val{false, true, 5};

            test("std::tuple< bool, bool,  int16_t >", val, {0b00000010, 5, 0});
        }

        {
            int8_t val = 4;
            test("int8_t", val, {4});
        }

        {
            int16_t val = 4;
            test("int16_t", val, {4, 0});
        }

        {
            int32_t val = 4;
            test("int32_t", val, {4, 0, 0, 0});
        }

        {
            int64_t val = 4;
            test("int64_t", val, {4, 0, 0, 0, 0, 0, 0, 0});
        }

        {
            uint8_t val = 4;
            test("uint8_t", val, {4});
        }

        {
            uint16_t val = 4;
            test("uint16_t", val, {4, 0});
        }

        {
            uint32_t val = 4;
            test("uint32_t", val, {4, 0, 0, 0});
        }

        {
            uint64_t val = 4;
            test("uint64_t", val, {4, 0, 0, 0, 0, 0, 0, 0});
        }

        {
            std::vector< int16_t > val{4, 5};
            test("std::vector<int16_t>", val, {2, 4, 0, 5, 0});
        }

        {
            std::vector< int32_t > val{4, 5};
            test("std::vector<int32_t>", val, {2, 4, 0, 0, 0, 5, 0, 0, 0});
        }

        {
            std::vector< std::vector< int16_t > > val{{4, 5}, {6, 7}};
            test("std::vector< std::vector< int16_t > >", val, {2, 2, 4, 0, 5, 0, 2, 6, 0, 7, 0});
        }

        {
            std::string val = "hello";
            test("std::string", val, {5, 'h', 'e', 'l', 'l', 'o'});
        }

       
    }
    catch (std::exception const & er)
    {
        std::cout << er.what() << std::endl;
        return -1;
    }
}