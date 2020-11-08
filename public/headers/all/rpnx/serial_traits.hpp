/*
Copyright (c) 2016, 2017, 2018, 2020 Ryan P. Nicholl <rnicholl@protonmail.com> http://rpnx.net/
RPNX Serial Traits 2
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS
IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
This is currently a work in progress. It's a version 2 of the original version I published as a separate library.
This version should perform better due to the inclusion of generator interfaces, which should, at least in theory,
optimize much better than a strictly iterator based approach, while maintaining safety.


*/
#ifndef RPNX_SERIAL_TRAITS_2_HPP
#define RPNX_SERIAL_TRAITS_2_HPP

#include <algorithm>
#include <assert.h>
#include <cinttypes>
#include <cstddef>
#include <deque>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "rpnx/meta.hpp"

namespace rpnx
{

    // These are specialization placeholders
    // They are used to represent integers of any
    // size in a machine independent format.
    struct intany;
    struct uintany;

    template < typename I >
    struct big_endian;

    template < typename I >
    struct little_endian;

    template < typename T >
    struct serial_traits;

    template < typename T, typename ItGenerator >
    struct synchronous_generator_serial_traits;

    template < typename T, typename It >
    struct synchronous_iterator_serial_traits;

    template < typename T >
    struct c_fixed_serial_size;

    template < typename T, typename It >
    auto quick_serialize(T const& val, It iterator) -> It;

    // The following 4 specializations are traits for unsigned integers
    // has_fixed_serial_size should return true, and returns the number of bytes in
    // an integer
    template <>
    struct serial_traits< std::uint8_t >
    {
        static inline constexpr bool has_fixed_serial_size() noexcept
        {
            return true;
        }
        static inline constexpr std::size_t fixed_serial_size() noexcept
        {
            return 1;
        }
        static inline constexpr std::size_t serial_size(const std::uint8_t&) noexcept
        {
            return 1;
        }
    };

    template <>
    struct serial_traits< std::uint16_t >
    {
        static inline constexpr bool has_fixed_serial_size() noexcept
        {
            return true;
        }
        static inline constexpr std::size_t fixed_serial_size() noexcept
        {
            return 2;
        }
        static inline constexpr std::size_t serial_size(const std::uint16_t&) noexcept
        {
            return 2;
        }
    };

    template <>
    struct serial_traits< std::uint32_t >
    {
        static inline constexpr bool has_fixed_serial_size() noexcept
        {
            return true;
        }
        static inline constexpr std::size_t fixed_serial_size() noexcept
        {
            return 4;
        }
        static inline constexpr std::size_t serial_size(const std::uint32_t&) noexcept
        {
            return 4;
        }
    };

    template <>
    struct serial_traits< std::uint64_t >
    {
        static inline constexpr bool has_fixed_serial_size() noexcept
        {
            return true;
        }
        static inline constexpr std::size_t fixed_serial_size() noexcept
        {
            return 8;
        }
        static inline constexpr std::size_t serial_size(const std::uint64_t&) noexcept
        {
            return 8;
        }
    };

    template <>
    struct serial_traits< std::int8_t >
    {
        static inline constexpr bool has_fixed_serial_size() noexcept
        {
            return true;
        }
        static inline constexpr std::size_t fixed_serial_size() noexcept
        {
            return 1;
        }
        static inline constexpr std::size_t serial_size(const std::int8_t&) noexcept
        {
            return 1;
        }
    };

    template <>
    struct serial_traits< std::int16_t >
    {
        static inline constexpr bool has_fixed_serial_size() noexcept
        {
            return true;
        }
        static inline constexpr std::size_t fixed_serial_size() noexcept
        {
            return 2;
        }
        static inline constexpr std::size_t serial_size(const std::int16_t&) noexcept
        {
            return 2;
        }
    };

    template <>
    struct serial_traits< std::int32_t >
    {
        static inline constexpr bool has_fixed_serial_size() noexcept
        {
            return true;
        }
        static inline constexpr std::size_t fixed_serial_size() noexcept
        {
            return 4;
        }
        static inline constexpr std::size_t serial_size(const std::int32_t&) noexcept
        {
            return 4;
        }
    };

