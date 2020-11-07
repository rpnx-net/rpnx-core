#include "rpnx/serial_traits.hpp"
#include <future>
#include <iostream>
#include <string_view>

template < typename T >
void test(std::string_view const& str, T const& val, std::vector< char > const& expected)
{
    std::vector< char > output;
    std::vector< char > output2;

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

    
    T val_2{};

    std::size_t used = 0;

    rpnx::quick_generator_deserialize(val_2, [&](std::size_t n) {
        auto it = output.begin() + used;
        used += n;
        if (used > output.size())
            throw std::runtime_error((std::string(str) + ": deserialization out of bounds").c_str());
        return it;
    });

    // Note: not checking quick_iterator_deserialize for now...

    if (val == val_2)
    {
        std::cerr << str << ": Deserialized value matches expected value." << std::endl;
    }
    else
    {
        throw std::runtime_error((std::string(str) + ": Deserialized value does not equal expected value").c_str());
    }
    
}

int main()
{
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
}