    template <>
    struct serial_traits< std::int64_t >
    {
        static inline constexpr bool has_fixed_serial_size() noexcept
        {
            return true;
        }
        static inline constexpr std::size_t fixed_serial_size() noexcept
        {
            return 8;
        }
        static inline constexpr std::size_t serial_size(const std::int64_t&) noexcept
        {
            return 8;
        }
    };

    template <>
    struct serial_traits< char > : serial_traits< int8_t >
    {
    };

    // The following 4 specializations are for serial synchronous generator interfaces for the
    // unsigned integers

    template < typename ItGenerator >
    struct synchronous_generator_serial_traits< std::uint8_t, ItGenerator >
    {
        static inline constexpr auto serialize(std::uint8_t val, ItGenerator out_generator)
        {
            auto it = out_generator(1);
            *it++ = val;
        }

        static inline constexpr auto deserialize(std::uint8_t& val, ItGenerator in_generator)
        {
            auto it = in_generator(1);
            val = *it++;
        }
    };

    template < typename ItGenerator >
    struct synchronous_generator_serial_traits< std::uint16_t, ItGenerator >
    {
        static inline constexpr auto serialize(std::uint16_t val, ItGenerator out_generator)
        {
            auto it = out_generator(2);
            *it++ = val & 0xFF;
            *it = (val >> 8) & 0xFF;
        }

        static inline constexpr auto deserialize(std::uint16_t& val, ItGenerator in_generator)
        {
            auto it = in_generator(2);
            val = *it++;
            val |= (*it << 8);
        }
    };

    template < typename ItGenerator >
    struct synchronous_generator_serial_traits< std::uint32_t, ItGenerator >
    {
        static inline constexpr auto serialize(std::uint32_t val, ItGenerator out_generator)
        {
            auto it = out_generator(4);
            *it++ = val & 0xFF;
            *it++ = (val >> 8) & 0xFF;
            *it++ = (val >> 16) & 0xFF;
            *it = (val >> 24) & 0xFF;
            // TODO: Update this to use iterator trait specializations
            // synchronous_iterator_serialize_traits<std::uint32_t, std::remove_reference_t<decltype(*it)>>(it, val);
        }

        static inline constexpr auto deserialize(std::uint32_t& val, ItGenerator in_generator)
        {
            auto it = in_generator(4);
            val = *it++;
            val |= (*it++ << 8);
            val |= (*it++ << 16);
            val |= (*it++ << 24);
        }
    };

    template < typename ItGenerator >
    struct synchronous_generator_serial_traits< std::uint64_t, ItGenerator >
    {
        static inline constexpr auto serialize(std::uint64_t val, ItGenerator out_generator)
        {
            auto it = out_generator(8);
            *it++ = val & 0xFF;
            *it++ = (val >> 8) & 0xFF;
            *it++ = (val >> 16) & 0xFF;
            *it++ = (val >> 24) & 0xFF;
            *it++ = (val >> 32) & 0xFF;
            *it++ = (val >> 40) & 0xFF;
            *it++ = (val >> 48) & 0xFF;
            *it = (val >> 56) & 0xFF;
            // TODO: Update this to use iterator trait specializations
            // synchronous_iterator_serialize_traits<std::uint32_t, std::remove_reference_t<decltype(*it)>>(it, val);
        }

        static inline constexpr auto deserialize(std::uint64_t& val, ItGenerator in_generator)
        {
            auto it = in_generator(8);
            val = std::uint64_t(*it++);
            val |= (std::uint64_t(*it++) << 8);
            val |= (std::uint64_t(*it++) << 16);
            val |= (std::uint64_t(*it++) << 24);
            val |= (std::uint64_t(*it++) << 32);
            val |= (std::uint64_t(*it++) << 40);
            val |= (std::uint64_t(*it++) << 48);
            val |= (std::uint64_t(*it) << 56);
        }
    };

    template < typename ItGenerator >
    struct synchronous_generator_serial_traits< std::int8_t, ItGenerator >
    {
        static inline constexpr auto serialize(std::int8_t val, ItGenerator out_generator)
        {
            auto it = out_generator(1);
            *it++ = val;
        }

        static inline constexpr auto deserialize(std::int8_t& val, ItGenerator in_generator)
        {
            auto it = in_generator(1);
            val = *it++;
        }
    };

    template < typename ItGenerator >
    struct synchronous_generator_serial_traits< std::int16_t, ItGenerator >
    {
        static inline constexpr auto serialize(std::int16_t val, ItGenerator out_generator)
        {
            auto it = out_generator(2);
            *it++ = val & 0xFF;
            *it = (val >> 8) & 0xFF;
        }

        static inline constexpr auto deserialize(std::int16_t& val, ItGenerator in_generator)
        {
            auto it = in_generator(2);
            val = *it++;
            val |= (*it << 8);
        }
    };

    template < typename ItGenerator >
    struct synchronous_generator_serial_traits< std::int32_t, ItGenerator >
    {
        static inline constexpr auto serialize(std::int32_t val, ItGenerator out_generator)
        {
            auto it = out_generator(4);
            *it++ = val & 0xFF;
            *it++ = (val >> 8) & 0xFF;
            *it++ = (val >> 16) & 0xFF;
            *it = (val >> 24) & 0xFF;
            // TODO: Update this to use iterator trait specializations
            // synchronous_iterator_serialize_traits<std::int32_t, std::remove_reference_t<decltype(*it)>>(it, val);
        }

        static inline constexpr auto deserialize(std::int32_t& val, ItGenerator in_generator)
        {
            auto it = in_generator(4);
            val = *it++;
            val |= (*it++ << 8);
            val |= (*it++ << 16);
            val |= (*it++ << 24);
        }
    };

    template < typename ItGenerator >
    struct synchronous_generator_serial_traits< std::int64_t, ItGenerator >
    {
        static inline constexpr auto serialize(std::int64_t val, ItGenerator out_generator)
        {
            auto it = out_generator(8);
            *it++ = val & 0xFF;
            *it++ = (val >> 8) & 0xFF;
            *it++ = (val >> 16) & 0xFF;
            *it++ = (val >> 24) & 0xFF;
            *it++ = (val >> 32) & 0xFF;
            *it++ = (val >> 40) & 0xFF;
            *it++ = (val >> 48) & 0xFF;
            *it = (val >> 56) & 0xFF;
            // TODO: Update this to use iterator trait specializations
            // synchronous_iterator_serialize_traits<std::int32_t, std::remove_reference_t<decltype(*it)>>(it, val);
        }

        static inline constexpr auto deserialize(std::int64_t& val, ItGenerator in_generator)
        {
            auto it = in_generator(8);
            val = *it++;
            val |= (*it++ << 8);
            val |= (*it++ << 16);
            val |= (*it++ << 24);
            val |= (std::uint64_t(*it++) << 32);
            val |= (std::uint64_t(*it++) << 40);
            val |= (std::uint64_t(*it++) << 48);
            val |= (std::uint64_t(*it) << 56);
        }
    };

    template < typename It >
    struct synchronous_iterator_serial_traits< std::uint8_t, It >
    {
        static inline constexpr auto serialize(std::uint8_t val, It outIt) -> It
        {
            *outIt++ = val;
            return outIt;
        }

        static inline constexpr auto deserialize(std::uint8_t& val, It in) -> It
        {
            val = *in++;
            return in;
        }
    };

    template < typename It >
    struct synchronous_iterator_serial_traits< char, It >
    {
        static inline constexpr auto serialize(char val, It outIt) -> It
        {
            *outIt++ = val;
            return outIt;
        }

        static inline constexpr auto deserialize(char& val, It in) -> It
        {
            val = *in++;
            return in;
        }
    };

    template < typename It >
    struct synchronous_iterator_serial_traits< std::uint16_t, It >
    {
        static inline constexpr auto serialize(std::uint16_t val, It outIt) -> It
        {
            *outIt++ = val;
            *outIt++ = val >> 8;
            return outIt;
        }

        static inline constexpr auto deserialize(std::uint16_t& val, It in) -> It
        {
            val = *in++;
            val |= (*in++ << 8);
            return in;
        }
    };

    template < typename It >
    struct synchronous_iterator_serial_traits< std::int16_t, It >
    {
        static inline constexpr auto serialize(std::int16_t val, It outIt) -> It
        {
            *outIt++ = val;
            *outIt++ = val >> 8;
            return outIt;
        }

        static inline constexpr auto deserialize(std::int16_t& val, It in) -> It
        {
            val = *in++;
            val |= (*in++ << 8);
            return in;
        }
    };

    template < typename It >
    struct synchronous_iterator_serial_traits< std::int32_t, It >
    {
        static inline constexpr auto serialize(std::int32_t val, It outIt) -> It
        {
            *outIt++ = val;
            *outIt++ = val >> 8;
            *outIt++ = val >> 16;
            *outIt++ = val >> 24;
            return outIt;
        }

        static inline constexpr auto deserialize(std::int32_t& val, It in) -> It
        {
            val = *in++;
            val |= (*in++ << 8);
            val |= (*in++ << 16);
            val |= (*in++ << 24);
            return in;
        }
    };

    template < typename It >
    struct synchronous_iterator_serial_traits< std::uint32_t, It >
    {
        static inline constexpr auto serialize(std::uint32_t val, It outIt) -> It
        {
            *outIt++ = val;
            *outIt++ = val >> 8;
            *outIt++ = val >> 16;
            *outIt++ = val >> 24;
            return outIt;
        }

        static inline constexpr auto deserialize(std::uint32_t& val, It in) -> It
        {
            val = *in++;
            val |= (*in++ << 8);
            val |= (*in++ << 16);
            val |= (*in++ << 24);
            return in;
        }
    };

    template < typename It >
    struct synchronous_iterator_serial_traits< std::uint64_t, It >
    {
        static inline constexpr auto serialize(std::uint64_t val, It outIt) -> It
        {
            *outIt++ = val;
            *outIt++ = val >> 8;
            *outIt++ = val >> 16;
            *outIt++ = val >> 24;
            *outIt++ = val >> 32;
            *outIt++ = val >> 40;
            *outIt++ = val >> 48;
            *outIt++ = val >> 56;
            return outIt;
        }

        static inline constexpr auto deserialize(std::uint64_t& val, It in) -> It
        {
            val = *in++;
            val |= (*in++ << 8);
            val |= (*in++ << 16);
            val |= (*in++ << 24);
            val |= (std::uint64_t(*in++) << 32);
            val |= (std::uint64_t(*in++) << 40);
            val |= (std::uint64_t(*in++) << 48);
            val |= (std::uint64_t(*in++) << 56);
            return in;
        }
    };

    template < typename It >
    struct synchronous_iterator_serial_traits< std::int64_t, It >
    {
        static inline constexpr auto serialize(std::int64_t val, It outIt) -> It
        {
            *outIt++ = val;
            *outIt++ = val >> 8;
            *outIt++ = val >> 16;
            *outIt++ = val >> 24;
            *outIt++ = val >> 32;
            *outIt++ = val >> 40;
            *outIt++ = val >> 48;
            *outIt++ = val >> 56;
            return outIt;
        }

        static inline constexpr auto deserialize(std::int64_t& val, It in) -> It
        {
            val = *in++;
            val |= (*in++ << 8);
            val |= (*in++ << 16);
            val |= (*in++ << 24);
            val |= (std::uint64_t(*in++) << 32);
            val |= (std::uint64_t(*in++) << 40);
            val |= (std::uint64_t(*in++) << 48);
            val |= (std::uint64_t(*in++) << 56);
            return in;
        }
    };

    template < typename It >
    struct synchronous_iterator_serial_traits< std::int8_t, It >
    {
        static inline constexpr auto serialize(std::int8_t val, It outIt) -> It
        {
            *outIt++ = val;
            return outIt;
        }

        static inline constexpr auto deserialize(std::int8_t& val, It in) -> It
        {
            val = *in++;
            return in;
        }
    };

    template < typename It, typename... Ts >
    struct synchronous_iterator_serial_traits< std::tuple< Ts... >, It >
    {
        static inline constexpr auto serialize(std::tuple<Ts...> const& val, It outIt) -> It
        {
            // clang-format off
            std::apply([&](auto const&... ts)
            {
                ([&](auto const& subvalue) 
                 {
                     outIt = synchronous_iterator_serial_traits< decltype(value) >::serialize(subvalue, outIt);
                 }(ts),
                ...);
            }, val);
            // clang-format on
            return outIt;
        }

        static inline constexpr auto deserialize(std::tuple< Ts... > & val, It inIt) -> It
        {
            // clang-format off
            std::apply([&](auto &... ts)
            {
                ([&](auto & subvalue) 
                 {
                     inIt = synchronous_iterator_serial_traits< decltype(value) >::deserialize(subvalue, inIt);
                 }(ts),
                ...);
            }, val);
            // clang-format on
            return inIt;
        }
    };

    template < typename ItGenerator, typename... Ts >
    struct synchronous_generator_serial_traits< std::tuple< Ts... >, ItGenerator >
    {
        static inline constexpr void serialize(std::tuple< Ts... > const& val, ItGenerator out_gen) 
        {
            // clang-format off
            std::apply([&](auto const&... ts)
            {
                ([&](auto const& subvalue) 
                 {
                     synchronous_generator_serial_traits< decltype(value) >::serialize(subvalue, out_gen);
                 }(ts),
                ...);
            }, val);
            // clang-format on
            return;
        }

        static inline constexpr auto deserialize(std::tuple< Ts... >& val, ItGenerator inIt) 
        {
            // clang-format off
            std::apply([&](auto &... ts)
            {
                ([&](auto & subvalue) 
                 {
                     synchronous_generator_serial_traits< decltype(value) >::deserialize(subvalue, inIt);
                 }(ts),
                ...);
            }, val);
            // clang-format on
            return;
        }
    };

    template <>
    struct serial_traits< uintany >
    {
        static inline constexpr bool has_fixed_serial_size()
        {
            return false;
        }
        static inline constexpr std::size_t serial_size(std::uint64_t value)
        {
            if (value < 128ull)
                return 1;
            else if (value < 16512ull)
                return 2;
            else if (value < 2113664ull)
                return 3;
            else if (value < 270549120ull)
                return 4;
            else if (value < 34630287488ull)
                return 5;
            else if (value < 4432676798592ull)
                return 6;
            else if (value < 567382630219904ull)
                return 7;
            else if (value < 72624976668147840ull)
                return 8;
            else if (value < 9295997013522923648ull)
                return 9;
            else
                return 10; // 10 units can represent all 64-bit integers.
        }
    };

    template < typename It >
    struct synchronous_iterator_serial_traits< uintany, It >
    {
        static inline It serialize(uintmax_t in, It out)
        {
            // This isn't the best function, it might give wrong results for really large values.
            // It's good enough for now.
            // Also, optimize this, it's not very fast

            uintmax_t base = in;

            uintmax_t bytecount = 1;
            uintmax_t max = 0;

            max = (1ull << (7)) - 1;
            while (base > max)
            {
                bytecount++;
                base -= max + 1;
                max = (1ull << ((7) * bytecount)) - 1;
            }

            for (uintmax_t i = 0; i < bytecount; i++)
            {
                uint8_t val = base & ((uintmax_t(1) << (7)) - 1);
                if (i != bytecount - 1)
                {
                    val |= 0b10000000;
                }
                *out++ = val;
                base >>= 7;
            }
            return out;
        }

        static inline It deserialize(uintmax_t& n, It in)
        {
            // This function is slow, optimize it
            n = 0;
            uintmax_t n2 = 0;
            while (true)
            {
                uint8_t a = *in++;
                uint8_t read_value = a & 0b1111111;
                n += (uintmax_t(read_value) << (n2 * 7));
                if (!(a & 0b10000000))
                    break;
                n2++;
            }
            for (uintmax_t i = 1; i <= n2; i++)
            {
                n += (uintmax_t(1) << (i * 7));
            }
            return in;
        }
    };

    template < typename ItF >
    struct synchronous_generator_serial_traits< uintany, ItF >
    {
        template < typename Integral >
        static inline constexpr void deserialize(Integral& i, ItF generator)
        {
            static_assert(std::is_integral_v< Integral >);

            i = 0;
            Integral i2 = 0;
            while (true)
            {
                auto it = generator(1);
                uint8_t val = *it++;
                i2 <<= 7;
                i2 |= val & 0b1111111;
                if (val & 0b10000000)
                {
                    // There are additional bytes
                    i <<= 7;
                    // TODO: Check for overflow here and throw an exception if the
                    // variable length integer is too large to be encoded in the destination type

                    i |= 0b10000000;
                }
                else
                    break;
            }

            i += i2;
        }
    };

    template < typename T, typename A >
    struct serial_traits< std::vector< T, A > >
    {
        static inline constexpr bool has_fixed_serial_size()
        {
            return false;
        }
        static inline constexpr std::size_t serial_size(std::vector< T, A > const& value)
        {
            if constexpr (serial_traits< typename std::vector< T, A >::value_type >::has_fixed_serial_size())
            {
                return serial_traits< uintany >::serial_size(value.size()) + serial_traits< typename std::vector< T, A >::value_type >::fixed_serial_size() * value.size();
            }
            else
            {
                std::size_t result = 0;
                result += serial_traits< uintany >::serial_size(value.size());
                for (auto const& x : value)
                {
                    result += serial_traits< typename std::vector< T, A >::value_type >::serial_size(x);
                }
                return result;
            }
        }
    };

    template < typename... Ts >
    struct serial_traits< std::tuple< Ts... > >
    {
        static inline constexpr bool has_fixed_serial_size()
        {
            return is_true_for_all< c_fixed_serial_size, Ts... >::value;
        }
        //  static_assert(false, "Unimplemented");

        static constexpr std::size_t fixed_serial_size()
        {
            static_assert(has_fixed_serial_size(), "Type must have a fixed size to use fixed_serial_size()");

            std::size_t val = 0;

            
        }

        
        
    };

    /*
    template < typename T, typename K, typename A >
    struct serial_traits< std::map< T, K, A > >
    {
        static inline constexpr bool has_fixed_serial_size()
        {
            return false;
        }
        static inline constexpr std::size_t serial_size(std::map< T, K, A > const& value)
        {
            if constexpr (serial_traits< typename std::map< T, K, A >::mapped_type >::has_fixed_serial_size() && serial_traits< typename std::map< T, K, A >::key_type >::has_fixed_serial_size())
            {
                return serial_traits< uintany >::serial_size(value.size()) + serial_traits< typename std::vector< T, A >::value_type >::fixed_serial_size() * value.size();
            }
            else
            {
                std::size_t result = 0;
                result += serial_traits< uintany >::serial_size(value.size());
                for (auto const& x : value)
                {
                    result += serial_traits< typename std::vector< T, A >::value_type >::serial_size(x);
                }
                return result;
            }
        }
    };
    */

    template <>
    struct serial_traits< std::string >
    {
        static inline constexpr bool has_fixed_serial_size()
        {
            return false;
        }
        static inline std::size_t serial_size(std::string const& value)
        {
            return serial_traits< uintany >::serial_size(value.size()) + value.size();
        }
    };

    template < typename ItGenerator >
    struct synchronous_generator_serial_traits< std::string, ItGenerator >
    {
        static inline constexpr auto serialize(std::string const& val, ItGenerator out_generator)
        {
            auto it = out_generator(serial_traits< std::string >::serial_size(val));
            it = synchronous_iterator_serial_traits< uintany, decltype(it) >::serialize(val.size(), it);
            std::copy(val.begin(), val.end(), it);
        }

        static inline constexpr auto deserialize(std::string& val, ItGenerator in_generator)
        {
            std::size_t count;
            synchronous_generator_serial_traits< uintany, ItGenerator >::deserialize(count, in_generator);
            auto it = in_generator(count);
            val.resize(count);
            std::copy_n(it, count, val.begin());
        }
    };

    template < typename T, typename Alloc, typename It >
    struct synchronous_iterator_serial_traits< std::vector< T, Alloc >, It >
    {
        //    static_assert(false, "debug message");
        static inline constexpr auto serialize(std::vector< T, Alloc > const& val, It it) -> It
        {
            it = synchronous_iterator_serial_traits< uintany, decltype(it) >::serialize(val.size(), it);
            for (T const& x : val)
            {
                it = synchronous_iterator_serial_traits< T, decltype(it) >::serialize(x, it);
            }
            return it;
        }

        static inline constexpr auto deserialize(std::vector< T, Alloc >& value, It it) -> It
        {
            static_assert(false, "Unimplemented");
        }
    };

    template < typename It >
    struct synchronous_iterator_serial_traits< std::string, It >
    {
        //    static_assert(false, "debug message");
        static inline constexpr auto serialize(std::string const& val, It it) -> It
        {
            it = synchronous_iterator_serial_traits< uintany, decltype(it) >::serialize(val.size(), it);
            return std::copy(val.cbegin(), val.cend(), it);
        }

        static inline constexpr auto deserialize(std::string& value, It it) -> It
        {
            std::size_t size;
            it = synchronous_iterator_serial_traits< uintany, decltype(it) >::serialize(size, it);
            // TODO: This is not efficient with all types of iterators.
            // May not even be legal in some cases.
            value.reserve(size);
            std::copy_n(it, size, std::back_inserter(value));
            std::advance(it, size);
            return it;
        }
    };

    template < typename T2, typename Alloc, typename ItGenerator >
    struct synchronous_generator_serial_traits< std::vector< T2, Alloc >, ItGenerator >
    {
        // template <typename T2, typename Alloc2>
        static inline constexpr auto serialize(std::vector< T2, Alloc > const& val, ItGenerator out_generator)
        {
            if constexpr (serial_traits< typename std::vector< T2, Alloc >::value_type >::has_fixed_serial_size())
            {
                auto it = out_generator(serial_traits< std::vector< T2, Alloc > >::serial_size(val));
                it = synchronous_iterator_serial_traits< uintany, decltype(it) >::serialize(val.size(), it);
                for (T2 const& x : val)
                {
                    it = synchronous_iterator_serial_traits< T2, decltype(it) >::serialize(x, it);
                }
            }
            else
            {
                auto it = out_generator(serial_traits< uintany >::serial_size(val.size()));
                it = synchronous_iterator_serial_traits< uintany, decltype(it) >::serialize(val.size(), it);
                for (auto const& x : val)
                {
                    synchronous_generator_serial_traits< T2, ItGenerator >::serialize(x, out_generator);
                }
            }
        }

        static inline constexpr auto deserialize(std::vector< T2, Alloc >& val, ItGenerator in_generator)
        {
            if constexpr (serial_traits< typename std::vector< T2, Alloc >::value_type >::has_fixed_serial_size())
            {
                std::size_t sz;
                synchronous_generator_serial_traits< uintany, ItGenerator >::deserialize(sz, in_generator);
                std::size_t total_size = sz * serial_traits< typename std::vector< T2, Alloc >::value_type >::fixed_serial_size();

                auto it = in_generator(total_size);
                for (std::size_t i = 0; i != sz; i++)
                {
                    T2 t;
                    it = synchronous_iterator_serial_traits< T2, decltype(it) >::deserialize(t, it);
                    val.emplace_back(std::move(t));
                }

                return;
            }
            else
            {
                std::size_t sz;
                synchronous_generator_serial_traits< uintany, ItGenerator >::deserialize(sz, in_generator);

                for (std::size_t i = 0; i != sz; i++)
                {
                    T2 t;
                    synchronous_generator_serial_traits< T2, ItGenerator >::deserialize(t, in_generator);
                    val.emplace_back(std::move(t));
                }

                return;
            }
        }
    };

    /*template <typename I>
    struct serial_traits<big_endian<I>>
            : serial_traits<I>
    {
            static_assert(std::is_integral<I>::value);
    };
    */

    template < typename T, typename ItF >
    inline void quick_generator_serialize(T const& t, ItF f)
    {
        synchronous_generator_serial_traits< T, ItF >::serialize(t, f);
    }

    template < typename T, typename It >
    inline auto quick_iterator_serialize(T const& t, It i) -> It
    {
        return synchronous_iterator_serial_traits< T, It >::serialize(t, i);
    }

    template < typename T, typename ItF >
    inline void quick_generator_deserialize(T& t, ItF f)
    {
        synchronous_generator_serial_traits< T, ItF >::deserialize(t, f);
    }

    template < typename T, typename It >
    inline auto quick_iterator_deserialize(T& t, It i) -> It
    {
        return synchronous_iterator_serial_traits< T, It >::deserialize(t, i);
    }

    template < typename T >
    inline std::size_t get_serial_size(T const& t)
    {
        return serial_traits< T >::serial_size(t);
    }

    template < typename T >
    struct c_fixed_serial_size
    {
        static const constexpr bool value = serial_traits< T >::has_fixed_serial_size();
    };

    static_assert(c_fixed_serial_size< std::tuple< std::int32_t, std::int32_t > >::value == true);
    static_assert(c_fixed_serial_size< std::tuple< std::int32_t, std::string > >::value == false);

} // namespace rpnx

#endif